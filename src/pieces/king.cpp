#include "piece.h"
#include "game.h"
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
        rook *r = dynamic_cast<rook *>(Board.getAt({firstCoord, secondCoord + 3}));
        if (r != nullptr && r->canRookCastle())
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
        rook *r = dynamic_cast<rook *>(Board.getAt({firstCoord, secondCoord - 4}));
        if (r != nullptr && r->canRookCastle())
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