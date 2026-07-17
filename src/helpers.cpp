#include "helpers.h"
#include <iostream>
#include <sstream>
#include <cstdlib>
#include <cctype>

void clearScreen()
{
#ifdef _WIN32 // For Windows
    system("cls");
#else // For Unix-like systems (Linux, macOS)
    system("clear");
#endif
}

string moveTOstring(pos Position)
{
    string move = "";
    move += (static_cast<char>('a' + Position.second));
    move += to_string(Position.first + 1);
    return move;
}

pos stringTOmove(string move)
{
    size_t col, first;
    char file = move[0], rank = move[1];
    col = static_cast<size_t>(file - 'a');
    first = static_cast<size_t>(rank - '0') - 1;
    return {first, col};
}

vector<string> splitFEN(string FEN)
{
    vector<string> res(6);
    // {pieces , turn , castling , enpassant , halfmoves , fullmoves}

    stringstream ss(FEN);
    string t;
    int i = 0;

    while (getline(ss, t, ' '))
    {
        res[i] = t;
        i++;
    }
    return res;
}

char getPromotionPiece()
{
    string piece;
    cout << "Choose What piece to promote to (N,Q,B,R): ";
    getline(cin, piece);
    return tolower(piece[0]);
}
