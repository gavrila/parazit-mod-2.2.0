#ifndef GAME_CMD_HELP_H
#define GAME_CMD_HELP_H

///////////////////////////////////////////////////////////////////////////////

class Help : public AbstractBuiltin
{
protected:
    PostAction doExecute( Context& );

public:
    Help();
    ~Help();
};

///////////////////////////////////////////////////////////////////////////////

#endif // GAME_CMD_HELP_H
