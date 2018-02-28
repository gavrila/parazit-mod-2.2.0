#ifndef GAME_CMD_FASTPANZERKILLSPREE_H
#define GAME_CMD_FASTPANZERKILLSPREE_H

///////////////////////////////////////////////////////////////////////////////

class FastPanzerKillSpree : public AbstractBuiltin
{
protected:
    PostAction doExecute( Context& );

public:
    FastPanzerKillSpree();
    ~FastPanzerKillSpree();
};

///////////////////////////////////////////////////////////////////////////////

#endif // GAME_CMD_FASTPANZERKILLSPREE_H
