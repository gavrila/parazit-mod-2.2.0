#include <bgame/impl.h>

namespace cmd {

///////////////////////////////////////////////////////////////////////////////

Restart::Restart()
    : AbstractBuiltin( "restart" )
{
    __usage << xvalue( "!" + _name );
    __descr << "Restart the current map and keep all of the XP gained during the map.";
}

///////////////////////////////////////////////////////////////////////////////

Restart::~Restart()
{
}

///////////////////////////////////////////////////////////////////////////////

AbstractCommand::PostAction
Restart::doExecute( Context& txt )
{
    if (txt._args.size() != 1)
        return PA_USAGE;

    Svcmd_ResetMatch_f( qfalse, qtrue );
    return PA_NONE;
}

///////////////////////////////////////////////////////////////////////////////

} // namespace cmd
