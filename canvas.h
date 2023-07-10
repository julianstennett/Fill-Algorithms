#pragma once
#include "pixel.h"

class canvas {
    std::map<int, std::map<int, pixel*>> pixelsTree; // nested tree used for positional search over specified pixel log(n^(1/2)) + log(n^(1/2)) = O(log(n))
    std::vector<pixel*> pixelsList; // general list used for iteration over all pixels O(n)
    int sideLen = 0;
    bool BreadthFill = true;
public:
    // parameratized constructor taking in a single screen dimmension Side x Side
    canvas(int screenDimmension) {
        this->sideLen = screenDimmension;
        int x = 0, y = 0;
        pixel* insertion;

        // sets positions and places pixels in canvas objects for later access
        for (int column = 1; column <= sideLen; column++) {
            std::map<int, pixel*> currentColumn;
            for (int cell = 1; cell <= sideLen; cell++) {
                if (x == sideLen * 5) {
                    x = 0;
                    y += 5;
                }
                insertion = new pixel(x, y);
                pixelsList.push_back(insertion);
                currentColumn.emplace(cell, insertion);
                x += 5;
            }
            pixelsTree.emplace(column, currentColumn);
        }

        // sets each pixel's adjecent vector for filling operations
        for (int c = 0; c < pixelsList.size(); c++) {
            bool NotFirstRow{ true }, NotLastRow{ true }, NotFirstCol{ true }, NotLastCol{ true };

            if ((c + 1) <= sideLen)
                NotFirstRow = false;
            if ((c + 1) > (pixelsList.size() - 1) - sideLen)
                NotLastRow = false;
            if ((c + 1) % sideLen == 1)
                NotFirstCol = false;
            if ((c + 1) % sideLen == 0)
                NotLastCol = false;

            if (NotFirstRow) {
                pixelsList[c]->border(pixelsList.at((c)-sideLen));
                if (NotFirstCol)
                    pixelsList[c]->border(pixelsList.at((c)-(sideLen + 1)));
                if (NotLastCol)
                    pixelsList[c]->border(pixelsList.at((c)-(sideLen - 1)));
            }
            if (NotLastRow) {
                pixelsList[c]->border(pixelsList.at((c)+sideLen));
                if (NotFirstCol)
                    pixelsList[c]->border(pixelsList.at((c)+(sideLen - 1)));
                if (NotLastCol)
                    pixelsList[c]->border(pixelsList.at((c)+(sideLen + 1)));
            }
            if (NotFirstCol) {
                pixelsList[c]->border(pixelsList.at((c)-1));
            }
            if (NotLastCol) {
                pixelsList[c]->border(pixelsList.at((c)+1));
            }
        }
        display();
    }

    void click(sf::Vector2i pos, bool BFS, sf::Color color, sf::RenderWindow& window) {
        if (!((pos.x > 0 && pos.x < sideLen * 5) && (pos.y > 0 && pos.y < sideLen * 5)))
            return;
        int y = 0, x = 0;
        if (pos.y % 5 == 0) {
            y = std::ceil(pos.y / 5);
        }
        else {
            y = std::ceil(pos.y / 5) + 1;
        }
        if (pos.x % 5 == 0) {
            x = std::ceil(pos.x / 5);
        }
        else {
            x = std::ceil(pos.x / 5) + 1;
        }
        pixel* found = pixelsTree.at(y).at(x);
        if (BFS)
            found->breadth(color, window);
        else
            found->depth(color, window);
    }

    void display() {
        sf::RenderWindow window(sf::VideoMode(sideLen * 5 + 150, sideLen * 5), "Virtual Vortex");
        sf::Event event;
        sf::Mouse mouse;
        bool updateWindow = true;
        window.setFramerateLimit(60);
        while (window.isOpen()) {

            while (window.pollEvent(event)) {

                if (event.type == sf::Event::Closed) {

                    window.close();
                }
                if (event.type == sf::Event::MouseButtonPressed) {
                    if (event.key.code == sf::Mouse::Left) {
                        updateWindow = true;
                        auto pos = mouse.getPosition(window);
                        click(pos, BreadthFill, sf::Color::Blue, window);
                    }
                }
                if (event.type == sf::Event::KeyPressed) {

                    if (event.key.code == sf::Keyboard::Space) {
                        BreadthFill = !BreadthFill;
                        if (BreadthFill)
                            std::cout << "bucket currently set to breadth fill\n";
                        else
                            std::cout << "bucket currently set to depth fill\n";
                    }

                    if (event.key.code == sf::Keyboard::W) {
                        std::cout << "returning cavas data...\n";
                    }

                    if (event.key.code == sf::Keyboard::R) {
                        std::cout << "random squares...\n";
                        randomSquares();
                        updateWindow = true;
                    }

                    if (event.key.code == sf::Keyboard::C) {
                        std::cout << "random circles...\n";
                        randomCircles();
                        updateWindow = true;
                    }
                }
                if (updateWindow) {
                    for (pixel* p : pixelsList) { window.draw(p->getPixel()); }
                    window.display();
                    updateWindow = false;
                }
            }
        }
    }

    void randomSquares() {
        std::vector<sf::RectangleShape> squares;
        sf::RectangleShape temp;
        for (int c = 0; c < 10; c++) {
            temp.setSize(sf::Vector2f(rand() % sideLen * 5 + 20, rand() % sideLen * 5 + 20));
            temp.setPosition(sf::Vector2f(rand() % sideLen * 5, rand() % sideLen * 5));
            temp.setFillColor(sf::Color(rand(), rand(), rand()));
            squares.push_back(temp);
        }
        for (pixel* p : pixelsList) {
            for (sf::RectangleShape square : squares) {
                if (square.getGlobalBounds().intersects(p->getPixel().getGlobalBounds())) {
                    p->setColor(square.getFillColor());
                    break;
                }
            }
        }
    }

    void randomCircles() {
        std::vector<sf::CircleShape> circles;
        sf::CircleShape temp;
        for (int c = 0; c < 10; c++) {
            temp.setRadius(rand() % sideLen * 5 + 20);
            temp.setPosition(sf::Vector2f(rand() % sideLen * 5, rand() % sideLen * 5));
            temp.setFillColor(sf::Color(rand(), rand(), rand()));
            circles.push_back(temp);
        }
        for (pixel* p : pixelsList) {
            for (sf::CircleShape circle : circles) {
                if (circle.getGlobalBounds().intersects(p->getPixel().getGlobalBounds())) {
                    p->setColor(circle.getFillColor());
                    break;
                }
            }
        }
    }

    // experimental read and write methods are not currently functional at the moment
    void read(std::string data) { // reads string to convert into pixels for display- used for reading examples
        std::string n = "", red = "", green = "", blue = "";
        std::queue<std::vector<int>> q;
        std::vector<int> colors;
        int type = 0;
        for (char c : data) {
            if (std::isspace(c)) {
                type++;
            }
            if (c == '.') {
                colors.push_back(stoi(n));
                colors.push_back(stoi(red));
                colors.push_back(stoi(green));
                colors.push_back(stoi(blue));
                q.push(colors);
                colors.clear();
                type = 0;
            }
            if (std::isdigit(c)) {
                switch (type) {
                case 0:
                    n += c;
                    break;
                case 1:
                    red += c;
                    break;
                case 2:
                    green += c;
                    break;
                case 3:
                    blue += c;
                    break;
                }
            }
        }
        int colorChange = 0;
        std::vector<int> currentColor;
        for (pixel* currPixel : pixelsList) {
            if (colorChange == 0) {
                currentColor = q.front();
                colorChange = currentColor[0];
                q.pop();
            }
            currPixel->setColor(currentColor);
            colorChange--;
        }
    }

    void write() { // returns the current string of the pixel information on the canvas- used for creating and saving examples
        int numberofColor = -1;
        sf::Color base = pixelsList[0]->color();
        std::string info = "";
        for (int n = 1; n < pixelsList.size(); n++) {
            if (base != pixelsList[n]->color()) {
                base = pixelsList[n]->color();
                if (numberofColor == -1 || n == pixelsList.size()) {
                    numberofColor = 0;
                }
                else {
                    info += std::to_string(numberofColor);
                    info += " ";
                    info += std::to_string(base.r);
                    info += " ";
                    info += std::to_string(base.g);
                    info += " ";
                    info += std::to_string(base.b);
                    info += ".";
                    numberofColor = 0;
                }
            }
            numberofColor++;
        }
        std::cout << "done" << "\n";
    }
};