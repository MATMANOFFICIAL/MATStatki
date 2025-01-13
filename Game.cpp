#include "Game.h"


Game::Game() : playerBoard(), aiBoard() {
    if (!font.loadFromFile("arial.ttf")) {
        // b³¹d wczytywania czcionki
    }
    //ustawienie pocz¹tkowych wartoœci
	gameStarted = false; 
	progress = 0;
    gameOver = false;
    shipsPlaced = false;
    ktowygral = false;
    currentshipsize = 0;
	mute = true;
	playershotsoundfilenames = { "bum1.wav","hihihiha.wav" };
	aishotsoundfilenames = {"bum.wav","garr.wav"  };
	winsoundfilenames = { "win.wav","wincr.wav" };
	losesoundfilenames = { "lose.wav","losecr.wav" };
}



void Game::run() {
    sf::RenderWindow window(sf::VideoMode(1050, 500), "Statki by Mateusz Jarzab", sf::Style::Titlebar |sf::Style::Close); //Tworzenie okna
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
                }
                else if (event.type == sf::Event::MouseButtonPressed && !gameOver && gameStarted) { //Faza gry
                    handlePlayerAttack(event.mouseButton.x, event.mouseButton.y); //Wywo³anie funkcji odpowiadaj¹cej za atak gracza
                    playerBoard.sunkshipsupdater();
                    playerBoard.sunkshipboardchanger();
                    aiBoard.sunkshipsupdater();
                    aiBoard.sunkshipboardchanger();
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

			if (event.type == sf::Event::KeyPressed) { //Wy³¹czanie/W³¹czanie dŸwiêku
				if (event.key.code == sf::Keyboard::M) {
					mute = !mute;
				}
                if (event.key.code == sf::Keyboard::T)
                {
					if (track == winsoundfilenames.size()) track = 0;
                    else track++;
                }
			}
        }
        
        // Usuwanie dŸwiêków, które siê zakoñczy³y
        sounds.erase(std::remove_if(sounds.begin(), sounds.end(),
            [](const std::shared_ptr<sf::Sound>& sound) { return sound->getStatus() == sf::Sound::Stopped; }),
            sounds.end());

        window.clear();
        drawpngBackground(window, "tlo.png"); //Rysowanie t³a
        drawMisc(window, font, BOARD_OFFSET_X, BOARD_OFFSET_Y); //Rysowanie pobocznych napisów
        
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

        }

        window.display();
    }
}

void Game::playsound(const std::string& filename) {
    if (!mute)
    {
        sf::SoundBuffer buffer;
        if (!buffer.loadFromFile(filename)) {
            std::cerr << "Error loading sound file: " << filename << std::endl;
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
    if (!muteicon.loadFromFile("mute.png")) {
        // b³¹d wczytywania obrazka
    }
	sf::Sprite sprite,sprite1;
	sprite.setTexture(tlo);
	sprite.setScale(0.75, 0.75);
	sprite1.setTexture(muteicon);
	sprite1.setScale(0.1, 0.1);
	sprite1.setPosition(1000, 0);
	window.draw(sprite);
    if (mute)
    {
        window.draw(sprite1);
    }
}


void Game::drawMisc(sf::RenderWindow & window, sf::Font & font, int BOARD_OFFSET_X, int BOARD_OFFSET_Y) { //funkcja pisz¹ca poboczne napisy
    sf::Text napisstatki, podpis;
    napisstatki.setFont(font);
    podpis.setFont(font);
    podpis.setCharacterSize(20);
    podpis.setFillColor(sf::Color::Black);
    podpis.setStyle(sf::Text::Bold);
    podpis.setPosition(BOARD_OFFSET_X + 600, BOARD_OFFSET_Y + 420);
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
        info.setString("R - Reset statków \nMysz lewy - postaw poziomo   Mysz prawy - postaw pionowo");

        // Draw black rectangle behind the info text
        sf::RectangleShape infoBackground(sf::Vector2f(textRect.width + 20, textRect.height + 20));
        infoBackground.setFillColor(sf::Color::Black);
        infoBackground.setPosition(info.getPosition().x - 10, info.getPosition().y - 10);
        window.draw(infoBackground);

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
        // b³¹d wczytywania obrazka
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
	PlayerPlacement(sf::Event(), &shipsPlaced, true); //W player placement jest statyczna zmienna currentShip, 
                                                    //która trzeba zresetowaæ, st¹d zastosowa³em alternatywne jej wywo³anie 
}

//void Game::handleShipChoiceEvent(const sf::Event& event) { //Funkcja wyboru jakie statki mo¿na postawiæ na planszy
//	int maxships = 12;
//    if (progress == 0) {
//
//        if (event.type == sf::Event::KeyPressed) {
//            if (event.key.code == sf::Keyboard::Space) { //Kontynyuuj wybór statków
//                progress = 1;
//            }
//			else if (event.key.code == sf::Keyboard::Enter) { //Wybór domyœlnych statków
//                //default ships are 4 3 3 2 2 2 1 1 1 1
//                shipSizes.push_back(4);
//                shipSizes.push_back(3);
//                shipSizes.push_back(3);
//                shipSizes.push_back(2);
//                shipSizes.push_back(2);
//                shipSizes.push_back(2);
//                shipSizes.push_back(1);
//                shipSizes.push_back(1);
//                shipSizes.push_back(1);
//                shipSizes.push_back(1);
//                progress = 5;
//            }
//        }
//    }
//	else if (progress == 1) {  //Statek 1 kratkowy
//        if (event.type == sf::Event::KeyPressed) {
//
//
//            if (event.key.code == sf::Keyboard::Num0)
//            {
//				progress = 2;
//            }
//            else if (event.key.code == sf::Keyboard::Num1 && shipSizes.size() + 1 <= maxships) {
//                shipSizes.push_back(5);
//                progress = 2;
//            }
//            else if (event.key.code == sf::Keyboard::Num2 && shipSizes.size()+2<=maxships) {
//                shipSizes.push_back(5);
//                shipSizes.push_back(5);
//                progress = 2;
//            }
//            else if (event.key.code == sf::Keyboard::Num3 && shipSizes.size() + 3 <= maxships) {
//                shipSizes.push_back(5);
//                shipSizes.push_back(5);
//                shipSizes.push_back(5);
//                progress = 2;
//            }
//            else
//            {
//
//            }
//
//        }
//    }
//	else if (progress == 2) { //Statek 4 kratkowy
//        
//        if (event.type == sf::Event::KeyPressed) {
//
//            if (event.key.code == sf::Keyboard::Num0)
//            {
//                progress = 3;
//            }
//            else if (event.key.code == sf::Keyboard::Num1 && shipSizes.size() + 1 <= maxships) {
//                shipSizes.push_back(4);
//                progress = 3;
//            }
//            else if (event.key.code == sf::Keyboard::Num2 && shipSizes.size() + 2 <= maxships) {
//                shipSizes.push_back(4);
//                shipSizes.push_back(4);
//                progress = 3;
//            }
//            else if (event.key.code == sf::Keyboard::Num3 && shipSizes.size() + 3 <= maxships) {
//                shipSizes.push_back(4);
//                shipSizes.push_back(4);
//                shipSizes.push_back(4);
//                progress = 3;
//            }
//        }
//    }
//	else if (progress == 3) {       //Statek 3 kratkowy
//        if (event.type == sf::Event::KeyPressed) {
//            if (event.key.code == sf::Keyboard::Num0)
//            {
//                progress = 4;
//            }
//            else if (event.key.code == sf::Keyboard::Num1 && shipSizes.size() + 1 <= maxships) {
//                shipSizes.push_back(3);
//                progress = 4;
//            }
//            else if (event.key.code == sf::Keyboard::Num2 && shipSizes.size() + 2 <= maxships) {
//                shipSizes.push_back(3);
//                shipSizes.push_back(3);
//                progress = 4;
//            }
//            else if (event.key.code == sf::Keyboard::Num3 && shipSizes.size() + 3 <= maxships) {
//                shipSizes.push_back(3);
//                shipSizes.push_back(3);
//                shipSizes.push_back(3);
//                progress = 4;
//            }
//        }
//    }
//	else if (progress == 4) { //Statek 2 kratkowy
//        if (event.type == sf::Event::KeyPressed) {
//            if (event.key.code == sf::Keyboard::Num0)
//            {
//                progress = 5;
//            }
//            else if (event.key.code == sf::Keyboard::Num1 && shipSizes.size() + 1 <= maxships) {
//                shipSizes.push_back(2);
//                progress = 5;
//            }
//            else if (event.key.code == sf::Keyboard::Num2 && shipSizes.size() + 2 <= maxships) {
//                shipSizes.push_back(2);
//                shipSizes.push_back(2);
//                progress = 5;
//            }
//            else if (event.key.code == sf::Keyboard::Num3 && shipSizes.size() + 3 <= maxships) {
//                shipSizes.push_back(2);
//                shipSizes.push_back(2);
//                shipSizes.push_back(2);
//                progress = 5;
//            }
//        }
//    }
//	else if (progress == 5) { //Koniec wyboru statków
//        aiRandomPlacement(); // Losowe rozmieszczenie statków przeciwnika
//        gameStarted = true;
//    }
//}

void Game::handleShipChoiceEvent(const sf::Event& event) {
    static int max;
    if (progress == 0) {

        if (event.type == sf::Event::KeyPressed) {
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
                
                
                progress = 6;

            }
        }
    }
    else if (progress < 6 && progress >= 1) {
        if (max == 0) progress = 6;
        textbox(event, BOARD_OFFSET_X + 425, BOARD_OFFSET_Y + 90, max);
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
        aiRandomPlacement(); // Losowe rozmieszczenie statków przeciwnika
        gameStarted = true;
        
    }

}

//void Game::DrawShipsChoiceScreen(sf::RenderWindow& window) { //Funkcja rysuj¹ca ekran wyboru statków
//    sf::Text text;
//    text.setFont(font);
//    text.setCharacterSize(30);
//    text.setFillColor(sf::Color::Green);
//    text.setStyle(sf::Text::Bold);
//    sf::FloatRect textRect = text.getLocalBounds();
//    text.setOrigin((textRect.left + textRect.width / 2.0f)-30, textRect.top + textRect.height / 2.0f - 20);
//    text.setPosition(sf::Vector2f(BOARD_OFFSET_X, BOARD_OFFSET_Y + 100));
//
//
//    sf::RectangleShape rectangle(sf::Vector2f(800, 180)); //Rysowanie prostok¹ta za tekstem
//	rectangle.setOrigin(rectangle.getSize().x / 2.0f, rectangle.getSize().y / 2.0f);
//	rectangle.setPosition(sf::Vector2f(window.getSize().x / 2.0f, window.getSize().y / 2.0f));
//	rectangle.setFillColor(sf::Color::Black);
//	rectangle.setOutlineThickness(5);
//	rectangle.setOutlineColor(sf::Color::Green);
//	window.draw(rectangle);
//    drawaboatimage(window, BOARD_OFFSET_X, -100, "statek.png");
//    drawaboatimage(window, BOARD_OFFSET_X + 200, 300, "statek1.png", 0.4);
//	if (progress == 0) { //Instrukcja wyboru statków
//        text.setString("Wybierz ile ma byc statkow poszczegolnych rozmiarow. \nNa kolejnych ekranach klikaj 0,1,2 lub 3 na klawiaturze \nKliknij SPACE aby kontynuowac, \nlub ENTER aby grac z domyslna iloscia statkow");
//    }
//	else if (progress == 1) { //Wybór statku 5 kratkowego
//        text.setString("5-Kratkowiec - Wybierz 0, 1, 2 lub 3");
//    }
//	else if (progress == 2) { //Wybór statku 4 kratkowego
//        text.setString("4-Kratkowiec - Wybierz 0, 1, 2 lub 3");
//    }
//	else if (progress == 3) { //Wybór statku 3 kratkowego
//        text.setString("3-Kratkowiec - Wybierz 0, 1, 2 lub 3");
//    }
//	else if (progress == 4) {   //Wybór statku 2 kratkowego
//        text.setString("2-Kratkowiec - Wybierz 0, 1, 2 lub 3");
//    }
//
//    window.draw(text);
//}

void Game::DrawShipsChoiceScreen(sf::RenderWindow& window) {
    sf::Text text;
    text.setFont(font);
    text.setCharacterSize(30);
    text.setFillColor(sf::Color::Green);
    text.setStyle(sf::Text::Bold);
    sf::FloatRect textRect = text.getLocalBounds();
    text.setOrigin((textRect.left + textRect.width / 2.0f) - 30, textRect.top + textRect.height / 2.0f - 20);
    text.setPosition(sf::Vector2f(BOARD_OFFSET_X, BOARD_OFFSET_Y + 70));


    sf::RectangleShape rectangle(sf::Vector2f(800, 220)); //Rysowanie prostok¹ta za tekstem
    rectangle.setOrigin(rectangle.getSize().x / 2.0f, rectangle.getSize().y / 2.0f);
    rectangle.setPosition(sf::Vector2f(window.getSize().x / 2.0f, window.getSize().y / 2.0f));
    rectangle.setFillColor(sf::Color::Black);
    rectangle.setOutlineThickness(5);
    rectangle.setOutlineColor(sf::Color::Green);
    window.draw(rectangle);
    drawaboatimage(window, BOARD_OFFSET_X, -100, "statek.png");
    drawaboatimage(window, BOARD_OFFSET_X + 200, 300, "statek1.png", 0.4);
    if (progress == 0) { //Instrukcja wyboru statków
        text.setString("Enter - domyslna ilosc statkow (4x1,3x2,2x3,1x4) \n Spacja - wybierz samodzielnie");
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
    window.draw(textboxtext);
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
    std::srand(std::time(nullptr)); // Ziarno losowania
    while (!attacked) { 
        if (nextAIX != -1 && nextAIY != -1) //je¿eli przeciwnik ma zaplanowany kolejny atak to go wykonuje
        {
            x = nextAIX; 
            y = nextAIY;
        }
        else //inaczej jest losowo wybierany
        {
            x = std::rand() % GRID_SIZE;
            y = std::rand() % GRID_SIZE;
        }

        if (!(playerBoard.getcellstatus(x, y) == 2 || playerBoard.getcellstatus(x, y) == 3)) { //je¿eli wylosowany atak ju¿ wyst¹pi³ 
            if(playerBoard.attack(sf::Vector2i(x, y))) playsound(aishotsoundfilenames[track]); // to losowanie wystêpuje ponownie
            attacked = true;
            setnextaimove(x, y);//wywo³anie planowania kolejnego ataku
                                 //(x i y to poprzednie koordynaty)
        }
    }
   
}


//Funkcja planowania nastêpnego ataku
void Game::setnextaimove(int x,int y) { 

    if (playerBoard.getcellstatus(x,y)==2) { //upewnienie siê czy obecny strza³ by³ trafiony
        if ((playerBoard.getcellstatus(x+1, y) == 0 || playerBoard.getcellstatus(x+1, y) == 1) && x+1 <= GRID_SIZE)
        {                                   //5 opcji - jeœli strza³ by³ trafiony, kolejny bêdzie po prawej
            nextAIX = x + 1;
            nextAIY = y;
        }
        else if ((playerBoard.getcellstatus(x - 1, y) == 0 || playerBoard.getcellstatus(x - 1, y) == 1) && x - 1 >= 0) {
            nextAIX = x - 1;                 // chyba ¿e pole na górze by³o strzelone wczeœniej, wtedy po lewej
            nextAIY = y;
        }
        else if ((playerBoard.getcellstatus(x, y + 1) == 0 || playerBoard.getcellstatus(x, y + 1) == 1) && y + 1 <= GRID_SIZE) {
            nextAIX = x;                   // chyba ¿e pole po prawej by³o strzelone wczeœniej, wtedy na dole
            nextAIY = y+1;
        }
        else if ((playerBoard.getcellstatus(x, y - 1) == 0 || playerBoard.getcellstatus(x, y - 1) == 1) && y - 1 >= 0) {
            nextAIX = x;                    // chyba ¿e pole na dole by³o strzelone wczeœniej, wtedy na górze
            nextAIY = y-1;
        }
        else {
            nextAIX = -1;                    //je¿eli wszystkie strony wokó³ s¹ ju¿ sprawdzone, wtedy strzelamy losowo
            nextAIY = -1;
        }
    }
    else
    {
        nextAIX = -1;               //je¿eli ostatni strza³ nie by³ trafiony wtedy strzelamy losowo
        nextAIY = -1;
    }




}
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
