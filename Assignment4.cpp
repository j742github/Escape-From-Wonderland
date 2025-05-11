// Alice's Quest: Escape from Wonderland
//long  assignment man...

#include <iostream>
#include <string>
#include <map>
#include <vector>
#include <fstream>
#include <sstream>

using namespace std;

// Forward declarations for class dependencies
class Game;         // Controls game logic and win/loss conditions
class Control;      // Parses and validates user input commands
class Locations;    // Manages all room data and movement between rooms
class Inventory;    // Stores items the player picks up, and allows usage
class Characters;   // Handles character interactions and combat

// ------------------ Game Class ------------------
class Game {
public:
    void start();                                       // Displays intro
    void endGame(bool win);                             // Ends game based on result
    bool checkWin(Inventory &inventory);                // Win condition check
};

// ------------------ Control Class ------------------
class Control {
private:
    map<string, string> validActions;                   // Maps user commands to actions

public:
    Control();                                          // Loads actions from file
    void loadActions();                                // Reads action mappings
    string processInput(string input);                 // Returns associated action
    bool isValidCommand(string command);               // Validates command
};

// ------------------ Locations Class ------------------
class Locations {
private:
    map<string, string> roomDescriptions;               // Description of each room
    map<string, map<string, string>> roomConnections;   // Connections (e.g., north -> another room)
    string currentRoom;                                 // Tracks where player is

public:
    Locations();                                       // Loads locations on creation
    void loadLocations();                              // Reads room data from file
    void describeRoom();                               // Prints room description
    bool move(string direction);                       // Tries to move in a direction
    string getCurrentRoom();                           // Returns current location name
};

// ------------------ Inventory Class ------------------
class Inventory {
private:
    vector<string> items;                              // Stores item names

public:
    void addItem(string item);                         // Adds item to inventory
    void removeItem(string item);                      // Drops item
    bool hasItem(string item);                         // Checks if player has item
    void listItems();                                  // Prints inventory
    void useItem(string item);                         // Uses item
};

// ------------------ Characters Class ------------------
class Characters {
private:
    map<string, string> characterDescriptions;         // Character names and descriptions
    map<string, string> responses;                     // Dialogue responses

public:
    Characters();                                      // Loads character data
    void loadCharacters();                             // Reads from characters.txt
    void describeCharacter(string name);               // Displays description
    void talkTo(string name);                          // Talks to character
    void askAbout(string name, string item);           // Asks character about item
    bool attack(string name, Inventory &inv, Game &g); // Combat handler
};

// ------------------ Game Class Definitions ------------------

void Game::start() {
    cout << "Welcome to Alice's Quest: Escape from Wonderland!" << endl;
    cout << "You find yourself at the entrance of a rabbit hole..." << endl;
}

void Game::endGame(bool win) {
    if (win) {
        cout << "Congratulations bruh! You have defeated the Red Queen and escaped Wonderland!" << endl;
    } else {
        cout << "Game Over. You did not make it out of Wonderland." << endl;
    }
}

bool Game::checkWin(Inventory &inventory) {
    return inventory.hasItem("flamingo mallet"); // This is the key to win
}

// ------------------ Control Class Definitions ------------------

Control::Control() {
    loadActions(); // Automatically load valid player actions when a Control object is created
}

// Reads from actions.txt and stores valid command mappings (e.g. "n" -> "go north")
void Control::loadActions() {
    ifstream file("actions.txt"); // Open the action definitions file
    string line, key, value;

    // Each line is formatted like: "n|go north"
    while (getline(file, line)) {
        stringstream ss(line);
        getline(ss, key, '|');    // Extract the command alias (e.g., "n")
        getline(ss, value);       // Extract the corresponding full command (e.g., "go north")
        validActions[key] = value; // Store in map for lookup
    }
}

// Processes raw player input and returns a normalized action (e.g. "n" → "go north")
string Control::processInput(string input) {
    for (auto &pair : validActions) {
        if (input == pair.first) {
            return pair.second; // Return the full version of the command
        }
    }
    return "Invalid command. Try again."; // If no match, return error message
}

// Checks if a command is valid (used for extra validation if needed)
bool Control::isValidCommand(string command) {
    return validActions.find(command) != validActions.end();
}



// ------------------ Locations Class Definitions ------------------

Locations::Locations() {
    loadLocations();               // Load all room data and connections from text file
    currentRoom = "rabbit hole";   // Set the starting location of the player
}

void Locations::loadLocations() {
    ifstream file("locations.txt"); // Open the file containing location data
    string line, room, description, connections;

    // Each line format in the file: room|description|direction1:destination1,direction2:destination2,...
    while (getline(file, line)) {
        stringstream ss(line);
        getline(ss, room, '|');           // Extract the name of the room
        getline(ss, description, '|');    // Extract the room's description
        getline(ss, connections);         // Extract the connection string

        roomDescriptions[room] = description; // Store the description for later use

        // Parse the connection string (e.g., "north:tea room,east:hedge maze")
        stringstream connStream(connections);
        string conn, dir, dest;
        while (getline(connStream, conn, ',')) {
            stringstream connPair(conn);
            getline(connPair, dir, ':');   // Direction command (e.g., "north")
            getline(connPair, dest);       // Destination room (e.g., "tea room")
            roomConnections[room][dir] = dest; // Map current room's direction to destination room
        }
    }
}

void Locations::describeRoom() {
    // Print the description of the current room
    cout << roomDescriptions[currentRoom] << endl;
}

bool Locations::move(string direction) {
    // Check if there's a valid path in the given direction from the current room
    if (roomConnections[currentRoom].find(direction) != roomConnections[currentRoom].end()) {
        currentRoom = roomConnections[currentRoom][direction]; // Move to the new room
        describeRoom(); // Show the new room's description
        return true;
    }

    // If there's no valid path in that direction
    cout << "You can't go that way." << endl;
    return false;
}

string Locations::getCurrentRoom() {
    // Return the name of the room the player is currently in
    return currentRoom;
}

// ------------------ Inventory Class Definitions ------------------

// Adds an item to the inventory
void Inventory::addItem(string item) {
    items.push_back(item); // Store the item in the vector
    cout << "You have taken the " << item << "." << endl;
}

// Removes an item from the inventory
void Inventory::removeItem(string item) {
    // Loop through the vector to find the item
    for (auto it = items.begin(); it != items.end(); ++it) {
        if (*it == item) {
            items.erase(it); // Remove it if found
            cout << "You have dropped the " << item << "." << endl;
            return;
        }
    }
    // If item wasn't found
    cout << "You don't have that item." << endl;
}

// Checks if the player has a specific item
bool Inventory::hasItem(string item) {
    // Look through the inventory
    for (const string &i : items) {
        if (i == item) {
            return true; // Found it
        }
    }
    return false; // Didn't find it
}

// Lists all items the player is carrying
void Inventory::listItems() {
    if (items.empty()) {
        cout << "Your inventory is empty." << endl;
        return;
    }

    cout << "You are carrying: ";
    for (const string &item : items) {
        cout << item << ", "; // Print each item followed by a comma
    }
    cout << endl;
}

// Simulates using an item (only prints a message)
void Inventory::useItem(string item) {
    if (hasItem(item)) {
        cout << "You used the " << item << ". Something happens..." << endl;
        // You could add special logic here for different items
    } else {
        cout << "You don't have that item." << endl;
    }
}

// ------------------ Characters Class Definitions ------------------

// Constructor: Automatically loads character data from file when Characters object is created
Characters::Characters() {
    loadCharacters();
}

// Reads characters.txt and fills characterDescriptions and responses maps
void Characters::loadCharacters() {
    ifstream file("characters.txt"); // Open the character data file
    string line, name, description, response;

    // Each line contains a character's name, description, and response
    while (getline(file, line)) {
        stringstream ss(line);
        getline(ss, name, '|');          // Read the character's name
        getline(ss, description, '|');   // Read the character's description
        getline(ss, response);           // Read what the character says

        characterDescriptions[name] = description; // Store the description
        responses[name] = response;               // Store the default response
    }
}

// Prints the description of a character if they exist in the map
void Characters::describeCharacter(string name) {
    if (characterDescriptions.find(name) != characterDescriptions.end()) {
        cout << characterDescriptions[name] << endl;
    } else {
        cout << "You dont see anyone by that name." << endl;
    }
}

// Prints what a character says when you talk to them
void Characters::talkTo(string name) {
    if (responses.find(name) != responses.end()) {
        cout << responses[name] << endl;
    } else {
        cout << "There's no one to talk to here." << endl;
    }
}

// Handles a special case where asking Mad Hatter about tea triggers a unique response
void Characters::askAbout(string name, string item) {
    if (name == "mad hatter" && item == "tea") {
        cout << "The Mad Hatter smiles and offers you a cup of tea." << endl;
    } else {
        cout << name << " doesn't seem to know about that." << endl;
    }
}

// Attack logic, specifically for the Red Queen
bool Characters::attack(string name, Inventory &inventory, Game &game) {
    if (name == "red queen") {
        // You can only win if you have the flamingo mallet
        if (game.checkWin(inventory)) {
            cout << "You bravely challenge the Red Queen with the flamingo mallet!" << endl;
            cout << "With a swift strike, you defeat her and escape Wonderland!" << endl;
            game.endGame(true); // Ends the game with a win
            return true;
        } else {
            cout << "You try to fight the Red Queen, but without the flamingo mallet, you stand no chance." << endl;
            game.endGame(false); // Ends the game with a loss
            return true;
        }
    }

    // If you try to attack a character that’s not meant to be attacked
    cout << "Attacking " << name << " might not be a wise choice." << endl;
    return false;
}


// ------------------ Main Function ------------------
int main() {
    // Create core game components
    Game game;
    game.start(); // Display intro message

    Control control;         // Handles user command parsing
    Locations locations;     // Manages the rooms and movement
    Inventory inventory;     // Manages player's items
    Characters characters;   // Handles NPCs
    string command;          // Stores player input

    // ------------------ Main Game Loop ------------------
    while (true) {
        cout << "\n> ";
        getline(cin, command); // Read entire line of user input

        // Quit condition
        if (command == "quit" || command == "exit") {
            game.endGame(false); // End game with failure message
            break;
        }

        // Translate input to an action (using actions.txt mappings)
        string action = control.processInput(command);

        // Handle directional movement: "go north", "n", etc.
        if (action.find("go") != string::npos) {
            locations.move(action.substr(3)); // Move using substring after "go "
        }

        // Handle taking items: "take key"
        else if (action.find("take") != string::npos) {
            inventory.addItem(action.substr(5)); // Add item after "take "
        }

        // Handle dropping items: "drop key"
        else if (action.find("drop") != string::npos) {
            inventory.removeItem(action.substr(5)); // Remove item after "drop "
        }

        // Show inventory contents
        else if (action == "view inventory") {
            inventory.listItems();
        }

        // Use item in inventory: "use potion"
        else if (action.find("use") != string::npos) {
            inventory.useItem(action.substr(4)); // Use item after "use "
        }

        // Talk to a character: "talk to mad hatter"
        else if (action.find("talk to") != string::npos) {
            characters.talkTo(action.substr(8)); // Extract name after "talk to "
        }

        // Ask a character about an item: "ask mad hatter about tea"
        else if (action.find("ask") != string::npos) {
            size_t pos = action.find("about"); // Find where "about" is
            if (pos != string::npos) {
                string name = action.substr(4, pos - 5);       // Extract name after "ask " up to " about"
                string item = action.substr(pos + 6);          // Extract item after "about "
                characters.askAbout(name, item);
            }
        }

        // Attack a character: "attack red queen"
        else if (action.find("attack") != string::npos) {
            // Ends game on success or failure
            if (characters.attack(action.substr(7), inventory, game)) {
                break; // Exit loop if game ended
            }
        }

        // Invalid command or fallback
        else {
            cout << action << endl;
        }
    }

    return 0; // Program ends
}
