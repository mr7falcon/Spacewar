#include "StdAfx.h"
#include "Game.h"

int main()
{
    CGame& game = CGame::Get();
    game.Start();

    return 0;
}