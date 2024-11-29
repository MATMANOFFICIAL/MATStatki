#ifndef BOARD_H
#define BOARD_H

#include <SFML/Graphics.hpp>
#include <vector>

// Constants for grid dimensions and cell sizes
const int GRID_SIZE = 10;
const int CELL_SIZE = 40;

// Ship structure
struct Ship {
    int size;                    // Size of the ship
    bool horizontal;             // Orientation of the ship
    std::vector<sf::Vector2i> positions; // List of grid positions occupied by the ship

    Ship(int s) : size(s), horizontal(true) {}
};

// Board class
class Board {
public:
    Board();

    // Place a ship on the board
    void placeShip(Ship& ship, const sf::Vector2i& start, bool horizontal);

    // Check if a ship can be placed at the given position and orientation
    bool isValidPlacement(const Ship& ship, const sf::Vector2i& start, bool horizontal);

    // Handle an attack on the board
    bool attack(const sf::Vector2i& target);

    // Draw the board using SFML
    void draw(sf::RenderWindow& window, const sf::Vector2f& offset, bool showShips = true);
    
    int getcellstatus(int x, int y);

    bool isGameOver();
private:
    std::vector<std::vector<int>> grid; // 2D grid to store cell states
    std::vector<Ship> ships;            // List of ships placed on the board
};

#endif // BOARD_H
