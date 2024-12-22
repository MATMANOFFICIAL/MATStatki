#ifndef BOARD_H
#define BOARD_H

#include <SFML/Graphics.hpp>
#include <vector>

// Wielko�� kom�rek i ca�ej planszy
const int GRID_SIZE = 10;
const int CELL_SIZE = 40;

// struktura statku
struct Ship {
    int size;                    // Rozmiar statku
    bool horizontal;             // zmienna przechowuj�ca czy statek jest poziomo czy pionowo
    std::vector<sf::Vector2i> positions; // koordynaty r�nych cz�ci statku

    Ship(int s) : size(s), horizontal(true) {}
};

// Plansza
class Board {
public:
    Board();

    // Funkcja k�adzenia statku
    void placeShip(Ship& ship, const sf::Vector2i& start, bool horizontal);

    // Funkcja sprawdzania czy po�o�enie jest poprawne
    bool isValidPlacement(const Ship& ship, const sf::Vector2i& start, bool horizontal);

    // funckja atakowania
    bool attack(const sf::Vector2i& target);

    // Funkcja tworz�ca obraz planszy
    void draw(sf::RenderWindow& window, const sf::Vector2f& offset, bool showShips = true);
    //funkcja sprawdzania co jest na okre�lonych koordynatach
    int getcellstatus(int x, int y);
    bool isonboard(int x, int y) const;
    //funkcja sprawdzaj�ca czy gra si� zako�czy�a
    bool isGameOver();
private:
    std::vector<std::vector<int>> grid; // Lista 2d przsechowuj�ca stan kom�rek
    std::vector<Ship> ships;            // Lista statk�w na planszy
};

#endif
