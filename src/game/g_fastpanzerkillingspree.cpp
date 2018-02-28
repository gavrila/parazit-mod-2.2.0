#include <bgame/impl.h>

/************************************************************
*                                                           *
*   File: g_fastpanzerkillingspree.c                        *
*   Description: Functions that facilitate new fast panzer  *
*   killing sprees	                                        *
*   Author: AndyStutz                                       *
*                                                           *
************************************************************/

// AndyStutz
// First level is when fast panzer killing spree will start
// Second level is when we want to end the killing spree if 
// the user has that many kills.  We can give a killing spree
// won notification or we could even kill the user for the fun
// of it - this would have the added bonus of resetting values
// such as killing spree counts for us
int		fastpanzerkillspreeLevels[2] = { 5, 35 };

/*
===============
G_SendFastPanzerKillSpree
===============
*/
static void G_SendFastPanzerKillSpree( gentity_t *client, fpks_t fpksLevel, int kills ) {
	gentity_t* tent = G_TempEntity( vec3_origin, EV_FASTPANZERKILLSPREE );
	tent->s.density = fpksLevel;
	tent->s.eventParms[0] = client - g_entities;
	tent->s.eventParms[1] = kills;
	tent->r.svFlags = SVF_BROADCAST;
}

/*
==============
G_AddFastPanzerKillSpree
==============
*/
void G_AddFastPanzerKillSpree(gentity_t *ent) {
	int kills, i;

	// All FastPanzer settings disabled
	if (!g_fastpanzer.integer)
		return;

	// Not a player
 	if (!ent || !ent->client)
		return;

	// Increment kills
	kills = ++ent->client->pers.fastpanzerkillspreekills;

	// FastPanzerKillSpree disabled
	if( !g_fastpanzerkillspree.integer )
		return;

	// AndyStutz
	for (i = 0; i <= FPKS_KILLINGSPREESTOP; i++) {
		if (kills == fastpanzerkillspreeLevels[i]) {
			// Send event for client side work (displaying message and playing sound)
			G_SendFastPanzerKillSpree(ent, (fpks_t)i, kills);

			// Now do the server side work
			// Now start the fastpanzerkillspree
			//string buffer = va("!fastpanzerkillspree start %d\n", ent->client->ps.clientNum);
			//trap_SendConsoleCommand( EXEC_NOW, buffer.c_str() );
			if (i == FPKS_KILLINGSPREESTART) {
				G_StartFastPanzerKillSpree(ent);
			}
			else if (i == FPKS_KILLINGSPREESTOP) {
				// Splat the user since they reached the top level for the spree
				string buffer = va("!splat %d\n", ent->client->ps.clientNum);
				// Let's exec it now so there isn't a timing issue if they die before
				// the event were to get raised.
				trap_SendConsoleCommand( EXEC_NOW, buffer.c_str() );
			}				
		}
	}

}


void G_StartFastPanzerKillSpree(gentity_t *ent)
{
    // bail if no clients
    if (level.numConnectedClients < 1) {
		return;
    }

	// Clear any existing fastpanzerkillspreeclientdata objects in our vector
	g_FastPanzerKillSpreeClientData.clear();

	// Set bots to not balance teams as we're going to be moving them if there are any
	// Append seems to not execute fast enough, setting to NOW which is never a good
	trap_SendConsoleCommand( EXEC_NOW, "bot balanceteams 0\n" );

	Client* killspreeclient = &g_clientObjects[ent->s.number];
	//int ksplayerslot = lookupplayerslot( va("%d", ent->client->ps.clientNum) );
	//if (ksplayerslot == -1) // client match not found, error out (return) here
		//return;
	
	//Client* killspreeclient = &g_clientObjects[ksplayerslot];
	const User& killspreeuser = *connectedUsers[killspreeclient->slot];

	// Make sure killspree player is currently on a team before continuing
	team_t killspreeplayerteam = ent->client->sess.sessionTeam;
	//team_t killspreeplayerteam = killspreeclient->gclient.sess.sessionTeam;
	if (killspreeplayerteam != TEAM_AXIS && killspreeplayerteam != TEAM_ALLIES)
		return;

	// Don't set g_fastpanzerkillspreeon = true yet as the move causes a kill
	// event to trigger and it will think the kill spree player died, restoring
	// everything back immediately after setting it.  We have to set the true
	// within the putteam code after the player has been moved to the proper team
	// But we can still record their clientNum here now...
	// UDPATE: Changing this now so that when a player gets a killing spree, we
	// don't change their team (they stay on the team they're on) and everyone
	// else gets put to the opposite team as them.  This way it doesn't register
	// a kill event for them and we don't have to worry about setting the
	// fastpanzer killspree flag to true here.  It's also just nicer for the
	// killspree player as they're being rewarded and shouldn't have to change teams.

	//g_fastpanzerkillspreeclientnum = killspreeclient->gclient.ps.clientNum;
	g_fastpanzerkillspreeclientnum = ent->client->ps.clientNum;
		
	// We can now turn flag on here based on update above
	g_fastpanzerkillspreeon = true;

	// Set other players to axis.  If the killspreeplayer is axis, then change
	// it to allies.
	team_t otherplayersteam = TEAM_AXIS;
	if (killspreeplayerteam == TEAM_AXIS)
		otherplayersteam = TEAM_ALLIES;


	for (int i = 0; i < MAX_CLIENTS; i++) {
		User& currentuser = *connectedUsers[i];
		if (currentuser == User::BAD)
			continue;
			
		string iuser = va("%d",i);
		int slot = lookupplayerslot( iuser );
		if (slot == -1) // client match not found
			continue;

		Client* currentclient = &g_clientObjects[slot];

		fastpanzer_killspree_clientdata_t fpkscd = fastpanzer_killspree_clientdata_t();

		fpkscd.guid = currentuser.guid;
		fpkscd.name = currentuser.name;

		gclient_t* const currentgclient = &currentclient->gclient;
		gentity_t* const currentgentity = &currentclient->gentity;

		fpkscd.killspreekills = currentgclient->pers.killspreekills;
		fpkscd.fastpanzerkillspreekills = currentgclient->pers.fastpanzerkillspreekills;

		// If user is in spectator mode, simply ignore their settings
		// because we don't need to make any adjustments for them
		// since we're not changing any of their settings now
		if (currentgclient->sess.sessionTeam == TEAM_SPECTATOR)
			continue;

		// Store current settings before changing them so we can set them back
		fpkscd.panzerreloadtimems = currentgclient->sess.panzerreloadtimems;
		fpkscd.team = currentgclient->sess.sessionTeam;

		// Now push this player's settings into the vector
		g_FastPanzerKillSpreeClientData.push_back(fpkscd);

		string buffer;

		// If this is our killing spree user, give them fastpanzer and
		// set them on axis team
		if (currentuser.guid == killspreeuser.guid) {

			// Give fast panzer
			//buffer = va("!setvar fastpanzerplayer 20 %d\n", currentgclient->ps.clientNum);
			//trap_SendConsoleCommand( EXEC_NOW, buffer.c_str() );
			currentgclient->sess.panzerreloadtimems = 200;


			// Set on axis
			// Since this is the kill spree player, we want to send a 4th argument to the
			// !putteam command.  It doesn't matter what it is at this point so we'll just
			// specify the clientNum again.  This is to indicate to !putteam that after the
			// player has been moved to the appropriate team, they should set the flag to
			// indicate that fastpanzerkillspree has begun.
			// UPDATE: We no longer change the team of the killspree player so this isn't necessary
			//buffer = va("!putteam %d r %d\n", currentgclient->ps.clientNum, currentgclient->ps.clientNum);
			//trap_SendConsoleCommand( EXEC_APPEND, buffer.c_str() );

			// Now continue through the list
			continue;
		}

		// If this is not the killing spree user, reset their killspreekills count in case
		// they're already on opposite team as it won't trigger a death and so they could
		// get a kill spree on the next kill.  We'll restore it once killing spree has stopped
		currentgclient->pers.killspreekills = 0;
		currentgclient->pers.fastpanzerkillspreekills = 0;


		// If this is not the killing spree user, set them on opposite team of killspree player
		// and set fastpanzer to 0

		// Remove fast panzer
		//buffer = va("!setvar fastpanzerplayer 0 %d\n", currentgclient->ps.clientNum);
		//trap_SendConsoleCommand( EXEC_NOW, buffer.c_str() );
		currentgclient->sess.panzerreloadtimems = 2000;


		// Set on opposite team of killspree player

		// (blue) b=allies, (red) r=axis
		string steam = "b";
		if (otherplayersteam == TEAM_AXIS)
			steam = "r";

		if (otherplayersteam != currentgclient->sess.sessionTeam) {
			if (!SetTeam( currentgentity, steam.c_str(), qtrue, (weapon_t)-1, (weapon_t)-1, qfalse )) {
				// Failed to move player to other team. Going to keep trying with other players
				// though and ignore this player
			}
		}
	}

}

void G_StopFastPanzerKillSpree()
{
	g_fastpanzerkillspreeon = false;
	g_fastpanzerkillspreeclientnum = 0;
	string sconnectedusers;
	
	// Loop through our vector of stored fastpanzerkillspreeclientdata
	for (int i = 0; i < g_FastPanzerKillSpreeClientData.size(); i++) {
		// Loop through our list of connected clients looking for the current
		// fastpanzerkillspreeclientdata matching guid
		for (int j = 0; j < MAX_CLIENTS; j++) {

			// If we've already found this connected user, move to next
			if (sconnectedusers.find(va(",%i,", j)) != -1)
				continue;

			User& currentuser = *connectedUsers[j];
			if (currentuser == User::BAD)
				continue;

			int playerslot = lookupplayerslot( va("%d", j) );
			if (playerslot == -1) // client match not found, continue
				continue;

			Client* currentclient = &g_clientObjects[playerslot];
			gclient_t* const currentgclient = &currentclient->gclient;
			gentity_t* const currentgentity = &currentclient->gentity;

			string buffer;

			// If the user moved to spectator during the kill spree, we still
			// want to restore their settings, we just don't want to move them
			// to a team, so set a flag to determine this later
			bool bSpec = (currentgclient->sess.sessionTeam == TEAM_SPECTATOR);


			if (g_FastPanzerKillSpreeClientData[i].guid == currentuser.guid)
			{
				// Found our user so reset their settings and store in our string

				sconnectedusers += va(",%i,", j);

				// Restore fastpanzer setting
				//buffer = va("!setvar fastpanzerplayer %d %d\n", g_FastPanzerKillSpreeClientData[j].deathsforpanzerreload, currentgclient->ps.clientNum);
				//trap_SendConsoleCommand( EXEC_NOW, buffer.c_str() );
				currentgclient->sess.panzerreloadtimems = g_FastPanzerKillSpreeClientData[i].panzerreloadtimems;


				string steam = "b";
				if (g_FastPanzerKillSpreeClientData[i].team == TEAM_AXIS)
					steam = "r";

				// Only run if the user has not switched to spectator
				if (!bSpec) {

					// UPDATE: Because death is handled in event, switching immediately causes a
					// false teamkill message.  But there are other issues of counts not being
					// transferred back to players correctly.  So for now we're going to accept
					// the false teamkill message and see if this fixes the other issues as the
					// teamkill problem isn't that big a deal.
					if (g_FastPanzerKillSpreeClientData[i].team != currentgclient->sess.sessionTeam) {
						if (!SetTeam( currentgentity, steam.c_str(), qtrue, (weapon_t)-1, (weapon_t)-1, qfalse )) {
							 //Failed to move player to other team. Going to keep trying with other players
							 //though and ignore this player
						}
					}

					// Unfortunately, because a death is registered in an event, if we directly
					// set players back to their teams it happens faster than the death event
					// can finish - so we end up sometimes with an invalid team kill registered.
					// In order to fix this, we're simply using the !PutTeam cmd to put players
					// back on original teams as this uses events to fire so it will ensure it
					// happens after the death event has completed.
					//if (g_FastPanzerKillSpreeClientData[i].team != currentgclient->sess.sessionTeam) {
					//	buffer = va("!putteam %d %s\n", currentgclient->ps.clientNum, steam.c_str());
					//	trap_SendConsoleCommand( EXEC_APPEND, buffer.c_str() );
					//}
				}

				// Now restore the users killspreekills count.  This MUST be done after
				// moving them to their original team because a respawn causes this
				// count to get reset to 0.
				currentgclient->pers.killspreekills = g_FastPanzerKillSpreeClientData[i].killspreekills;
				currentgclient->pers.fastpanzerkillspreekills = g_FastPanzerKillSpreeClientData[i].fastpanzerkillspreekills;

				// Break out of for loop since we found our user and restored their settings
				break;
			}
		}
	}

	// Now that we've moved all players/bots back to original teams, re-enable
	// bot setting for balanced teams
	trap_SendConsoleCommand( EXEC_APPEND, "bot balanceteams 1\n" );
}


// Basically brought this over from AbstractCommand.cpp
// but switched to return slot number.  Returns slot number
// or -1 if not found.
int lookupplayerslot( const string& name )
{
	string err; // Not being used currently except in here

	if (name.empty()) {
        err = "is empty";
        return -1;
    }

    // handle slot mode
    {
        bool slotmode = true;
        const string::size_type max = name.length();
        if (max > 2) {
            slotmode = false;
        }
        else {
            for ( string::size_type i = 0; i < max; i++ ) {
                const char c = name[i];
                if (c < '0' || c > '9') {
                    slotmode = false;
                    break;
                }
            }
        }

        if (slotmode) {
            const int slot = atoi( name.c_str() );
            if (slot < 0) {
                err = "minimum is 0";
                return -1;
            }
            else if (slot >= MAX_CLIENTS) {
                err = "maximum is 63";
                return -1;
            }

            Client& client = g_clientObjects[slot];
            if (client.gclient.pers.connected != CON_CONNECTED) {
                err = "not connected";
                return -1;
            }

			return slot;
        }
    }

	vector<Client*> clients;
    clients.clear();

    string lname = name;
    str::toLower( lname );

	int slot = -1;

    // search connected users
    for (int i = 0; i < MAX_CLIENTS; i++) {
        Client& client = g_clientObjects[i];
        if (client.gclient.pers.connected != CON_CONNECTED)
            continue;

        string cname = connectedUsers[i]->name;
        str::toLower( cname );
        if (cname.find( lname ) != string::npos)
		{
			if (slot != -1) {
				// We already found one, so there's multiple matches, return -1
				err = "multiple matches";
				return -1;
			}
			else { 
				// Found one so set slot as the current slot value and keep searching
				// in case we find multiple matches (which would be an error)
				slot = i;
			}
		}
    }

    if (slot == -1) {
        err = "not found";
        return -1;
    }

    return slot;
}



