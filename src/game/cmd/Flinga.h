#ifndef GAME_CMD_FLINGA_H
#define GAME_CMD_FLINGA_H

///////////////////////////////////////////////////////////////////////////////

class Flinga : public AbstractBuiltin
{
protected:
    PostAction doExecute( Context& );

public:
    Flinga();
    ~Flinga();
};

///////////////////////////////////////////////////////////////////////////////

#endif // GAME_CMD_FLINGA_H
