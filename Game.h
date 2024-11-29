#ifndef GAME_H
#define GAME_H

#include "board.h"
#include <SFML/Graphics.hpp>
#include <ctime>
#include <cstdlib>
#include <iostream>

// Game class
class Game {
    Board playerBoard; // Player's board
    Board aiBoard;     // AI's board
    std::vector<int> shipSizes = { 5, 4, 3, 3, 2 }; // Sizes of ships to place
    // Handle player's attack on the AI board
    void handlePlayerAttack(int mouseX, int mouseY);
    void aiAttack();
    // Randomly place ships on the AI board
    void aiRandomPlacement();
    void PlayerPlacement(sf::Event event, bool* shipsplaced);
    void setnextaimove(int x, int y);
    const int BOARD_OFFSET_X = 100;
    const int BOARD_OFFSET_Y = 50;
    int nextAIX=-1;
    int nextAIY=-1;
public:
    Game();

    // Run the main game loop
    void run();

};

#endif // GAME_H
