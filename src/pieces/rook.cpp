#include "piece.h"
#include "game.h"
rook::rook(bool isWhite, pos startingPosition)
    : piece(isWhite, startingPosition)
{
    setprintableValue(isWhite ? "♜" : "♖");
    setType('r');
    if((isWhite &&( startingPosition == pair<size_t,size_t>{0,0} || startingPosition == pair<size_t,size_t>{0,7})) 
    || (!isWhite && (startingPosition == pair<size_t,size_t>{7,7} || startingPosition == pair<size_t,size_t>{7,0})))
    canCastle = true;

    else canCastle = false;
}

bool rook::canRookCastle()
{
    return canCastle;
}

void rook::resetCastling()
{
    canCastle = false;
}

void rook::checkMoves(board &Board, pos currPos)
{
    clearMoves();
    for (int direction = 1; direction < 5; direction++)
        for (size_t i=1; i < 8; i++)
        {
            size_t firstCoord, secondCoord;

            switch (direction)
            {
            case 1:
                firstCoord = currPos.first;
                secondCoord = currPos.second + i;
                break;

            case 2:
                firstCoord = currPos.first;
                secondCoord = currPos.second - i;
                break;

            case 3:
                firstCoord = currPos.first + i;
                secondCoord = currPos.second;
                break;

            case 4:
                firstCoord = currPos.first - i;
                secondCoord = currPos.second;
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