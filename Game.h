#ifndef GAME_H
#define GAME_H

#include "board.h"
#include <SFML/Graphics.hpp>
#include <ctime>
#include <cstdlib>
#include <iostream>
#include <SFML/Audio.hpp>
// Klasa gry
class Game {
    Board playerBoard; // Plansza gracza
    Board aiBoard;     // Plansza przeciwnika
    std::vector<int> shipSizes; //Rozmiary statk�w
    //funkcja ataku gracza
    void handlePlayerAttack(int mouseX, int mouseY);
    void aiAttack();
    //Losowe rozmieszczenie statk�w
    void DrawShipsChoiceScreen(sf::RenderWindow& window);
    void handleShipChoiceEvent(const sf::Event& event);
    void aiRandomPlacement();
    bool isplacementpossible(std::vector<int> shipSizes);
	void resetGame();
    void PlayerPlacement(sf::Event event, bool* shipsplaced, bool restart = false);
    void setnextaimove(int x, int y);
    void drawaboatimage(sf::RenderWindow& window, int x, int y, const std::string& filename, float scale=0.5);
    void drawpngBackground(sf::RenderWindow& window, const std::string& filename);
    void playsound(const std::string& filename);
    void drawMisc(sf::RenderWindow& window, sf::Font& font, int BOARD_OFFSET_X, int BOARD_OFFSET_Y);
    void textbox(sf::Event event, float x, float y, int max);
    std::vector<std::tuple<int, int, bool>> getValidPlacements(Board& board, Ship& ship);
    int FindActMax(int size);
    const int BOARD_OFFSET_X = 100;
    const int BOARD_OFFSET_Y = 50;
    int nextAIX=-1;
    int nextAIY=-1;
    sf::Font font;
    bool gameStarted;
    int progress;
    bool gameOver;
    bool shipsPlaced;
    bool ktowygral;
    int currentshipsize;
	std::vector<std::shared_ptr<sf::Sound>> sounds; // vektor dzwiekow
	std::vector<sf::SoundBuffer> soundBuffers; // vektor buforow dzwiekow
    bool mute;
    std::vector<std::string> playershotsoundfilenames;
	std::vector<std::string> aishotsoundfilenames;
    std::vector<std::string> winsoundfilenames;
    std::vector<std::string> losesoundfilenames;
	int track = 0;
    sf::Text textboxtext;
    int currentnumberofships;
    bool typedin;
    sf::String playerInput;

public:
    Game();
    
    // Funkcja uruchamiania gry
    void run();

};

#endif
