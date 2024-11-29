#include "board.h"

Board::Board() {
    // Initialize the grid with zeros (empty cells)
    grid.resize(GRID_SIZE, std::vector<int>(GRID_SIZE, 0));
}

void Board::placeShip(Ship& ship, const sf::Vector2i& start, bool horizontal) {
    ship.horizontal = horizontal;
    ship.positions.clear();

    for (int i = 0; i < ship.size; ++i) {
        int x = start.x + (horizontal ? i : 0);
        int y = start.y + (horizontal ? 0 : i);
        grid[y][x] = 1; // Mark ship on grid
        ship.positions.emplace_back(x, y);
    }
    ships.push_back(ship);
}

bool Board::isValidPlacement(const Ship& ship, const sf::Vector2i& start, bool horizontal) {
    for (int i = 0; i < ship.size; i++) {
        int x = start.x + (horizontal ? i : 0);
        int y = start.y + (horizontal ? 0 : i);

        if (x < 0 || y < 0 || x >= GRID_SIZE || y >= GRID_SIZE || grid[y][x] != 0) {
            return false;
        }
    }

    /*
    if (horizontal) {
        for (int i = 0; i < ship.size; i++) {
            int x = start.x + 1 + i;
            int y = start.y;
            if (x < 0 || y < 0 || x >= GRID_SIZE || y >= GRID_SIZE) {
                if (grid[y][x] != 0) {
                    return false;
                }
            }
        }

        for (int i = 0; i < ship.size; i++) {
            int x = start.x + 1 - i;
            int y = start.y;
            if (x < 0 || y < 0 || x >= GRID_SIZE || y >= GRID_SIZE) {
                if (grid[y][x] != 0) {
                    return false;
                }
            }
        }

        int x = start.x;
        int y = start.y-1;
        if (x < 0 || y < 0 || x >= GRID_SIZE || y >= GRID_SIZE) {
            if (grid[y][x] != 0) {
                return false;
            }
        }
        x = start.x;
        y = start.y + ship.size-1;
        if (x < 0 || y < 0 || x >= GRID_SIZE || y >= GRID_SIZE) {
            if (grid[y][x] != 0) {
                return false;
            }
        }
    }
    else {
        for (int i = 0; i < ship.size; i++) {
            int x = start.x;
            int y = start.y + 1 + i;
            if (x < 0 || y < 0 || x >= GRID_SIZE || y >= GRID_SIZE) {
                if (grid[y][x] != 0) {
                    return false;
                }
            }
        }

        for (int i = 0; i < ship.size; i++) {
            int x = start.x;
            int y = start.y + 1 - i;
            if (x < 0 || y < 0 || x >= GRID_SIZE || y >= GRID_SIZE) {
                if (grid[y][x] != 0) {
                    return false;
                }
            }
        }

        int x = start.x - 1;
        int y = start.y;
        if (x < 0 || y < 0 || x >= GRID_SIZE || y >= GRID_SIZE) {
            if (grid[y][x] != 0) {
                return false;
            }
        }
        x = start.x + ship.size - 1;
        y = start.y;
        if (x < 0 || y < 0 || x >= GRID_SIZE || y >= GRID_SIZE) {
            if (grid[y][x] != 0) {
                return false;
            }
        }
    }


    */
    return true;
}

bool Board::attack(const sf::Vector2i& target) {
    if (grid[target.y][target.x] == 1) {
        grid[target.y][target.x] = 2; // Mark hit
        return true;
    }
    else if (grid[target.y][target.x] == 0) {
        grid[target.y][target.x] = 3; // Mark miss
        return false;
    }
    return false; // Already attacked cell
}

void Board::draw(sf::RenderWindow& window, const sf::Vector2f& offset, bool showShips) {
    for (int y = 0; y < GRID_SIZE; ++y) {
        for (int x = 0; x < GRID_SIZE; ++x) {
            sf::RectangleShape cell(sf::Vector2f(CELL_SIZE - 2, CELL_SIZE - 2));
            cell.setPosition(offset.x + x * CELL_SIZE, offset.y + y * CELL_SIZE);

            if (grid[y][x] == 0) {
                cell.setFillColor(sf::Color::Blue); // Empty cell
            }
            else if (grid[y][x] == 1) {
                cell.setFillColor(showShips ? sf::Color::Green : sf::Color::Blue); // Ship (hidden by default)
            }
            else if (grid[y][x] == 2) {
                cell.setFillColor(sf::Color::Red); // Hit
            }
            else if (grid[y][x] == 3) {
                cell.setFillColor(sf::Color::White); // Miss
            }

            window.draw(cell);
        }
    }
}

bool Board::isGameOver() {
    for (const auto& row : grid) {
        for (int cell : row) {
            if (cell == 1) { // If there's any part of a ship left
                return false;
            }
        }
    }
    return true; // No unsunk ships remain
}


int Board::getcellstatus(int x, int y) {
    
    return grid[y][x];
}

