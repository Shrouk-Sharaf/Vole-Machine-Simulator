#include "VoleMachine.h"

int main() {
    Simulator simulator;
    int choice;
    string filename;

    while (true) {
        cout << "\n--- Simulator Menu ---\n";
        cout << "1. Load a new program\n";
        cout << "2. Run the program step-by-step\n";
        cout << "3. Run the entire program\n";
        cout << "4. Display current state\n";
        cout << "5. Reset simulator\n";
        cout << "6. Exit\n";
        cout << "Enter your choice:\n";
        cin >> choice;
        if (cin.fail()) {
            cin.clear();
            cin.ignore();
            cout << "Invalid input. Please enter a number between 1 and 6.\n";
            continue;
        }
        switch (choice) {
            case 1:
                cout << "Enter the filename of the program to load: ";
                cin.ignore();
                getline(cin, filename);
                simulator.loadProgram(filename);
                break;
            case 2:
                simulator.runStepByStep();
                break;
            case 3:
                simulator.run_entire_program();
                break;
            case 4:
                simulator.displayState();
                break;
            case 5:
                simulator.reset();
                break;
            case 6:
                return 0;
            default:
                cout << "Invalid choice. Try again.\n";
                break;
        }
    }
}