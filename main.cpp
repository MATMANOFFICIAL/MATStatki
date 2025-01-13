#include <SFML/Graphics.hpp>
#include <vector>
#include <ctime>
#include <cstdlib>
#include <iostream>
#include "Game.h"
int main() {
    
    Game game;// inicjalizacja gry
    game.run(); //uruchomienie gry
    //sf::String playerInput;
    //sf::Text playerText;
    //sf::Font font;
    //font.loadFromFile("arial.ttf");
    //playerText.setFont(font);
    //playerText.setString("Nigger");
    //playerText.setCharacterSize(20);
    //playerText.setFillColor(sf::Color::Red);
    //playerText.setStyle(sf::Text::Bold);
    //playerText.setPosition(30, 30);
    //sf::RenderWindow window(sf::VideoMode(1050, 500), "Statki by Mateusz Jarzab", sf::Style::Titlebar | sf::Style::Close); //Tworzenie okna
    //while (window.isOpen()) { //Pêtla gry
    //    sf::Event event;
    //    int max = 20;
    //    while (window.pollEvent(event)){
    //        if (event.type == sf::Event::Closed) //Obs³uga zamkniêcia okna
    //            window.close();

    //        if (event.type == sf::Event::KeyPressed && event.key.code == sf::Keyboard::BackSpace && playerInput.getSize() >0)
    //        {
    //            std::string temp = playerInput;
    //            temp.resize(temp.size() - 1);
    //            playerInput = temp;
    //        }


    //        if (event.type == sf::Event::TextEntered) {
    //            if (event.text.unicode <= 57 && event.text.unicode>=48) //je¿eli liczba
    //            {
    //                if (!(playerInput.getSize() == 0 && '0' == event.text.unicode))
    //                {
    //                    if (std::stoi((std::string)(playerInput + event.text.unicode)) <= max) //je¿eli nie bêdzie wieksza ni¿ max
    //                    {
    //                        playerInput += event.text.unicode;
    //                    }
    //                    else
    //                    {
    //                        playerInput = std::to_string(max);
    //                    }
    //                }
    //                
    //            }
    //            if (playerInput.getSize()!=0)
    //            {
    //                playerText.setString(playerInput);
    //            }
    //            else
    //            {
    //                playerText.setString("0");
    //            }
    //            
    //        }
    //    }
    //    window.clear();
    //    window.draw(playerText);
    //    window.display();
    //}
    return 0;
}

