// src/main.cpp
#include <iostream>
#include <vector>
#include <iomanip>
#include "allocator.hpp"
using namespace std;

int main() {
    initialize_memory();
    string command;
    cout << "Mini Runtime Allocator (type 'help' for commands)\n";
    while (true) {
        cout << "> ";
        cin >> command;
        if (command == "alloc") {
            size_t sz;
            cin >> sz;
            int id = allocate(sz);
            if (id == -1)
                cout << "Allocation failed\n";
            else
                cout << "Allocated ID: " << id << "\n";
        } else if (command == "free") {
            int id;
            cin >> id;
            if (free_block(id))
                cout << "Freed ID: " << id << "\n";
            else
                cout << "Free failed\n";
        } else if (command == "strategy") {
            string strat;
            cin >> strat;
            if (strat == "first")
                set_strategy(FirstFit);
            else if (strat == "best")
                set_strategy(BestFit);
            else if (strat == "worst")
                set_strategy(WorstFit);
            else if (strat == "Buddy")
                set_strategy(Buddy);
            else
                cout << "Unknown strategy\n";
        }else if (command == "show") {
            show_memory();
        } else if (command == "exit") {
            break;
        } else if (command == "help") {
            cout << "Commands:\n  alloc <size>  - Allocate memory\n  free <id>     - Free block by ID\n  show          - Show memory layout\n  exit          - Quit\n";
        } else if (command == "frag" || command == "stats") {
            show_fragmentation_stats();
        }else {
            cout << "Unknown command\n";
        }
    }
    return 0;
}
