#pragma once
#include "board.h"

class game
{
public:
    game();
    void run (board &board); 
    ~game();
    void saveGame(board &board);
    void loadGame();
};