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
    void drawShipsChoiceScreen(sf::RenderWindow& window);
    void handleShipChoice(const sf::Event& event);
    void aiRandomPlacement();
    bool isPlacementPossible(std::vector<int> shipSizes);
    Board placeRemainingShips(std::vector<int> theoryshipSizes, Board currentboard);
	void resetGame();
    void playerPlacement(sf::Event event, bool* shipsplaced, bool restart = false);
    void setNextAiMove();
    void drawImage(sf::RenderWindow& window, int x, int y, const std::string& filename, float scale=0.5);
    void drawPngBackground(sf::RenderWindow& window, const std::string& filename);
    void playSound(const std::string& filename);
    void drawMisc(sf::RenderWindow& window);
    void handleTextBox(sf::Event event, float x, float y, int max);
    std::vector<std::tuple<int, int, bool>> getValidPlacements(Board& board, Ship& ship);
	void drawPauseScreen(sf::RenderWindow& window);
    int findCurrentMax(int size);
    void drawChosenShips(sf::RenderWindow& window);
    std::vector<int> getUnplacedShipSizes();
    void drawRemainingShipsToPlace(sf::RenderWindow& window);
    std::vector<int> unplacedShipSizes;
    const int boardOffsetX = 100;
    const int boardOffsetY = 50;
    int nextAIX=-1;
    int nextAIY=-1;
    sf::Font font;
    bool gameStarted;
    int shipChoiceProgress;
    bool gameOver;
    bool shipsPlaced; 
    bool playerWon; //gracz wygrał - true, przeciwnik wygrał - false
    int currentShipSize;
    bool autoMarker;
	std::vector<std::shared_ptr<sf::Sound>> sounds; // vektor dzwiekow
	std::vector<sf::SoundBuffer> soundBuffers; // vektor buforow dzwiekow
    bool mute;
    std::vector<std::string> playerShotSoundFilenames;
	std::vector<std::string> aiShotSoundFilenames;
    std::vector<std::string> winSoundFilenames;
    std::vector<std::string> loseSoundFilenames;
	int soundTrackIndex = 0;
    sf::Text textBoxText;
    int currentNumberOfShips;
    bool typedIn;
    sf::String playerInput;
    bool showPauseScreen;

public:
    Game();
    
    // Funkcja uruchamiania gry
    void run();

};

#endif
