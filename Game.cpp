#include "Game.h"


Game::Game() : playerBoard(), aiBoard() {
    //ustawienie pocz¹tkowych wartoœci
    font.loadFromFile("arial.ttf");
	gameStarted = false; 
	progress = 0;
    gameOver = false;
    shipsPlaced = false;
    ktowygral = false;
    currentshipsize = 0;
	mute = true;
	automarker = true;
	playershotsoundfilenames = { "bum1.wav","hihihiha.wav" };
	aishotsoundfilenames = {"bum.wav","garr.wav"  };
	winsoundfilenames = { "win.wav","wincr.wav" };
	losesoundfilenames = { "lose.wav","losecr.wav" };
}

void Game::run() {
    sf::RenderWindow window(sf::VideoMode(1250, 500), "Statki by Mateusz Jarzab", sf::Style::Titlebar |sf::Style::Close); //Tworzenie okna
    while (window.isOpen()) { //Pêtla gry
        sf::Event event;
        while (window.pollEvent(event)) { //Pêtla w której mo¿na korzystaæ z event
            if (event.type == sf::Event::Closed) //Obs³uga zamkniêcia okna
                window.close();

            if (!gameStarted) { // Faza wyboru statków
                handleShipChoiceEvent(event); //Wywo³anie funkcji wyboru statków
            }
            else { 
                if (!shipsPlaced) { //Faza k³adzenia statków przez gracza
                    
                    PlayerPlacement(event, &shipsPlaced);
					if (event.type == sf::Event::MouseButtonPressed) unplacedshipsizes = getUnplacedShipsizes(); //Aktualizacja tablicy niepostawionych statków
                }
                else if (event.type == sf::Event::MouseButtonPressed && !gameOver && gameStarted) { //Faza gry
                    handlePlayerAttack(event.mouseButton.x, event.mouseButton.y); //Wywo³anie funkcji odpowiadaj¹cej za atak gracza
					playerBoard.sunkshipsupdater(automarker);
                    aiBoard.sunkshipsupdater(automarker); 
                    if (aiBoard.isGameOver()) { //sprawdzanie czy gracz wygra³
                        playsound(winsoundfilenames[track]);
                        gameOver = true;
                        ktowygral = true; //true = gracz wygra³
                    }
                   
                    if (playerBoard.isGameOver()) { //sprawdzanie czy przeciwnik wygra³
                        playsound(losesoundfilenames[track]);
                        gameOver = true;
                        ktowygral = false; //false = przeciwnik wygra³
                    }
                }
            }
            if (gameOver && event.type == sf::Event::KeyPressed) { //Restart gry
                resetGame();
            }
            if (event.type == sf::Event::KeyPressed && event.key.code == sf::Keyboard::Escape) //Ekran pauzy
            {
                pausescreen = true;

            }
            if (pausescreen && event.type == sf::Event::KeyPressed && event.key.code == sf::Keyboard::R) //Reset gry
            {
                resetGame();
                pausescreen = false;

            }
            if (gameStarted && pausescreen && event.type == sf::Event::KeyPressed && event.key.code == sf::Keyboard::Enter) //Powrót do gry
            {
                pausescreen = false;
            }
            if (!gameStarted && pausescreen && event.type == sf::Event::KeyPressed && event.key.code == sf::Keyboard::W) //Odpauzowywanie podczas wyboru statków
            {
                pausescreen = false;
            }
			if (gameStarted && pausescreen && event.type == sf::Event::KeyPressed && event.key.code == sf::Keyboard::T) //zmiana œcie¿ki dŸwiêkowej
            {
                if (track == 1) track = 0;
                else track = 1;
            }
            if (gameStarted && event.type == sf::Event::KeyPressed && event.key.code == sf::Keyboard::M) {//Wy³¹czanie/W³¹czanie dŸwiêku
                mute = !mute;
            }
        }
        


        // Usuwanie dŸwiêków, które siê zakoñczy³y
        sounds.erase(std::remove_if(sounds.begin(), sounds.end(),
            [](const std::shared_ptr<sf::Sound>& sound) { return sound->getStatus() == sf::Sound::Stopped; }),
            sounds.end());

        window.clear();
        drawpngBackground(window, "tlo.png"); //Rysowanie t³a
        drawMisc(window); //Rysowanie pobocznych napisów
		if(shipsPlaced)aiBoard.drawRemainingShips(window);
        if (gameStarted) { //Je¿eli gra siê zaczê³a rysujê plansze
            
            if (!gameOver) {
                // Tworzenie graficznych reprezentacji planszy w oknie
                playerBoard.DrawTitle(window, sf::Vector2f(BOARD_OFFSET_X, BOARD_OFFSET_Y)); 
                playerBoard.draw(window, sf::Vector2f(BOARD_OFFSET_X, BOARD_OFFSET_Y));
                aiBoard.draw(window, sf::Vector2f(BOARD_OFFSET_X + 500, BOARD_OFFSET_Y), false);
                
            }
            else { //Je¿eli gra siê zakoñczy³a rysujê ekran koñcowy
                playerBoard.DrawTitle(window, sf::Vector2f(BOARD_OFFSET_X, BOARD_OFFSET_Y));
                playerBoard.draw(window, sf::Vector2f(BOARD_OFFSET_X, BOARD_OFFSET_Y));
                aiBoard.draw(window, sf::Vector2f(BOARD_OFFSET_X + 500, BOARD_OFFSET_Y), true);
                sf::RectangleShape rectangle(sf::Vector2f(600, 100));
                rectangle.setFillColor(sf::Color::Blue);
                rectangle.setOrigin(rectangle.getSize().x / 2.0f, rectangle.getSize().y / 2.0f);
                rectangle.setPosition(sf::Vector2f(window.getSize().x / 2.0f, window.getSize().y / 2.0f));
                window.draw(rectangle);

                sf::Text text;
                text.setFont(font);

                if (ktowygral) { //Zale¿nie od wygranego, wiadomoœæ jest inna i ma inny kolor
                    text.setString("Koniec gry, Wygrales!");
                    text.setFillColor(sf::Color::Green);
                }
                else {
                    text.setString("Koniec gry, Przegrales!");
                    text.setFillColor(sf::Color::Red);
                }

                text.setCharacterSize(50);
                text.setStyle(sf::Text::Bold);

                sf::FloatRect textRect = text.getLocalBounds(); //Rysujê prostok¹t za tekstem aby by³o bardziej przejrzyœcie
                text.setOrigin(textRect.left + textRect.width / 2.0f, textRect.top + textRect.height / 2.0f);
                text.setPosition(sf::Vector2f(window.getSize().x / 2.0f, window.getSize().y / 2.0f));

                window.draw(text);
            }
        }
        else { //Je¿eli gra siê nie zaczê³a, Rysujê ekran wyboru statków
            DrawShipsChoiceScreen(window);
			drawChosenShips(window);
        }

		if (pausescreen) drawResetScreen(window);  //Rysujê ekran resetowania gry
        window.display();
    }
}

void Game::playsound(const std::string& filename) {
    if (!mute)
    {
        sf::SoundBuffer buffer;
        if (!buffer.loadFromFile(filename)) {
            return;
        }

        // Store the buffer in the container to keep it alive
        soundBuffers.push_back(buffer);

        // Create a shared pointer to a new sf::Sound object
        auto sound = std::make_shared<sf::Sound>();
        sound->setBuffer(soundBuffers.back());
        sound->play();

        // Store the shared pointer in the container to keep the sound alive
        sounds.push_back(sound);
    }
    
}
void Game::drawpngBackground(sf::RenderWindow& window, const std::string& filename) { //Funkcja rysuj¹ca t³o
	sf::Texture tlo,muteicon;
	if (!tlo.loadFromFile(filename)) {
		// b³¹d wczytywania obrazka
	}
    muteicon.loadFromFile("mute.png");
	sf::Sprite sprite,sprite1;
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


void Game::drawMisc(sf::RenderWindow & window) { //funkcja pisz¹ca poboczne napisy
    sf::Text napisstatki, podpis;
    napisstatki.setFont(font);
    podpis.setFont(font);
    podpis.setCharacterSize(20);
    podpis.setFillColor(sf::Color::Black);
    podpis.setStyle(sf::Text::Bold);
    podpis.setPosition(BOARD_OFFSET_X + 800, BOARD_OFFSET_Y + 420);
    podpis.setString("Mateusz Jarzab 2024 L02 177086");
    napisstatki.setCharacterSize(30);
    napisstatki.setFillColor(sf::Color::Cyan);
    napisstatki.setStyle(sf::Text::Bold);
    napisstatki.setPosition(BOARD_OFFSET_X + 360, BOARD_OFFSET_Y - 50);
    napisstatki.setString("Gra w Statki");
    window.draw(podpis);
    window.draw(napisstatki);

    if (gameStarted && !shipsPlaced)
    {
        sf::Text info;
        info.setFont(font);
        info.setCharacterSize(20);
        info.setFillColor(sf::Color::Red);
        info.setStyle(sf::Text::Bold);
        sf::FloatRect textRect = info.getLocalBounds();
        info.setOrigin((textRect.left + textRect.width / 2.0f) - 30, textRect.top + textRect.height / 2.0f - 20);
        info.setPosition(sf::Vector2f(5, BOARD_OFFSET_Y + 380));

        info.setString("R - Reset statków  Q - Ustaw Reszte losowo  Esc - Pauza\nMysz lewy - postaw poziomo   Mysz prawy - postaw pionowo");

        
        sf::RectangleShape infoBackground(sf::Vector2f(600, 100));
        infoBackground.setFillColor(sf::Color::Black);
        infoBackground.setPosition(info.getPosition().x + 25, info.getPosition().y + 20);
		infoBackground.setOutlineThickness(2);
		infoBackground.setOutlineColor(sf::Color::Green);
        window.draw(infoBackground);
        

        drawRemainingShipstoplace(window);
        window.draw(info);
        
        sf::Text CurShipSizeDraw;
        CurShipSizeDraw.setFont(font);
        CurShipSizeDraw.setCharacterSize(30);
        CurShipSizeDraw.setFillColor(sf::Color::Magenta);
        CurShipSizeDraw.setStyle(sf::Text::Bold);
        sf::FloatRect textRect2 = CurShipSizeDraw.getLocalBounds();
        CurShipSizeDraw.setOrigin((textRect2.left + textRect2.width / 2.0f) - 30, textRect2.top + textRect2.height / 2.0f - 20);
        CurShipSizeDraw.setPosition(sf::Vector2f(-5, -5));
        CurShipSizeDraw.setString("Statek: \n    " + std::to_string(currentshipsize));
        window.draw(CurShipSizeDraw);
    }
}


void Game::drawaboatimage(sf::RenderWindow& window, int x, int y, const std::string& filename, float scale) { //Funkcja rysuj¹ca obrazek statku na ekranie pocz¹tkowym
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
	progress = 0;
	gameOver = false;
	shipsPlaced = false;
	ktowygral = false;
	nextAIX = -1;
	nextAIY = -1;
	typedin = false;
	pausescreen = false;
	PlayerPlacement(sf::Event(), &shipsPlaced, true); //W player placement jest statyczna zmienna currentShip, 
                                                    //która trzeba zresetowaæ, st¹d zastosowa³em alternatywne jej wywo³anie 
}


void Game::handleShipChoiceEvent(const sf::Event& event) {
    static int max;
    if (progress == 0) {

        if (event.type == sf::Event::KeyPressed && !pausescreen) {
            if (event.key.code == sf::Keyboard::Space) { //Kontynyuuj wybór statków
                progress = 1;
                textboxtext.setString("0");
                currentnumberofships = 0;
                typedin = false;
                playerInput = "";
                max = FindActMax(progress);
            }
            else if (event.key.code == sf::Keyboard::Enter) { //Wybór domyœlnych statków
                //default ships are 4 3 3 2 2 2 1 1 1 1
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
                
                unplacedshipsizes = getUnplacedShipsizes();
                progress = 6;

            }
            else if (event.key.code == sf::Keyboard::A) { //W³¹cz/Wy³¹cz autooznaczanie
                automarker = !automarker;
            }
        }
    }
    else if (progress < 6 && progress >= 1) {
        if (max == 0) progress = 6;
        textbox(event, BOARD_OFFSET_X + 505, BOARD_OFFSET_Y + 90, max);
        if (typedin) {
            for (int i = 0; i < currentnumberofships; i++)
            {
                shipSizes.push_back(progress);
            }
            progress++;
            textboxtext.setString("0");
            currentnumberofships = 0;
            playerInput = "";
            typedin = false;
            max = FindActMax(progress);
        }
    }
    else if(progress == 6)
    {
        
        if (shipSizes.size()==0) //jeœli nie wybrano ¿adnych statków to daje domyœlnie 1 ma³y
        {
            shipSizes.push_back(1);
        }
        unplacedshipsizes = getUnplacedShipsizes();
        aiRandomPlacement(); // Losowe rozmieszczenie statków przeciwnika
        gameStarted = true;
        
    }
}

void Game::DrawShipsChoiceScreen(sf::RenderWindow& window) {
    sf::Text text;
    text.setFont(font);
    text.setCharacterSize(30);
    text.setFillColor(sf::Color::Green);
    text.setStyle(sf::Text::Bold);
    sf::FloatRect textRect = text.getLocalBounds();
    text.setOrigin((textRect.left + textRect.width / 2.0f) - 30, textRect.top + textRect.height / 2.0f - 20);
    text.setPosition(sf::Vector2f(BOARD_OFFSET_X + 80, BOARD_OFFSET_Y + 70));


    sf::RectangleShape rectangle(sf::Vector2f(BOARD_OFFSET_X + 650, BOARD_OFFSET_Y + 220)); //Rysowanie prostok¹ta za tekstem
    rectangle.setOrigin(rectangle.getSize().x / 2.0f, rectangle.getSize().y / 2.0f);
    rectangle.setPosition(sf::Vector2f(window.getSize().x / 2.0f - 50, window.getSize().y / 2.0f));
    rectangle.setFillColor(sf::Color::Black);
    rectangle.setOutlineThickness(5);
    rectangle.setOutlineColor(sf::Color::Green);
    window.draw(rectangle);
    drawaboatimage(window, BOARD_OFFSET_X + 200, 270, "statek.png");
    drawaboatimage(window, BOARD_OFFSET_X + 800, 270, "statek1.png", 0.4);
    if (progress == 0) { //Instrukcja wyboru statków

        text.setString(" Enter - domyslna ilosc statkow (4x1,3x2,2x3,1x4) \n Spacja - wybierz samodzielnie \n A - Wylacz/wlacz autooznaczanie miejsc \n dookola zatopionych statkow: " + std::string(automarker ? "ON" : "OFF"));
    }
    else if (progress < 6 && progress >= 1) { //Wybór statku 5 kratkowego
        static int lastprogress=0;
        static std::string napis;
        
        if(lastprogress != progress){
            napis = "Wpisz ilosc " + std::to_string(progress) + "-masztowcow:";
            for (int i = progress; i < 6; i++)
            {
                int max = FindActMax(i);
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
        lastprogress = progress;
        text.setString(napis);
    }
    if(progress!=0 && progress !=6) window.draw(textboxtext);
    window.draw(text);


}
//Funkcja ataku gracza
void Game::handlePlayerAttack(int mouseX, int mouseY) {
    // Obliczanie koordynatów komórki klikniêtej myszk¹
    int col = (mouseX - (BOARD_OFFSET_X + 500)) / CELL_SIZE;
    int row = (mouseY - BOARD_OFFSET_Y) / CELL_SIZE;
    if (col >= 0 && row >= 0 && col < GRID_SIZE && row < GRID_SIZE) {   // Sprawdzenie czy atak jest na planszy
		bool bylopuste = aiBoard.getcellstatus(col, row) == 0 || aiBoard.getcellstatus(col, row) == 1;
        if (aiBoard.attack(sf::Vector2i(col, row))) { playsound(playershotsoundfilenames[track]); } // Atak na planszê przeciwnika
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

        if(playerBoard.attack(sf::Vector2i(x, y))) playsound(aishotsoundfilenames[track]); // to losowanie wystêpuje ponownie
        attacked = true;
        playerBoard.sunkshipsupdater(automarker);
        aiBoard.sunkshipsupdater(automarker);
        setnextaimove();//wywo³anie planowania kolejnego ataku
                                 //(x i y to poprzednie koordynaty)
        }
    }

void Game::setnextaimove()
{
    nextAIX = -1;
    nextAIY = -1;
	std::vector<Ship> LongShips = playerBoard.getVisibleShips();
	std::vector<Ship> SingleShips = playerBoard.getVisibleSingleShips();
    std::random_shuffle(LongShips.begin(), LongShips.end());
	std::random_shuffle(SingleShips.begin(), SingleShips.end());

	if (LongShips.size() > 0) {
		Ship ship = LongShips[0];
        if (!ship.horizontal)
        {
            int x = ship.positions[ship.positions.size() - 1].x;
			int y = ship.positions[ship.positions.size() - 1].y+1;

            if (playerBoard.isonboard(x,y) && playerBoard.getcellstatus(x, y) != 2 && playerBoard.getcellstatus(x, y) != 3) {
					nextAIX = x;
					nextAIY = y;
                    return;
            }
            x = ship.positions[0].x;
            y = ship.positions[0].y - 1;
        if (playerBoard.isonboard(x, y) && playerBoard.getcellstatus(x, y) != 2 && playerBoard.getcellstatus(x, y) != 3) {
                    
            nextAIX = x;
            nextAIY = y;
			return;
                }
            }
        else
        {
            int x = ship.positions[ship.positions.size() - 1].x+1;
            int y = ship.positions[ship.positions.size() - 1].y;
            if (playerBoard.isonboard(x, y)) {
                if (playerBoard.getcellstatus(x, y) != 2 && playerBoard.getcellstatus(x, y) != 3) {
                    nextAIX = x;
                    nextAIY = y;
					return;
                }
            }

                x = ship.positions[0].x-1;
                y = ship.positions[0].y;
                if (playerBoard.isonboard(x, y)) {
                    if (playerBoard.getcellstatus(x, y) != 2 && playerBoard.getcellstatus(x, y) != 3) {
                        nextAIX = x;
                        nextAIY = y;
                        return;
                    }
                }
            }
    }
	else if (SingleShips.size() > 0) {
		Ship ship = SingleShips[0];
		int x = ship.positions[0].x;
		int y = ship.positions[0].y - 1;
		if (playerBoard.isonboard(x, y)) {
			if (playerBoard.getcellstatus(x, y) != 2 && playerBoard.getcellstatus(x, y) != 3) {
				nextAIX = x;
				nextAIY = y;
				return;
			}
		}
		
		
			x = ship.positions[0].x - 1;
			y = ship.positions[0].y;
			if (playerBoard.isonboard(x, y)) {
				if (playerBoard.getcellstatus(x, y) != 2 && playerBoard.getcellstatus(x, y) != 3) {
					nextAIX = x;
					nextAIY = y;
					return;

				
			}
		}

		
			x = ship.positions[0].x + 1;
			y = ship.positions[0].y;
			if (playerBoard.isonboard(x, y)) {
				if (playerBoard.getcellstatus(x, y) != 2 && playerBoard.getcellstatus(x, y) != 3) {
					nextAIX = x;
					nextAIY = y;
					return;
				
			}
		}

			x = ship.positions[0].x;
			y = ship.positions[0].y + 1;
			if (playerBoard.isonboard(x, y)) {
				if (playerBoard.getcellstatus(x, y) != 2 && playerBoard.getcellstatus(x, y) != 3) {
					nextAIX = x;
					nextAIY = y;
					return;
		
			}
		}
   
		
	}
	

}//Funkcja planowania nastêpnego ataku

//funkcja losowego rozmieszczenia statków przeciwnika
void Game::aiRandomPlacement() {
    while (true) {
        bool allShipsPlaced = true;
        for (int size : shipSizes) {
            Ship ship(size);
            std::vector<std::tuple<int, int, bool>> validplacements = getValidPlacements(aiBoard, ship);
            if (validplacements.empty()) {
                aiBoard.clear();
                allShipsPlaced = false;
                break;
            }
            aiBoard.placeShip(ship, sf::Vector2i(std::get<0>(validplacements[0]), std::get<1>(validplacements[0])), std::get<2>(validplacements[0]));
        }
        if (allShipsPlaced) {
            break;
        } 
    }
}
  

bool Game::isplacementpossible(std::vector<int> theoryshipSizes) {
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

Board Game::placerestifpossible(std::vector<int> theoryshipSizes, Board currentboard) {
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

void Game::drawResetScreen(sf::RenderWindow& window)
{
	//Funkcja rysuj¹ca ekran pytaj¹cy czy na pewno chcesz zresetowaæ grê?
	sf::Text text;  
	text.setFont(font);
	text.setCharacterSize(30);
	text.setFillColor(sf::Color::Green);
	text.setStyle(sf::Text::Bold);
	sf::FloatRect textRect = text.getLocalBounds();
	text.setOrigin((textRect.left + textRect.width / 2.0f) - 30, textRect.top + textRect.height / 2.0f - 20);
	text.setPosition(sf::Vector2f(BOARD_OFFSET_X, BOARD_OFFSET_Y + 70));
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
void Game::PlayerPlacement(sf::Event event, bool* shipsplaced, bool restart) {
    static int currentShip = 0;

    if (restart) {
        currentShip = 0;
    }
    else {
        if (currentShip >= shipSizes.size()) {
            *shipsplaced = true;
            return;
        }

        int mouseX = event.mouseButton.x;
        int mouseY = event.mouseButton.y;
        int col = (mouseX - BOARD_OFFSET_X) / CELL_SIZE;
        int row = (mouseY - BOARD_OFFSET_Y) / CELL_SIZE;
        Ship ship(shipSizes[currentShip]); //Lewym poziomo, prawym pionowo
        currentshipsize = shipSizes[currentShip];
        if (event.key.code == sf::Keyboard::R)  //je¿eli wciœniêto r
        {
            playerBoard.clear(); //resetuje ustawienie statków
            currentShip = 0;
        }
        if (!pausescreen && event.type == sf::Event::MouseButtonPressed) {

            bool horizontal = event.mouseButton.button == sf::Mouse::Left;
            if (col >= 0 && row >= 0 && col < GRID_SIZE && row < GRID_SIZE) {
                if (playerBoard.isValidPlacement(ship, sf::Vector2i(col, row), horizontal)) {
                    playerBoard.placeShip(ship, sf::Vector2i(col, row), horizontal);
                    currentShip++;
                }
            }
        }
        if (!pausescreen && event.type == sf::Event::KeyPressed && event.key.code == sf::Keyboard::Q)
        {
            
            std::vector<int> theoryshipSizes;
			for (int i = currentShip; i < shipSizes.size(); i++)
			{
				theoryshipSizes.push_back(shipSizes[i]);
			}
            Board temp = placerestifpossible(theoryshipSizes, playerBoard);
            if (temp.GetNumberOfShips()!=0)
            {
				playerBoard = temp;
				currentShip = shipSizes.size();
            }


        }
    }
}

std::vector<std::tuple<int, int, bool>> Game::getValidPlacements(Board& board, Ship &ship) {
    std::vector<std::tuple<int, int, bool>> validPlacements;
    for (int x = 0; x < 10; ++x) {
        for (int y = 0; y < 10; ++y) {
            for (bool horizontal : {true, false}) {
                if (board.isValidPlacement(ship, sf::Vector2i(x, y), horizontal)) {
                    validPlacements.emplace_back(x, y, horizontal);
                }
            }
        }
    }
    std::random_shuffle(validPlacements.begin(), validPlacements.end());
    return validPlacements;
}

void Game::textbox(sf::Event event, float x, float y, int max) {
    textboxtext.setFont(font);
    textboxtext.setCharacterSize(30);
    textboxtext.setFillColor(sf::Color::Green);
    textboxtext.setStyle(sf::Text::Bold);
    textboxtext.setPosition(x, y);
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
            currentnumberofships = std::stoi((std::string)(playerInput));
        }
        else
        {
            currentnumberofships = 0;
        }
        typedin = true;
    }

    if (event.type == sf::Event::TextEntered) {
        if (event.text.unicode <= 57 && event.text.unicode >= 48) //je¿eli liczba
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
            textboxtext.setString(playerInput);
        }
        else
        {
            textboxtext.setString("0");
        }

    }
}


int Game::FindActMax(int size) {
    int wynik = 0;
    std::vector<int> theoryshipSizes = shipSizes;
    while (true) {
        theoryshipSizes.push_back(size);
        if (isplacementpossible(theoryshipSizes))
        { wynik++; }
        else {
            return wynik;
        }
    }
}

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
    if (progress>1)
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

std::vector<int> Game::getUnplacedShipsizes() {
	std::vector<int> unplaced;
	int i = 0;
    int j = 0;
	std::vector<int> placedships = playerBoard.getshipSizes();
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

void Game::drawRemainingShipstoplace(sf::RenderWindow& window) {
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
    for (int size : unplacedshipsizes)
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
