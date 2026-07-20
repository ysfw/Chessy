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

    bool isWhite() const;
    char getType() const;
    pos getPosition() const;
    set<pos> getPossibleMoves ();
    set<pos> getPossibleCaptures ();
    string getprintableValue();

    void updatePos(pos newPosition);
    void addPossibleCapture(pos move);
    void setType(char newType);
    void addPossibleMove(pos move);
    void setprintableValue(string);

    virtual bool canCastle() const { return false; }
    virtual bool attacks(pos /*target*/, board& /*b*/) const { return false; }


    virtual bool Move(board &Board,pos newPosition);
    virtual void checkMoves(board &Board, pos currPosition);
    void clearMoves();
};


class rook : public piece {
private:
    bool hasCastlingRights;
public:
   rook(bool White, pos startingPosition);
   
   bool canCastle() const override;
   bool attacks(pos target, board& b) const override;
   void resetCastling();
   
   void checkMoves(board &Board, pos currPos)override;
   bool Move(board &Board,pos newPosition)override;
};


class bishop : public piece
{
public:
    bishop(bool isWhite, pos startingPosition);
    
    void checkMoves(board &Board, pos currPosition)override;
    bool attacks(pos target, board& b) const override;
};


class king : public piece
{
private: 
    bool canCastle, canKingside, canQueenside ;
public:
    king(bool White, pair<int, int> startingPosition);
    void checkMoves(board &Board, pos position)override;
    bool Move(board &Board,pos newPosition) override;
    bool attacks(pos target, board& b) const override;

    bool canKingCastle();
    
    bool canKingsideCastle (board &Board);
    bool canQueensideCastle (board &Board);
    
    bool getKingsideCastle ();
    bool getQueensideCastle();
    
    void setKingsideCastle ();
    void setQueensideCastle();
    void resetKingsideCastle ();
    void resetQueensideCastle();
    void resetCastling();
    
    bool isCheck(board &Board,pos targetSquare);
};

class knight : public piece
{
    public:
    knight(bool White, pos startingPosition);
    
    void checkMoves(board &Board, pos currPosition)override;
    bool attacks(pos target, board& b) const override;

};


class pawn : public piece
{
    private:
    bool enpassant = false;
    
    public:
    bool isEnpassant();
    void setenpassant();
    void resetenpassant();

    pawn(bool isWhite,pos startingPosition);
    
    void checkMoves(board &Board,pos currPosition)override;
    bool Move(board &Board,pos newPosition) override;
    bool attacks(pos target, board& b) const override;

};



class queen : public piece
{
    public:
    queen(bool White, pos startingPosition);

    void checkMoves(board &Board, pos currPosition)override;
    bool attacks(pos target, board& b) const override;
};