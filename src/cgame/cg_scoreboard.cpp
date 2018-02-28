// cg_scoreboard -- draw the scoreboard on top of the game screen

#include <bgame/impl.h> 

#define	SCOREBOARD_WIDTH	(31*BIGCHAR_WIDTH)

vec4_t clrUiBack = { 0.f, 0.f, 0.f, .6f };
vec4_t clrUiBar = { .16f, .2f, .17f, .8f };

/*
=================
WM_DrawObjectives
=================
*/

#define INFO_PLAYER_WIDTH		134
#define INFO_SCORE_WIDTH		64
#define INFO_KD_WIDTH			60 // AndyStutz - Kills/Deaths width
#define INFO_XP_WIDTH			44
#define INFO_CLASS_WIDTH		42
#define INFO_LATENCY_WIDTH		40
#define INFO_LIVES_WIDTH		20
#define INFO_TEAM_HEIGHT		24
#define INFO_BORDER				2
#define INFO_LINE_HEIGHT		30
//#define INFO_TOTAL_WIDTH		(INFO_PLAYER_WIDTH + INFO_CLASS_WIDTH + INFO_SCORE_WIDTH + INFO_LATENCY_WIDTH)
#define INFO_TOTAL_WIDTH		(INFO_PLAYER_WIDTH + INFO_KD_WIDTH + INFO_SCORE_WIDTH + INFO_LATENCY_WIDTH)

int WM_DrawObjectives( int x, int y, int width, float fade ) {
	const char *s, *str;
	int tempy, rows;
	int msec, mins, seconds, tens; // JPW NERVE
	vec4_t tclr =	{ 0.6f,		0.6f,		0.6f,		1.0f };

	if ( cg.snap->ps.pm_type == PM_INTERMISSION ) {
		const char *s, *buf, *shader = NULL, *flagshader = NULL, *nameshader = NULL;

		// Moved to CG_DrawIntermission
/*		static int doScreenshot = 0, doDemostop = 0;

		// OSP - End-of-level autoactions
		if(!cg.demoPlayback) {
			if(!cg.latchVictorySound) {
				if(cg_autoAction.integer & AA_SCREENSHOT) {
					doScreenshot = cg.time + 1000;
				}
				if(cg_autoAction.integer & AA_STATSDUMP) {
					CG_dumpStats_f();
				}
				if((cg_autoAction.integer & AA_DEMORECORD) && (cgs.gametype == GT_WOLF_STOPWATCH && cgs.currentRound != 1)) {
					doDemostop = cg.time + 5000;	// stats should show up within 5 seconds
				}
			}
			if(doScreenshot > 0 && doScreenshot < cg.time) {
				CG_autoScreenShot_f();
				doScreenshot = 0;
			}
			if(doDemostop > 0 && doDemostop < cg.time) {
				trap_SendConsoleCommand("stoprecord\n");
				doDemostop = 0;
			}
		}
*/
		rows = 8;
		y += SMALLCHAR_HEIGHT * ( rows - 1 );

		s = CG_ConfigString( CS_MULTI_MAPWINNER );
		buf = Info_ValueForKey( s, "winner" );

		if ( atoi( buf ) == -1 )
			str = "ITS A TIE!";
		else if ( atoi( buf ) ) {
			str = "ALLIES";
//			shader = "ui/assets/portraits/allies_win";
			flagshader = "ui/assets/portraits/allies_win_flag.tga";
			nameshader = "ui/assets/portraits/text_allies.tga";

/*			if ( !cg.latchVictorySound ) {
				cg.latchVictorySound = qtrue;
				trap_S_StartLocalSound( trap_S_RegisterSound( "sound/music/allies_win.wav", qtrue ), CHAN_LOCAL_SOUND );	// FIXME: stream
			}*/
		}
		else {
			str = "AXIS";
//			shader = "ui/assets/portraits/axis_win";
			flagshader = "ui/assets/portraits/axis_win_flag.tga";
			nameshader = "ui/assets/portraits/text_axis.tga";

/*			if ( !cg.latchVictorySound ) {
				cg.latchVictorySound = qtrue;
				trap_S_StartLocalSound( trap_S_RegisterSound( "sound/music/axis_win.wav", qtrue ), CHAN_LOCAL_SOUND );	// FIXME: stream
			}*/
		}

		y += SMALLCHAR_HEIGHT * ( ( rows - 2 ) / 2 );

		if ( flagshader ) {
			CG_DrawPic( 100, 10, 210, 136, trap_R_RegisterShaderNoMip( flagshader ) );
			CG_DrawPic( 325, 10, 210, 136, trap_R_RegisterShaderNoMip( flagshader ) );
		}

		if ( shader )
			CG_DrawPic( 229, 10, 182, 136, trap_R_RegisterShaderNoMip( shader ) );
		if ( nameshader ) {
			CG_DrawPic( 140, 50, 127, 64, trap_R_RegisterShaderNoMip( nameshader ) );
			CG_DrawPic( 365, 50, 127, 64, trap_R_RegisterShaderNoMip( "ui/assets/portraits/text_win.tga" ) );
		}
		return y;
	}
// JPW NERVE -- mission time & reinforce time
	else {
		tempy = y;
		rows = 1;

		CG_FillRect( x-5, y-2, width+5, 21, clrUiBack );
		CG_FillRect( x-5, y-2, width+5, 21, clrUiBar );
		CG_DrawRect_FixedBorder( x-5, y-2, width+5, 21, 1, colorBlack );

		y += SMALLCHAR_HEIGHT * ( rows - 1 );
		if( cgs.timelimit > 0.0f ) {
			msec = int( ( cgs.timelimit * 60.f * 1000.f ) - ( cg.time - cgs.levelStartTime ) );

			seconds = msec / 1000;
			mins = seconds / 60;
			seconds -= mins * 60;
			tens = seconds / 10;
			seconds -= tens * 10;
		} else {
			msec = mins = tens = seconds = 0;
		}

		if( cgs.gamestate != GS_PLAYING ) {
			s = va("%s %s", CG_TranslateString("MISSION TIME:"),  CG_TranslateString("WARMUP"));
		} else if ( msec < 0 && cgs.timelimit > 0.0f ) {
			if ( cgs.gamestate == GS_WAITING_FOR_PLAYERS )
				s = va( "%s %s", CG_TranslateString( "MISSION TIME:" ),  CG_TranslateString( "GAME STOPPED" ) );
			else
				s = va( "%s %s", CG_TranslateString( "MISSION TIME:" ),  CG_TranslateString( "SUDDEN DEATH" ) );
		} else {
			s = va( "%s   %2.0f:%i%i", CG_TranslateString( "MISSION TIME:" ), (float)mins, tens, seconds ); // float cast to line up with reinforce time
		}

		CG_Text_Paint_Ext( x, y + 13, 0.25f, 0.25f, tclr, s, 0, 0, 0, &cgs.media.limboFont1 );

		if( cgs.gametype != GT_WOLF_LMS ) {
			if(cgs.clientinfo[cg.snap->ps.clientNum].team == TEAM_AXIS || cgs.clientinfo[cg.snap->ps.clientNum].team == TEAM_ALLIES) {
				msec = CG_CalculateReinfTime( qfalse ) * 1000;
			}
			else // no team (spectator mode)
				msec = 0;

			if (msec) {
				seconds = msec / 1000;
				mins = seconds / 60;
				seconds -= mins * 60;
				tens = seconds / 10;
				seconds -= tens * 10;

				s = va( "%s %2.0f:%i%i", CG_TranslateString( "REINFORCE TIME:" ), (float)mins, tens, seconds );
				CG_Text_Paint_Ext( 640 - 20 - CG_Text_Width_Ext( s, 0.25f, 0, &cgs.media.limboFont1 ), y + 13, 0.25f, 0.25f, tclr, s, 0, 0, 0, &cgs.media.limboFont1 );
			}
		}

		// NERVE - SMF
		if ( cgs.gametype == GT_WOLF_STOPWATCH ) {
			int w;
			s = va( "%s %i", CG_TranslateString( "STOPWATCH ROUND" ), cgs.currentRound + 1 );

			w = CG_Text_Width_Ext( s, 0.25f, 0, &cgs.media.limboFont1 );

			CG_Text_Paint_Ext( x + 300 - w*0.5f, y + 13, 0.25f, 0.25f, tclr, s, 0, 0, 0, &cgs.media.limboFont1 );
		} else if( cgs.gametype == GT_WOLF_LMS ) {
			int w;
			s = va( "%s %i  %s %i-%i", CG_TranslateString( "ROUND" ), cgs.currentRound + 1, CG_TranslateString( "SCORE" ), cg.teamWonRounds[1], cg.teamWonRounds[0] );
			w = CG_Text_Width_Ext( s, 0.25f, 0, &cgs.media.limboFont1 );

			CG_Text_Paint_Ext( x + 300 - w*0.5f, y + 13, 0.25f, 0.25f, tclr, s, 0, 0, 0, &cgs.media.limboFont1 );
		} else if( cgs.gametype == GT_WOLF_CAMPAIGN ) {
			int w;
			s = va( "MAP %i of %i", cgs.currentCampaignMap + 1, cgs.campaignData.mapCount );
			w = CG_Text_Width_Ext( s, 0.25f, 0, &cgs.media.limboFont1 );

			CG_Text_Paint_Ext( x + 300 - w*0.5f, y + 13, 0.25f, 0.25f, tclr, s, 0, 0, 0, &cgs.media.limboFont1 );
		}
		
		y += SMALLCHAR_HEIGHT * 2;
	}
// jpw

	return y;
}

static void WM_DrawClientScore( int x, int y, score_t *score, float *color, float fade ) {
	int maxchars, offset;
	int i, j;
	float tempx;
	vec4_t hcolor;
	clientInfo_t *ci;
	char buf[64];

	

	if ( y + SMALLCHAR_HEIGHT >= 470 )
		return;

	ci = &cgs.clientinfo[score->client];

    // Highlight background of your slot
	if ( score->client == cg.snap->ps.clientNum ) {
		tempx = x;

		hcolor[3] = fade * 0.3;
		VectorSet( hcolor, .5f, .5f, .2f );			// DARK-RED

        // Player box
		CG_FillRect( tempx - 3, y + 1, INFO_PLAYER_WIDTH - INFO_BORDER + 3, SMALLCHAR_HEIGHT - 1, hcolor );
		tempx += INFO_PLAYER_WIDTH;

		if ( score->ping < 0 || (ci->team == TEAM_SPECTATOR && ci->shoutcaster)) {
            // Connecting or shoutcasters get simpler row
			int width;
			//width = INFO_CLASS_WIDTH + INFO_SCORE_WIDTH + INFO_LATENCY_WIDTH;
			width = INFO_KD_WIDTH + INFO_SCORE_WIDTH + INFO_LATENCY_WIDTH;

			CG_FillRect( tempx, y + 1, width - INFO_BORDER, SMALLCHAR_HEIGHT - 1, hcolor );
			tempx += width;
		} else {
            // Class box
			//CG_FillRect( tempx, y + 1, INFO_CLASS_WIDTH - INFO_BORDER, SMALLCHAR_HEIGHT - 1, hcolor );
			CG_FillRect( tempx, y + 1, INFO_KD_WIDTH - INFO_BORDER, SMALLCHAR_HEIGHT - 1, hcolor );
			//tempx += INFO_CLASS_WIDTH;
			tempx += INFO_KD_WIDTH;

			if( cg_gameType.integer == GT_WOLF_LMS ) {
                // LMS gets score
				CG_FillRect( tempx, y + 1, INFO_SCORE_WIDTH - INFO_BORDER, SMALLCHAR_HEIGHT - 1, hcolor );
				tempx += INFO_SCORE_WIDTH;
			} else {
                // XP Box
				CG_FillRect( tempx, y + 1, INFO_XP_WIDTH - INFO_BORDER, SMALLCHAR_HEIGHT - 1, hcolor );
				tempx += INFO_XP_WIDTH;				
			}

            // Ping
			CG_FillRect( tempx, y + 1, INFO_LATENCY_WIDTH - INFO_BORDER, SMALLCHAR_HEIGHT - 1, hcolor );
			tempx += INFO_LATENCY_WIDTH;

			if( cg_gameType.integer != GT_WOLF_LMS ) {
                // Lives box
				CG_FillRect( tempx, y + 1, INFO_LIVES_WIDTH - INFO_BORDER, SMALLCHAR_HEIGHT - 1, hcolor );
				tempx += INFO_LIVES_WIDTH;
			}
		}
	}

	tempx = x;

	// DHM - Nerve
	VectorSet( hcolor, 1, 1, 1 );
	hcolor[3] = fade;

	maxchars = 16;
	offset = 0;

    // Icons
	if ( ci->team != TEAM_SPECTATOR ) {
        // Have the objective
		if ( ci->powerups & ( (1 << PW_REDFLAG) | (1 << PW_BLUEFLAG) ) ) {
			// CHRUKER: b078 - Medic, death and objective icons on the scoreboard are drawn too big
			CG_DrawPic( tempx - 3, y + 1, 14, 14, cgs.media.objectiveShader );
			offset += 14;	// CHRUKER: b072 - Need to match tempx or else the other text gets offset
			tempx += 14;
			maxchars -= 2;
		}

		// Uniformed (don't shoot!)
		else if( cgs.clientinfo[cg.clientNum].team != TEAM_SPECTATOR && ci->team == cgs.clientinfo[cg.clientNum].team && ci->powerups & ((1 << PW_OPS_DISGUISED))) { 
			CG_DrawPic( tempx -3, y + 1, 14, 14, cgs.media.friendShader ); 
			offset += 14; 
			tempx += 14; 
			maxchars -= 2; 
		} 

		// Dead
		else if( score->respawnsLeft == -2 || (cgs.clientinfo[cg.clientNum].team != TEAM_SPECTATOR && ci->team == cgs.clientinfo[cg.clientNum].team && cgs.clientinfo[score->client].health == -1 ) ) {
			// CHRUKER: b078 - Medic, death and objective icons on the scoreboard are drawn too big
			CG_DrawPic( tempx - 3, y + 1, 14, 14, cgs.media.scoreEliminatedShader );
			offset += 14;
			tempx += 14;
			maxchars -= 2;

        // Need revive
		} else if( cgs.clientinfo[cg.clientNum].team != TEAM_SPECTATOR && ci->team == cgs.clientinfo[cg.clientNum].team && cgs.clientinfo[score->client].health == 0 ) {
			// CHRUKER: b078 - Medic, death and objective icons on the scoreboard are drawn too big
			CG_DrawPic( tempx - 3, y + 1, 14, 14, cgs.media.medicIcon );
			offset += 14;
			tempx += 14;
			maxchars -= 2;			
		}

        // Jaybird - muted icon
        else if (ci->muted) {
            CG_DrawPic( tempx - 3, y + 1, 14, 14, cgs.media.mutedShader );
            offset += 14;
            tempx += 14;
            maxchars -= 2;
        }
	}

	// Draw name
	CG_DrawStringExt( int(tempx), y, ci->name, hcolor, qfalse, qfalse, SMALLCHAR_WIDTH, SMALLCHAR_HEIGHT, maxchars );
	maxchars -= CG_DrawStrlen( ci->name );

	// Draw medals
	buf[0] = '\0';
	for( i = 0; i < SK_NUM_SKILLS; i++ ) {
		for( j = 0; j < ci->medals[i]; j++ )
			Q_strcat( buf, sizeof(buf), va( "^%c%c", COLOR_RED + i, skillNames[i][0] ) );
	}
	maxchars--;
	// CHRUKER: b032 - Medals clipped wrong in scoreboard when you're dead, because CG_DrawStringExt will draw
	// everything if maxchars <= 0
	if (maxchars > 0)
		CG_DrawStringExt( int(tempx + (BG_drawStrlen(ci->name) * SMALLCHAR_WIDTH + SMALLCHAR_WIDTH)), y, buf, hcolor, qfalse, qfalse, SMALLCHAR_WIDTH, SMALLCHAR_HEIGHT, maxchars );

	tempx += INFO_PLAYER_WIDTH - offset;

	if ( score->ping < 0 || (ci->team == TEAM_SPECTATOR && ci->shoutcaster)) {
        // Simpler box for connecting and shoutcasters
		const char *s;
		int w, totalwidth;

		//totalwidth = INFO_CLASS_WIDTH + INFO_SCORE_WIDTH + INFO_LATENCY_WIDTH - 8;
		totalwidth = INFO_KD_WIDTH + INFO_SCORE_WIDTH + INFO_LATENCY_WIDTH - 8;

		 // CHRUKER: b031 - Show connecting people as connecting
		s = CG_TranslateString( (ci->team == TEAM_SPECTATOR && ci->shoutcaster)?"^3SHOUTCASTER":"^2CONNECTING" );
		w = CG_DrawStrlen( s ) * SMALLCHAR_WIDTH;

		CG_DrawSmallString( int(tempx + totalwidth - w), y, s, fade );
		return;
	}
	else if ( ci->team == TEAM_SPECTATOR) {
        // Spectator for class
		CG_DrawSmallString( int(tempx), y, CG_TranslateString("^3SPEC"), fade);
	}
	// AndyStutz - Want to draw kills/deaths for both teams, not just the current clients team
	//else if ( cg.snap->ps.persistant[PERS_TEAM] == ci->team || CG_mvMergedClientLocate(score->client) ) {
	else {
        // Draw class
		//CG_DrawSmallString( int(tempx), y, CG_TranslateString( BG_ShortClassnameForNumber( score->playerClass ) ), fade );
		// AndyStutz - changing class name to kills/deaths
		CG_DrawSmallString( int(tempx), y, CG_TranslateString(va( "^2%3i^7/^1%i", score->kills, score->deaths )), fade );
	}
	//tempx += INFO_CLASS_WIDTH;
	tempx += INFO_KD_WIDTH;

    // XP
	//CG_DrawSmallString( int(tempx), y, va( "%5i", score->score ), fade );
	CG_DrawSmallString( int(tempx + 1 * SMALLCHAR_WIDTH), y, va( "%i", score->score ), fade );
	if( cg_gameType.integer == GT_WOLF_LMS ) {
		tempx += INFO_SCORE_WIDTH;
	} else {
		tempx += INFO_XP_WIDTH;
	}

    // Ping
	if (score->ping == 0)
		CG_DrawSmallString( int(tempx), y, va( "%4s", "Bot" ), fade );
	else
		CG_DrawSmallString( int(tempx), y, va( "%4i", score->ping ), fade );
	tempx += INFO_LATENCY_WIDTH;

    // Lives left
	if( cg_gameType.integer != GT_WOLF_LMS ) {
		if( score->respawnsLeft >= 0 && cg_gameType.integer != GT_WOLF_CAMPAIGN) {
			CG_DrawSmallString( int(tempx), y, va( "%2i", score->respawnsLeft ), fade );
		} else {
			CG_DrawSmallString( int(tempx), y, " -", fade );
		}
		tempx += INFO_LIVES_WIDTH;
	}
}

const char* WM_TimeToString( float msec ) {
	int mins, seconds, tens;

	seconds = int(msec / 1000);
	mins = seconds / 60;
	seconds -= mins * 60;
	tens = seconds / 10;
	seconds -= tens * 10;

	return va( "%i:%i%i", mins, tens, seconds );
}

static void WM_DrawClientScore_Small( int x, int y, score_t *score, float *color, float fade ) {
	int maxchars, offset;
	float tempx;
	vec4_t hcolor;
	clientInfo_t *ci;

	// CHRUKER: b033 - Added to draw medals
	int i, j;
	char buf[64];

	// CHRUKER: b0?? - Was using the wrong char height for this calculation
	if ( y + MINICHAR_HEIGHT >= 470 )
		return;

	ci = &cgs.clientinfo[score->client];

	if ( score->client == cg.snap->ps.clientNum ) {
		tempx = x;

		hcolor[3] = fade * 0.3;
		VectorSet( hcolor, .5f, .5f, .2f );			// DARK-RED

		CG_FillRect( tempx - 3, y + 1, INFO_PLAYER_WIDTH - INFO_BORDER + 3, MINICHAR_HEIGHT - 1, hcolor );
		tempx += INFO_PLAYER_WIDTH;

		if ( score->ping < 0 || (ci->team == TEAM_SPECTATOR && ci->shoutcaster)) {
			int width;
			//width = INFO_CLASS_WIDTH + INFO_SCORE_WIDTH + INFO_LATENCY_WIDTH;
			width = INFO_KD_WIDTH+ INFO_SCORE_WIDTH + INFO_LATENCY_WIDTH;

			CG_FillRect( tempx, y + 1, width - INFO_BORDER, MINICHAR_HEIGHT - 1, hcolor );
			tempx += width;
		} else {
			//CG_FillRect( tempx, y + 1, INFO_CLASS_WIDTH - INFO_BORDER, MINICHAR_HEIGHT - 1, hcolor );
			CG_FillRect( tempx, y + 1, INFO_KD_WIDTH - INFO_BORDER, MINICHAR_HEIGHT - 1, hcolor );
			//tempx += INFO_CLASS_WIDTH;
			tempx += INFO_KD_WIDTH;

			if( cg_gameType.integer == GT_WOLF_LMS ) {
				CG_FillRect( tempx, y + 1, INFO_SCORE_WIDTH - INFO_BORDER, MINICHAR_HEIGHT - 1, hcolor );
				tempx += INFO_SCORE_WIDTH;
			} else {
				CG_FillRect( tempx, y + 1, INFO_XP_WIDTH - INFO_BORDER, MINICHAR_HEIGHT - 1, hcolor );
				tempx += INFO_XP_WIDTH;				
			}

			CG_FillRect( tempx, y + 1, INFO_LATENCY_WIDTH - INFO_BORDER, MINICHAR_HEIGHT - 1, hcolor );
			tempx += INFO_LATENCY_WIDTH;

			if( cg_gameType.integer != GT_WOLF_LMS ) {
				CG_FillRect( tempx, y + 1, INFO_LIVES_WIDTH - INFO_BORDER, MINICHAR_HEIGHT - 1, hcolor );
				tempx += INFO_LIVES_WIDTH;
			}
		}
	}

	tempx = x;

	// DHM - Nerve
	VectorSet( hcolor, 1, 1, 1 );
	hcolor[3] = fade;

	// CHRUKER: b033 - Corrected to draw medals
	maxchars = 16;
	offset = 0;

	if ( ci->team != TEAM_SPECTATOR ) {
        // Has the objective
		if ( ci->powerups & ( (1 << PW_REDFLAG) | (1 << PW_BLUEFLAG) ) ) {
			// CHRUKER: b071 - Objective carrier icon missing on compact scoreboard
			CG_DrawPic( tempx + 1, y + 1, 10, 10, cgs.media.objectiveShader );
			offset += 14;
			tempx += 14;
			maxchars -= 2;
		}

		// forty - draw no shoot for cov-ops 
		else if( cgs.clientinfo[cg.clientNum].team != TEAM_SPECTATOR && ci->team == cgs.clientinfo[cg.clientNum].team && ci->powerups & ((1 << PW_OPS_DISGUISED))) { 
			// Jaybird - size used to be 20/20
			CG_DrawPic( tempx + 1, y + 1, 10, 10, cgs.media.friendShader ); 
			offset += 14; 
			tempx += 14; 
			maxchars -= 2;
		}

		// draw the skull icon if out of lives
		else if ( score->respawnsLeft == -2 || ( cgs.clientinfo[cg.clientNum].team != TEAM_SPECTATOR && ci->team == cgs.clientinfo[cg.clientNum].team && cgs.clientinfo[score->client].health == -1 ) ) {
			CG_DrawPic( tempx, y, 12, 12, cgs.media.scoreEliminatedShader );
			offset += 14;
			tempx += 14;
			maxchars -= 2;
		}
        
        // Medic icon
        else if( cgs.clientinfo[cg.clientNum].team != TEAM_SPECTATOR && ci->team == cgs.clientinfo[cg.clientNum].team && cgs.clientinfo[score->client].health == 0 ) {
			CG_DrawPic( tempx + 1, y + 1, 10, 10, cgs.media.medicIcon );
			offset += 14;
			tempx += 14;
			maxchars -= 2;
		}

        // Muted icon
        else if (ci->muted) {
            CG_DrawPic( tempx + 1, y + 1, 10, 10, cgs.media.mutedShader );
            offset += 14;
            tempx += 14;
            maxchars -= 2;
        }
	}

	// draw name
	CG_DrawStringExt( int(tempx), y, ci->name, hcolor, qfalse, qfalse, MINICHAR_WIDTH, MINICHAR_HEIGHT, maxchars );

	// CHRUKER: b033 - Added to draw medals
	maxchars -= CG_DrawStrlen( ci->name );
	
	buf[0] = '\0';
	for( i = 0; i < SK_NUM_SKILLS; i++ ) {
		for( j = 0; j < ci->medals[i]; j++ )
			Q_strcat( buf, sizeof(buf), va( "^%c%c", COLOR_RED + i, skillNames[i][0] ) );
	}
	maxchars--;
	
	if (maxchars > 0)
		CG_DrawStringExt( int(tempx + (BG_drawStrlen(ci->name) * MINICHAR_WIDTH + MINICHAR_WIDTH)), y, buf, hcolor, qfalse, qfalse, MINICHAR_WIDTH, MINICHAR_HEIGHT, maxchars );
	// b033

	// Jaybird
	hcolor[0] = hcolor[1] = hcolor[2] = 1;

	tempx += INFO_PLAYER_WIDTH - offset;
	// dhm - nerve

	if ( score->ping < 0 || (ci->team == TEAM_SPECTATOR && ci->shoutcaster)) {
		const char *s;
		int w, totalwidth;

		//totalwidth = INFO_CLASS_WIDTH + INFO_SCORE_WIDTH + INFO_LATENCY_WIDTH - 8;
		totalwidth = INFO_KD_WIDTH + INFO_SCORE_WIDTH + INFO_LATENCY_WIDTH - 8;

		s = CG_TranslateString( (ci->team == TEAM_SPECTATOR && ci->shoutcaster)?"^3SHOUTCASTER":"^2CONNECTING" );
		w = CG_DrawStrlen( s ) * MINICHAR_WIDTH;

		// CHRUKER: b034 - Using the mini char height
		CG_DrawStringExt( int(tempx + totalwidth - w), y, s, hcolor, qfalse, qfalse, MINICHAR_WIDTH, MINICHAR_HEIGHT, 0 );

		return;
	}
	else if ( ci->team == TEAM_SPECTATOR) {
		CG_DrawStringExt( int(tempx), y, CG_TranslateString("^3SPEC"), hcolor, qfalse, qfalse, MINICHAR_WIDTH, MINICHAR_HEIGHT, 0);
	}
	// AndyStutz - Want to draw kills/deaths for both teams, not just the current clients team
	//else if ( cg.snap->ps.persistant[PERS_TEAM] == ci->team ) {
	else {
		//CG_DrawStringExt(	int(tempx), y, CG_TranslateString( BG_ShortClassnameForNumber( score->playerClass ) ), hcolor, qfalse, qfalse, MINICHAR_WIDTH, MINICHAR_HEIGHT, 0 );
//		CG_DrawSmallString( tempx, y, CG_TranslateString( s ), fade );
		// AndyStutz - changing class name to kills/deaths
		CG_DrawStringExt( int(tempx), y, CG_TranslateString(va( "^2%3i^7/^1%i", score->kills, score->deaths )), hcolor, qfalse, qfalse, MINICHAR_WIDTH, MINICHAR_HEIGHT, 0);
		//CG_DrawStringExt( int(tempx), y, va( "^2%2i^7/^1%i", score->kills, score->deaths ), hcolor, qfalse, qfalse, MINICHAR_WIDTH, MINICHAR_HEIGHT, 0 );
	}
	//tempx += INFO_CLASS_WIDTH;
	tempx += INFO_KD_WIDTH;

	//CG_DrawStringExt( int(tempx), y, va( "%3i", score->score ), hcolor, qfalse, qfalse, MINICHAR_WIDTH, MINICHAR_HEIGHT, 0 );
	CG_DrawStringExt( int(tempx + 1 * SMALLCHAR_WIDTH), y, va( "%i", score->score ), hcolor, qfalse, qfalse, MINICHAR_WIDTH, MINICHAR_HEIGHT, 0 );
	if( cg_gameType.integer == GT_WOLF_LMS ) {
		tempx += INFO_SCORE_WIDTH;
	} else {
		tempx += INFO_XP_WIDTH;
	}

	if (score->ping == 0)
		CG_DrawStringExt( int(tempx), y, va( "%4s", "Bot" ), hcolor, qfalse, qfalse, MINICHAR_WIDTH, MINICHAR_HEIGHT, 0 );
	else
		CG_DrawStringExt( int(tempx), y, va( "%4i", score->ping ), hcolor, qfalse, qfalse, MINICHAR_WIDTH, MINICHAR_HEIGHT, 0 );
	tempx += INFO_LATENCY_WIDTH;

	if( cg_gameType.integer != GT_WOLF_LMS ) {
		if( score->respawnsLeft >= 0 ) {
			CG_DrawStringExt( int(tempx), y, va( "%2i", score->respawnsLeft ), hcolor, qfalse, qfalse, MINICHAR_WIDTH, MINICHAR_HEIGHT, 0 );
		} else {
			CG_DrawStringExt( int(tempx), y, " -", hcolor, qfalse, qfalse, MINICHAR_WIDTH, MINICHAR_HEIGHT, 0 );
		}
		tempx += INFO_LIVES_WIDTH;
	}
}

static int WM_DrawInfoLine( int x, int y, float fade ) {
	int w, defender, winner;
	const char *s;
	vec4_t tclr =	{ 0.6f,		0.6f,		0.6f,		1.0f };

	if ( cg.snap->ps.pm_type != PM_INTERMISSION ) {
		return y;
	}

	w = 360;
//	CG_DrawPic( 320 - w/2, y, w, INFO_LINE_HEIGHT, trap_R_RegisterShaderNoMip( "ui/assets/mp_line_strip.tga" ) );

	s = CG_ConfigString( CS_MULTI_INFO );
	defender = atoi( Info_ValueForKey( s, "defender" ) );

	s = CG_ConfigString( CS_MULTI_MAPWINNER );
	winner = atoi( Info_ValueForKey( s, "winner" ) );

	if ( cgs.currentRound ) {
		// first round
		s = va( CG_TranslateString( "CLOCK IS NOW SET TO %s!" ), WM_TimeToString( cgs.nextTimeLimit * 60.f * 1000.f ) );
	}
	else {
		// second round
		if ( !defender ) {
			if ( winner != defender )
				s = "ALLIES SUCCESSFULLY BEAT THE CLOCK!";
			else
				s = "ALLIES COULDN'T BEAT THE CLOCK!";
		}
		else {
			if ( winner != defender )
				s = "AXIS SUCCESSFULLY BEAT THE CLOCK!";
			else
				s = "AXIS COULDN'T BEAT THE CLOCK!";
		}

		s = CG_TranslateString( s );
	}

	CG_FillRect( 320 - w/2, y, w, 20, clrUiBar );
	CG_DrawRect_FixedBorder( 320 - w/2, y, w, 20, 1, colorBlack );

	w = CG_Text_Width_Ext( s, 0.25f, 0, &cgs.media.limboFont1 );

	CG_Text_Paint_Ext( 320 - w*0.5f, y + 15, 0.25f, 0.25f, tclr, s, 0, 0, 0, &cgs.media.limboFont1 );
//	CG_DrawSmallString( 320 - w/2, ( y + INFO_LINE_HEIGHT / 2 ) - SMALLCHAR_HEIGHT / 2, s, fade );
	return y + INFO_LINE_HEIGHT + 6;
}

// CHRUKER: b035 - Added absolute maximum rows
static int WM_TeamScoreboard( int x, int y, team_t team, float fade, int maxrows, int absmaxrows ) {
	vec4_t hcolor;
	float tempx, tempy;
	int height, width;
	int i;
	int count = 0;
	qboolean use_mini_chars = qfalse; // CHRUKER: b035 - Needed to check if using mini chars
	vec4_t tclr =	{ 0.6f,		0.6f,		0.6f,		1.0f };

	height = SMALLCHAR_HEIGHT * maxrows;
	//width = INFO_PLAYER_WIDTH + INFO_CLASS_WIDTH + INFO_SCORE_WIDTH + INFO_LATENCY_WIDTH;
	width = INFO_PLAYER_WIDTH + INFO_KD_WIDTH + INFO_SCORE_WIDTH + INFO_LATENCY_WIDTH;

	// Jaybird - 10 px change
	CG_FillRect( x-5, y-12, width+5, 31, clrUiBack );
	CG_FillRect( x-5, y-12, width+5, 31, clrUiBar );	
	
	Vector4Set( hcolor, 0, 0, 0, fade );
	CG_DrawRect_FixedBorder( x-5, y-12, width+5, 31, 1, colorBlack );

	// Jaybird - draw pings
	{
		float stdDeviation = 0;
		float mean = 0;
		int numPings = 0;
		float total = 0;

		for( i = 0; i < cg.numScores && count < maxrows; i++ ) {
			if( team != cgs.clientinfo[ cg.scores[i].client ].team ) {
				continue;
			}
			if( cg.scores[i].ping < 0 || cg.scores[i].ping >= 999 ) {
				continue;
			}
			total += cg.scores[i].ping;
			numPings++;
		}
		if( numPings > 0 ) {
			mean = total / numPings;
			total = 0;
			for( i = 0; i < cg.numScores && count < maxrows; i++ ) {
				if( team != cgs.clientinfo[ cg.scores[i].client ].team ) {
					continue;
				}
				if( cg.scores[i].ping < 0 || cg.scores[i].ping >= 999 ) {
					continue;
				}
				total += pow((cg.scores[i].ping - mean), 2);
			}
			stdDeviation = sqrt(total/numPings);
		}
		CG_Text_Paint_Ext( x, y, 0.2f, 0.2f, tclr, va( "AVERAGE PING: %.2fms � %.2fms", mean, stdDeviation ), 0, 0, 0, &cgs.media.limboFont1 );
	}

	// draw header
	if( cg_gameType.integer == GT_WOLF_LMS ) {
		char *s;
		if ( team == TEAM_AXIS ) {
			s = va( "%s [%d] (%d %s)", CG_TranslateString( "AXIS" ), cg.teamScores[0], cg.teamPlayers[team], CG_TranslateString("PLAYERS") );
			s = va( "%s ^3%s", s, cg.teamFirstBlood == TEAM_AXIS ? CG_TranslateString("FIRST BLOOD") : "" );

			CG_Text_Paint_Ext( x, y + 13, 0.25f, 0.25f, tclr, s, 0, 0, 0, &cgs.media.limboFont1 );
		} else if ( team == TEAM_ALLIES ) {
			s = va( "%s [%d] (%d %s)", CG_TranslateString( "ALLIES" ), cg.teamScores[1], cg.teamPlayers[team], CG_TranslateString("PLAYERS") );
			s = va( "%s ^3%s", s, cg.teamFirstBlood == TEAM_ALLIES ? CG_TranslateString("FIRST BLOOD") : "" );

			CG_Text_Paint_Ext( x, y + 13, 0.25f, 0.25f, tclr, s, 0, 0, 0, &cgs.media.limboFont1 );
		}
	} else {
		if ( team == TEAM_AXIS ) {
			CG_Text_Paint_Ext( x, y + 13, 0.25f, 0.25f, tclr, va( "%s [%d] (%d %s)", CG_TranslateString( "AXIS" ), cg.teamScores[0], cg.teamPlayers[team], CG_TranslateString("PLAYERS") ), 0, 0, 0, &cgs.media.limboFont1 );
		} else if ( team == TEAM_ALLIES ) {
			CG_Text_Paint_Ext( x, y + 13, 0.25f, 0.25f, tclr, va( "%s [%d] (%d %s)", CG_TranslateString( "ALLIES" ), cg.teamScores[1], cg.teamPlayers[team], CG_TranslateString("PLAYERS") ), 0, 0, 0, &cgs.media.limboFont1 );
		}
	}

	y += SMALLCHAR_HEIGHT + 3;

	tempx = x;

	// CHRUKER: b076 - Adjusted y coordinate, and changed to use DrawBottom instead of DrawTopBottom
	CG_FillRect( x-5, y, width+5, 18, clrUiBack );
	trap_R_SetColor( colorBlack );
	CG_DrawBottom_NoScale( x-5, y, width+5, 18, 1 );
	trap_R_SetColor( NULL );

	// draw player info headings
	CG_DrawSmallString( int(tempx), y, CG_TranslateString( "Name" ), fade );
	tempx += INFO_PLAYER_WIDTH;
	
	// AndyStutz - changes for kills/deaths
	//CG_DrawSmallString( int(tempx), y, CG_TranslateString( "Class" ), fade );
	CG_DrawSmallString( int(tempx), y, CG_TranslateString( va("%5s", "K/D") ), fade );
	//tempx += INFO_CLASS_WIDTH;
	tempx += INFO_KD_WIDTH;

	if( cgs.gametype == GT_WOLF_LMS ) {
		CG_DrawSmallString( int(tempx), y, CG_TranslateString( "Score" ), fade );
		tempx += INFO_SCORE_WIDTH;
	} else {
		CG_DrawSmallString( int(tempx + 1 * SMALLCHAR_WIDTH), y, CG_TranslateString( "XP" ), fade );
		tempx += INFO_XP_WIDTH;
	}

	CG_DrawSmallString( int(tempx), y, CG_TranslateString( "Ping" ), fade );
	tempx += INFO_LATENCY_WIDTH;

	if( cgs.gametype != GT_WOLF_LMS ) {
		CG_DrawPicST( tempx + 2, y, INFO_LIVES_WIDTH - 4, 16, 0.f, 0.f, 0.5f, 1.f, team == TEAM_ALLIES ? cgs.media.hudAlliedHelmet : cgs.media.hudAxisHelmet );
		tempx += INFO_LIVES_WIDTH;
	}
	
	// CHRUKER: b076 - The math says char height + 2 * border width (1 pixel)
	y += SMALLCHAR_HEIGHT + 2;

	cg.teamPlayers[team] = 0; // JPW NERVE
	for ( i = 0; i < cg.numScores; i++ ) {
		if ( team != cgs.clientinfo[ cg.scores[i].client ].team )
			continue;

		cg.teamPlayers[team]++;
	}

	// CHRUKER: b035 - Adjust maxrows
	if ( cg.teamPlayers[team] > maxrows ) {
		maxrows = absmaxrows;
		use_mini_chars = qtrue;
	}
	
	// save off y val
	tempy = y;
	
	// draw color bands
	for ( i = 0; i < maxrows; i++ ) {
		if ( i % 2 == 0 )
			VectorSet( hcolor, (80.f/255.f), (80.f/255.f), (80.f/255.f) );// LIGHT BLUE
		else
			VectorSet( hcolor, (0.f/255.f), (0.f/255.f), (0.f/255.f) ); // DARK BLUE
		hcolor[3] = fade * 0.3;
		
		if ( use_mini_chars ) {
			// CHRUKER: b076 - Adjusted y height, and changed to DrawBottom instead of DrawTopBottom
			CG_FillRect( x-5, y, width+5, MINICHAR_HEIGHT, hcolor );
			trap_R_SetColor( colorBlack );
			CG_DrawBottom_NoScale( x-5, y, width+5, MINICHAR_HEIGHT, 1 );
			trap_R_SetColor( NULL );
			y += MINICHAR_HEIGHT;
		} else {
			// CHRUKER: b076 - Adjusted y height, and changed to DrawBottom instead of DrawTopBottom
			CG_FillRect( x-5, y, width+5, SMALLCHAR_HEIGHT, hcolor );
			trap_R_SetColor( colorBlack );
			CG_DrawBottom_NoScale( x-5, y, width+5, SMALLCHAR_HEIGHT, 1 );
			trap_R_SetColor( NULL );
			y += SMALLCHAR_HEIGHT;
		}
	}
	hcolor[3] = 1;
	y = int(tempy) - 1;
	
	// draw player info
	VectorSet( hcolor, 1, 1, 1 );
	hcolor[3] = fade;

	count = 0;
	for( i = 0; i < cg.numScores && count < maxrows; i++ ) {
		if( team != cgs.clientinfo[ cg.scores[i].client ].team ) {
			continue;
		}

		// CHRUKER: b035 - Using the flag instead
		if( use_mini_chars ) {
			WM_DrawClientScore_Small( x, y, &cg.scores[i], hcolor, fade );
			y += MINICHAR_HEIGHT;
		} else {
			WM_DrawClientScore( x, y, &cg.scores[i], hcolor, fade );
			y += SMALLCHAR_HEIGHT;
		}

		count++;
	}

	// draw spectators
	// CHRUKER: b035 - Missing support for mini char height scoreboard background
	if ( use_mini_chars )
		y += MINICHAR_HEIGHT;
	else
		y += SMALLCHAR_HEIGHT;

	for ( i = 0; i < cg.numScores; i++ ) {
		if ( cgs.clientinfo[ cg.scores[i].client ].team != TEAM_SPECTATOR )
			continue;
		if ( team == TEAM_AXIS && ( i % 2 ) )
			continue;
		if ( team == TEAM_ALLIES && ( ( i + 1 ) % 2 ) )
			continue;

		// CHRUKER: b034 - Missing support for minichars; b035 - Using the flag instead
		if( use_mini_chars ) {
			WM_DrawClientScore_Small( x, y, &cg.scores[i], hcolor, fade );
			y += MINICHAR_HEIGHT;
		} else {
			WM_DrawClientScore( x, y, &cg.scores[i], hcolor, fade );
			y += SMALLCHAR_HEIGHT;
		}
	}

	return y;
}
// -NERVE - SMF

/*
=================
CG_DrawScoreboard

Draw the normal in-game scoreboard
=================
*/
qboolean CG_DrawScoreboard( void ) {
	int		x = 0, y = 0, x_right;
	float	fade;
	float	*fadeColor;

	x = SCREEN_X_OFFSET + 20;
	y = 10;

	x_right = SCREEN_WIDTH - x - (INFO_TOTAL_WIDTH - 5);

	// don't draw anything if the menu or console is up
	if ( cg_paused.integer ) {
		return qfalse;
	}

	// don't draw scoreboard during death while warmup up
	// OSP - also for pesky scoreboards in demos
	if ((cgs.gamestate == GS_WARMUP || (cg.demoPlayback && cg.snap->ps.pm_type != PM_INTERMISSION)) && !cg.showScores) {
		return qfalse;
	}

	// don't draw if in cameramode
	if( cg.cameraMode ) {
		return qtrue;
	}

	if( cg.showScores || cg.predictedPlayerState.pm_type == PM_INTERMISSION ) {
		fade = 1.0;
		fadeColor = colorWhite;
	} else {
		fadeColor = CG_FadeColor( cg.scoreFadeTime, FADE_TIME );
		
		if( !fadeColor ) {
			// next time scoreboard comes up, don't print killer
			*cg.killerName = 0;
			return qfalse;
		}
 		fade = fadeColor[3];
	}

	y = WM_DrawObjectives( x, y, SCREEN_WIDTH - 2*x + 5, fade );

	if ( cgs.gametype == GT_WOLF_STOPWATCH && ( cg.snap->ps.pm_type == PM_INTERMISSION ) ) {
		y = WM_DrawInfoLine( x, 155, fade );

		// CHRUKER: b035 - The maxrows has been split into one for when to use the mini chars and one for when to stop writing.
		WM_TeamScoreboard( x, y, TEAM_AXIS, fade, 8, 10 );
		x = x_right;
		WM_TeamScoreboard( x, y, TEAM_ALLIES, fade, 8, 10 );
	} else {
		if(cg.snap->ps.pm_type == PM_INTERMISSION) {
			WM_TeamScoreboard( x, y, TEAM_AXIS, fade, 9, 12 );
			x = x_right;
			WM_TeamScoreboard( x, y, TEAM_ALLIES, fade, 9, 12 );
		} else {
			WM_TeamScoreboard( x, y, TEAM_AXIS, fade, 25, 33 );
			x = x_right;
			WM_TeamScoreboard( x, y, TEAM_ALLIES, fade, 25, 33 );
		} // b035
	}

	return qtrue;
}
