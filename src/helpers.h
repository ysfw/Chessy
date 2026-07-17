#pragma once
#include <string>
#include <vector>
#include "pieces/piece.h"

using namespace std;

string moveTOstring(pos Position);
pos stringTOmove(string move);
char getPromotionPiece();
void clearScreen();
vector<string> splitFEN(string FEN);
