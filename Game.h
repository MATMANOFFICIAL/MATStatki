#ifndef GAME_H
#define GAME_H

#include "board.h"
#include <SFML/Graphics.hpp>
#include <ctime>
#include <cstdlib>
#include <iostream>

// Klasa gry
class Game {
    Board playerBoard; // Plansza gracza
    Board aiBoard;     // Plansza przeciwnika
    std::vector<int> shipSizes = { 5, 4, 3, 3, 2 }; //Rozmiary statk�w
    //funkcja ataku gracza
    void handlePlayerAttack(int mouseX, int mouseY);
    void aiAttack();
    //Losowe rozmieszczenie statk�w
    void aiRandomPlacement();
    void PlayerPlacement(sf::Event event, bool* shipsplaced);
    void setnextaimove(int x, int y);
    const int BOARD_OFFSET_X = 100;
    const int BOARD_OFFSET_Y = 50;
    int nextAIX=-1;
    int nextAIY=-1;
public:
    Game();

    // Funkcja uruchamiania gry
    void run();

};

#endif
