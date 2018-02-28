#include <bgame/impl.h>

namespace cmd {

///////////////////////////////////////////////////////////////////////////////

Bots::Bots()
    : AbstractBuiltin( "bots" )
{
    __usage << xvalue( "!" + _name ) << " [" << xvalue( "on" ) << '|' << xvalue( "off" ) << '|' << xvalue( "number of bots - 2, 4, 6, 8, or 10" ) << ']';
    __descr << "Turns bots on or off.";
}

///////////////////////////////////////////////////////////////////////////////

Bots::~Bots()
{
}

///////////////////////////////////////////////////////////////////////////////

AbstractCommand::PostAction
Bots::doExecute( Context& txt )
{
    if (txt._args.size() > 2)
        return PA_USAGE;

    Buffer buf;

    // If there are no arguments, just report usage
    if (txt._args.size() == 1) {
        return PA_USAGE;
    }
        
    string action = txt._args[1];
    str::toLower(action);
    
    if (action == "off") {
		trap_SendConsoleCommand( EXEC_APPEND, "bot maxbots 0\n" );
		trap_SendConsoleCommand( EXEC_APPEND, "bot kickall\n" );

        buf << _name << ": Bots are now " << xvalue( "turned off" ) << '.';
        printCpm( txt._client, buf, true );
    }
    else if (action == "on" || action == "10") {
		trap_SendConsoleCommand( EXEC_APPEND, "bot maxbots 10\n" );
		trap_SendConsoleCommand( EXEC_APPEND, "bot addbot 1 1\n" );
		trap_SendConsoleCommand( EXEC_APPEND, "bot addbot 2 1\n" );
		trap_SendConsoleCommand( EXEC_APPEND, "bot addbot 1 2\n" );
		trap_SendConsoleCommand( EXEC_APPEND, "bot addbot 2 2\n" );
		trap_SendConsoleCommand( EXEC_APPEND, "bot addbot 1 3\n" );
		trap_SendConsoleCommand( EXEC_APPEND, "bot addbot 2 3\n" );
		trap_SendConsoleCommand( EXEC_APPEND, "bot addbot 1 4\n" );
		trap_SendConsoleCommand( EXEC_APPEND, "bot addbot 2 4\n" );
		trap_SendConsoleCommand( EXEC_APPEND, "bot addbot 1 5\n" );
		trap_SendConsoleCommand( EXEC_APPEND, "bot addbot 2 5\n" );

        buf << _name << ": Bots are now " << xvalue( "turned on" ) << '.';
        printCpm( txt._client, buf, true );
    }
	else if (action == "2") {
		trap_SendConsoleCommand( EXEC_APPEND, "bot maxbots 2\n" );
		trap_SendConsoleCommand( EXEC_APPEND, "bot addbot 1 1\n" );
		trap_SendConsoleCommand( EXEC_APPEND, "bot addbot 2 1\n" );
	}
	else if (action == "4") {
		trap_SendConsoleCommand( EXEC_APPEND, "bot maxbots 4\n" );
		trap_SendConsoleCommand( EXEC_APPEND, "bot addbot 1 1\n" );
		trap_SendConsoleCommand( EXEC_APPEND, "bot addbot 2 1\n" );
		trap_SendConsoleCommand( EXEC_APPEND, "bot addbot 1 2\n" );
		trap_SendConsoleCommand( EXEC_APPEND, "bot addbot 2 2\n" );
	}
	else if (action == "6") {
		trap_SendConsoleCommand( EXEC_APPEND, "bot maxbots 6\n" );
		trap_SendConsoleCommand( EXEC_APPEND, "bot addbot 1 1\n" );
		trap_SendConsoleCommand( EXEC_APPEND, "bot addbot 2 1\n" );
		trap_SendConsoleCommand( EXEC_APPEND, "bot addbot 1 2\n" );
		trap_SendConsoleCommand( EXEC_APPEND, "bot addbot 2 2\n" );
		trap_SendConsoleCommand( EXEC_APPEND, "bot addbot 1 3\n" );
		trap_SendConsoleCommand( EXEC_APPEND, "bot addbot 2 3\n" );
	}
	else if (action == "8") {
		trap_SendConsoleCommand( EXEC_APPEND, "bot maxbots 8\n" );
		trap_SendConsoleCommand( EXEC_APPEND, "bot addbot 1 1\n" );
		trap_SendConsoleCommand( EXEC_APPEND, "bot addbot 2 1\n" );
		trap_SendConsoleCommand( EXEC_APPEND, "bot addbot 1 2\n" );
		trap_SendConsoleCommand( EXEC_APPEND, "bot addbot 2 2\n" );
		trap_SendConsoleCommand( EXEC_APPEND, "bot addbot 1 3\n" );
		trap_SendConsoleCommand( EXEC_APPEND, "bot addbot 2 3\n" );
		trap_SendConsoleCommand( EXEC_APPEND, "bot addbot 1 4\n" );
		trap_SendConsoleCommand( EXEC_APPEND, "bot addbot 2 4\n" );
	}
    else {
        txt._ebuf << "Invalid argument: " << xvalue( txt._args[1] );
        return PA_ERROR;
    }

    return PA_NONE;
}

///////////////////////////////////////////////////////////////////////////////

} // namespace cmd
