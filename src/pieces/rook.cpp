#include "piece.h"
#include "game.h"

#include <algorithm>
#include <map>
#include <cmath>
#include <cstdint>

rook::rook(bool isWhite, pos startingPosition)
    : piece(isWhite, startingPosition)
{
    setprintableValue(isWhite ? "♜" : "♖");
    setType('r');
    if((isWhite &&( startingPosition == pair<size_t,size_t>{0,0} || startingPosition == pair<size_t,size_t>{0,7})) 
    || (!isWhite && (startingPosition == pair<size_t,size_t>{7,7} || startingPosition == pair<size_t,size_t>{7,0})))
    hasCastlingRights = true;

    else hasCastlingRights = false;
}

bool rook::canCastle() const
{
    return hasCastlingRights;
}

void rook::resetCastling()
{
    hasCastlingRights = false;
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


bool rook::Move(board &Board, pos newPosition)
{
    set<pos> possibleMoves = getPossibleMoves();
    if (!binary_search(possibleMoves.begin(), possibleMoves.end(), newPosition))
    {
        return false; // Not a valid move at all
    }
    pos oldPosition = getPosition();
    piece *targetOnNextSquare = Board.getAt(newPosition);
    int movingPieceColor = this->isWhite() ? 1 : 0;
    char movingPieceType = 'r';
    bool isCapture = (targetOnNextSquare != nullptr);

    uint64_t newHash = Board.getPreviousHash();

        newHash ^= Board.getWhiteTurnkey();
        if (Board.getEnPassantFile() != NO_FILE)
        {
            newHash ^= Board.getenPassantFileKey(Board.getEnPassantFile());
        }
        newHash ^= Board.getPiecehash(movingPieceType, movingPieceColor, oldPosition);
        
    // --- BRANCH 1: Capture ---
    if (isCapture)
    {
        char capturedPieceType = targetOnNextSquare->getType();
        int capturedPieceColor = targetOnNextSquare->isWhite() ? 1 : 0;
        
        Board.resetHalfMovesNoCaptures();
        if(movingPieceColor) Board.setWhitecaptured();
        Board.minusPiece(capturedPieceType,capturedPieceColor);

        if (targetOnNextSquare->canCastle())
        {
            if ((newPosition.first == 0 || newPosition.first == 7) && newPosition.second == 7)
                newHash ^= Board.getcastlingKey(1, targetOnNextSquare->isWhite());
            if ((newPosition.first == 0 || newPosition.first == 7) && newPosition.second == 0)
                newHash ^= Board.getcastlingKey(2, targetOnNextSquare->isWhite());
        }
        newHash ^= Board.getPiecehash(movingPieceType, movingPieceColor, newPosition);
        newHash ^= Board.getPiecehash(capturedPieceType, capturedPieceColor, newPosition);
        
        delete Board.getAt(newPosition);
        
        Board.setAt(newPosition, this);
        Board.setAt(oldPosition, nullptr);
        this->updatePos(newPosition);
        
    }

    // --- BRANCH 2: Non-Capture Move ---
    else
    {
        if(movingPieceColor) Board.resetWhitecaptured();

        newHash ^= Board.getPiecehash(movingPieceType, movingPieceColor, newPosition);
        
        Board.setAt(newPosition, this);
        Board.setAt(oldPosition, nullptr);
        this->updatePos(newPosition);

        Board.plusHalfMoveNoCaptures();

    }

    if (this->canCastle())
    {
        king *k = Board.getKing(this->isWhite());
        if (oldPosition.second == 7 && k->canKingsideCastle(Board))
        {
            newHash ^= Board.getcastlingKey(1, this->isWhite());
            k->resetKingsideCastle();
        }
        if (oldPosition.second == 0 && k->canQueensideCastle(Board))
        {
            newHash ^= Board.getcastlingKey(2, this->isWhite());
            k->resetQueensideCastle();
        }
    }
    this->resetCastling();


    Board.resetEnpassant(); 
    Board.resetEnPassantFile();
    Board.setEnpassantstr("");

    Board.addHash(newHash);
    Board.setPreviousHash(newHash);
    
    if(!Board.isWhiteTurn())
        Board.plusFullMove();

    return true;

}

bool rook::attacks(pos target, board& b) const
{
    pos from = getPosition();
    pair<int,int> dirs[4] = {{0,1},{0,-1},{1,0},{-1,0}};
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