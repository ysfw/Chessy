#pragma once
#include <utility>
#include <string>
#include <set>
using namespace std;
class player;
class board;
class game;
typedef pair<size_t,size_t> pos;

class piece
{
private:
    pos position;
    bool White;
    string printableValue;
    char type;
    set<pos> possibleMoves;
    set<pos> possibleCaptures;

public:
    piece();
    piece(bool isWhite, pos startingPosition);
    virtual ~piece();
    bool isWhite();
    void updatePos(pos newPosition);
    pos getPosition();
    char getType();
    void setType(char newType);
    string getprintableValue();
    void setprintableValue(string);
    void addPossibleMove(pos move);
    void addPossibleCapture(pos move);
    set<pos> getPossibleMoves ();
    set<pos> getPossibleCaptures ();
    bool Move(board &Board,pos newPosition);
    virtual void checkMoves(board &Board, pos currPosition);
    void clearMoves();

};


class rook : public piece {
private:
    bool canCastle;
public:
   rook(bool White, pos startingPosition);
   bool canRookCastle();
   void resetCastling();
   void checkMoves(board &Board, pos currPos)override;
};


class bishop : public piece
{
public:
    bishop(bool isWhite, pos startingPosition);
    void checkMoves(board &Board, pos currPosition)override;
};


class king : public piece
{
private: 
    bool canCastle, canKingside, canQueenside ;
public:
    king(bool White, pair<int, int> startingPosition);
    void checkMoves(board &Board, pos position)override;
    bool canKingCastle();
    bool canKingsideCastle (board &Board);
    bool canQueensideCastle (board &Board);
    void setKingsideCastle ();
    void setQueensideCastle();
    bool getKingsideCastle ();
    bool getQueensideCastle();
    void resetKingsideCastle ();
    void resetQueensideCastle();
    void resetCastling();
    bool isCheck(board &Board,pos targetSquare);
};

class Knight : public piece
{
    public:
    Knight(bool White, pos startingPosition);
    void checkMoves(board &Board, pos currPosition)override;
};


class pawn : public piece
{
    private:
    
    bool enpassant = false;
    
    public:
    bool ISenpassant();
    void setenpassant();
    void resetenpassant();
    pawn(bool isWhite,pos startingPosition);
    void checkMoves(board &Board,pos currPosition)override;
};



class queen : public piece
{
public:
queen(bool White, pos startingPosition);
void checkMoves(board &Board, pos currPosition)override;
};



