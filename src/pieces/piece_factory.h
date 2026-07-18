#pragma once
#include "piece.h"

// Factory for creating chess piece instances from a FEN character.
// Isolates piece instantiation logic from the board.
class PieceFactory
{
public:
    // Creates and returns a heap-allocated piece corresponding to
    // the given FEN character (e.g. 'R' = white rook, 'p' = black pawn).
    // Returns nullptr for unrecognized characters.
    // Caller takes ownership of the returned pointer.
    static piece* create(char pieceChar, pos position);
};
