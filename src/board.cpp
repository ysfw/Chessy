#include "board.h"
#include <iostream>
#include <regex>
#include <cctype>
#include <random>
#include <map>

piece *board::createPieceFromChar(char pieceChar, pos position)
{
    bool isWhite = (isupper(pieceChar) != 0);

    switch (tolower(pieceChar))
    {
    case 'r':
        isWhite ? whiteRooks++ : blackRooks++;
        return new rook(isWhite, position);
    case 'n':
        isWhite ? whiteKnights++ : blackKnights++;
        return new Knight(isWhite, position);
    case 'b':
        isWhite ? whiteBishops++ : blackBishops++;
        return new bishop(isWhite, position);
    case 'q':
        isWhite ? whiteQueens++ : blackQueens++;
        return new queen(isWhite, position);
    case 'k':
        isWhite ? whiteKingPosition = position : blackKingPosition = position;
        return new king(isWhite, position);
    case 'p':
        isWhite ? whitePawns++ : blackPawns++;
        return new pawn(isWhite, position);
    default:
        return nullptr;
    }
}

bool board::isWhiteTurn()
{
    return whiteTurn;
}
void board::switchTurns()
{
    whiteTurn = !whiteTurn;
}
void board::setWhitecaptured()
{
    didWhiteCapture = true;
}

void board::resetWhitecaptured()
{
    didWhiteCapture = false;
}

bool board::DidWhitecapture()
{
    return didWhiteCapture;
}

void board::initZobrist()
{
    mt19937_64 randomEngine(12345);
    uniform_int_distribution<uint64_t> distribution;

    for (int p = 0; p < 6; ++p)
    {
        for (int c = 0; c < 2; ++c)
        {
            for (int s = 0; s < 64; ++s)
            {
                zobristTable[p][c][s] = distribution(randomEngine);
            }
        }
    }

    WhiteTurnkey = distribution(randomEngine);
    for (int i = 0; i < 4; ++i)
        castlingKeys[i] = distribution(randomEngine);
    for (int i = 0; i < 8; ++i)
        enPassantFileKeys[i] = distribution(randomEngine);
}

uint64_t board ::calculateintitialZobristHash()
{
    uint64_t hash = 0;
    hash ^= WhiteTurnkey;
    enum PieceType
    {
        PAWN,
        KNIGHT,
        BISHOP,
        ROOK,
        QUEEN,
        KING
    };
    static const map<char, int> pieceType = {
        {'p', PAWN}, {'n', KNIGHT}, {'b', BISHOP}, {'r', ROOK}, {'q', QUEEN}, {'k', KING}};
    // XOR keys for each piece
    for (int i = 0; i < 8; i++)
    {
        for (int j = 0; j < 8; j++)
        {
            piece *piece = this->getAt({i, j});
            if (piece != nullptr)
            {
                char type = piece->getType();
                int color = piece->isWhite() ? 1 : 0;
                uint64_t pieceHash = zobristTable[pieceType.at(type)][color][8 * i + j];
                hash ^= pieceHash;

                if (king *k = dynamic_cast<king *>(piece))
                {
                    if (k->isWhite())
                    {
                        if (k->canKingsideCastle(*this))
                            hash ^= castlingKeys[2];
                        if (k->canQueensideCastle(*this))
                            hash ^= castlingKeys[3];
                    }
                    else
                    {
                        if (k->canKingsideCastle(*this))
                            hash ^= castlingKeys[0];
                        if (k->canQueensideCastle(*this))
                            hash ^= castlingKeys[1];
                    }
                }
            }
        }
    }

    int epFile = this->getEnPassantFile();
    if (epFile != NO_FILE)
    {
        hash ^= enPassantFileKeys[epFile];
    }
    return hash;
}

uint64_t board::getPiecehash(char piece, bool isWhite, pos position)
{
    int color = isWhite ? 1 : 0;
    enum PieceType
    {
        PAWN,
        KNIGHT,
        BISHOP,
        ROOK,
        QUEEN,
        KING
    };
    static const map<char, int> pieceType = {
        {'p', PAWN}, {'n', KNIGHT}, {'b', BISHOP}, {'r', ROOK}, {'q', QUEEN}, {'k', KING}};

    return zobristTable[pieceType.at(piece)][color][(int)position.first * 8 + (int)position.second];
}

uint64_t board::getPreviousHash()
{
    return this->PreviousHash;
}
void board::setPreviousHash(uint64_t newHash)
{
    this->PreviousHash = newHash;
}

uint64_t board::getWhiteTurnkey()
{
    return this->WhiteTurnkey;
}
uint64_t board::getcastlingKey(int side, bool isWhite)
{
    return this->castlingKeys[side + isWhite];
}
uint64_t board::getenPassantFileKey(int file)
{
    return this->enPassantFileKeys[file];
}
void board::addHash(uint64_t newHash)
{
    if (this->zobristHistory.find(newHash) != zobristHistory.end())
    {
        zobristHistory[newHash]++;
    }
    else
    {
        this->zobristHistory.insert({newHash, 1});
    }
}

void board::minusFullMove()
{
    fullmoves--;
}
void board::plusFullMove()
{
    fullmoves++;
}
void board::minusHalfMoveNoCaptures()
{
    halfmovesNoCaptures--;
}
void board::plusHalfMoveNoCaptures()
{
    halfmovesNoCaptures++;
}

void board::resetHalfMovesNoCaptures()
{
    halfmovesNoCaptures = 0;
}

board::board(bool fullBoard)
    : Board(8, vector<piece *>(8, nullptr))
{
    whiteQueens = 0, whiteRooks = 0, whiteBishops = 0, whiteKnights = 0, whitePawns = 0;
    blackQueens = 0, blackRooks = 0, blackBishops = 0, blackKnights = 0, blackPawns = 0;
    fullmoves = 0, halfmovesNoCaptures = 0;
    whiteTurn = true;
    enPassantFile = NO_FILE;
    initZobrist();
    if (fullBoard)
    {
        for (int rank = 0; rank < 8; rank++)
        {
            for (int file = 0; file < 8; file++)
            {
                char pieceChar = layout[rank][file];
                if (pieceChar != '.')
                {
                    Board[rank][file] = createPieceFromChar(pieceChar, {rank, file});
                }
            }
        }
        fullmoves = 1;
        king *wk = dynamic_cast<king *>(getAt(getKingPosition(true)));
        king *bk = dynamic_cast<king *>(getAt(getKingPosition(false)));
        wk->setKingsideCastle();
        wk->setQueensideCastle();
        bk->setKingsideCastle();
        bk->setQueensideCastle();
    }
    uint64_t inititalHash = calculateintitialZobristHash();
    this->addHash(inititalHash);
    PreviousHash = inititalHash;
}
board *board::boardFromFEN(string FEN)
{
    regex e(R"(^([rnbqkpRNBQKP1-8]+\/){7}[rnbqkpRNBQKP1-8]+\s[wb]\s(?:K?Q?k?q?|-)\s(?:[a-h][36]|-)\s\d+\s[1-9]\d*$)");
    if (!regex_match(FEN, e))
    {
        return nullptr;
    }

    board *Board = new board(false);
    vector<string> splitFENstrs = splitFEN(FEN);
    bool loadedwhiteKing = false, loadedblackKing = false;
    int rank = 7, file = 0;
    for (char a : splitFENstrs[0])
    {
        if (a == '/')
        {
            if (file != 8)
            {
                delete Board;
                return nullptr;
            }

            file = 0;
            rank--;
            continue;
        }

        if (file > 7 || Board->whitePawns > 8 || Board->blackPawns > 8)
        {
            delete Board;
            return nullptr;
        }

        if (isdigit(a))
        {
            file += (a - '0');
            continue;
        }
        else if (file == 8)
            break;
        else
        {
            if ((a == 'k' && loadedblackKing) || (a == 'K' && loadedwhiteKing))
            {
                delete Board;
                return nullptr;
            }

            else if ((a == 'k' && !loadedblackKing))
                loadedblackKing = true;
            else if ((a == 'K' && !loadedwhiteKing))
                loadedwhiteKing = true;

            if ((a == 'p' || a == 'P') && (rank == 7 || rank == 0))
            {
                delete Board;
                return nullptr;
            }

            Board->setAt({rank, file}, Board->createPieceFromChar(a, {rank, file}));
            file++;
        }
    }

    if (rank != 0 || file != 8)
    {
        delete Board;
        return nullptr; // The FEN did not describe a full 8x8 board.
    }

    if (splitFENstrs[1][0] == 'w')
    {
        if (Board->AttackedBy(Board->blackKingPosition, false))
        {
            delete Board;
            return nullptr;
        }

        Board->whiteTurn = true;
    }
    else
    {
        if (Board->AttackedBy(Board->whiteKingPosition, true))
        {
            delete Board;
            return nullptr;
        }

        Board->whiteTurn = false;
    }

    king *wk = dynamic_cast<king *>(Board->getAt(Board->whiteKingPosition));
    king *bk = dynamic_cast<king *>(Board->getAt(Board->blackKingPosition));

    for (char flag : splitFENstrs[2])
    {
        switch (flag)
        {
        case 'K':
        {
            piece *r = Board->getAt({0, 7});
            if (Board->getKingPosition(true) != pos{0, 4} || r == nullptr || r->getType() != 'r')
            {
                delete Board;
                return nullptr;
            }
            wk->setKingsideCastle();
            break;
        }
        case 'Q':
        {
            piece *r = Board->getAt({0, 0});
            if (Board->getKingPosition(true) != pos{0, 4} || r == nullptr || r->getType() != 'r')
            {
                delete Board;
                return nullptr;
            }
            wk->setQueensideCastle();
            break;
        }
        case 'k':
        {
            piece *r = Board->getAt({7, 7});
            if (Board->getKingPosition(false) != pos{7, 4} || r == nullptr || r->getType() != 'r')
            {
                delete Board;
                return nullptr;
            }
            bk->setKingsideCastle();
            break;
        }
        case 'q':
        {
            piece *r = Board->getAt({7, 0});
            if (Board->getKingPosition(false) != pos{7, 4} || r == nullptr || r->getType() != 'r')
            {
                delete Board;
                return nullptr;
            }
            bk->setQueensideCastle();
            break;
        }
        }
    }

    if (splitFENstrs[3] != "-")
    {
        pos newenpassantpos = stringTOmove(splitFENstrs[3]);
        Board->setEnpassantstr(splitFENstrs[3]);

        if ((newenpassantpos.first == 5 && !Board->whiteTurn) || (newenpassantpos.first == 2 && Board->whiteTurn))
        {
            delete Board;
            return nullptr;
        }
        if (Board->getAt(newenpassantpos) != nullptr ||
            (Board->whiteTurn && Board->getAt({newenpassantpos.first + 1, newenpassantpos.second}) != nullptr) ||
            (!Board->whiteTurn && Board->getAt({newenpassantpos.first - 1, newenpassantpos.second}) != nullptr))
        {
            delete Board;
            return nullptr;
        }
        Board->enPassantFile = newenpassantpos.second;

        if (Board->whiteTurn)
        {
            pawn *p = dynamic_cast<pawn *>(Board->getAt({newenpassantpos.first - 1, newenpassantpos.second}));
            if (p != nullptr)
            {
                p->setenpassant();
                Board->setEnpassant();
            }
            else
            {
                delete Board;
                return nullptr;
            }
        }

        if (!Board->whiteTurn)
        {
            pawn *p = dynamic_cast<pawn *>(Board->getAt({newenpassantpos.first + 1, newenpassantpos.second}));
            if (p != nullptr)
            {
                p->setenpassant();
                Board->setEnpassant();
            }
            else
            {
                delete Board;
                return nullptr;
            }
        }
    }
    else
        Board->enPassantFile = NO_FILE;

    Board->halfmovesNoCaptures = (stoi(splitFENstrs[4]));
    Board->fullmoves = stoi(splitFENstrs[5]);

    uint64_t inititalHash = Board->calculateintitialZobristHash();
    Board->addHash(inititalHash);
    Board->setPreviousHash(inititalHash);
    return Board;
}
board::~board()
{
    for (int i = 0; i < 8; i++)
    {
        for (int j = 0; j < 8; j++)
            delete this->getAt({i, j});
    }
}

void board::minusPiece(char pieceType, bool isWhite)
{
    if (isWhite)
    {
        switch (pieceType)
        {
        case 'q':
            whiteQueens--;
            break;
        case 'r':
            whiteRooks--;
            break;
        case 'b':
            whiteBishops--;
            break;
        case 'n':
            whiteKnights--;
            break;
        case 'p':
            whitePawns--;
            break;
        default:
            break;
        }
    }
    else
    {
        switch (pieceType)
        {
        case 'q':
            blackQueens--;
            break;
        case 'r':
            blackRooks--;
            break;
        case 'b':
            blackBishops--;
            break;
        case 'n':
            blackKnights--;
            break;
        case 'p':
            blackPawns--;
            break;
        default:
            break;
        }
    }
}

void board::plusPiece(char pieceType, bool isWhite)
{
    if (isWhite)
    {
        switch (pieceType)
        {
        case 'q':
            whiteQueens++;
            break;
        case 'r':
            whiteRooks++;
            break;
        case 'b':
            whiteBishops++;
            break;
        case 'n':
            whiteKnights++;
            break;
        case 'p':
            whitePawns++;
            break;
        default:
            break;
        }
    }
    else
    {
        switch (pieceType)
        {
        case 'q':
            blackQueens++;
            break;
        case 'r':
            blackRooks++;
            break;
        case 'b':
            blackBishops++;
            break;
        case 'n':
            blackKnights++;
            break;
        case 'p':
            blackPawns++;
            break;
        default:
            break;
        }
    }
}

piece *board ::getAt(pos position)
{
    return Board[position.first][position.second];
}

void board ::setAt(pos position, piece *Pointer2piece)
{
    Board[position.first][position.second] = Pointer2piece;
}

bool board ::isEnpassant()
{
    return enpassant;
}

void board ::setEnpassant()
{
    enpassant = true;
}

void board ::resetEnpassant()
{
    enpassant = false;
}

int board::getEnPassantFile()
{
    return this->enPassantFile;
}

void board::setEnPassantFile(int file)
{
    enPassantFile = file;
}

void board::setEnpassantstr(string pos)
{
    this->enpassantPositionstr = pos;
}

string board::getEnpassantstr()
{
    return this->enpassantPositionstr;
}

void board::resetEnPassantFile()
{
    enPassantFile = NO_FILE;
}

int board::repeted(uint64_t key)
{
    return zobristHistory[key];
}

void board::setKingPosition(bool isWhite, pos newPosition)
{
    isWhite ? whiteKingPosition = newPosition : blackKingPosition = newPosition;
}

pos board ::getKingPosition(bool isWhite)
{
    return isWhite ? whiteKingPosition : blackKingPosition;
}


bool board::AttackedBy(pos Position, bool isDefenderWhite)
{
    for (size_t i = 0; i < 8; i++)
    {
        for (size_t j = 0; j < 8; j++)
        {
            piece *current = this->getAt({i, j});
            if (current == nullptr || current->isWhite() == isDefenderWhite)
                continue;

            if (pawn *p = dynamic_cast<pawn *>(current))
            {
                size_t forwardOne = p->isWhite() ? p->getPosition().first + 1 : p->getPosition().first - 1;
                if (forwardOne == Position.first && (Position.second == p->getPosition().second + 1 || Position.second == p->getPosition().second - 1))
                    return true;
            }
            else if (Knight *N = dynamic_cast<Knight *>(current))
            {
                pair<int, int> directions[8] = {{2, 1}, {2, -1}, {-2, 1}, {-2, -1}, {1, 2}, {-1, 2}, {1, -2}, {-1, -2}};
                for (pair<int, int> direction : directions)
                {
                    int firstCoord = (int)N->getPosition().first + direction.first;
                    int secondCoord = (int)N->getPosition().second + direction.second;
                    if ((firstCoord < 8 && firstCoord >= 0 && secondCoord < 8 && secondCoord >= 0) &&
                        ((size_t)firstCoord == Position.first && (size_t)secondCoord == Position.second))
                        return true;
                }
            }
            else if (dynamic_cast<rook *>(current))
            {
                // Check straight lines (rook movement)
                pair<int, int> dirs[4] = {{0, 1}, {0, -1}, {1, 0}, {-1, 0}};
                for (auto &dir : dirs)
                {
                    for (size_t k = 1; k < 8; k++)
                    {
                        size_t firstCoord = i + dir.first * k;
                        size_t secondCoord = j + dir.second * k;
                        if (firstCoord >= 8 || secondCoord >= 8)
                            break;
                        if (firstCoord == Position.first && secondCoord == Position.second)
                            return true;
                        if (this->getAt({firstCoord, secondCoord}) != nullptr)
                            break;
                    }
                }
            }
            else if (bishop *B = dynamic_cast<bishop *>(current))
            {
                // Check diagonals (bishop movement)
                pair<int, int> dirs[4] = {{1, 1}, {1, -1}, {-1, 1}, {-1, -1}};
                for (auto &dir : dirs)
                {
                    for (size_t k = 1; k < 8; k++)
                    {
                        size_t firstCoord = B->getPosition().first + dir.first * k;
                        size_t secondCoord = B->getPosition().second + dir.second * k;
                        if (firstCoord >= 8 || secondCoord >= 8)
                            break;
                        if (firstCoord == Position.first && secondCoord == Position.second)
                            return true;
                        if (this->getAt({firstCoord, secondCoord}) != nullptr)
                            break;
                    }
                }
            }
            else if (queen *Q = dynamic_cast<queen *>(current))
            {
                // Check all 8 directions (queen = rook + bishop)
                pair<int, int> dirs[8] = {{0, 1}, {0, -1}, {1, 0}, {-1, 0}, {1, 1}, {1, -1}, {-1, 1}, {-1, -1}};
                for (auto &dir : dirs)
                {
                    for (size_t k = 1; k < 8; k++)
                    {
                        size_t firstCoord = Q->getPosition().first + dir.first * k;
                        size_t secondCoord = Q->getPosition().second + dir.second * k;
                        if (firstCoord >= 8 || secondCoord >= 8)
                            break;
                        if (firstCoord == Position.first && secondCoord == Position.second)
                            return true;
                        if (this->getAt({firstCoord, secondCoord}) != nullptr)
                            break;
                    }
                }
            }
            else if (king *K = dynamic_cast<king *>(current))
            {
                pair<int, int> directions[8] = {{0, 1}, {0, -1}, {-1, 0}, {1, 0}, {1, 1}, {1, -1}, {-1, 1}, {-1, -1}};
                for (pair<int, int> direction : directions)
                {
                    int firstCoord = (int)K->getPosition().first + direction.first;
                    int secondCoord = (int)K->getPosition().second + direction.second;
                    if ((firstCoord < 8 && firstCoord >= 0 && secondCoord < 8 && secondCoord >= 0) &&
                        ((size_t)firstCoord == Position.first && (size_t)secondCoord == Position.second))
                        return true;
                }
            }
        }
    }
    return false;
}
bool board::isPinned(piece *p, pos newPosition)
{
    pos oldPosition = p->getPosition();
    piece *capturedPiece = this->getAt(newPosition);

    bool isEnPassantCapture = (dynamic_cast<pawn *>(p) && capturedPiece == nullptr && p->getPossibleCaptures().count(newPosition));
    pos enPassantPawnPos;
    piece *enPassantPawn = nullptr;
    if (isEnPassantCapture)
    {
        int capturedPawnRow = p->isWhite() ? newPosition.first - 1 : newPosition.first + 1;
        enPassantPawnPos = {(size_t)capturedPawnRow, newPosition.second};
        enPassantPawn = this->getAt(enPassantPawnPos);
    }

    this->setAt(newPosition, p);
    this->setAt(oldPosition, nullptr);
    if (isEnPassantCapture)
    {
        this->setAt(enPassantPawnPos, nullptr);
    }
    p->updatePos(newPosition);

    bool isKingAttacked = this->AttackedBy(this->getKingPosition(p->isWhite()), p->isWhite());

    this->setAt(oldPosition, p);
    this->setAt(newPosition, capturedPiece);
    if (isEnPassantCapture)
    {
        this->setAt(enPassantPawnPos, enPassantPawn);
    }
    p->updatePos(oldPosition);

    return isKingAttacked;
}

bool board::isCheckmate(bool isWhiteTurn)
{
    for (size_t i = 0; i < 8; i++)
    {
        for (size_t j = 0; j < 8; j++)
        {
            if ((this->getAt({i, j}) == nullptr))
                continue;
            if (this->getAt({i, j})->isWhite() != isWhiteTurn)
                continue;

            this->getAt({i, j})->checkMoves(*this, {i, j});
            if (!(this->getAt({i, j})->getPossibleMoves().empty()))
                return false;
        }
    }

    if ((isWhiteTurn && this->AttackedBy(whiteKingPosition, isWhiteTurn)) || (!isWhiteTurn && this->AttackedBy(blackKingPosition, isWhiteTurn)))
    {
        return true;
    }
    return false;
}

bool board::isStalemate(bool isWhiteTurn)
{
    for (size_t i = 0; i < 8; i++)
    {
        for (size_t j = 0; j < 8; j++)
        {
            if ((this->getAt({i, j}) == nullptr))
                continue;
            if (this->getAt({i, j})->isWhite() != isWhiteTurn)
                continue;

            this->getAt({i, j})->checkMoves(*this, {i, j});
            if (!(this->getAt({i, j})->getPossibleMoves().empty()))
                return false;
        }
    }
    if ((isWhiteTurn && !this->AttackedBy(whiteKingPosition, isWhiteTurn)) || (!isWhiteTurn && !this->AttackedBy(blackKingPosition, isWhiteTurn)))
    {
        return true;
    }
    return false;
}

bool board::isTrifoldDraw()
{
    for (auto &hash : this->zobristHistory)
    {
        if (hash.second >= 3)
        {
            return true;
        }
    }
    return false;
}

bool board::isInsufficientMaterial()
{

    // If any player has a Queen, Rook, or Pawn, it's not a draw.
    if (whiteQueens > 0 || whiteRooks > 0 || whitePawns > 0)
        return false;
    if (blackQueens > 0 || blackRooks > 0 || blackPawns > 0)
        return false;

    int totalBishops = whiteBishops + blackBishops;
    int totalKnights = whiteKnights + blackKnights;

    // King vs King, King + Bishop vs King, or King + Knight vs King
    if (totalBishops <= 1 && totalKnights <= 1)
    {
        return true;
    }

    // King + Two Knights vs King
    if (whiteKnights == 2 && blackBishops == 0 && blackKnights == 0)
        return true;
    if (blackKnights == 2 && whiteBishops == 0 && whiteKnights == 0)
        return true;

    // King + Bishop vs King + Bishop on same-colored squares
    if (blackBishops == 1 && whiteBishops == 1 && whiteKnights == 0 && blackKnights == 0)
    {
        bool WhiteBishopOnWhiteSquare = false, BlackBishopOnWhiteSquare = false;
        int bishopsFound = 0;
        for (size_t i = 0; i < 8; i++)
        {
            if (bishopsFound == 2)
                break;
            for (size_t j = 0; j < 8; j++)
            {
                if (bishopsFound == 2)
                    break;
                piece *target = this->getAt({i, j});
                if (target == nullptr)
                    continue;
                else if (bishop *b = dynamic_cast<bishop *>(target))
                {
                    bishopsFound++;
                    bool isWhite = b->isWhite();
                    if (isWhite && ((i + j) % 2 != 0))
                        WhiteBishopOnWhiteSquare = true;
                    if (!isWhite && ((i + j) % 2 != 0))
                        BlackBishopOnWhiteSquare = true;
                }
            }
        }
        if (WhiteBishopOnWhiteSquare == BlackBishopOnWhiteSquare)
            return true;
    }

    return false;
}

bool board::is50MoveDraw()
{
    return (halfmovesNoCaptures >= 100);
}
bool board::is75MoveDraw()
{
    return (halfmovesNoCaptures == 150);
}

void board ::printBoardB()
{
    for (size_t i = 0; i < 8; i++)
    {
        cout << i + 1 << ' ';
        for (int j = 7; j >= 0; j--)
        {
            if (Board[i][j] == nullptr)
                (i + j) % 2 == 0 ? cout << "□" << ' ' : cout << "■" << ' ';
            else
                cout << Board[i][j]->getprintableValue() << ' ';
        }
        cout << '\n';
    }
    cout << "  ";
    for (size_t i = 0; i < 8; i++)
        cout << static_cast<char>('h' - i) << ' ';
    cout << '\n';
}

void board ::printBoardW()
{
    for (int i = 7; i >= 0; i--)
    {
        cout << i + 1 << ' ';
        for (size_t j = 0; j < 8; j++)
        {
            if (Board[i][j] == nullptr)
                (i + j) % 2 == 0 ? cout << "□" << ' ' : cout << "■" << ' ';
            else
                cout << Board[i][j]->getprintableValue() << ' ';
        }
        cout << '\n';
    }
    cout << "  ";
    for (size_t i = 0; i < 8; i++)
        cout << static_cast<char>('a' + i) << ' ';
    cout << '\n';
}

int board::getHalfmovesNoCaptures()
{
    return halfmovesNoCaptures;
}

int board::getFullmoves()
{
    return this->fullmoves;
}

string buildFEN(board &board)
{
    string FEN = "";

    for (int i = 7; i >= 0; i--)
    {
        int emptyCounter = 0;
        for (int j = 0; j < 8; j++)
        {
            piece *curr = board.getAt({i, j});
            if (curr == nullptr)
                emptyCounter++;
            else
            {
                if (emptyCounter > 0)
                {
                    FEN += to_string(emptyCounter);
                    emptyCounter = 0;
                }

                if (curr->isWhite())
                {
                    FEN += toupper(curr->getType());
                }
                else
                {
                    FEN += curr->getType();
                }
            }
        }
        if (emptyCounter > 0)
            FEN += to_string(emptyCounter);
        if (i - 1 >= 0)
            FEN += '/';
        else
            FEN += ' ';
    }

    if (board.isWhiteTurn())
        FEN += "w ";
    else
        FEN += "b ";

    king *wk = dynamic_cast<king *>(board.getAt(board.getKingPosition(true)));
    king *bk = dynamic_cast<king *>(board.getAt(board.getKingPosition(false)));
    bool whiteKingside = wk->getKingsideCastle();
    bool whiteQueenside = wk->getQueensideCastle();
    bool blackKingside = bk->getKingsideCastle();
    bool blackQueenside = bk->getQueensideCastle();

    if (!(whiteKingside || whiteQueenside || blackKingside || blackQueenside))
        FEN += '-';
    else
    {
        if (whiteKingside)
            FEN += 'K';
        if (whiteQueenside)
            FEN += 'Q';
        if (blackKingside)
            FEN += 'k';
        if (blackQueenside)
            FEN += 'q';
    }

    FEN += ' ';

    if (!board.isEnpassant())
    {
        FEN += "- ";
    }
    else
    {
        if (board.getEnpassantstr() != "")
        {

            FEN += board.getEnpassantstr();
        }
        else
        {
            FEN += "-";
        }
        FEN += ' ';
    }

    FEN += to_string(board.getHalfmovesNoCaptures());
    FEN += ' ';
    FEN += to_string(board.getFullmoves());

    return FEN;
}
