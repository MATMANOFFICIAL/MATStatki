#include "board.h"

Board::Board() {
    // Inicjalizacja planszy wype�nionej zerami
    grid.resize(GRID_SIZE, std::vector<int>(GRID_SIZE, 0));
}

void Board::placeShip(Ship& ship, const sf::Vector2i& start, bool horizontal) {
    ship.horizontal = horizontal;
    ship.positions.clear();

    for (int i = 0; i < ship.size; ++i) {
        int x = start.x + (horizontal ? i : 0);
        int y = start.y + (horizontal ? 0 : i);

        // Sprawdź, czy współrzędne są w granicach planszy
        if (isonboard(x, y)) {
            grid[y][x] = 1; // oznaczanie statku na planszy
            ship.positions.emplace_back(x, y);
        }
    }
    ships.push_back(ship);
}


bool Board::isValidPlacement(const Ship& ship, const sf::Vector2i& start, bool horizontal) {
    for (int i = 0; i < ship.size; i++) {
        int x = start.x + (horizontal ? i : 0);
        int y = start.y + (horizontal ? 0 : i);

        // Sprawdź, czy współrzędne są w granicach planszy
        if (!isonboard(x, y)) {
            return false;
        }

        // Sprawdź, czy komórka jest pusta
        if (grid[y][x] != 0) {
            return false;
        }

        // Sprawdź otaczające komórki (czy statek nie dotyka innego statku)
        for (int dx = -1; dx <= 1; dx++) {
            for (int dy = -1; dy <= 1; dy++) {
                int nx = x + dx;
                int ny = y + dy;

                // Pomiń sprawdzanie komórek poza planszą
                if (isonboard(nx, ny)) {
                    if (grid[ny][nx] != 0) {
                        return false;
                    }
                }
            }
        }
    }
    return true;
}



bool Board::attack(const sf::Vector2i& target) {
    if (grid[target.y][target.x] == 1) {
        grid[target.y][target.x] = 2; //Trafienie
        return true;
    }
    else if (grid[target.y][target.x] == 0) {
        grid[target.y][target.x] = 3; // Chybienie
        return false;
    }
    return false; //Je�li kom�rka by�a ju� atakowana
}

bool Board::isonboard(int x, int y) const {
    return x >= 0 && x < GRID_SIZE && y >= 0 && y < GRID_SIZE;
}


void Board::draw(sf::RenderWindow& window, const sf::Vector2f& offset, bool showShips) {
    for (int y = 0; y < GRID_SIZE; ++y) {
        for (int x = 0; x < GRID_SIZE; ++x) {
            sf::RectangleShape cell(sf::Vector2f(CELL_SIZE - 2, CELL_SIZE - 2));
            cell.setPosition(offset.x + x * CELL_SIZE, offset.y + y * CELL_SIZE);

            if (grid[y][x] == 0) {
                cell.setFillColor(sf::Color::Blue); // Pusta kom�rka
            }
            else if (grid[y][x] == 1) {
                cell.setFillColor(showShips ? sf::Color::Green : sf::Color::Blue); // Statek (domy�lnie w kolorze pustej kom�rki)
            }
            else if (grid[y][x] == 2) {
                cell.setFillColor(sf::Color::Red); // Trafiony
            }
            else if (grid[y][x] == 3) {
                cell.setFillColor(sf::Color::White); // Chybiony
            }

            window.draw(cell);
        }
    }
}

bool Board::isGameOver() {
    for (const auto& row : grid) {
        for (int cell : row) {
            if (cell == 1) { //Je�eli jaka� cz�� statku nie jest trafiona 
                return false; //gra si� nie ko�czy
            }
        }
    }
    return true; // Wszystkie statki s� zatopione
}


int Board::getcellstatus(int x, int y) { //funkcja zwracaj�ca status kom�rki
    
    return grid[y][x]; 
}

