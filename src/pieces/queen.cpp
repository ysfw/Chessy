#include "piece.h"
#include "game.h"
queen::queen(bool isWhite, pos startingPosition)
    : piece(isWhite, startingPosition)
{
    setprintableValue(isWhite ? "♛" : "♕");
    setType('q');
}

void queen::checkMoves(board &Board, pos currPosition)
{
    clearMoves();
    for (size_t direction = 1; direction < 5; direction++)
    {
        for (size_t i = 1; i < 8; i++)
        {
            size_t firstCoord, secondCoord;
            switch (direction)
            {
            case 1: // Right Diagonal Up
                firstCoord = currPosition.first + i;
                secondCoord = currPosition.second + i;
                break;

            case 2: // Left Diagonal Down
                firstCoord = currPosition.first + i;
                secondCoord = currPosition.second - i;
                break;

            case 3: // Right Diagonal Down
                firstCoord = currPosition.first - i;
                secondCoord = currPosition.second + i;
                break;

            case 4: // left Diagonal Down
                firstCoord = currPosition.first - i;
                secondCoord = currPosition.second - i;
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

    for (int direction = 1; direction < 5; direction++)
    {
        for (size_t i=1; i < 8; i++)
        {
            size_t firstCoord, secondCoord;

            switch (direction)
            {
            case 1:
                firstCoord = currPosition.first;
                secondCoord = currPosition.second + i;
                break;

            case 2:
                firstCoord = currPosition.first;
                secondCoord = currPosition.second - i;
                break;

            case 3:
                firstCoord = currPosition.first + i;
                secondCoord = currPosition.second;
                break;

            case 4:
                firstCoord = currPosition.first - i;
                secondCoord = currPosition.second;
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

bool queen::attacks(pos target, board& b) const
{
    pos from = getPosition();
    pair<int,int> dirs[8] = {{0,1},{0,-1},{1,0},{-1,0},{1,1},{1,-1},{-1,1},{-1,-1}};
    for (auto& dir : dirs)
        for (size_t k = 1; k < 8; k++) {
            size_t r = from.first  + dir.first  * k;
            size_t c = from.second + dir.second * k;
            if (r >= 8 || c >= 8) break;
            if (r == target.first && c == target.second) return true;
            if (b.getAt({r, c}) != nullptr) break;
        }
    return false;
}
