#include <bgame/impl.h>

namespace cmd {

///////////////////////////////////////////////////////////////////////////////

FastPanzerKillSpree::FastPanzerKillSpree()
    : AbstractBuiltin( "fastpanzerkillspree" )
{
    __usage << xvalue( "!" + _name ) << " <start|stop> " << xvalue( "PLAYER" );
    __descr << "Mainly server call only as it sets player up against all other players with fast panzer.";
}

///////////////////////////////////////////////////////////////////////////////

FastPanzerKillSpree::~FastPanzerKillSpree()
{
}

///////////////////////////////////////////////////////////////////////////////

AbstractCommand::PostAction
FastPanzerKillSpree::doExecute( Context& txt )
{
    if (txt._args.size() > 3 || txt._args.size() < 2)
        return PA_USAGE;

    // bail if nothing to display
    if (level.numConnectedClients < 1) {
        txt._ebuf << "There are no players online.";
        return PA_ERROR;
    }

    string cmd = txt._args[1]; // should be on or off
    str::toLower( cmd );

	if (cmd == "start") 
	{
		// Set bots to not balance teams as we're going to be moving them if there are any
		// Append seems to not execute fast enough, setting to NOW which is never a good
		trap_SendConsoleCommand( EXEC_NOW, "bot balanceteams 0\n" );


		// Get the user we're setting up for fast panzer killing spree
		Client* killspreeclient;
		if (lookupPLAYER( txt._args[2], txt, killspreeclient ))
			return PA_ERROR;

		if (isHigherLevelError( *killspreeclient, txt ))
			return PA_ERROR;

		if (isNotOnTeamError( *killspreeclient, txt ))
			return PA_ERROR;

		const User& killspreeUser = *connectedUsers[killspreeclient->slot];

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
		g_fastpanzerkillspreeclientnum = killspreeclient->gclient.ps.clientNum;
		
		// We can now turn flag on here based on update above
		g_fastpanzerkillspreeon = true;

		team_t killspreeplayerteam = killspreeclient->gclient.sess.sessionTeam;

		// Set other players to axis.  If the killspreeplayer is axis, then change
		// it to allies.
		team_t otherplayersteam = TEAM_AXIS;
		if (killspreeplayerteam == TEAM_AXIS)
			otherplayersteam = TEAM_ALLIES;


		for (int i = 0; i < MAX_CLIENTS; i++) {
			User& currentuser = *connectedUsers[i];
			if (currentuser == User::BAD)
				continue;
			
			Client* currentclient;
			string iuser = va("%d",i);
			if (lookupPLAYER( iuser, txt, currentclient ))
				continue;

			g_FastPanzerKillSpreeClientData[i].guid = currentuser.guid;
			g_FastPanzerKillSpreeClientData[i].name = currentuser.name;

			gclient_t* const currentgclient = &currentclient->gclient;

			g_FastPanzerKillSpreeClientData[i].killspreekills = currentgclient->pers.killspreekills;

			// If user is in spectator mode, simply ignore their settings
			// because we don't need to make any adjustments for them
			// since we're not changing any of their settings now
			if (currentgclient->sess.sessionTeam == TEAM_SPECTATOR)
				continue;

			// Store current settings before changing them so we can set them back
			g_FastPanzerKillSpreeClientData[i].deathsforpanzerreload = currentgclient->sess.deathsforpanzerreload;
			g_FastPanzerKillSpreeClientData[i].team = currentgclient->sess.sessionTeam;

			string buffer;

			// If this is our killing spree user, give them fastpanzer and
			// set them on axis team
			if (currentuser.guid == killspreeUser.guid) {

				// Give fast panzer
				buffer = va("!setvar fastpanzerplayer 50 %d\n", currentgclient->ps.clientNum);
				trap_SendConsoleCommand( EXEC_APPEND, buffer.c_str() );

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


			// If this is not the killing spree user, set them on opposite team of killspree player
			// and set fastpanzer to 0

			// Remove fast panzer
			buffer = va("!setvar fastpanzerplayer 0 %d\n", currentgclient->ps.clientNum);
			trap_SendConsoleCommand( EXEC_APPEND, buffer.c_str() );


			// Set on opposite team of killspree player

			// (blue) b=allies, (red) r=axis
			if (otherplayersteam == TEAM_ALLIES)
				buffer = va("!putteam %d b\n", currentgclient->ps.clientNum);
			else
				buffer = va("!putteam %d r\n", currentgclient->ps.clientNum);
			trap_SendConsoleCommand( EXEC_APPEND, buffer.c_str() );
		}

		//Buffer buf;
		//print( txt._client, buf );
	}
	else if (cmd == "stop") 
	{
		g_fastpanzerkillspreeon = false;
		g_fastpanzerkillspreeclientnum = 0;

		for (int i = 0; i < MAX_CLIENTS; i++) {
			User& currentuser = *connectedUsers[i];
			if (currentuser == User::BAD)
				continue;
			
			Client* currentclient;
			string iuser = va("%d",i);
			if (lookupPLAYER( iuser, txt, currentclient ))
				continue;

			gclient_t* const currentgclient = &currentclient->gclient;

			// If user is in spectator mode, simply ignore their settings
			// because we don't need to make any adjustments for them
			// since we're not changing any of their settings now
			if (currentgclient->sess.sessionTeam == TEAM_SPECTATOR)
				continue;

			string buffer;

			for (int j = 0; j < MAX_CLIENTS; j++) {
				if (g_FastPanzerKillSpreeClientData[j].guid == currentuser.guid)
				{
					// Found our user so reset their settings

					// Restore fastpanzer setting
					buffer = va("!setvar fastpanzerplayer %d %d\n", g_FastPanzerKillSpreeClientData[j].deathsforpanzerreload, currentgclient->ps.clientNum);
					trap_SendConsoleCommand( EXEC_APPEND, buffer.c_str() );

					// Restore team
					if (g_FastPanzerKillSpreeClientData[j].team == TEAM_AXIS)
						buffer = va("!putteam %d r\n", currentgclient->ps.clientNum);
					else
						buffer = va("!putteam %d b\n", currentgclient->ps.clientNum);
					trap_SendConsoleCommand( EXEC_APPEND, buffer.c_str() );

					// Now restore the users killspreekills count.  This MUST be done after
					// moving them to their original team because a respawn causes this
					// count to get reset to 0.
					currentgclient->pers.killspreekills = g_FastPanzerKillSpreeClientData[j].killspreekills;

					// Break out of for loop since we found our user and restored their settings
					break;
				}
			}
		}

		// Now that we've moved all players/bots back to original teams, re-enable
		// bot setting for balanced teams
		trap_SendConsoleCommand( EXEC_APPEND, "bot balanceteams 1\n" );
	}

    return PA_NONE;
}

///////////////////////////////////////////////////////////////////////////////

} // namespace cmd
