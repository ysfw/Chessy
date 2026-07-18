#pragma once
#include <vector>
#include <string>
#include <unordered_map>
#include <cstdint>
#include "pieces/piece.h"
#include "pieces/piece_factory.h"
#include "helpers.h"

using namespace std;

const int NO_FILE = -1; 



class board
{
private:
    vector<vector<piece*>> Board;
    const string layout[8] = {
        "RNBQKBNR",
        "PPPPPPPP",
        "........",
        "........",
        "........",
        "........",
        "pppppppp",
        "rnbqkbnr"
    }; //White Pieces in Capital letters
    
    /*Using Zobrist table & keys for threefold repetition draw logic
    - this is way more efficient than saving the whole board 
    - it works by genarating a random number (key) for each piece of each color on each square
    in the beginning of each game and then Xor all keys for each position int a one unique hash
    that represents the whole game state
    - the use of uint64_t here is typically for portability and safety*/
    uint64_t zobristTable[6][2][64]; 
    uint64_t WhiteTurnkey;
    uint64_t castlingKeys[4];  // [BKSide, BQSide, WKSide , WQSide]
    uint64_t enPassantFileKeys[8];
    uint64_t calculateintitialZobristHash(); //Making it private because it won't be used each time only for the initial position
    void initZobrist();
    unordered_map<uint64_t,int> zobristHistory; // {hash, #occurences}
    uint64_t PreviousHash;
    pos whiteKingPosition;
    pos blackKingPosition;
    bool whiteTurn;
    int fullmoves;
    int halfmovesNoCaptures;
    bool enpassant = false;
    bool didWhiteCapture =false; 
    int enPassantFile;
    string enpassantPositionstr;
    // Piece creation is delegated to PieceFactory.
    // Board only updates its own counters and king position tracking after creation.
    piece* createAndRegisterPiece(char pieceChar, pos position);
    int whiteQueens, whiteRooks, whiteBishops, whiteKnights , whitePawns ;
    int blackQueens, blackRooks, blackBishops, blackKnights , blackPawns ;
public:
    board(bool fullBoard);
    // true for normal game -> initialized full board
    static board* boardFromFEN(string FEN);

    ~board();

    bool isWhiteTurn();
    void switchTurns();
    void setWhitecaptured();
    void resetWhitecaptured();
    bool DidWhitecapture();

    void minusPiece(char pieceType,bool isWhite);
    void plusPiece(char pieceType,bool isWhite);
    
    void minusFullMove();
    void plusFullMove();
    void minusHalfMoveNoCaptures();
    void plusHalfMoveNoCaptures();
    int getHalfmovesNoCaptures(); // for debugging purposes 
    int getFullmoves();
    void resetHalfMovesNoCaptures();
    
    void setAt(pos position,piece* Pointer2piece);
    piece* getAt(pos position);
    bool AttackedBy(pos Position,bool isDefenderWhite); //returns true if the square is attacked
    bool isPinned(piece* piece, pos newPosition);
    void setKingPosition(bool isWhite, pos newPosition);
    pos getKingPosition(bool isWhite);
    
    uint64_t getPiecehash(char piece,bool isWhite,pos position);
    uint64_t getPreviousHash();
    uint64_t getWhiteTurnkey();
    uint64_t getcastlingKey(int side,bool isWhite);
    uint64_t getenPassantFileKey(int file);
    void addHash (uint64_t newHash);
    void setPreviousHash(uint64_t newHash);

    int repeted(uint64_t key); // for debugging purposes 

    bool isEnpassant ();
    void setEnpassant();
    void resetEnpassant();
    int getEnPassantFile();
    void setEnPassantFile(int file);
    void resetEnPassantFile();
    void resetEnpassantFile();
    void setEnpassantstr(string pos);
    string getEnpassantstr();

    bool isCheckmate (bool isWhiteTurn);
    bool isStalemate (bool isWhiteTurn);
    bool isTrifoldDraw ();
    bool isInsufficientMaterial();
    bool is50MoveDraw();
    bool is75MoveDraw();

    void printBoardW();
    void printBoardB();
};

string buildFEN(board &board);
