#include <algorithm>
#include <map>
#include <cmath>
#include <cstdint>
#include "piece.h"
#include "game.h"
using namespace std;

bool piece::isWhite() { return White; }

piece ::piece(bool isWhite, pos startingPosition)
    : position(startingPosition), White(isWhite), printableValue(""), possibleMoves() {}

piece::piece()
    : position({0, 0}), White(false), printableValue(""), possibleMoves() {}

pos piece::getPosition()
{
    return this->position;
}

piece ::~piece()
{
}

void piece::updatePos(pos newPosition)
{
    this->position = newPosition;
}

string piece::getprintableValue()
{
    return printableValue;
}

void piece::setprintableValue(string newprintableValue)
{
    this->printableValue = newprintableValue;
}

char piece::getType()
{
    return type;
}

void piece::setType(char newType)
{
    this->type = newType;
}

void piece::addPossibleMove(pos move)
{
    possibleMoves.insert(move);
}

void piece::addPossibleCapture(pos move)
{
    possibleCaptures.insert(move);
}
set<pos> piece ::getPossibleMoves() { return possibleMoves; };
set<pos> piece ::getPossibleCaptures() { return possibleCaptures; };

void piece::clearMoves()
{
    possibleMoves.clear();
    possibleCaptures.clear();
}

void piece ::checkMoves(board &/*Board*/, pos /*currPosition*/) {}

bool piece::Move(board &Board, pos newPosition)
{
    set<pos> possibleMoves = getPossibleMoves();
    if (!binary_search(possibleMoves.begin(), possibleMoves.end(), newPosition))
    {
        return false; // Not a valid move at all
    }
    pos oldPosition = getPosition();
    piece *targetOnNextSquare = Board.getAt(newPosition);
    int movingPieceColor = this->isWhite() ? 1 : 0;
    char movingPieceType = getType();
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

    Board.resetEnpassant(); 
    Board.resetEnPassantFile();
    Board.setEnpassantstr("");
    
    Board.addHash(newHash);
    Board.setPreviousHash(newHash);
    
    if(!Board.isWhiteTurn())
        Board.plusFullMove();

    return true;

}