#include "game.h"
#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <regex>
#include <algorithm>
#include <thread>
#include <chrono>
#include <ctime>
#include <cstring>
#include <limits>
#include <cctype>

game::game()
{
}

game::~game()
{
}

void game ::run(board &board)
{
    regex e("^[a-hA-H][1-8]$");
    while (true)
    {
        bool whiteTurn = board.isWhiteTurn();
        clearScreen();
        // //debug zobrist History
        //     ofstream outputFile("output.txt", ios::app);
        //     if (!outputFile.is_open()) {
        //         cerr << "Error opening file!" << endl;
        //         return;
        //     }
        //     streambuf* originalCoutBuffer = cout.rdbuf();
        //     cout.rdbuf(outputFile.rdbuf());
        //     cout << "Last Hash = " << board.getPreviousHash() << "  Repeated = "<< board.repeted(board.getPreviousHash())<< endl;
        //     cout.rdbuf(originalCoutBuffer);
        //     outputFile.close();
        // //endofdebug

        if (board.isCheckmate(whiteTurn))
        {
            cout << "White Wins By Checkmate." << endl;
            this_thread::sleep_for(chrono::seconds(3));
            return;
        }
        else if (board.isCheckmate(!whiteTurn))
        {
            cout << "Black Wins By Checkmate." << endl;
            this_thread::sleep_for(chrono::seconds(3));
            return;
        }
        else if (board.isStalemate(whiteTurn))
        {
            cout << "Draw By Stalemate." << endl;
            this_thread::sleep_for(chrono::seconds(3));
            return;
        }
        else if (board.isInsufficientMaterial())
        {
            cout << "Draw By Insufficient Material." << endl;
            this_thread::sleep_for(chrono::seconds(3));
            return;
        }
        else if (board.is75MoveDraw())
        {
            cout << "Draw By 75 move rule." << endl;
            this_thread::sleep_for(chrono::seconds(3));
            return;
        }
        else if (board.is50MoveDraw())
        {
            cout << "Do you want to claim draw By 50 move rule? (y/n) : " << endl;
            string choice;
            getline(cin, choice);
            if (tolower(choice[0]) == 'y')
            {
                cout << "Draw By 50 move rule." << endl;
            this_thread::sleep_for(chrono::seconds(3));
                return;
            }
        }
        whiteTurn ? board.printBoardW() : board.printBoardB();
        string input;
        cout << "Select a piece (e.g., e2 e4) , type exit to quit or save to save the game: ";
        getline(cin, input);
        transform(input.begin(), input.end(), input.begin(), ::tolower);
        if (input == "exit")
        {
            cout << "Exiting the game." << endl;
            return;
        }
        else if (input == "save")
        {
            saveGame(board);
            continue;
        }

        else if (regex_match(input, e))
        {
            pos position = stringTOmove(input);
            piece *selected = board.getAt(position);
            if (selected == nullptr || whiteTurn != selected->isWhite())
            {
                cout << "Illegal move" << endl;
                this_thread::sleep_for(chrono::seconds(2));
                continue;
            }

            clearScreen();
            whiteTurn ? board.printBoardW() : board.printBoardB();
            selected->checkMoves(board, position);
            set<pos> moves = selected->getPossibleMoves();
            set<pos> captures = selected->getPossibleCaptures();
            if (moves.empty())
            {
                cout << "No possible moves" << endl;
                this_thread::sleep_for(chrono::seconds(2));
                continue;
            }
            cout << "Selected piece: " << selected->getprintableValue() << "  at " << input << endl;
            cout << "Possible moves (Red is capturable): ";
            for (pos move : moves)
            {
                string moveString = moveTOstring(move);
                if (binary_search(captures.begin(), captures.end(), move))
                {
                    moveString = "\x1b[31m" + moveString + "\x1b[0m";
                }
                cout << moveString << "  ";
            }
            cout << endl;
            cout << "Play a move or type \"back\" to select another piece : ";
            getline(cin, input);
            transform(input.begin(), input.end(), input.begin(), ::tolower);
            if (input == "back")
                continue;
            else
            {
                if (regex_match(input, e) && selected->Move(board, stringTOmove(input)))
                {
                    if (board.isTrifoldDraw())
                    {
                        cout << "Draw By Repetition" << endl;
                        return;
                    }
                    board.switchTurns();
                    continue;
                }
                else
                {
                    cout << "Illegal move" << endl;
                    this_thread::sleep_for(chrono::seconds(2));
                    continue;
                }
            }
        }
        else
        {
            cout << "Invalid position" << endl;
            this_thread::sleep_for(chrono::seconds(2));
            continue;
        }
    }
}
void game::saveGame(board &board)
{
    // saving into a csv file with three columns (Date&Time,SaveName,FEN)
    cout << "Enter a name for the saved game: ";
    string saveName;
    getline(cin, saveName);
    ofstream outFile("SavedGames.csv", ios::app);
    if (outFile)
    {
        time_t now = time(0);
        char *dt = ctime(&now);
        dt[strlen(dt) - 1] = '\0';
        outFile << dt << "," << saveName << "," << buildFEN(board) << endl;
        outFile.close();
        cout << "Game saved successfully." << endl;
    }
    else
    {
        cerr << "Failed to save game." << endl;
    }
}

void game::loadGame()
{
    // lists all games available with options to choose between them
    ifstream inFile("SavedGames.csv");
    if (!inFile)
    {
        cerr << "No saved games found." << endl;
        this_thread::sleep_for(chrono::seconds(2));
        return;
    }

    vector<string> lines;
    string line;
    while (getline(inFile, line))
    {
        lines.push_back(line);
    }
    inFile.close();

    if (lines.empty())
    {
        cerr << "No saved games found." << endl;
        this_thread::sleep_for(chrono::seconds(2));
        return;
    }

    cout << "Available saved games:" << endl;
    for (size_t i = 0; i < lines.size(); i++)
    {
        stringstream ss(lines[i]);
        string dateTime, saveName, FEN;
        getline(ss, dateTime, ',');
        getline(ss, saveName, ',');
        getline(ss, FEN, ',');

        cout << i + 1 << ". " << saveName << " (Saved on: " << dateTime << ")" << endl;
    }
    string FEN;
    board* loadedBoard;
    while (true)
    {
        
        cout << "Enter the number of the game you want to load (or 0 to cancel): ";
        int choice;
        cin >> choice;


        if (cin.fail())
        {
            cerr << "Invalid input. Please enter a number." << endl;
            cin.clear();
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
            this_thread::sleep_for(chrono::seconds(2));
            continue;
        }
        if (choice == 0)
        {
            cout << "Load cancelled." << endl;
            cin.clear();
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
            this_thread::sleep_for(chrono::seconds(2));
            return;
        }
        if (choice < 1 || static_cast<size_t>(choice) > lines.size())
        {
            cerr << "Invalid choice." << endl;
            cin.clear();
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
            this_thread::sleep_for(chrono::seconds(2));
            continue;
        }

        cin.clear();
        cin.ignore(numeric_limits<streamsize>::max(), '\n');    
        stringstream ss(lines[choice - 1]);
        string dateTime, saveName;
        getline(ss, dateTime, ',');
        getline(ss, saveName, ',');
        getline(ss, FEN, ',');
        loadedBoard = board::boardFromFEN(FEN);
        if (loadedBoard== nullptr)
        {
            cerr << "Failed to load the selected game." << endl;
            this_thread::sleep_for(chrono::seconds(2));
            return;
        }
        else
        {
            break;
        }

    }
    this->run(*loadedBoard);
    


}
