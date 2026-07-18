#include "piece_factory.h"
#include <cctype>

piece* PieceFactory::create(char pieceChar, pos position)
{
    bool isWhite = (isupper(pieceChar) != 0);

    switch (tolower(pieceChar))
    {
    case 'r': return new rook(isWhite, position);
    case 'n': return new Knight(isWhite, position);
    case 'b': return new bishop(isWhite, position);
    case 'q': return new queen(isWhite, position);
    case 'k': return new king(isWhite, position);
    case 'p': return new pawn(isWhite, position);
    default:  return nullptr;
    }
}
