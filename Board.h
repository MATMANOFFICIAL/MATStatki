#ifndef BOARD_H
#define BOARD_H

#include <SFML/Graphics.hpp>
#include <vector>

// Wielkoœæ komórek i ca³ej planszy
const int GRID_SIZE = 10;
const int CELL_SIZE = 40;

// struktura statku
struct Ship {
    int size;                    // Rozmiar statku
    bool horizontal;             // zmienna przechowuj¹ca czy statek jest poziomo czy pionowo
    std::vector<sf::Vector2i> positions; // koordynaty ró¿nych czêœci statku

    Ship(int s) : size(s), horizontal(true) {}
};

// Plansza
class Board {
public:
    Board();

    // Funkcja k³adzenia statku
    void placeShip(Ship& ship, const sf::Vector2i& start, bool horizontal);

    // Funkcja sprawdzania czy po³o¿enie jest poprawne
    bool isValidPlacement(const Ship& ship, const sf::Vector2i& start, bool horizontal);

    // funckja atakowania
    bool attack(const sf::Vector2i& target);

    // Funkcja tworz¹ca obraz planszy
    void draw(sf::RenderWindow& window, const sf::Vector2f& offset, bool showShips = true);
    //funkcja sprawdzania co jest na okreœlonych koordynatach
    int getcellstatus(int x, int y);
    //funkcja sprawdzaj¹ca czy gra siê zakoñczy³a
    bool isGameOver();
private:
    std::vector<std::vector<int>> grid; // Lista 2d przsechowuj¹ca stan komórek
    std::vector<Ship> ships;            // Lista statków na planszy
};

#endif
