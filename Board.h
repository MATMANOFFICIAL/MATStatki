#ifndef BOARD_H
#define BOARD_H

#include <SFML/Graphics.hpp>
#include <vector>
#include <thread>
#include <chrono>
// Wielkość komórek i planszy
const int GRID_SIZE = 10;
const int CELL_SIZE = 40;

// struktura statku
struct Ship {
    int size;                    // Rozmiar statku
    bool horizontal;             // zmienna przechowuj�ca czy statek jest poziomo czy pionowo
    std::vector<sf::Vector2i> positions; // koordynaty r�nych cz�ci statku

    Ship(int s) : size(s), horizontal(true) {}
};

// Klasa planszy
class Board {
    std::vector<std::vector<int>> grid; // Lista 2d przsechowująca stan poszczególnych komórek
    std::vector<Ship> ships;            // Lista statków na planszy
public:
    Board();
    sf::Font font;
    // Funkcja kładzenia statku
    void placeShip(Ship& ship, const sf::Vector2i& start, bool horizontal);

    // Funkcja sprawdzania czy położenie jest poprawne
    bool isValidPlacement(const Ship& ship, const sf::Vector2i& start, bool horizontal);

    void clear();
    // funckja atakowania
    bool attack(const sf::Vector2i& target);

    // Funkcja tworz�ca obraz planszy
    void DrawTitle(sf::RenderWindow& window, const sf::Vector2f& offset);
    void draw(sf::RenderWindow& window, const sf::Vector2f& offset, bool showShips = true);
    //funkcja sprawdzania co jest na określonych koordynatach
    int getcellstatus(int x, int y);
    bool isonboard(int x, int y) const;
    //funkcja sprawdzająca czy gra się zakończyła
    bool isGameOver();
   
};

#endif
