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
    bool isHorizontal;             // zmienna przechowuj�ca czy statek jest poziomo czy pionowo
    std::vector<sf::Vector2i> positions; // koordynaty r�nych cz�ci statku
	bool isSunk = false;        //informacja o tym czy statek został zatopiony
    Ship(int s) : size(s), isHorizontal(true) {}
};

// Klasa planszy
class Board {
    std::vector<std::vector<int>> grid; // Lista 2d przsechowująca stan poszczególnych komórek
    std::vector<Ship> ships;    // Lista statków na planszy
	sf::Font font;			  // Czcionka
public:
	// Konstruktor
    Board();
    // Funkcja kładzenia statku
    void placeShip(Ship& ship, const sf::Vector2i& start, bool isHorizontal); 
	// Funkcja ktora po kazdym ruchu sprawdza czy jakis statek zostal zatopiony, jesli tak to zmienia go na czarnego 
    // i oznacza pola wokol niego na nieatakowalne
    void updateSunkShips(bool autoMarker);
    // Funkcja sprawdzania czy położenie jest poprawne
    bool isValidPlacement(const Ship& ship, const sf::Vector2i& start, bool isHorizontal);
	Board& operator=(const Board& board);
	// Funkcja resetowania planszy
    void clear();
    // funckja atakowania okreslonego pola
    bool attack(const sf::Vector2i& target);
    // Funkcja tworząca graficzna reprezentacje planszy
    void draw(sf::RenderWindow& window, const sf::Vector2f& offset, bool showShips = true);
    //funkcja sprawdzania co jest na określonych koordynatach
    int getCellStatus(int x, int y);
	//funkcja sprawdzająca czy koordynaty są na planszy
    bool isOnBoard(int x, int y) const;
    //funkcja sprawdzająca czy gra się zakończyła na danej planszy
    bool isGameOver(); 
	//funkcja sprawdzająca czy statek na danym polu jest zatopiony (przed zmienieniem go na czarny)
    bool isSunkOn(int x, int y);
	std::vector<Ship> getRemainingShips(); //funkcja zwracająca statki które nie zostały zatopione
	void drawRemainingShips(sf::RenderWindow& window); //funkcja rysująca statki które nie zostały zatopione
	std::vector<Ship> getVisibleLongShips(); //funkcja zwracająca statki które są widoczne dla przeciwnika i sa dluzsze niz 1
	std::vector<Ship> getVisibleSingleShips(); //funkcja zwracająca statki które są widoczne dla przeciwnika i sa pojedyncze
	std::vector<std::pair<int, int>> getAttackableTiles(); //funkcja zwracająca vektor pól które nie były atakowane
	int getNumberOfShips(); //funkcja zwracająca ilość statków na planszy
	std::vector<int> getShipSizes(); //funkcja zwracająca rozmiary statków na planszy
};

#endif

