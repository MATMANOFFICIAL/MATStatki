#include "game.h"

Game::Game() : playerBoard(), aiBoard() {
    // Losowe rozmieszczenie statk�w przeciwnika
    aiRandomPlacement();
}

//Funkcja rozpoczynaj�ca gr�
void Game::run() {
    sf::RenderWindow window(sf::VideoMode(1050, 500), "Statki by Mateusz Jarz�b"); //Tworzenie okna z gr�
    bool gameOver = false; 
    bool shipsPlaced = false;

    while (window.isOpen()) { //P�tla gry
        sf::Event event;
        while (window.pollEvent(event)) {
            if (event.type == sf::Event::Closed)
                window.close();

            if (!shipsPlaced) { //Wywo�ywanie funkcji k�adzenia statk�w przez Gracza
                PlayerPlacement(event, &shipsPlaced);
            }

            else if (event.type == sf::Event::MouseButtonPressed && !gameOver) { 
                handlePlayerAttack(event.mouseButton.x, event.mouseButton.y); //Wywo�anie funkcji odpowiadaj�cej za atak gracza
                if (aiBoard.isGameOver()) { //sprawdzanie czy gracz wygra�
                    std::cout << "Gracz wygra�! Wszystkie statki przeciwnika zaton�y." << std::endl;
                    gameOver = true;
                }
                aiAttack(); //wywo�ywanie funkcji ataku przeciwnika
                if (playerBoard.isGameOver()) { //sprawdzanie czy przeciwnik wygra�
                    std::cout << "Przeviwnik wygra�! Wszystkie statki gracza zaton�y." << std::endl;
                    gameOver = true;
                }
            }
        }

        window.clear();

        // Tworzenie graficznych reprezentacji planszy w oknie
        playerBoard.draw(window, sf::Vector2f(BOARD_OFFSET_X, BOARD_OFFSET_Y));
        aiBoard.draw(window, sf::Vector2f(BOARD_OFFSET_X + 500, BOARD_OFFSET_Y), false);

        window.display();
    }
}


//Funkcja ataku gracza
void Game::handlePlayerAttack(int mouseX, int mouseY) {
    // Obliczanie koordynat�w kom�rki klikni�tej myszk�
    int col = (mouseX - (BOARD_OFFSET_X + 500)) / CELL_SIZE;
    int row = (mouseY - BOARD_OFFSET_Y) / CELL_SIZE;

    if (col >= 0 && row >= 0 && col < GRID_SIZE && row < GRID_SIZE) {   // Sprawdzenie czy atak jest na planszy
        if (aiBoard.attack(sf::Vector2i(col, row))) { //wywo�ywanie funkcji ataku kt�ra sparwdza czy statek zosta� trafiony
            std::cout << "Statek przeciwnika Trafiony!" << std::endl;
        }
        else {
            std::cout << "Statek przeciwnika Chybiony!" << std::endl;
        }
    }
}

//Funkcja ataku przeciwnika
void Game::aiAttack() {
    bool attacked = false;
    int x;
    int y;
    std::srand(std::time(nullptr)); // Ziarno losowania
    while (!attacked) { 
        if (nextAIX != -1 && nextAIY != -1) //je�eli przeciwnik ma zaplanowany kolejny atak to go wykonuje
        {
            x = nextAIX;
            y = nextAIY;
        }
        else //inaczej jest losowo wybierany
        {
            x = std::rand() % GRID_SIZE;
            y = std::rand() % GRID_SIZE;
        }

        if (!(playerBoard.getcellstatus(x, y) == 2 || playerBoard.getcellstatus(x, y) == 3)) { //je�eli wylosowany atak ju� wyst�pi� 
            playerBoard.attack(sf::Vector2i(x, y));                                            // to losowanie wyst�puje ponownie
            attacked = true;
            setnextaimove(x, y);                                                               //wywo�anie planowania kolejnego ataku
                                                                                               //(x i y to poprzednie koordynaty)
        }
    }
   
}

//Funkcja planowania nast�pnego ataku
void Game::setnextaimove(int x,int y) { 

    if (playerBoard.getcellstatus(x,y)==2) { //upewnienie si� czy obecny strza� by� trafiony
        if ((playerBoard.getcellstatus(x+1, y) == 0 || playerBoard.getcellstatus(x+1, y) == 1) && x+1 <= GRID_SIZE)
        {                                   //5 opcji - je�li strza� by� trafiony, kolejny b�dzie po prawej
            nextAIX = x + 1;
            nextAIY = y;
        }
        else if ((playerBoard.getcellstatus(x, y + 1) == 0 || playerBoard.getcellstatus(x, y + 1) == 1) && y + 1 <= GRID_SIZE) {
            nextAIX = x;                   // chyba �e pole po prawej by�o strzelone wcze�niej, wtedy na dole
            nextAIY = y+1;
        }
        else if ((playerBoard.getcellstatus(x, y - 1) == 0 || playerBoard.getcellstatus(x, y - 1) == 1) && y - 1 >= 0) {
            nextAIX = x;                    // chyba �e pole na dole by�o strzelone wcze�niej, wtedy na g�rze
            nextAIY = y-1;
        }
        else if ((playerBoard.getcellstatus(x - 1, y) == 0 || playerBoard.getcellstatus(x - 1, y) == 1) && x - 1 >= 0) {
            nextAIX = x - 1;                 // chyba �e pole na g�rze by�o strzelone wcze�niej, wtedy po lewej
            nextAIY = y;
        }
        else {
            nextAIX = -1;                    //je�eli wszystkie strony wok� s� ju� sprawdzone, wtedy strzelamy losowo
            nextAIY = -1;
        }
    }
    else
    {
        nextAIX = -1;               //je�eli ostatni strza� nie by� trafiony wtedy strzelamy losowo
        nextAIY = -1;
    }
}
//funkcja losowego rozmieszczenia statk�w przeciwnika
void Game::aiRandomPlacement() {
    std::srand(std::time(nullptr)); // Ziarno losowania

    for (int size : shipSizes) {
        Ship ship(size);
        bool placed = false;

        while (!placed) {
            int x = std::rand() % GRID_SIZE; //losowanie koordynat�w
            int y = std::rand() % GRID_SIZE;
            bool horizontal = std::rand() % 2; //losowanie czy statek ma by� poziomo czy nie

            if (aiBoard.isValidPlacement(ship, sf::Vector2i(x, y), horizontal)) {//wywo�anie sprawdzania czy mo�na w tym miejscu postawi� statek
                aiBoard.placeShip(ship, sf::Vector2i(x, y), horizontal);
                placed = true;
            }
        }
    }
}
//funkcja k�adzenia statku przez gracza
void Game::PlayerPlacement(sf::Event event, bool* shipsplaced) {
    static int currentShip = 0; 

    if (currentShip >= shipSizes.size()) {
        *shipsplaced = true;
        return;
    }

    int mouseX = event.mouseButton.x;
    int mouseY = event.mouseButton.y;
    int col = (mouseX - BOARD_OFFSET_X) / CELL_SIZE;
    int row = (mouseY - BOARD_OFFSET_Y) / CELL_SIZE;
    Ship ship(shipSizes[currentShip]);

    //Lewym poziomo, prawym pionowo
    if (event.type == sf::Event::MouseButtonPressed) {
        bool horizontal = event.mouseButton.button == sf::Mouse::Left;
        if (col >= 0 && row >= 0 && col < GRID_SIZE && row < GRID_SIZE) {
            if (playerBoard.isValidPlacement(ship, sf::Vector2i(col, row), horizontal)) {
                playerBoard.placeShip(ship, sf::Vector2i(col, row), horizontal);
                currentShip++;
            }
        }
    }
}
