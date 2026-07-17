#include "game.h"
#include <iostream>
#include <string>

int main (){
    game game;
   

    while (true) {
        clearScreen();
        cout << "--- Chessy ---" << endl;
        cout << "1. New Game" << endl;
        cout << "2. Load from FEN" << endl;
        cout << "3. Load Saved Game" << endl;
        cout << "4. Exit" << endl;
        cout << "Choose an option: ";

        string choice;
        getline(cin, choice);

        if (choice == "1") {
            board newBoard(true);
            game.run(newBoard);
            break;

        } else if (choice == "2") {
            cout << "Please enter the FEN string: " << endl;
            string fenString;
            getline(cin, fenString);

            if (auto boardOptional = board::boardFromFEN(fenString)) {

                board Board = *boardOptional; 
                game.run(Board); 
                break;      
            } else {
                cout << "Error: Invalid or malformed FEN string." << endl;
                cout << "Press Enter to return to the menu." << endl;
                cin.get();
            }

        } 
        else if (choice == "3")
        {
            cout << "Loading saved games..." << endl;
            clearScreen();
            game.loadGame();
            break;
        }
        else if (choice == "4")
        {
            cout << "Exiting." << endl;
            break;
        }
        else
        {
            cout << "Invalid choice. Press Enter to try again." << endl;
            cin.get();
        }
    }
}