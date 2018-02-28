#include <bgame/impl.h>

namespace cmd {

///////////////////////////////////////////////////////////////////////////////

Setvar::Setvar()
    : AbstractBuiltin( "setvar" )
{
    __usage << xvalue( "!" + _name ) << " [" << xvalue( "fastpanzer" ) << ']' << " [" << xvalue("25 - 10000") << "]\n" 
		<< "Sets panzer reload time in milliseconds\n"
		<< xvalue( "!" + _name ) << " [" << xvalue( "panzerrecoil" ) << ']' << " <" << xvalue("on") << "|" << xvalue("off") << ">\n"
		<< "Turns on/off panzer recoil setting\n";
    __descr << "Sets values for different session variables.";
}

///////////////////////////////////////////////////////////////////////////////

Setvar::~Setvar()
{
}

///////////////////////////////////////////////////////////////////////////////

AbstractCommand::PostAction
Setvar::doExecute( Context& txt )
{
    if (txt._args.size() > 4)
        return PA_USAGE;

    Buffer buf;

    // If there are no arguments, just report usage
    if (txt._args.size() == 1) {
        return PA_USAGE;
    }
        
    string vartoset = txt._args[1];
    str::toLower(vartoset);
    

    if (vartoset == "fastpanzer") {
		// We do NOT allow fastpanzer changes while a killing spree is in effect
		if (g_fastpanzerkillspreeon) {
			txt._ebuf << "We don't allow fastpanzer changes while a killing spree is in effect.";
			return PA_ERROR;
		}

		// Can't use string here because ubuntu compiler isn't version 11 so doesn't
		// support stoi() method (string to int).  Have to use older atoi() which requires
		// const char*
		//const char *panzerreloadtimems = txt._args[2].c_str();
	    string panzerreloadtimems = txt._args[2];
	    str::toLower( panzerreloadtimems );
		int ipanzerreloadtimems = atoi(panzerreloadtimems.c_str());
		if (ipanzerreloadtimems >= 25 && ipanzerreloadtimems <= 10000)
		{
			txt._client->gclient.sess.panzerreloadtimems = ipanzerreloadtimems;
			// Notify user changes have been made
			buf << _name << ": fastpanzer reload time has been set to " << xvalue( va("%i", ipanzerreloadtimems) ) << "ms.";
			printCpm( txt._client, buf, true );
		}
		else {
			txt._ebuf << "Invalid fastpanzer reload time (should be between 25 and 10000): " << xvalue( txt._args[2] );
			return PA_ERROR;
		}
    }
    else if (vartoset == "fastpanzerplayer") {
		// Can't use string here because ubuntu compiler isn't version 11 so doesn't
		// support stoi() method (string to int).  Have to use older atoi() which requires
		// const char*
		//const char *panzerreloadtimems = txt._args[2].c_str();
	    string panzerreloadtimems = txt._args[2];
	    str::toLower( panzerreloadtimems );
		int ipanzerreloadtimems = atoi(panzerreloadtimems.c_str());
		if (ipanzerreloadtimems >= 25 && ipanzerreloadtimems <= 10000)
		{
			Client* target;
			if (lookupPLAYER( txt._args[3], txt, target ))
				return PA_ERROR;

			if (isHigherLevelError( *target, txt ))
				return PA_ERROR;

			const User& targetUser = *connectedUsers[target->slot];
			gentity_t* const targetEnt = &target->gentity;
			targetEnt->client->sess.panzerreloadtimems = ipanzerreloadtimems;
			// Notify user changes have been made
			buf << _name << ": fastpanzer reload time has been set to " << xvalue( va("%i", ipanzerreloadtimems) ) << "ms.";
			printCpm( txt._client, buf, true );
		}
		else {
			txt._ebuf << "Invalid fastpanzerplayer reload time (should be between 25 and 10000): " << xvalue( txt._args[2] );
			return PA_ERROR;
		}
    }
	else if (vartoset == "panzerrecoil") {
		if (txt._args[2] == "on" || txt._args[2] == "off")
		{
			bool bPanzerRecoil = (bool)(txt._args[2] == "on");
			txt._client->gclient.sess.panzerrecoil = bPanzerRecoil;
			// Notify user changes have been made
			buf << _name << ": panzerrecoil has been turned " << xvalue( txt._args[2] ) << '.';
			printCpm( txt._client, buf, true );
		}
		else {
			txt._ebuf << "Invalid panzerrecoil value (should be 'on' or 'off'): " << xvalue( txt._args[2] );
			return PA_ERROR;
		}
	}
    else {
        txt._ebuf << "Invalid argument: " << xvalue( txt._args[1] );
        return PA_ERROR;
    }

    return PA_NONE;
}

///////////////////////////////////////////////////////////////////////////////

} // namespace cmd
