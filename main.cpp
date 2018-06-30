#include <SFML/Graphics.hpp>
#include <iostream>
#include <string>
#include <sstream>
#include <time.h>

using namespace std;

int WIDTH = 600;
int HEIGHT = 600;
int row = 8;
int col = 8;
int bWIDTH = WIDTH / col;
int bHEIGHT = HEIGHT / row;

int gameMode = 0;
int e = 0;
int l = 400;
bool hax = false;

int randomRange(int min, int max){
    return min + (rand() % static_cast<int>(max - min + 1));
}

string int_to_string(int i){
    stringstream ss;
    ss<<i;
    return ss.str();
}

vector<array<int, 2>> getMoves(int x, int y, vector<array<int, 2>> rules, vector<array<int, 2>> was){
    vector<array<int, 2>> ret;
    for(int i = 0; i < rules.size(); ++i){
        if(x + rules.at(i)[0] < col && x + rules.at(i)[0] > -1 && y + rules.at(i)[1] < row && y + rules.at(i)[1] > -1){
            bool pass = true;
			for(int j = 0; j < was.size(); ++j){
                if(x + rules.at(i)[0] == was.at(j)[0] && y + rules.at(i)[1] == was.at(j)[1]) pass = false;
            }
			if(was.size() == 0 || pass){
				ret.push_back({x + rules.at(i)[0], y + rules.at(i)[1]});
			}
		}
    }
    return ret;
}

class ChessFigure{
public:
    int x, y;
    vector<array<int, 2>> rules;
    vector<array<int, 2>> was;
    sf::RectangleShape shape;
    sf::Texture texture;
    bool hasMoves = true;
    vector<array<int, 2>> moves;
    ChessFigure(){}
    ChessFigure(int _x, int _y, vector<array<int, 2>> _rules){
        x = _x;
        y = _y;
        rules = _rules;
        shape = sf::RectangleShape(sf::Vector2f(bWIDTH, bHEIGHT));
        texture.loadFromFile("chess-knight-icon.png");
        shape.setTexture(&texture);
        shape.setFillColor(sf::Color::Black);
        was.push_back({x, y});
        moves = getMoves(x, y, rules, was);
    }

    void draw(sf::RenderWindow* window){
        for(int j = 0; j < was.size(); ++j){
            sf::RectangleShape w(sf::Vector2f(bWIDTH, bHEIGHT));
            w.setPosition(sf::Vector2f(bWIDTH * was.at(j)[0], bHEIGHT * was.at(j)[1]));
            w.setFillColor(sf::Color(255, 255, 0, 200));
            window->draw(w);
        }
        shape.setPosition(sf::Vector2f(bWIDTH * x, bHEIGHT * y));
        window->draw(shape);
    }

    void drawMoves(sf::RenderWindow* window){
        sf::Font font;
        if (!font.loadFromFile("arial.ttf"))
        {
            // error...
        }
        for(int j = 0; j < moves.size(); ++j){
            sf::RectangleShape w(sf::Vector2f(bWIDTH, bHEIGHT));
            w.setPosition(sf::Vector2f(bWIDTH * moves.at(j)[0], bHEIGHT * moves.at(j)[1]));
            w.setFillColor(sf::Color(100, 255, 100, 200));
            window->draw(w);
            if(hax){
                sf::Text txt;
                txt.setFont(font);
                vector<array<int, 2>> minMove = getMoves(moves.at(j)[0], moves.at(j)[1], rules, was);
                txt.setString(int_to_string(minMove.size()));
                txt.setCharacterSize(24);
                txt.setColor(sf::Color::Red);
                txt.setPosition(sf::Vector2f(bWIDTH * moves.at(j)[0], bHEIGHT * moves.at(j)[1]));
                window->draw(txt);
            }
        }
        shape.setPosition(sf::Vector2f(bWIDTH * x, bHEIGHT * y));
        window->draw(shape);
    }

    void goTo(int _x, int _y){
        for(int i = 0; i < rules.size(); ++i){
			if(x + rules.at(i)[0] == _x && y + rules.at(i)[1] == _y && _x < col && _y < row && _x >= 0 && _y >= 0){
				for(int j = 0; j < was.size(); ++j){
                    if(was.at(j)[0] == _x && was.at(j)[1] == _y) return;
				}
				x = _x;
				y = _y;
				was.push_back({_x, _y});
                moves = getMoves(x, y, rules, was);
                if(moves.size() == 0){
                    hasMoves = false;
                    l = 3000;
                }
				return;
			}
		}
    }

    void act(){
        moves = getMoves(x, y, rules, was);
		if(moves.size() == 0) return;
		vector<array<int, 2>> minMove = getMoves(moves.at(0)[0], moves.at(0)[1], rules, was);
		int minI = 0;
		for(int i = 1; i < moves.size(); ++i){
            vector<array<int, 2>> t = getMoves(moves.at(i)[0], moves.at(i)[1], rules, was);
			if(minMove.size() > t.size()){
				minMove = t;
				minI = i;
			}

		}
		goTo(moves.at(minI)[0], moves.at(minI)[1]);
    }

    void reset(){
        x = randomRange(0, col - 1);
        y = randomRange(0, col - 2);
        shape = sf::RectangleShape(sf::Vector2f(bWIDTH, bHEIGHT));
        shape.setTexture(&texture);
        shape.setFillColor(sf::Color::Black);
        was.clear();
        was.push_back({x, y});
        hasMoves = true;
        moves = getMoves(x, y, rules, was);
        l = 400;
    }

    void goBack(){
        if(was.size() == 1) return;
        array<int, 2> last = was.at(was.size() - 2);
        x = last[0];
        y = last[1];
        was.pop_back();
        moves = getMoves(x, y, rules, was);
    }
};

class Button{
public:
    int x, y, w, h, g;
    string text;
    sf::Color color;
    sf::RectangleShape shape;
    sf::Font font;
    sf::Text sText;
    Button(int _x, int _y, int _w, int _h, string _text,sf::Color _color, int _g){
        x = _x;
        y = _y;
        w = _w;
        h = _h;
        g = _g;
        text = _text;
        color = _color;
        shape = sf::RectangleShape(sf::Vector2f(w, h));
        shape.setPosition(sf::Vector2f(x, y));
        shape.setFillColor(color);
        if (!font.loadFromFile("arial.ttf"))
        {
            // error...
        }
        sText.setFont(font);
        sText.setString(text);
        sText.setCharacterSize(24);
        sText.setColor(sf::Color::Black);
        sf::FloatRect textRect = sText.getLocalBounds();
        sText.setOrigin(textRect.left + textRect.width / 2.0f, textRect.top  + textRect.height / 2.0f);
        sText.setPosition(sf::Vector2f(x + w / 2.0f, y + h / 2.0f));
    }

    void draw(sf::RenderWindow* window){
        window->draw(shape);
        window->draw(sText);
    }

    void click(){
        if(g == 4){
            row++;
            col++;
            bWIDTH = WIDTH / col;
            bHEIGHT = HEIGHT / row;
            return;
        }else if(g == 5){
            if(row == 5) return;
            col--;
            row--;
            bWIDTH = WIDTH / col;
            bHEIGHT = HEIGHT / row;
            return;
        }
        gameMode = g;
    }
};

void drawField(sf::RenderWindow* window){
    for(int i = 0; i < col; ++i){
        for(int j = 0; j < row; ++j){
            if((i + j) % 2 == 0){
                sf::RectangleShape piece(sf::Vector2f(bWIDTH, bHEIGHT));
                piece.setFillColor(sf::Color(35, 35, 35));
                piece.setPosition(sf::Vector2f(bWIDTH * i, bHEIGHT * j));
                window->draw(piece);
            }
        }
    }
}

int main()
{
    srand(time(0));

    vector<array<int, 2>> rules;
    rules.push_back({1, -2});
    rules.push_back({-1, -2});
    rules.push_back({1, 2});
    rules.push_back({-1, 2});
    rules.push_back({2, -1});
    rules.push_back({2, 1});
    rules.push_back({-2, -1});
    rules.push_back({-2, 1});

    sf::Clock clock;

    sf::RenderWindow window(sf::VideoMode(WIDTH, HEIGHT), "Horse");
    ChessFigure horse(randomRange(0, col - 1), randomRange(0, row - 1), rules);


    sf::Text sText;
    sf::Font font;
    if (!font.loadFromFile("arial.ttf"))
    {
        // error...
    }
    sText.setFont(font);
    sText.setCharacterSize(40);
    sText.setColor(sf::Color::Black);


    sf::RectangleShape modalBg;

    Button AI_Button(150, 210, 300, 50, "Horse AI", sf::Color::Green, 1);
    Button Pl_Button(150, 275, 300, 50, "Play Horse", sf::Color::Green, 2);
    Button Chess_Button(150, 340, 300, 50, "Chess", sf::Color::Green, 3);
    Button Add(500, 225, 50, 50, "+", sf::Color::Green, 4);
    Button Sub(500, 325, 50, 50, "-", sf::Color::Green, 5);

    Button buttons[5] = {AI_Button, Pl_Button, Chess_Button, Add, Sub};

    while (window.isOpen())
    {
        sf::Event event;
        while (window.pollEvent(event))
        {
            if (event.type == sf::Event::Closed)
                window.close();

            if (event.type == sf::Event::MouseButtonPressed){
                if (event.mouseButton.button == sf::Mouse::Left){
                    if(gameMode == 0){
                        for(int i = 0; i < 5; i++){
                            if(event.mouseButton.x < buttons[i].x + buttons[i].w && event.mouseButton.x > buttons[i].x
                               && event.mouseButton.y < buttons[i].y + buttons[i].h && event.mouseButton.y > buttons[i].y){
                                buttons[i].click();
                                horse.reset();
                                if(buttons[i].g == 1){
                                    clock.restart();
                                }
                                break;
                            }
                        }
                    }
                    if(gameMode == 2){
                        int x = event.mouseButton.x / bWIDTH;
                        int y = event.mouseButton.y / bHEIGHT;
                        horse.goTo(x, y);
                    }
                }else if(event.mouseButton.button == sf::Mouse::Right){
                    if(gameMode == 2){
                        horse.goBack();
                    }
                }
            }
        }

        if(gameMode == 1){
            if(clock.getElapsedTime().asMilliseconds() > l){
                if(!horse.hasMoves){
                    horse.reset();
                    gameMode = 0;
                }else{
                    horse.act();
                    clock.restart();
                }
            }
        }

        window.clear(sf::Color(245, 245, 245));

        if(gameMode == 0){
            for(int i = 0; i < 5; i++){
                buttons[i].draw(&window);
            }
            sf::Text rText;
            rText.setString(int_to_string(col));
            rText.setFont(font);
            rText.setCharacterSize(40);
            rText.setColor(sf::Color::Black);
            sf::FloatRect textRect = rText.getLocalBounds();
            rText.setOrigin(textRect.left + textRect.width / 2.0f, textRect.top  + textRect.height / 2.0f);
            rText.setPosition(sf::Vector2f(525, 300));
            window.draw(rText);
        }else if(gameMode == 2 || gameMode == 1){
            drawField(&window);
            horse.draw(&window);
            if(gameMode == 2){
                if(e == 1 && clock.getElapsedTime().asSeconds() > 6){
                    horse.reset();
                    e = 0;
                    gameMode = 0;
                }
                if(!horse.hasMoves){
                    if(horse.was.size() != 64){
                        sText.setString("You lost with score of: " + int_to_string(horse.was.size()));
                    }else {
                        sText.setString("You won!");
                    }
                    sf::FloatRect textRect = sText.getLocalBounds();
                    sText.setOrigin(textRect.left + textRect.width / 2.0f, textRect.top  + textRect.height / 2.0f);
                    sText.setPosition(sf::Vector2f(WIDTH / 2.0f, HEIGHT / 2.0f));
                    modalBg.setSize(sf::Vector2f(textRect.width + 50, textRect.height + 50));
                    textRect = modalBg.getLocalBounds();
                    modalBg.setOrigin(textRect.left + textRect.width / 2.0f, textRect.top  + textRect.height / 2.0f);
                    modalBg.setPosition(sf::Vector2f(WIDTH / 2.0f, HEIGHT / 2.0f));
                    window.draw(modalBg);
                    window.draw(sText);
                    if(e == 0){
                        clock.restart();
                        e++;
                    }
                }
                horse.drawMoves(&window);
            }
        }

        window.display();
    }

    return 0;
}
