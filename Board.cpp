#include "Board.h"

Board::Board() {
    // Inicjalizacja planszy wypełnionej zerami
    grid.resize(GRID_SIZE, std::vector<int>(GRID_SIZE,0));
    font.loadFromFile("arial.ttf");
}

void Board::updateSunkShips(bool autoMarker) {
    for(Ship& ship : ships)
    {
        if (!ship.isSunk)
        {
            bool wyn = true;
            for (auto& pos : ship.positions)
            {
                if (getCellStatus(pos.x, pos.y) != 2) {
                    wyn = false;
                }
            }
            ship.isSunk = wyn;
            
        }
        
    }


    for (Ship& ship : ships)
    {
        if (ship.isSunk)
        {
            for (auto& pos : ship.positions)
            {
                grid[pos.y][pos.x] = 4;
                if (autoMarker)
                {
                    for (int i = -1; i <= 1; i++)
                    {
                        for (int j = -1; j <= 1; j++) {
                            int x = pos.x + i, y = pos.y + j;
                            if (isOnBoard(x, y) && getCellStatus(x, y) == 0) grid[y][x] = 3;
                        }
                    }
                }
            }


        }
    }
}


bool Board::isSunkOn(int x, int y) {
    
    for (Ship& ship : ships) {
        if (ship.isSunk) {
            for (auto& pos : ship.positions)
            {
                if (x == pos.x && y == pos.y)
                {
                    return true;
                }
            }

        }

    }
    return false;
}

void Board::drawRemainingShips(sf::RenderWindow& window) {
    sf::Vector2f offset = sf::Vector2f(430, 140);
	sf::Text text;
	std::vector<Ship> statki = getRemainingShips();
	text.setFont(font);
	text.setCharacterSize(24);
	text.setFillColor(sf::Color::Red);
	text.setPosition(offset.x + 600, offset.y - 50);
	int remshipcount[] = { 0,0,0,0,0,0 };
    for (Ship ship : statki)
    {
		remshipcount[ship.size]++;
    }
    float smallCELL_SIZE = CELL_SIZE * 0.75;
    text.setPosition(offset.x + 600, offset.y - 50);
    text.setString("Niezatopione statki: ");
    
    if(remshipcount[1] != 0 || remshipcount[2] != 0 || remshipcount[3] != 0 || remshipcount[4] != 0 || remshipcount[5] != 0)
    {
        window.draw(text);
    }
    for (int i = 1; i <= 5; i++)
    {
        if (remshipcount[i]!=0)
        {
			
            for (int a = 0; a < i; a++)
            {
                sf::RectangleShape cell(sf::Vector2f(smallCELL_SIZE - 2, smallCELL_SIZE - 2));
                cell.setOutlineColor(sf::Color::Black);
                cell.setOutlineThickness(2);
                cell.setPosition(offset.x + 660 + a * smallCELL_SIZE, offset.y - 50 + 55 * i);
                cell.setFillColor(sf::Color::Red);
                window.draw(cell);
            }
            text.setString(std::to_string(remshipcount[i]));
            text.setPosition(offset.x + 630, offset.y - 50 + 55 * i);
            window.draw(text);
        }
    }

}


void Board::placeShip(Ship& ship, const sf::Vector2i& start, bool isHorizontal) {
    ship.isHorizontal = isHorizontal;
    ship.positions.clear();

    for (int i = 0; i < ship.size; ++i) {
        int x = start.x + (isHorizontal ? i : 0);
        int y = start.y + (isHorizontal ? 0 : i);

        // Sprawdź, czy współrzędne są w granicach planszy
        if (isOnBoard(x, y)) {
            grid[y][x] = 1; // oznaczanie statku na planszy
            ship.positions.emplace_back(x, y);
        }
    }
    ships.push_back(ship);
}


int Board::getNumberOfShips() {
	return ships.size();
}

void Board::clear() {
	grid.clear();
	grid.resize(GRID_SIZE, std::vector<int>(GRID_SIZE, 0));
	ships.clear();
}

bool Board::isValidPlacement(const Ship& ship, const sf::Vector2i& start, bool isHorizontal) {
	for (int i = 0; i < ship.size; i++) { //dla kazdego pola statku
        int x = start.x + (isHorizontal ? i : 0);
        int y = start.y + (isHorizontal ? 0 : i);

        // Sprawdź, czy współrzędne są w granicach planszy
        if (!isOnBoard(x, y)) {
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
                if (isOnBoard(nx, ny)) {
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
	return false; //Jeżeli komórka jest już trafiona
}

bool Board::isOnBoard(int x, int y) const {
    return x >= 0 && x < GRID_SIZE && y >= 0 && y < GRID_SIZE;
}



void Board::draw(sf::RenderWindow& window, const sf::Vector2f& offset, bool showShips) {

    for (int y = 0; y < GRID_SIZE; ++y) {
        for (int x = 0; x < GRID_SIZE; ++x) {
            sf::RectangleShape cell(sf::Vector2f(CELL_SIZE - 2, CELL_SIZE - 2));
            cell.setPosition(offset.x + x * CELL_SIZE, offset.y + y * CELL_SIZE);

            if (grid[y][x] == 0) {
                cell.setFillColor(sf::Color::Blue); // Pusta komórka
            }
            else if (grid[y][x] == 1) {
                cell.setFillColor(showShips ? sf::Color::Green : sf::Color::Blue); // Statek (domyślnie w kolorze pustej komórki)
            }
            else if (grid[y][x] == 2) {
                cell.setFillColor(sf::Color::Red); // Trafiony
            }
            else if (grid[y][x] == 3) {
                cell.setFillColor(sf::Color::White); // Chybiony
            }
            else if (grid[y][x] == 4) { //zatopiony
                cell.setFillColor(sf::Color::Black);
            }
            window.draw(cell); 
        }
    }
}

bool Board::isGameOver() {
    for (const auto& row : grid) {
        for (int cell : row) {
            if (cell == 1) { //Jeżeli jakaś część statku nie jest trafiona 
                return false; //gra się nie kończy
            }
        }
    }
    return true; // Wszystkie statki są zatopione
}


int Board::getCellStatus(int x, int y) { //funkcja zwracająca status komórki
    
    return grid[y][x]; 
}

std::vector<Ship> Board::getVisibleLongShips() { 
    std::vector<Ship> foundships;
    std::vector<sf::Vector2i> curpos;

    // Check horizontally
    for (int y = 0; y < GRID_SIZE; ++y) {
        int ciag = 0;
        for (int x = 0; x < GRID_SIZE; ++x) {
            if (getCellStatus(x, y) == 2) {
                ciag++;
                curpos.push_back(sf::Vector2i(x, y));
            } else {
                if (ciag > 1) {
                    Ship ship(ciag);
                    ship.positions = curpos;
                    ship.isHorizontal = true;
                    foundships.push_back(ship);
                }
                curpos.clear();
                ciag = 0;
            }
        }
        if (ciag > 1) {
            Ship ship(ciag);
            ship.positions = curpos;
            ship.isHorizontal = true;
            foundships.push_back(ship);
        }
        curpos.clear();
    }

    // Check vertically
    for (int x = 0; x < GRID_SIZE; ++x) {
        int ciag = 0;
        for (int y = 0; y < GRID_SIZE; ++y) {
            if (getCellStatus(x, y) == 2) {
                ciag++;
                curpos.push_back(sf::Vector2i(x, y));
            } else {
                if (ciag > 1) {
                    Ship ship(ciag);
                    ship.positions = curpos;
                    ship.isHorizontal = false;
                    foundships.push_back(ship);
                }
                curpos.clear();
                ciag = 0;
            }
        }
        if (ciag > 1) {
            Ship ship(ciag);
            ship.positions = curpos;
            ship.isHorizontal = false;
            foundships.push_back(ship);
        }
        curpos.clear();
    }

    return foundships;
}
std::vector<Ship> Board::getVisibleSingleShips() {
    std::vector<Ship> foundships;
    for (int x = 0; x < 10; ++x) {
        for (int y = 0; y < 10; ++y) {
			if (getCellStatus(x, y) == 2)
			{
                bool sam = true;
                for (int i = -1; i <= 1; i++)
                {
                    for (int j = -1; j <= 1; j++) {
                        if (isOnBoard(x+i,y+j))
                        {
                            if (i!=0 && j!=0 && getCellStatus(x+i,y+j) == 2) sam = false;
                        }
                    }
                }
                if (sam)
                {
                    Ship ship(1);
                    ship.positions.push_back(sf::Vector2i(x, y));
                    ship.isHorizontal = true;
                    foundships.push_back(ship);
                }
			}
        
        }
    }
	return foundships;
}

//funkcja zwracająca vektor pól które nie były atakowane
std::vector<std::pair<int, int>> Board::getAttackableTiles() {
	std::vector<std::pair<int, int>> notAttackedFields;
	for (int x = 0; x < 10; ++x) {
		for (int y = 0; y < 10; ++y) {
			
            
            bool czykolozbitego = false;
            for (int i = -1; i <= 1; i++)
            {
                for (int j = -1; j <= 1; j++) {
                    if (isOnBoard(x+i, y+j) && getCellStatus(x+i, y+j) == 4) czykolozbitego = true;
                }
            }

			if (!czykolozbitego && (getCellStatus(x, y) == 0 || getCellStatus(x, y) == 1)) {
				notAttackedFields.push_back(std::make_pair(x, y));
			}
		}
	}
	return notAttackedFields;
}

Board& Board::operator=(const Board& board) {
	grid = board.grid;
	ships = board.ships;
	font = board.font;
	return *this;
}


std::vector<Ship> Board::getRemainingShips() {
    std::vector<Ship> wynik;
	for (Ship& ship : ships)
	{
		if (!ship.isSunk)
		{
			wynik.push_back(ship);
		}
	}
	return wynik;
}

std::vector<int> Board::getShipSizes(){
    std::vector<int> wynik;
	for (Ship& ship : ships)
	{
		wynik.push_back(ship.size);
	}
	return wynik;
}






