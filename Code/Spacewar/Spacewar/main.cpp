#include "Game.h"

#include <SFML/Graphics/CircleShape.hpp>

int main()
{
    CGame& game = CGame::Get();
    game.Start();

    return 0;
}