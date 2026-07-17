#include "piece.h"
#include "game.h"

Knight::Knight(bool isWhite, pos startingPosition)
    : piece(isWhite, startingPosition)
{
    setprintableValue(isWhite ? "♞" : "♘");
    setType('n');
}

void Knight ::checkMoves(board &Board, pos /*currPosition*/)
{
    clearMoves();
    pair<int,int> directions [8]  = {{2,1},{2,-1},{-2,1},{-2,-1},{1,2},{-1,2},{1,-2},{-1,-2}};
    for(pair<int,int> direction : directions){
        int firstCoord =(int)this->getPosition().first + direction.first;
        int secondCoord = (int)this->getPosition().second + direction.second;
        if((firstCoord <8 &&firstCoord >= 0 && secondCoord < 8  && secondCoord >=0))
        {
            pos targetSquare = {(size_t)firstCoord, (size_t)secondCoord};
            piece* pieceOnTarget = Board.getAt(targetSquare);

            if (pieceOnTarget != nullptr && pieceOnTarget->isWhite() == this->isWhite()) {
                continue;
            }

            if (!Board.isPinned(this,{firstCoord,secondCoord})) {
                addPossibleMove(targetSquare);
                if (pieceOnTarget != nullptr) {
                    addPossibleCapture(targetSquare);
                }
            }
        }
    }
   
}