#include "piece.h"
#include "game.h"

#include <algorithm>
#include <map>
#include <cmath>
#include <cstdint>

king::king(bool isWhite, pair<int, int> startingPosition)
    : piece(isWhite, startingPosition)
{
    setprintableValue(isWhite ? "♚" : "♔");
    setType('k');
    if ((isWhite && startingPosition == pair<int, int>{0, 4}) || (!isWhite && startingPosition == pair<int, int>{7, 4}))
        canCastle = true;
    else
        canCastle = false;
    canQueenside = false, canKingside = false;
}

bool king::isCheck(board &Board, pos targetSquare)
{
    pos oldPosition = this->getPosition();
    piece *capturedPiece = Board.getAt(targetSquare);

    Board.setAt(targetSquare, this);
    Board.setAt(oldPosition, nullptr);
    this->updatePos(targetSquare);

    bool isKingAttacked = Board.AttackedBy(targetSquare, this->isWhite());

    Board.setAt(oldPosition, this);
    Board.setAt(targetSquare, capturedPiece);
    this->updatePos(oldPosition);

    return isKingAttacked;
}

// the canCastle is just an indicator of "did it make a move or not?"
bool king::canKingCastle()
{
    return canCastle;
}

void king::setKingsideCastle()
{
    canKingside = true;
}
void king::setQueensideCastle()
{
    canQueenside = true;
}
void king::resetKingsideCastle()
{
    canKingside = false;
}
void king::resetQueensideCastle()
{
    canQueenside = false;
}

bool king::getKingsideCastle()
{
    return this->canKingside;
}
bool king::getQueensideCastle()
{
    return this->canQueenside;
}

bool king::canKingsideCastle(board &Board)
{
    if (this->canCastle)
    {
        if (!this->canKingside)
            return false;
        size_t firstCoord = this->getPosition().first, secondCoord = this->getPosition().second;

        for (size_t i = 1; i < 3; i++)
        {
            if (Board.getAt({firstCoord, secondCoord + i}) != nullptr || Board.AttackedBy({firstCoord, secondCoord + i}, this->isWhite()))
                return false;
        }
        piece *r = Board.getAt({firstCoord, secondCoord + 3});
        if (r != nullptr && r->canCastle())
            return true;
    }
    return false;
}

bool king::canQueensideCastle(board &Board)
{
    if (this->canCastle)
    {
        if (!this->canQueenside)
            return false;
        size_t firstCoord = this->getPosition().first, secondCoord = this->getPosition().second;

        for (size_t i = 1; i < 4; i++)
        {
            if (Board.getAt({firstCoord, secondCoord - i}) != nullptr || Board.AttackedBy({firstCoord, secondCoord - i}, this->isWhite()))
                return false;
        }
        piece *r = Board.getAt({firstCoord, secondCoord - 4});
        if (r != nullptr && r->canCastle())
            return true;
    }
    return false;
}

void king::resetCastling()
{
    canCastle = false;
}

void king::checkMoves(board &Board, pos /*position*/)
{
    clearMoves();
    // Checking for a possible castle
    if (canKingsideCastle(Board))
    {
        addPossibleMove({this->getPosition().first, this->getPosition().second + 2});
    }
    if (canQueensideCastle(Board))
    {
        addPossibleMove({this->getPosition().first, this->getPosition().second - 2});
    }

    pair<int, int> directions[8] = {{0, 1}, {0, -1}, {-1, 0}, {1, 0}, {1, 1}, {1, -1}, {-1, 1}, {-1, -1}};
    for (pair<int, int> direction : directions)
    {
        int firstCoord = (int)this->getPosition().first + direction.first;
        int secondCoord = (int)this->getPosition().second + direction.second;

        if ((firstCoord < 8 && firstCoord >= 0 && secondCoord < 8 && secondCoord >= 0))
        {
            pair<size_t, size_t> targetSquare = {(size_t)firstCoord, (size_t)secondCoord};
            piece *pieceOnTarget = Board.getAt(targetSquare);

            if (pieceOnTarget != nullptr && pieceOnTarget->isWhite() == this->isWhite())
            {
                continue;
            }

            if (!isCheck(Board, targetSquare))
            {
                addPossibleMove(targetSquare);
                if (pieceOnTarget != nullptr)
                {
                    addPossibleCapture(targetSquare);
                }
            }
        }
    }
}


bool king::Move(board &Board, pos newPosition)
{
    set<pos> possibleMoves = getPossibleMoves();
    if (!binary_search(possibleMoves.begin(), possibleMoves.end(), newPosition))
    {
        return false; // Not a valid move at all
    }
    pos oldPosition = getPosition();
    piece *targetOnNextSquare = Board.getAt(newPosition);
    int movingPieceColor = this->isWhite() ? 1 : 0;
    char movingPieceType = 'k';
    bool isCapture = (targetOnNextSquare != nullptr);
    bool isCastle = abs((int)(oldPosition.second - newPosition.second)) == 2;


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

    // --- BRANCH 2: Castling Move ---
    else if (isCastle)
    {
        if(movingPieceColor) Board.resetWhitecaptured();

        Board.plusHalfMoveNoCaptures();

        newHash ^= Board.getPiecehash(movingPieceType, movingPieceColor, newPosition);
        
        bool kingSide = (newPosition.second > oldPosition.second);
        Board.setAt(newPosition, this);
        
        pos oldRookPos = {oldPosition.first, kingSide ? 7 : 0};
        pos newRookPos = {oldPosition.first, kingSide ? newPosition.second - 1 : newPosition.second + 1};
        piece *rook = Board.getAt(oldRookPos);

        newHash ^= Board.getPiecehash('r', movingPieceColor, oldRookPos);
        newHash ^= Board.getPiecehash('r', movingPieceColor, newRookPos);
        
        Board.setAt(newRookPos, rook);
        Board.setAt(oldRookPos, nullptr);
        Board.setAt(oldPosition, nullptr);
        
        this->updatePos(newPosition);
        rook->updatePos(newRookPos);
    }

    // --- BRANCH 3: Non-Capture Move ---
    else
    {
        if(movingPieceColor) Board.resetWhitecaptured();

        newHash ^= Board.getPiecehash(movingPieceType, movingPieceColor, newPosition);
        
        Board.setAt(newPosition, this);
        Board.setAt(oldPosition, nullptr);
        this->updatePos(newPosition);
        
        Board.plusHalfMoveNoCaptures();

    }

    Board.resetEnpassant();
    Board.resetEnPassantFile();
    Board.setEnpassantstr("");

    if (this->canKingCastle())
    {
        newHash ^= Board.getcastlingKey(1, this->isWhite());
        newHash ^= Board.getcastlingKey(2, this->isWhite());
    }

    this->resetCastling();
    this->resetKingsideCastle();
    this->resetQueensideCastle();
    
    Board.setKingPosition(this->isWhite(), newPosition);

    Board.addHash(newHash);
    Board.setPreviousHash(newHash);
    
    if(!Board.isWhiteTurn())
        Board.plusFullMove();

    return true;

}

bool king::attacks(pos target, board& /*b*/) const
{
    int dr = abs((int)target.first  - (int)getPosition().first);
    int dc = abs((int)target.second - (int)getPosition().second);
    return dr <= 1 && dc <= 1 && (dr + dc) > 0;
}