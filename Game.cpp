#include "Game.h"


Game::Game() : playerBoard(), aiBoard() {
    //ustawienie pocz¹tkowych wartoœci
    font.loadFromFile("arial.ttf");
	gameStarted = false; 
	shipChoiceProgress = 0;
    gameOver = false;
    shipsPlaced = false;
    playerWon = false;
    currentShipSize = 0;
	mute = true;
	autoMarker = true;
    showPauseScreen = false;
	playerShotSoundFilenames = { "bum1.wav","hihihiha.wav" };
	aiShotSoundFilenames = {"bum.wav","garr.wav"  };
	winSoundFilenames = { "win.wav","wincr.wav" };
	loseSoundFilenames = { "lose.wav","losecr.wav" };
}

void Game::run() {
    //Tworzenie okna
    sf::RenderWindow window(sf::VideoMode(1250, 500), "Statki by Mateusz Jarzab", sf::Style::Titlebar |sf::Style::Close); 
    while (window.isOpen()) { //Pêtla gry
        sf::Event event;
        while (window.pollEvent(event)) { //Pêtla w której mo¿na korzystaæ z event
            if (event.type == sf::Event::Closed) //Obs³uga zamkniêcia okna
                window.close();

            if (!gameStarted) { // Faza wyboru statków
				handleShipChoice(event); //Wywo³anie funkcji obslugujacej wybór statków
            }
            else { 
                if (!shipsPlaced) { //Faza k³adzenia statków przez gracza
                    playerPlacement(event, &shipsPlaced);
                    //Aktualizacja tablicy niepostawionych statków
					if (event.type == sf::Event::MouseButtonPressed) unplacedShipSizes = getUnplacedShipSizes(); 
                }
                else if (event.type == sf::Event::MouseButtonPressed && !gameOver && gameStarted) { //Faza gry
                    //Wywo³anie funkcji odpowiadaj¹cej za atak gracza
                    handlePlayerAttack(event.mouseButton.x, event.mouseButton.y); 
					//Sprawdzamy czy jakies statki zostaly zatopione i je ustawiamy na zatopione
					playerBoard.updateSunkShips(autoMarker); 
                    aiBoard.updateSunkShips(autoMarker); 
                    if (aiBoard.isGameOver()) { //sprawdzanie czy gracz wygra³
                        playSound(winSoundFilenames[soundTrackIndex]); 
                        gameOver = true;
                        playerWon = true; //true = gracz wygra³
                    }
                   
                    if (playerBoard.isGameOver()) { //sprawdzanie czy przeciwnik wygra³
                        playSound(loseSoundFilenames[soundTrackIndex]);
                        gameOver = true;
                        playerWon = false; //false = przeciwnik wygra³
                    }
                }
            }
			if (gameOver && event.type == sf::Event::KeyPressed) { //Restart gry gdy gra siê zakoñczy
                resetGame();
            }
            if (event.type == sf::Event::KeyPressed && event.key.code == sf::Keyboard::Escape) //Ekran pauzy
            {
                showPauseScreen = true;

            }
            //Reset gry z ekranu pauzy
			if (showPauseScreen && event.type == sf::Event::KeyPressed && event.key.code == sf::Keyboard::R) 
            {
                resetGame();
                showPauseScreen = false;

            }
			//Odpausowanie gry z ekranu pauzy
            if ((gameStarted && showPauseScreen && event.type == sf::Event::KeyPressed && event.key.code == sf::Keyboard::Enter) || (!gameStarted && showPauseScreen && event.type == sf::Event::KeyPressed && event.key.code == sf::Keyboard::W))
            {
                showPauseScreen = false;
            }
            //zmiana œcie¿ki dŸwiêkowej 
			if (gameStarted && showPauseScreen && event.type == sf::Event::KeyPressed && event.key.code == sf::Keyboard::T) 
            {
                if (soundTrackIndex == 1) soundTrackIndex = 0;
                else soundTrackIndex = 1;
            }
            //Wy³¹czanie/W³¹czanie dŸwiêku
            if (gameStarted && event.type == sf::Event::KeyPressed && event.key.code == sf::Keyboard::M) {
                mute = !mute;
            }
        }
        


        // Usuwanie dŸwiêków, które siê zakoñczy³y
        sounds.erase(std::remove_if(sounds.begin(), sounds.end(),
            [](const std::shared_ptr<sf::Sound>& sound) { return sound->getStatus() == sf::Sound::Stopped; }),
            sounds.end());

		window.clear(); //Czyszczenie okna
        drawPngBackground(window, "tlo.png"); //Rysowanie t³a
        drawMisc(window); //Rysowanie pobocznych napisów
		
        if (gameStarted) { //Je¿eli gra siê zaczê³a rysujê plansze
            if (shipsPlaced)aiBoard.drawRemainingShips(window); //Rysowanie licznika niezatopionych statków przeciwnika
            if (!gameOver) { 
                // Tworzenie graficznych reprezentacji planszy w oknie
                playerBoard.draw(window, sf::Vector2f(boardOffsetX, boardOffsetY));
                aiBoard.draw(window, sf::Vector2f(boardOffsetX + 500, boardOffsetY), false);
            }
            else { //Je¿eli gra siê zakoñczy³a rysujê ekran koñcowy
                playerBoard.draw(window, sf::Vector2f(boardOffsetX, boardOffsetY)); 
                aiBoard.draw(window, sf::Vector2f(boardOffsetX + 500, boardOffsetY), true);
                sf::RectangleShape rectangle(sf::Vector2f(600, 100)); //Rysujê prostok¹t za tekstem aby by³o bardziej przejrzyœcie
                rectangle.setFillColor(sf::Color::Blue);
                rectangle.setOrigin(rectangle.getSize().x / 2.0f, rectangle.getSize().y / 2.0f);
                rectangle.setPosition(sf::Vector2f(window.getSize().x / 2.0f, window.getSize().y / 2.0f));
                window.draw(rectangle);

                sf::Text text;
                text.setFont(font); 

                if (playerWon) { //Zale¿nie od wygranego, wiadomoœæ jest inna i ma inny kolor
                    text.setString("Koniec gry, Wygrales!");
                    text.setFillColor(sf::Color::Green);
                }
                else {
                    text.setString("Koniec gry, Przegrales!");
                    text.setFillColor(sf::Color::Red);
                }

                text.setCharacterSize(50);
                text.setStyle(sf::Text::Bold);

                sf::FloatRect textRect = text.getLocalBounds(); 
                text.setOrigin(textRect.left + textRect.width / 2.0f, textRect.top + textRect.height / 2.0f);
                text.setPosition(sf::Vector2f(window.getSize().x / 2.0f, window.getSize().y / 2.0f));
                window.draw(text);
            }
        }
        else { //Je¿eli gra siê nie zaczê³a, Rysujê ekran wyboru statków
            drawShipsChoiceScreen(window);
			drawChosenShips(window);
        }

		if (showPauseScreen) drawPauseScreen(window);  //Rysujê ekran pauzy
        window.display();
    }
}

void Game::playSound(const std::string& filename) {
    if (!mute)
    {
        sf::SoundBuffer buffer;
        if (!buffer.loadFromFile(filename)) {
            return;
        }

		// Buffer dŸwiêku musi byæ przechowany w kontenerze, aby nie zosta³ zniszczony po zakoñczeniu funkcji
        soundBuffers.push_back(buffer);

		// Stworzenie obiektu dŸwiêku i odtworzenie go
        auto sound = std::make_shared<sf::Sound>();
        sound->setBuffer(soundBuffers.back());
        sound->play();

		// dodanie dŸwiêku do kontenera
        sounds.push_back(sound);
    }
    
}
void Game::drawPngBackground(sf::RenderWindow& window, const std::string& filename) { //Funkcja rysuj¹ca t³o
	sf::Texture tlo,muteicon;
	if (!tlo.loadFromFile(filename)) {
		return;
	}
    muteicon.loadFromFile("mute.png");
    sf::Sprite sprite, sprite1;
    sprite.setTexture(tlo);
    sprite.setScale(0.75, 0.75);
    sprite1.setTexture(muteicon);
    sprite1.setScale(0.1, 0.1);
    sprite1.setPosition(1200, 0);
    window.draw(sprite);
    if (mute)
    {
        window.draw(sprite1);
    }
}


void Game::drawMisc(sf::RenderWindow& window) { //funkcja rysuj¹ca inne, mniej wa¿ne elementy
    sf::Text napisstatki, podpis, text;
    napisstatki.setFont(font);
    podpis.setFont(font);
    podpis.setCharacterSize(20);
    podpis.setFillColor(sf::Color::Black);
    podpis.setStyle(sf::Text::Bold);
    podpis.setPosition(boardOffsetX + 800, boardOffsetY + 420);
    podpis.setString("Mateusz Jarzab 2024 L02 177086");
    napisstatki.setCharacterSize(30);
    napisstatki.setFillColor(sf::Color::Cyan);
    napisstatki.setStyle(sf::Text::Bold);
    napisstatki.setPosition(boardOffsetX + 360, boardOffsetY - 50);
    napisstatki.setString("Gra w Statki");
    window.draw(podpis);
    window.draw(napisstatki);
    if (gameStarted)
    {
        text.setFont(font);
        text.setString("Plansza Gracza");
        text.setCharacterSize(24);
        text.setFillColor(sf::Color::Green);
        text.setPosition(boardOffsetX + 100, boardOffsetY - 50);
        window.draw(text);
        text.setFillColor(sf::Color::Red);
        text.setString("Plansza Przeciwnika");
        text.setPosition(boardOffsetX + 600, boardOffsetY - 50);
        window.draw(text);
    }


    if (gameStarted && !shipsPlaced)
    {
        sf::Text info;
        info.setFont(font);
        info.setCharacterSize(20);
        info.setFillColor(sf::Color::Red);
        info.setStyle(sf::Text::Bold);
        sf::FloatRect textRect = info.getLocalBounds();
        info.setOrigin((textRect.left + textRect.width / 2.0f) - 30, textRect.top + textRect.height / 2.0f - 20);
        info.setPosition(sf::Vector2f(5, boardOffsetY + 380));

        info.setString("R - Reset statków  Q - Ustaw Reszte losowo  Esc - Pauza\nMysz lewy - postaw poziomo   Mysz prawy - postaw pionowo");

        
        sf::RectangleShape infoBackground(sf::Vector2f(600, 100));
        infoBackground.setFillColor(sf::Color::Black);
        infoBackground.setPosition(info.getPosition().x + 25, info.getPosition().y + 20);
		infoBackground.setOutlineThickness(2);
		infoBackground.setOutlineColor(sf::Color::Green);
        window.draw(infoBackground);
        

        drawRemainingShipsToPlace(window);
        window.draw(info);
        
        sf::Text CurShipSizeDraw;
        CurShipSizeDraw.setFont(font);
        CurShipSizeDraw.setCharacterSize(30);
        CurShipSizeDraw.setFillColor(sf::Color::Magenta);
        CurShipSizeDraw.setStyle(sf::Text::Bold);
        sf::FloatRect textRect2 = CurShipSizeDraw.getLocalBounds();
        CurShipSizeDraw.setOrigin((textRect2.left + textRect2.width / 2.0f) - 30, textRect2.top + textRect2.height / 2.0f - 20);
        CurShipSizeDraw.setPosition(sf::Vector2f(-5, -5));
        CurShipSizeDraw.setString("Statek: \n    " + std::to_string(currentShipSize));
        window.draw(CurShipSizeDraw);
    }
}


void Game::drawImage(sf::RenderWindow& window, int x, int y, const std::string& filename, float scale) { //Funkcja rysuj¹ca obrazek
    sf::Texture texture;
    if (!texture.loadFromFile(filename)) {
		return;
    }
    sf::Sprite sprite;
    sprite.setPosition(x, y);
    sprite.setTexture(texture);
    sprite.scale(scale, scale);
    window.draw(sprite);
}

void Game::resetGame() { //Funkcja Resetowania gry
	playerBoard.clear(); 
	aiBoard.clear();
	shipSizes.clear();
	gameStarted = false;
	shipChoiceProgress = 0;
	gameOver = false;
	shipsPlaced = false;
	playerWon = false;
	nextAIX = -1;
	nextAIY = -1;
	typedIn = false;
	showPauseScreen = false;
	playerPlacement(sf::Event(), &shipsPlaced, true); //W player placement jest statyczna zmienna currentShip, 
                                                    //która trzeba zresetowaæ, st¹d zastosowa³em alternatywne jej wywo³anie 
}

void Game::handleShipChoice(const sf::Event& event) {
    static int max;
    if (shipChoiceProgress == 0) {

        if (event.type == sf::Event::KeyPressed && !showPauseScreen) {
            if (event.key.code == sf::Keyboard::Space) { //Kontynyuuj wybór statków
                shipChoiceProgress = 1;
                textBoxText.setString("0");
                currentNumberOfShips = 0;
                typedIn = false;
                playerInput = "";
                max = findCurrentMax(shipChoiceProgress);
            }
            else if (event.key.code == sf::Keyboard::Enter) { //Wybór domyœlnych statków
                //domyslne statki to 4 3 3 2 2 2 1 1 1 1
                shipSizes.push_back(1);
                shipSizes.push_back(1);
                shipSizes.push_back(1);
                shipSizes.push_back(1);
                shipSizes.push_back(2);
                shipSizes.push_back(2);
                shipSizes.push_back(2);
                shipSizes.push_back(3);
                shipSizes.push_back(3);
                shipSizes.push_back(4);
                
                unplacedShipSizes = getUnplacedShipSizes();
                shipChoiceProgress = 6;

            }
            else if (event.key.code == sf::Keyboard::A) { //W³¹cz/Wy³¹cz autooznaczanie
                autoMarker = !autoMarker;
            }
        }
    }
	else if (shipChoiceProgress < 6 && shipChoiceProgress >= 1) { //Wybór ilosci poszczegolnych statkow od 1 do 5
        if (max == 0) shipChoiceProgress = 6;
		handleTextBox(event, boardOffsetX + 505, boardOffsetY + 90, max); //Obs³uga wpisywania iloœci statków
        if (typedIn) {
            for (int i = 0; i < currentNumberOfShips; i++)
            {
                shipSizes.push_back(shipChoiceProgress);
            }
            shipChoiceProgress++;
            textBoxText.setString("0");
            currentNumberOfShips = 0;
            playerInput = "";
            typedIn = false;
            max = findCurrentMax(shipChoiceProgress);
        }
    }
    else if(shipChoiceProgress == 6) 
    {
        
        if (shipSizes.size()==0) //jeœli nie wybrano ¿adnych statków to daje domyœlnie 1 ma³y
        {
            shipSizes.push_back(1);
        }
        unplacedShipSizes = getUnplacedShipSizes();
        aiRandomPlacement(); // Losowe rozmieszczenie statków przeciwnika
		gameStarted = true; // Przejscie do fazy gry
        
    }
}


// Funkcja rysuj¹ca ekran wyboru statków
void Game::drawShipsChoiceScreen(sf::RenderWindow& window) {
    sf::Text text;
    text.setFont(font);
    text.setCharacterSize(30);
    text.setFillColor(sf::Color::Green);
    text.setStyle(sf::Text::Bold);
    sf::FloatRect textRect = text.getLocalBounds();
    text.setOrigin((textRect.left + textRect.width / 2.0f) - 30, textRect.top + textRect.height / 2.0f - 20);
    text.setPosition(sf::Vector2f(boardOffsetX + 80, boardOffsetY + 70));
    sf::RectangleShape rectangle(sf::Vector2f(boardOffsetX + 650, boardOffsetY + 220)); //Rysowanie prostok¹ta za tekstem
    rectangle.setOrigin(rectangle.getSize().x / 2.0f, rectangle.getSize().y / 2.0f);
    rectangle.setPosition(sf::Vector2f(window.getSize().x / 2.0f - 50, window.getSize().y / 2.0f));
    rectangle.setFillColor(sf::Color::Black);
    rectangle.setOutlineThickness(5);
    rectangle.setOutlineColor(sf::Color::Green);
    window.draw(rectangle);
    drawImage(window, boardOffsetX + 200, 270, "statek.png");
    drawImage(window, boardOffsetX + 800, 270, "statek1.png", 0.4);
    if (shipChoiceProgress == 0) { //Instrukcja wyboru statków

        text.setString(" Enter - domyslna ilosc statkow (4x1,3x2,2x3,1x4) \n Spacja - wybierz samodzielnie \n A - Wylacz/wlacz autooznaczanie miejsc \n dookola zatopionych statkow: " + std::string(autoMarker ? "ON" : "OFF"));
    }
    else if (shipChoiceProgress < 6 && shipChoiceProgress >= 1) { //Wybór statku 5 kratkowego
        static int lastProgress=0;
        static std::string napis;
        
        if(lastProgress != shipChoiceProgress){
            napis = "Wpisz ilosc " + std::to_string(shipChoiceProgress) + "-masztowcow:";
            for (int i = shipChoiceProgress; i < 6; i++)
            {
                int max = findCurrentMax(i);
                std::string koncowka;
                if (max == 1)
                {
                    koncowka = "-masztowiec";
                }
                else if ((max >= 1 && max <5) || (max >= 22 && max <=24)) {
                    koncowka = "-masztowce";
                }
                else {
                    koncowka = "-masztowcow";
                }
                napis += "\nMozesz postawic jeszcze " + std::to_string(max) + " " + std::to_string(i) + koncowka;
            }
        }
        lastProgress = shipChoiceProgress;
        text.setString(napis);
    }
    if(shipChoiceProgress!=0 && shipChoiceProgress !=6) window.draw(textBoxText);
    window.draw(text);
}
//Funkcja ataku gracza
void Game::handlePlayerAttack(int mouseX, int mouseY) {
    // Obliczanie koordynatów komórki klikniêtej myszk¹
    int col = (mouseX - (boardOffsetX + 500)) / CELL_SIZE;
    int row = (mouseY - boardOffsetY) / CELL_SIZE;
    if (col >= 0 && row >= 0 && col < GRID_SIZE && row < GRID_SIZE) {   // Sprawdzenie czy atak jest na planszy
		bool bylopuste = aiBoard.getCellStatus(col, row) == 0 || aiBoard.getCellStatus(col, row) == 1;
        if (aiBoard.attack(sf::Vector2i(col, row))) { playSound(playerShotSoundFilenames[soundTrackIndex]); } // Atak na planszê przeciwnika
        if (bylopuste) aiAttack(); //wywo³ywanie funkcji ataku przeciwnika
    }

    
}

//Funkcja ataku przeciwnika
void Game::aiAttack() {
    bool attacked = false;
    int x;
    int y;
    while (!attacked) { 
        if (nextAIX != -1 && nextAIY != -1) //je¿eli przeciwnik ma zaplanowany kolejny atak to go wykonuje
        {
            x = nextAIX; 
            y = nextAIY;
        }
        else //inaczej jest losowo wybierany
        {
            std::vector<std::pair<int, int>> pozycje = playerBoard.getAttackableTiles();
			std::random_shuffle(pozycje.begin(), pozycje.end());
			x = pozycje[0].first;
			y = pozycje[0].second;
        }

        if(playerBoard.attack(sf::Vector2i(x, y))) playSound(aiShotSoundFilenames[soundTrackIndex]); // to losowanie wystêpuje ponownie
        attacked = true;
        playerBoard.updateSunkShips(autoMarker);
        aiBoard.updateSunkShips(autoMarker);
        setNextAiMove();//wywo³anie planowania kolejnego ataku
                                 //(x i y to poprzednie koordynaty)
        }
    }

//Logika Bota
void Game::setNextAiMove()
{
    //domyslnie ustawia na losowy atak czyli na -1 -1
    nextAIX = -1;
    nextAIY = -1;
    std::vector<Ship> LongShips = playerBoard.getVisibleLongShips(); //te funkcje tworza nowe statki na podstawie tego co mozna
    std::vector<Ship> SingleShips = playerBoard.getVisibleSingleShips();// zobaczyc i zwracaja je w wektorach z podzialem na 
    std::random_shuffle(LongShips.begin(), LongShips.end()); //pojedyncze i reszte
    std::random_shuffle(SingleShips.begin(), SingleShips.end());//a potem sa tasowane w celu uzyskania losowosci w wyborze,
    //co i tak nie powinno sie wydarzyc bo zawsze na planszy bedzie 1 niezbity

    if (LongShips.size() > 0) { //najpierw probujemy zatopic statki ktore maja trafione wiecej niz 1 pole, bo wiemy w ktora
        Ship ship = LongShips[0]; //strone moga znajdowac sie kolejne
        if (!ship.isHorizontal)
        {
            int x = ship.positions[ship.positions.size() - 1].x;
            int y = ship.positions[ship.positions.size() - 1].y + 1;// jesli poionowo, to probujemy zatopic w gore

            if (playerBoard.isOnBoard(x, y) && playerBoard.getCellStatus(x, y) != 2 && playerBoard.getCellStatus(x, y) != 3) {
                nextAIX = x;
                nextAIY = y;
                return;
            }
            x = ship.positions[0].x;
            y = ship.positions[0].y - 1; // a jesli nie da sie w gore to w dol
            if (playerBoard.isOnBoard(x, y) && playerBoard.getCellStatus(x, y) != 2 && playerBoard.getCellStatus(x, y) != 3) {

                nextAIX = x;
                nextAIY = y;
                return;
            }
        }
        else //z kolei gdy jest poziomo
        {
            int x = ship.positions[ship.positions.size() - 1].x + 1; //to probujemy zatopic w prawo
            int y = ship.positions[ship.positions.size() - 1].y;

            if (playerBoard.isOnBoard(x, y) && playerBoard.getCellStatus(x, y) != 2 && playerBoard.getCellStatus(x, y) != 3) {
                nextAIX = x;
                nextAIY = y;
                return;
            }


            x = ship.positions[0].x - 1; //a jesli nie da sie w prawo to w lewo
            y = ship.positions[0].y;

            if (playerBoard.isOnBoard(x, y) && playerBoard.getCellStatus(x, y) != 2 && playerBoard.getCellStatus(x, y) != 3) {
                nextAIX = x;
                nextAIY = y;
                return;
            }


        }
    }
    else if (SingleShips.size() > 0) { //jesli nie ma juz statkow ktore maja wiecej niz 1 pole trafione
        Ship ship = SingleShips[0];
        int x = ship.positions[0].x;
        int y = ship.positions[0].y - 1; //to probujemy zatopic pojedyncze najpierw w dol 

        if (playerBoard.isOnBoard(x, y) && playerBoard.getCellStatus(x, y) != 2 && playerBoard.getCellStatus(x, y) != 3) {
            nextAIX = x;
            nextAIY = y;
            return;
        }

        x = ship.positions[0].x - 1; //jesli nie, to w lewo
        y = ship.positions[0].y;

        if (playerBoard.isOnBoard(x, y) && playerBoard.getCellStatus(x, y) != 2 && playerBoard.getCellStatus(x, y) != 3) {
            nextAIX = x;
            nextAIY = y;
            return;
        }

        x = ship.positions[0].x + 1; //jesli nie, to w prawo
        y = ship.positions[0].y;

        if (playerBoard.isOnBoard(x, y) && playerBoard.getCellStatus(x, y) != 2 && playerBoard.getCellStatus(x, y) != 3) {
            nextAIX = x;
            nextAIY = y;
            return;
        }

        x = ship.positions[0].x;
        y = ship.positions[0].y + 1; //jesli nie, to w gore

        if (playerBoard.isOnBoard(x, y) && playerBoard.getCellStatus(x, y) != 2 && playerBoard.getCellStatus(x, y) != 3) {
            nextAIX = x;
            nextAIY = y;
            return;
        }


    }
}
    //Funkcja planowania nastêpnego ataku

//funkcja losowego rozmieszczenia statków przeciwnika
void Game::aiRandomPlacement() {
    while (true) { 
        bool allShipsPlaced = true;
        for (int size : shipSizes) {
            Ship ship(size);
            std::vector<std::tuple<int, int, bool>> validplacements = getValidPlacements(aiBoard, ship); //pobieramy liste mozliwych 
                                                                            //ustawien statku ktora od razu jest rozlosowana
			if (validplacements.empty()) { //jesli nie ma mozliwosci ustawienia statku to resetujemy plansze i zaczynamy od nowa
                aiBoard.clear();
                allShipsPlaced = false;
                break;
            }
			aiBoard.placeShip(ship, sf::Vector2i(std::get<0>(validplacements[0]), std::get<1>(validplacements[0])), std::get<2>(validplacements[0])); //jak jest ustawienie to ustawiamy na nim statek
        }
        if (allShipsPlaced) {
            break;
        } 
    }
}

//funkcja sprawdzajaca czy statki o podanej ilosci da sie ustawic na planszy
bool Game::isPlacementPossible(std::vector<int> theoryshipSizes) { 
    int i = 0;
    Board theoryboard;
    while (true) {
        bool allShipsPlaced = true;
        for (int size : theoryshipSizes) {
            Ship ship(size);
            std::vector<std::tuple<int, int, bool>> validplacements = getValidPlacements(theoryboard, ship);
            if (validplacements.empty()) {
                theoryboard.clear();
                allShipsPlaced = false;
                i++;
                if (i>1000)
                {
                    return false;
                }
                break;
            }
            theoryboard.placeShip(ship, sf::Vector2i(std::get<0>(validplacements[0]), std::get<1>(validplacements[0])), std::get<2>(validplacements[0]));
        }
        if (allShipsPlaced) {
            theoryboard.clear();
            return true;
        }
    }
}

//funkcja ustawiajaca reszte statkow, gdy czesc jest juz ustawiona
Board Game::placeRemainingShips(std::vector<int> theoryshipSizes, Board currentboard) {
    int i = 0;
	Board theoryboard = currentboard;
    while (true) {
        bool allShipsPlaced = true;
        for (int size : theoryshipSizes) {
            Ship ship(size);
            std::vector<std::tuple<int, int, bool>> validplacements = getValidPlacements(theoryboard, ship);
            if (validplacements.empty()) {
                theoryboard.clear();
				theoryboard = currentboard;
                allShipsPlaced = false;
                i++;
                if (i>1000)
                {
                    return Board();
                }
                break;
            }
            theoryboard.placeShip(ship, sf::Vector2i(std::get<0>(validplacements[0]), std::get<1>(validplacements[0])), std::get<2>(validplacements[0]));
        }
        if (allShipsPlaced) {
            return theoryboard;
        }
    }
}

//funkcja rysuj¹ca ekran pauzy
void Game::drawPauseScreen(sf::RenderWindow& window)
{
	//Funkcja rysuj¹ca ekran pytaj¹cy czy na pewno chcesz zresetowaæ grê?
	sf::Text text;  
	text.setFont(font);
	text.setCharacterSize(30);
	text.setFillColor(sf::Color::Green);
	text.setStyle(sf::Text::Bold);
	sf::FloatRect textRect = text.getLocalBounds();
	text.setOrigin((textRect.left + textRect.width / 2.0f) - 30, textRect.top + textRect.height / 2.0f - 20);
	text.setPosition(sf::Vector2f(boardOffsetX, boardOffsetY + 70));
    if (gameStarted)
    {
        text.setString("\t \t \t \t \t \t \t \t PAUZA \n R - Resetuj Gre \n T - Zmien Sciezke dzwiekowa \n M - Mute \n Enter - Wroc do gry");
    }
    else
    {
        text.setString("\t \t \t \t \t \t \t \t PAUZA \n R - Resetuj Wybor \n W - Wroc do wyboru statkow");
    }
	sf::RectangleShape rectangle(sf::Vector2f(800, 220));
	rectangle.setOrigin(rectangle.getSize().x / 2.0f, rectangle.getSize().y / 2.0f);
	rectangle.setPosition(sf::Vector2f(525, 250));
	rectangle.setFillColor(sf::Color::Black);
	rectangle.setOutlineThickness(5);
	rectangle.setOutlineColor(sf::Color::Green);
	window.draw(rectangle);
	window.draw(text);


}



//funkcja k³adzenia statku przez gracza
void Game::playerPlacement(sf::Event event, bool* shipsplaced, bool restart) {
    static int currentShip = 0;

	if (restart) { //resetowanie statycznej zmiennej currentShip
        currentShip = 0;
    }
    else {
        if (currentShip >= shipSizes.size()) {
            *shipsplaced = true;
            return;
        }

        int mouseX = event.mouseButton.x;
        int mouseY = event.mouseButton.y;
        int col = (mouseX - boardOffsetX) / CELL_SIZE;
        int row = (mouseY - boardOffsetY) / CELL_SIZE;
        Ship ship(shipSizes[currentShip]); //Lewym poziomo, prawym pionowo
        currentShipSize = shipSizes[currentShip];
        if (event.key.code == sf::Keyboard::R)  //je¿eli wciœniêto r
        {
            playerBoard.clear(); //resetuje ustawienie statków
            currentShip = 0;
        }
        if (!showPauseScreen && event.type == sf::Event::MouseButtonPressed) {

            bool isHorizontal = event.mouseButton.button == sf::Mouse::Left;
            if (col >= 0 && row >= 0 && col < GRID_SIZE && row < GRID_SIZE) {
                if (playerBoard.isValidPlacement(ship, sf::Vector2i(col, row), isHorizontal)) {
                    playerBoard.placeShip(ship, sf::Vector2i(col, row), isHorizontal);
                    currentShip++;
                }
            }
        }
        if (!showPauseScreen && event.type == sf::Event::KeyPressed && event.key.code == sf::Keyboard::Q)
        {
            
            std::vector<int> theoryshipSizes;
			for (int i = currentShip; i < shipSizes.size(); i++)
			{
				theoryshipSizes.push_back(shipSizes[i]);
			}
            Board temp = placeRemainingShips(theoryshipSizes, playerBoard);
            if (temp.getNumberOfShips()!=0)
            {
				playerBoard = temp;
				currentShip = shipSizes.size();
            }


        }
    }
}

//funkcja zwracaj¹ca mo¿liwe ustawienia statku na planszy
std::vector<std::tuple<int, int, bool>> Game::getValidPlacements(Board& board, Ship &ship) {
    std::vector<std::tuple<int, int, bool>> validPlacements;
    for (int x = 0; x < 10; ++x) {
        for (int y = 0; y < 10; ++y) {
            for (bool isHorizontal : {true, false}) {
                if (board.isValidPlacement(ship, sf::Vector2i(x, y), isHorizontal)) {
                    validPlacements.emplace_back(x, y, isHorizontal);
                }
            }
        }
    }
    std::random_shuffle(validPlacements.begin(), validPlacements.end());
    return validPlacements;
}

//funkcja obs³uguj¹ca pole do wpisywania tekstu
void Game::handleTextBox(sf::Event event, float x, float y, int max) {
    textBoxText.setFont(font);
    textBoxText.setCharacterSize(30);
    textBoxText.setFillColor(sf::Color::Green);
    textBoxText.setStyle(sf::Text::Bold);
    textBoxText.setPosition(x, y);
    if (event.type == sf::Event::KeyPressed && event.key.code == sf::Keyboard::BackSpace && playerInput.getSize() > 0) 
    {
        std::string temp = playerInput; 
        temp.resize(temp.size() - 1);
        playerInput = temp;
    }
    if (event.type == sf::Event::KeyPressed && event.key.code == sf::Keyboard::Enter)
    {
        if (playerInput.getSize() > 0) 
        {
            currentNumberOfShips = std::stoi((std::string)(playerInput));
        }
        else
        {
            currentNumberOfShips = 0;
        }
        typedIn = true;
    }

    if (event.type == sf::Event::TextEntered) {
        if (event.text.unicode <= '0' && event.text.unicode >= '9')
        {
            if (!(playerInput.getSize() == 0 && '0' == event.text.unicode))
            {
                if (std::stoi((std::string)(playerInput + event.text.unicode)) <= max) //je¿eli nie bêdzie wieksza ni¿ max
                {
                    playerInput += event.text.unicode;
                }
                else
                {
                    playerInput = std::to_string(max);
                }
            }

        }
        if (playerInput.getSize() != 0)
        {
            textBoxText.setString(playerInput);
        }
        else
        {
            textBoxText.setString("0");
        }

    }
}

//funkcja znajduj¹ca maksymaln¹ iloœæ statków o podanej wielkoœci, które mo¿na postawiæ na planszy
int Game::findCurrentMax(int size) {
    int wynik = 0;
    std::vector<int> theoryshipSizes = shipSizes;
    while (true) {
        theoryshipSizes.push_back(size);
        if (isPlacementPossible(theoryshipSizes))
        { wynik++; }
        else {
            return wynik;
        }
    }
}

//funkcja rysuj¹ca pole informujace o statkach ktore sa posrod wybranych
void Game::drawChosenShips(sf::RenderWindow& window) {
    sf::Vector2f offset = sf::Vector2f(430, 140);
    sf::Text text;
    text.setFont(font);
    text.setCharacterSize(24);
    text.setFillColor(sf::Color::Green);
    
    int remshipcount[] = { 0,0,0,0,0,0 };
    for (int shipsize : shipSizes)
    {
        remshipcount[shipsize]++;
    }
    float smallCELL_SIZE = CELL_SIZE * 0.75;
    text.setPosition(offset.x + 600, offset.y - 50);
	text.setString("Wybrane statki: ");
    if (shipChoiceProgress>1)
    {
        window.draw(text);
    }
    
    for (int i = 1; i <= 5; i++)
    {
        if (remshipcount[i] != 0)
        {
            for (int a = 0; a < i; a++)
            {
                sf::RectangleShape cell(sf::Vector2f(smallCELL_SIZE - 2, smallCELL_SIZE - 2));
                cell.setOutlineColor(sf::Color::Black);
                cell.setOutlineThickness(2);
                cell.setPosition(offset.x + 660 + a * smallCELL_SIZE, offset.y - 50 + 55 * i);
                cell.setFillColor(sf::Color::Green);
                window.draw(cell);
            }
            text.setString(std::to_string(remshipcount[i]));
            text.setPosition(offset.x + 630, offset.y - 50 + 55 * i);
            window.draw(text);
        }
    }
}

//funkcja zwracaj¹ca statki, które nie zosta³y jeszcze ustawione
std::vector<int> Game::getUnplacedShipSizes() {
	std::vector<int> unplaced;
	int i = 0;
    int j = 0;
	std::vector<int> placedships = playerBoard.getShipSizes();
    if (placedships.empty())
    {
		return shipSizes;
    }
    while (i<shipSizes.size()) {
        if (shipSizes[i]!=placedships[j])
        {
			unplaced.push_back(shipSizes[i]);
        }
		else
		{
			j++;
		}
        i++;
    }
	return unplaced;
}

//funkcja rysuj¹ca statki, które pozosta³y do ustawienia
void Game::drawRemainingShipsToPlace(sf::RenderWindow& window) {
    sf::Vector2f offset = sf::Vector2f(430, 140);
    sf::Text text;
    text.setFont(font);
    text.setCharacterSize(24);
    text.setFillColor(sf::Color::Green);
    text.setPosition(offset.x + 600, offset.y - 50);
	int remshipcount[] = { 0,0,0,0,0,0 };
    float smallCELL_SIZE = CELL_SIZE * 0.75;
    text.setPosition(offset.x + 600, offset.y - 50);
    text.setString("Statki do ustawienia: ");
	window.draw(text);
    for (int size : unplacedShipSizes)
	{
           remshipcount[size]++;
	}


    for (int i = 1; i <= 5; i++)
    {
        if (remshipcount[i] != 0)
        {

            for (int a = 0; a < i; a++)
            {
                sf::RectangleShape cell(sf::Vector2f(smallCELL_SIZE - 2, smallCELL_SIZE - 2));
                cell.setOutlineColor(sf::Color::Black);
                cell.setOutlineThickness(2);
                cell.setPosition(offset.x + 660 + a * smallCELL_SIZE, offset.y - 50 + 55 * i);
                cell.setFillColor(sf::Color::Green);
                window.draw(cell);
            }
            text.setString(std::to_string(remshipcount[i]));
            text.setPosition(offset.x + 630, offset.y - 50 + 55 * i);
            window.draw(text);
        }
    }

}
