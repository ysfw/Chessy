#include "piece.h"
#include "game.h"

#include <algorithm>
#include <map>
#include <cmath>
#include <cstdint>


pawn::pawn(bool isWhite, pos startingPosition)
    : piece(isWhite, startingPosition)
{
    setprintableValue(isWhite ? "♟" : "♙");
    setType('p');
}

void pawn ::setenpassant()
{
    enpassant = true;
}

void pawn ::resetenpassant()
{
    enpassant = false;
}

bool pawn ::isEnpassant()
{
    return enpassant;
}

void pawn ::checkMoves(board &Board, pos currPosition)
{
    clearMoves();
    if (this->isWhite())
    {
        int forwardOne = currPosition.first + 1;
        int forwardTwo = currPosition.first + 2;

        if (forwardOne < 8 && Board.getAt({(size_t)forwardOne, currPosition.second}) == nullptr)
        {
            if (!Board.isPinned(this, {(size_t)forwardOne, currPosition.second}))
            {
                addPossibleMove({(size_t)forwardOne, currPosition.second});
            }
            if (currPosition.first == 1 && Board.getAt({(size_t)forwardTwo, currPosition.second}) == nullptr && !Board.isPinned(this, {(size_t)forwardTwo, currPosition.second}))
            {
                addPossibleMove({(size_t)forwardTwo, currPosition.second});
            }
        }

        if (forwardOne < 8 && currPosition.second + 1 < 8)
        {
            piece *target = Board.getAt({(size_t)forwardOne, currPosition.second + 1});
            if (target != nullptr && !target->isWhite() && !Board.isPinned(this, {(size_t)forwardOne, currPosition.second + 1}))
            {
                addPossibleMove({(size_t)forwardOne, currPosition.second + 1});
                addPossibleCapture({(size_t)forwardOne, currPosition.second + 1});
            }
        }
        if (forwardOne < 8 && currPosition.second > 0)
        {
            piece *target = Board.getAt({(size_t)forwardOne, currPosition.second - 1});
            if (target != nullptr && !target->isWhite() && !Board.isPinned(this, {(size_t)forwardOne, currPosition.second - 1}))
            {
                addPossibleMove({(size_t)forwardOne, currPosition.second - 1});
                addPossibleCapture({(size_t)forwardOne, currPosition.second - 1});
            }
        }

        if (currPosition.first == 4 && Board.isEnpassant())
        {
            if (currPosition.second + 1 < 8)
            {
                piece *adjacent = Board.getAt({currPosition.first, currPosition.second + 1});
                if (adjacent != nullptr && !adjacent->isWhite())
                {
                    if (pawn *p = dynamic_cast<pawn *>(adjacent))
                    {
                        if (p->isEnpassant() && !Board.isPinned(this, {(size_t)forwardOne, currPosition.second + 1}))
                        {
                            addPossibleMove({(size_t)forwardOne, currPosition.second + 1});
                            addPossibleCapture({(size_t)forwardOne, currPosition.second + 1});
                        }
                    }
                }
            }
            if (currPosition.second > 0)
            {
                piece *adjacent = Board.getAt({currPosition.first, currPosition.second - 1});
                if (adjacent != nullptr && !adjacent->isWhite())
                {
                    if (pawn *p = dynamic_cast<pawn *>(adjacent))
                    {
                        if (p->isEnpassant() && !Board.isPinned(this, {(size_t)forwardOne, currPosition.second - 1}))
                        {
                            addPossibleMove({(size_t)forwardOne, currPosition.second - 1});
                            addPossibleCapture({(size_t)forwardOne, currPosition.second - 1});
                        }
                    }
                }
            }
        }
    }
    else
    {
        int forwardOne = currPosition.first - 1;
        int forwardTwo = currPosition.first - 2;

        if (forwardOne >= 0 && Board.getAt({(size_t)forwardOne, currPosition.second}) == nullptr && !Board.isPinned(this, {(size_t)forwardOne, currPosition.second}))
        {
            if (!Board.isPinned(this, {(size_t)forwardOne, currPosition.second}))
            {
                addPossibleMove({(size_t)forwardOne, currPosition.second});
            }
            if (currPosition.first == 6 && Board.getAt({(size_t)forwardTwo, currPosition.second}) == nullptr && !Board.isPinned(this, {(size_t)forwardTwo, currPosition.second}))
            {
                addPossibleMove({(size_t)forwardTwo, currPosition.second});
            }
        }

        if (forwardOne >= 0 && currPosition.second + 1 < 8)
        {
            piece *target = Board.getAt({(size_t)forwardOne, currPosition.second + 1});
            if (target != nullptr && target->isWhite() && !Board.isPinned(this, {(size_t)forwardOne, currPosition.second + 1}))
            {
                addPossibleMove({(size_t)forwardOne, currPosition.second + 1});
                addPossibleCapture({(size_t)forwardOne, currPosition.second + 1});
            }
        }
        if (forwardOne >= 0 && currPosition.second > 0)
        {
            piece *target = Board.getAt({(size_t)forwardOne, currPosition.second - 1});
            if (target != nullptr && target->isWhite() && !Board.isPinned(this, {(size_t)forwardOne, currPosition.second - 1}))
            {
                addPossibleMove({(size_t)forwardOne, currPosition.second - 1});
                addPossibleCapture({(size_t)forwardOne, currPosition.second - 1});
            }
        }

        if (currPosition.first == 3 && Board.isEnpassant())
        {
            if (currPosition.second + 1 < 8)
            {
                piece *adjacent = Board.getAt({currPosition.first, currPosition.second + 1});
                if (adjacent != nullptr && adjacent->isWhite())
                {
                    if (pawn *p = dynamic_cast<pawn *>(adjacent))
                    {
                        if (p->isEnpassant() && !Board.isPinned(this, {(size_t)forwardOne, currPosition.second + 1}))
                        {
                            addPossibleMove({(size_t)forwardOne, currPosition.second + 1});
                            addPossibleCapture({(size_t)forwardOne, currPosition.second + 1});
                        }
                    }
                }
            }
            if (currPosition.second > 0)
            {
                piece *adjacent = Board.getAt({currPosition.first, currPosition.second - 1});
                if (adjacent != nullptr && adjacent->isWhite())
                {
                    if (pawn *p = dynamic_cast<pawn *>(adjacent))
                    {
                        if (p->isEnpassant() && !Board.isPinned(this, {(size_t)forwardOne, currPosition.second - 1}))
                        {
                            addPossibleMove({(size_t)forwardOne, currPosition.second - 1});
                            addPossibleCapture({(size_t)forwardOne, currPosition.second - 1});
                        }
                    }
                }
            }
        }
    }
}


bool pawn::Move(board &Board, pos newPosition)
{
    set<pos> possibleMoves = getPossibleMoves();
    set<pos> possibleCaptures = getPossibleCaptures();
    if (!binary_search(possibleMoves.begin(), possibleMoves.end(), newPosition))
    {
        return false; // Not a valid move at all
    }
    pos oldPosition = getPosition();
    piece *targetOnNextSquare = Board.getAt(newPosition);
    int movingPieceColor = this->isWhite() ? 1 : 0;
    char movingPieceType = 'p';
    bool isCapture = (targetOnNextSquare != nullptr);
    bool isEnpassantCapture = !isCapture && binary_search(possibleCaptures.begin(), possibleCaptures.end(), newPosition);
    bool isPromotion = newPosition.first == 7 || newPosition.first == 0;

    uint64_t newHash = Board.getPreviousHash();

        newHash ^= Board.getWhiteTurnkey();
        if (Board.getEnPassantFile() != NO_FILE)
        {
            newHash ^= Board.getenPassantFileKey(Board.getEnPassantFile());
        }
        newHash ^= Board.getPiecehash(movingPieceType, movingPieceColor, oldPosition);


    // --- BRANCH 1: Promotion ---
    if (isPromotion)
    {

        piece *promotedPiece = nullptr;
        char promotionPieceType = getPromotionPiece(this->isWhite());
        char promotionPieceLower = tolower(promotionPieceType);
        Board.plusPiece(promotionPieceLower,this->isWhite());
        Board.minusPiece('p',this->isWhite());
        if (targetOnNextSquare != nullptr)
        {
            char capturedPieceType = targetOnNextSquare->getType();
            int capturedPieceColor = targetOnNextSquare->isWhite() ? 1 : 0;
            Board.minusPiece(capturedPieceType,capturedPieceColor);
            if(movingPieceColor) Board.setWhitecaptured();
            Board.resetHalfMovesNoCaptures();
            newHash ^= Board.getPiecehash(capturedPieceType, capturedPieceColor, newPosition);
            delete targetOnNextSquare;
        }
        else 
        {
            if(movingPieceColor) Board.resetWhitecaptured();
            Board.plusHalfMoveNoCaptures();
        }
        promotedPiece = PieceFactory::create(promotionPieceType, newPosition);
        Board.setAt(newPosition, promotedPiece);
        newHash ^= Board.getPiecehash(promotionPieceLower, movingPieceColor, newPosition);

        Board.setAt(oldPosition, nullptr);

        Board.addHash(newHash);
        Board.setPreviousHash(newHash);
        if(!Board.isWhiteTurn())
            Board.plusFullMove();

        Board.resetEnpassant();
        Board.resetEnPassantFile();
        Board.setEnpassantstr("");

        delete this; // deletes the pawn itself after it's safe to do so
        return true;
    }
        
    // --- BRANCH 2: Capture ---
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
        
        Board.resetEnpassant();
        Board.resetEnPassantFile();
        Board.setEnpassantstr("");
    }
    // --- BRANCH 3: En Passant Capture ---
    else if (isEnpassantCapture)
    {
        Board.resetHalfMovesNoCaptures();
        if(movingPieceColor) Board.setWhitecaptured();
        Board.minusPiece('p',!movingPieceColor);
        pos capturedPawnPos = {oldPosition.first, newPosition.second};
        newHash ^= Board.getPiecehash(movingPieceType, movingPieceColor, newPosition);
        newHash ^= Board.getPiecehash('p', !movingPieceColor, capturedPawnPos);
        delete Board.getAt(newPosition);
        Board.setAt(newPosition, this);
        Board.setAt(oldPosition, nullptr);
        Board.setAt(capturedPawnPos, nullptr);

        this->updatePos(newPosition);

        Board.resetEnpassant();
        Board.resetEnPassantFile();
        Board.setEnpassantstr("");

    }

    // --- BRANCH 4: Non-Capture Move ---
    else
    {
        if(movingPieceColor) Board.resetWhitecaptured();

        newHash ^= Board.getPiecehash(movingPieceType, movingPieceColor, newPosition);
        
        Board.setAt(newPosition, this);
        Board.setAt(oldPosition, nullptr);
        this->updatePos(newPosition);
        
        if (abs((int)newPosition.first - (int)oldPosition.first) == 2)
        {
            Board.setEnpassant();
            Board.setEnPassantFile(newPosition.second);
            this->setenpassant();
            pawn* possibility1 = nullptr;
            pawn* possibility2 = nullptr;
        
            if (newPosition.second > 0)
            {
                possibility1 = dynamic_cast<pawn *>(Board.getAt({newPosition.first, newPosition.second - 1}));
            }
            if (newPosition.second < 7)
            {
                possibility2 = dynamic_cast<pawn *>(Board.getAt({newPosition.first, newPosition.second + 1}));
            }
            if ((possibility1 != nullptr && possibility1->isWhite() != this->isWhite()) || (possibility2 != nullptr && possibility2->isWhite() != this->isWhite()))
            {
                Board.setEnpassantstr(moveTOstring({newPosition.first-1,newPosition.second}));
            }

            newHash ^= Board.getenPassantFileKey(newPosition.second);
        }

        Board.plusHalfMoveNoCaptures();

    }

    Board.addHash(newHash);
    Board.setPreviousHash(newHash);
    
    if(!Board.isWhiteTurn())
        Board.plusFullMove();

    return true;

}

bool pawn::attacks(pos target, board& /*b*/) const
{
    pos from = getPosition();
    size_t forward = isWhite() ? from.first + 1 : from.first - 1;
    return forward == target.first &&
           (target.second == from.second + 1 || target.second == from.second - 1);
}
