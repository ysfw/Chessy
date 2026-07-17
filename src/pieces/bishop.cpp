#include "piece.h"
#include "game.h"
bishop::bishop(bool isWhite, pos startingPosition)
    : piece(isWhite, startingPosition)
{
    setprintableValue(isWhite ? "♝":"♗");    
    setType('b');
}

void bishop :: checkMoves(board &Board, pos currPosition)
{
    clearMoves();
    for (size_t direction = 1; direction < 5; direction++){
    {
        for (size_t i=1;i < 8; i++)
        {
            size_t firstCoord, secondCoord;        
            switch (direction)
            {
            case 1://Right Diagonal Up
                firstCoord = currPosition.first + i;
                secondCoord = currPosition.second +i;
            break;
            
            case 2://Left Diagonal Down
                firstCoord = currPosition.first + i;
                secondCoord = currPosition.second -i;
            break;

            case 3://Right Diagonal Down
                firstCoord = currPosition.first-i;
                secondCoord = currPosition.second+i;
            break;

            case 4://left Diagonal Down
                firstCoord = currPosition.first-i;
                secondCoord = currPosition.second-i;
            break;


            default:
            break;
        }
    if (secondCoord < 8 && firstCoord<8)
    {
        if ((Board.getAt({firstCoord,secondCoord}) != nullptr && Board.getAt({firstCoord,secondCoord})->isWhite() == this->isWhite()))
        {
            break;
        }
        else
        {
            if (!Board.isPinned(this,{firstCoord,secondCoord})) {
                addPossibleMove({firstCoord,secondCoord});
                if (Board.getAt({firstCoord,secondCoord}) != nullptr) {
                    addPossibleCapture({firstCoord,secondCoord});
                    break;
                }
            }
        }
        
    }
    else break;
}      
    }
    
}
}
