#pragma once
#include <iostream>
#include <chrono>
#include "pixel.h"
#include "interface.h"
#include "presets.h"

class canvas {
    std::map<int, std::map<int, pixel*>> pixelsTree; // nested tree used for positional search over specified pixel log(n^(1/2)) + log(n^(1/2)) = O(log(n))
    std::vector<pixel*> pixelsList; // general list used for iteration over all pixels O(n)
    int sideLen = 30;
    int pixel_dim = 30;
    int density = 0;
    bool BreadthFill = true;

    sf::Event event;
    sf::Mouse mouse;


    bool drawing = false; // keeps track of if drawing mode is on or not
    int current_mode; // to keep track of what to display for interface current outline -> 0 = N/A (first start program)
    // 1 = "Random Rectangles", 2 = "Random Circles", 3 = "Random Sparse Maze", 4 = "Random Dense Maze", 5 = Simple Triangle
    // 6 - Simple Circle, 7 = Simple Rectangle, 8 = Surprise
    int current_color_mode; // to keep track of what color mode you are in 0 = Color Palette Mode (Rainbow), 1-9 = Gradient Color Mode (R-O-Y-G-B-I-V-Pink-Grey)

    sf::RenderWindow window;

    int duration = -1;
    int iteration_delay; // for speed of traversal change
    int current_pixels_checked = -1;

    interface ui;
    presets outlines;

public:
    // parameterized constructor taking in a single screen dimension Side x Side
    canvas(int density) {
        current_mode = 0;
        current_color_mode = 1;
        iteration_delay = 0;

        this->sideLen *= density;
        this->pixel_dim /= density;
        this->density = density;
        int x = 0, y = 200;
        pixel* insertion;

        // sets positions and places pixels in canvas objects for later access
        for (int column = 1; column <= sideLen; column++) {
            std::map<int, pixel*> currentColumn;
            for (int cell = 1; cell <= sideLen; cell++) {
                if (x == sideLen * pixel_dim) {
                    x = 0;
                    y += pixel_dim;
                }
                insertion = new pixel(x, y, pixel_dim);
                pixelsList.push_back(insertion);
                currentColumn.emplace(cell, insertion);
                x += pixel_dim;
            }
            pixelsTree.emplace(column, currentColumn);
        }

        // sets each pixel's adjacent vector for filling operations
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

        window.create(sf::VideoMode(1440, 1125, 32), "Visual Vortex");
        display();
    }

    // activates when clicked on active screen for bfs/dfs or drawing mode
    void click(bool baseColorFill, sf::Vector2i pos, bool BFS, sf::Color color, sf::RenderWindow& window) {
        // for debugging REMOVE BEFORE SUBMITTING
        std::cout << "X: " << pos.x << std::endl;
        std::cout << "Y: " << pos.y << std::endl;

        pos.y -= 200;
        if (!((pos.x > 0 && pos.x < sideLen * pixel_dim) && (pos.y > 0 && pos.y < sideLen * pixel_dim)))
            return;
        int y = 0, x = 0;
        if (pos.y % pixel_dim == 0) {
            y = std::ceil(pos.y / pixel_dim);
        }
        else {
            y = std::ceil(pos.y / pixel_dim) + 1;
        }
        if (pos.x % pixel_dim == 0) {
            x = std::ceil(pos.x / pixel_dim);
        }
        else {
            x = std::ceil(pos.x / pixel_dim) + 1;
        }

        pixel* found = pixelsTree.at(y).at(x);

        // drawing mode code
        if(drawing) {

                auto pos = mouse.getPosition(window);

                // Get the original position of the mouse click using the scale factors for screen resizing
                float xScale = static_cast<float>(window.getSize().x) / 1440.f;
                float yScale = static_cast<float>(window.getSize().y) / 1125.f;
                float originalX = static_cast<float>(event.mouseButton.x) / xScale;
                float originalY = static_cast<float>(event.mouseButton.y) / yScale;
                pos.x = originalX;
                pos.y = originalY;

                pos.y -= 200;
                int y = 0, x = 0;
                if (pos.y % pixel_dim == 0) {
                    y = std::ceil(pos.y / pixel_dim);
                }
                else {
                    y = std::ceil(pos.y / pixel_dim) + 1;
                }
                if (pos.x % pixel_dim == 0) {
                    x = std::ceil(pos.x / pixel_dim);
                }
                else {
                    x = std::ceil(pos.x / pixel_dim) + 1;
                }

                int i = (x - 1) % pixel_dim + (y - 1) * pixel_dim;
                if(i > 0){
                std::cout << std::to_string(i) << std::endl;
                pixelsList[i]->data.setFillColor(sf::Color::Black);
                for (pixel *c: pixelsList) { window.draw(c->getPixel()); }
                window.display();
                }

                return;

            }


        auto start = std::chrono::high_resolution_clock::now();
        if (BFS)
            // found->breadth(color, window);
            current_pixels_checked = breadth(baseColorFill, found, ui.custom_colors, ui.gradient_colors, window, iteration_delay, current_color_mode);

        else
            //found->depth(color, window);
            current_pixels_checked = depth(baseColorFill, found, ui.custom_colors, ui.gradient_colors, window, iteration_delay, current_color_mode);


        auto end = std::chrono::high_resolution_clock::now();
        auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count();
        std::cout << "Algorithm time: " << duration << " milliseconds. \n";

        ui.update_stats(window, duration, iteration_delay, current_pixels_checked);
        this->duration = duration;
        quick_update();
    }

    void display() { // SFML display and controls for the window and user interface

        bool updateWindow = true;
        window.setFramerateLimit(60 * density);
        while (window.isOpen()) {
            ui.display_interface(window);

            while (window.pollEvent(event)) {

                if (event.type == sf::Event::Closed) {

                    window.close();
                }
                if (event.type == sf::Event::Resized) {
                    // set screen size
                    float screenWidth = 1440.f;
                    float screenHeight = 1125.f;
                    // get the resized size
                    sf::Vector2u size = window.getSize();
                    // setup aspect ratio
                    float  heightRatio = screenHeight / screenWidth;
                    float  widthRatio = screenWidth / screenHeight;
                    // adapt the resized window to aspect ratio
                    if (size.y * widthRatio <= size.x)
                    {
                        size.x = size.y * widthRatio;
                    }
                    else if (size.x * heightRatio <= size.y)
                    {
                        size.y = size.x * heightRatio;
                    }
                    // set the new size
                    window.setSize(size);
                    ui.update_stats(window, duration, iteration_delay, current_pixels_checked);
                    quick_update();
                }
                if (event.type == sf::Event::MouseButtonPressed) {
                    if (event.key.code == sf::Mouse::Right) {
                        updateWindow = true;
                        auto pos = mouse.getPosition(window);

                        // Get the original position of the mouse click using the scale factors for a resized screen
                        float xScale = static_cast<float>(window.getSize().x) / 1440.f;
                        float yScale = static_cast<float>(window.getSize().y) / 1125.f;
                        float originalX = static_cast<float>(event.mouseButton.x) / xScale;
                        float originalY = static_cast<float>(event.mouseButton.y) / yScale;
                        pos.x = originalX;
                        pos.y = originalY;

                        click(true, pos, BreadthFill, sf::Color::Blue, window);
                        quick_update();
                    }
                    if (event.key.code == sf::Mouse::Left) {
                        updateWindow = true;
                        auto pos = mouse.getPosition(window);

                        // Get the original position of the mouse click using the scale factors for a resized screen
                        float xScale = static_cast<float>(window.getSize().x) / 1440.f;
                        float yScale = static_cast<float>(window.getSize().y) / 1125.f;
                        float originalX = static_cast<float>(event.mouseButton.x) / xScale;
                        float originalY = static_cast<float>(event.mouseButton.y) / yScale;
                        pos.x = originalX;
                        pos.y = originalY;


                        // turns on help menu if help button is clicked on
                        if (ui.help_bounds.contains(pos.x, pos.y)) {
                            ui.display_help();
                        }
                        // turns on bfs if not on already
                        else if (ui.bfs_bounds.contains(pos.x, pos.y) && !BreadthFill) {
                            BreadthFill = !BreadthFill;
                            ui.update_interface(window, current_mode, BreadthFill); // updates the interface
                            quick_update();
                        }
                        // turn on dfs if not on already
                        else if (ui.dfs_bounds.contains(pos.x, pos.y) && BreadthFill) {
                            BreadthFill = !BreadthFill;
                            ui.update_interface(window, current_mode, BreadthFill); // updates the interface
                            quick_update();
                        }

                        click(false, pos, BreadthFill, sf::Color::Blue, window);
                        quick_update();
                    }
                }
                if (event.type == sf::Event::KeyPressed) {
                    // activates breadth or depth fill
                    if (event.key.code == sf::Keyboard::Space) {
                        updateWindow = true;
                        BreadthFill = !BreadthFill;
                        ui.update_interface(window, current_mode, BreadthFill); // updates the interface

                        if (BreadthFill)
                            std::cout << "bucket currently set to breadth fill\n";
                        else
                            std::cout << "bucket currently set to depth fill\n";
                    }

                    else if (event.key.code >= sf::Keyboard::Num0 && event.key.code <= sf::Keyboard::Num9) {
                        if (sf::Keyboard::isKeyPressed(sf::Keyboard::LShift) || sf::Keyboard::isKeyPressed(sf::Keyboard::RShift))
                        {
                            // Shift + number key pressed
                            int number = event.key.code - sf::Keyboard::Num0; // Convert keycode to number (0-9)

                            switch (number) {
                            case 1:
                                current_mode = 1;
                                ui.update_interface(window, current_mode, BreadthFill);
                                randomSquares();
                                updateWindow = true;
                                break;
                            case 2:
                                current_mode = 2;
                                ui.update_interface(window, current_mode, BreadthFill);
                                randomCircles();
                                updateWindow = true;
                                break;
                            case 3:
                                sparse_maze();
                                current_mode = 3;
                                ui.update_interface(window, current_mode, BreadthFill);
                                quick_update();
                                break;
                            case 4:
                                dense_maze();
                                current_mode = 4;
                                ui.update_interface(window, current_mode, BreadthFill);
                                quick_update();
                                break;
                            case 5:
                                if (density == 1)
                                    preset_generation(outlines.simple_triangle);
                                else
                                    simpleTriangle();
                                current_mode = 5;
                                ui.update_interface(window, current_mode, BreadthFill);
                                quick_update();
                                break;
                            case 6:
                                if (density == 1)
                                    preset_generation(outlines.simple_circle);
                                else
                                    simpleCircle();
                                current_mode = 6;
                                ui.update_interface(window, current_mode, BreadthFill);
                                quick_update();
                                break;
                            case 7:
                                if (density == 1)
                                    preset_generation(outlines.simple_rectangle);
                                else
                                    simpleSquare();
                                current_mode = 7;
                                ui.update_interface(window, current_mode, BreadthFill);
                                quick_update();
                                break;
                            case 8:
                                preset_generation(outlines.surprise_1);
                                current_mode = 8;
                                ui.update_interface(window, current_mode, BreadthFill);
                                quick_update();
                                break;
                            case 9:
                                drawing = !drawing;
                                ui.update_color_mode(window, current_color_mode, drawing);
                                quick_update();
                                break;
                            }
                        }
                        else {
                            // COLOR MODES CHANGE
                            // Rainbow Color Palette
                            if (event.key.code == sf::Keyboard::Num0) {
                                current_color_mode = 0;
                                ui.update_color_mode(window, current_color_mode, drawing);
                                quick_update();
                            }
                            // Red Gradient
                            if (event.key.code == sf::Keyboard::Num1) {
                                current_color_mode = 1;
                                ui.update_color_mode(window, current_color_mode, drawing);
                                quick_update();
                            }
                            // Orange Gradient
                            if (event.key.code == sf::Keyboard::Num2) {
                                current_color_mode = 2;
                                ui.update_color_mode(window, current_color_mode, drawing);
                                quick_update();
                            }
                            // Yellow Gradient
                            if (event.key.code == sf::Keyboard::Num3) {
                                current_color_mode = 3;
                                ui.update_color_mode(window, current_color_mode, drawing);
                                quick_update();
                            }
                            // Green Gradient
                            if (event.key.code == sf::Keyboard::Num4) {
                                current_color_mode = 4;
                                ui.update_color_mode(window, current_color_mode, drawing);
                                quick_update();
                            }
                            // Blue Gradient
                            if (event.key.code == sf::Keyboard::Num5) {
                                current_color_mode = 5;
                                ui.update_color_mode(window, current_color_mode, drawing);
                                quick_update();
                            }
                            // Indigo Gradient
                            if (event.key.code == sf::Keyboard::Num6) {
                                current_color_mode = 6;
                                ui.update_color_mode(window, current_color_mode, drawing);
                                quick_update();
                            }
                            // Purple Gradient
                            if (event.key.code == sf::Keyboard::Num7) {
                                current_color_mode = 7;
                                ui.update_color_mode(window, current_color_mode, drawing);
                                quick_update();
                            }
                            // Pink Gradient
                            if (event.key.code == sf::Keyboard::Num8) {
                                current_color_mode = 8;
                                ui.update_color_mode(window, current_color_mode, drawing);
                                quick_update();
                            }
                            // Gray Gradient
                            if (event.key.code == sf::Keyboard::Num9) {
                                current_color_mode = 9;
                                ui.update_color_mode(window, current_color_mode, drawing);
                                quick_update();
                            }
                        }
                    }

                    else if (event.key.code == sf::Keyboard::R) {
                        current_mode = 1;
                        ui.update_interface(window, current_mode, BreadthFill);
                        randomSquares();
                        updateWindow = true;
                    }

                    else if (event.key.code == sf::Keyboard::C) {
                        current_mode = 2;
                        ui.update_interface(window, current_mode, BreadthFill);
                        randomCircles();
                        updateWindow = true;
                    }

                    // activates drawing mode
                    else if (event.key.code == sf::Keyboard::D) {
                        drawing = !drawing;
                        ui.update_color_mode(window, current_color_mode, drawing);
                        quick_update();
                    }

                    // decreases how long an iteration of either traversal takes to execute (waits until amount of time has passed)
                    else if (event.key.code == sf::Keyboard::Down) {
                        if (iteration_delay >= 1) {
                            iteration_delay -= 1;
                            ui.update_stats(window, duration, iteration_delay, current_pixels_checked);
                            quick_update();
                        }
                    }

                    // increases how long an iteration of either traversal takes to execute (waits until amount of time has passed)
                    else if (event.key.code == sf::Keyboard::Up) {
                        iteration_delay += 1;
                        ui.update_stats(window, duration, iteration_delay, current_pixels_checked);
                        quick_update();
                    }

                    // restarts the canvas with a blank slate
                    else if (event.key.code == sf::Keyboard::BackSpace) {
                        restart_canvas();
                        ui.update_interface(window, current_mode, BreadthFill);
                        quick_update();
                    }

                    // activates randomized sparse "maze" mode
                    else if (event.key.code == sf::Keyboard::S) {
                        sparse_maze();
                        current_mode = 3;
                        ui.update_interface(window, current_mode, BreadthFill);
                        quick_update();
                    }

                    // activates randomized dense "maze" mode
                    else if (event.key.code == sf::Keyboard::A) {
                        dense_maze();
                        current_mode = 4;
                        ui.update_interface(window, current_mode, BreadthFill);
                        quick_update();
                    }

                    // activates simple triangle preset mode
                    else if (event.key.code == sf::Keyboard::U) {
                        if (density == 1)
                            preset_generation(outlines.simple_triangle);
                        else
                            simpleTriangle();
                        current_mode = 5;
                        ui.update_interface(window, current_mode, BreadthFill);
                        quick_update();
                    }

                    // activates simple circle preset mode
                    else if (event.key.code == sf::Keyboard::I) {
                        if (density == 1)
                            preset_generation(outlines.simple_circle);
                        else
                            simpleCircle();
                        current_mode = 6;
                        ui.update_interface(window, current_mode, BreadthFill);
                        quick_update();
                    }

                    // activates simple rectangle preset mode
                    else if (event.key.code == sf::Keyboard::O) {
                        if (density == 1)
                            preset_generation(outlines.simple_rectangle);
                        else
                            simpleSquare();
                        current_mode = 7;
                        ui.update_interface(window, current_mode, BreadthFill);
                        quick_update();
                    }

                    // activates surprise preset
                    else if (event.key.code == sf::Keyboard::Slash) {
                        preset_generation(outlines.surprise_1);
                        current_mode = 8;
                        ui.update_interface(window, current_mode, BreadthFill);
                        quick_update();
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

    // updates the canvas visually
    void quick_update() {
        for (pixel* p : pixelsList) { window.draw(p->getPixel()); }
        window.display();
    }

    // deallocates memory before clearing pixel storage
    void clear() {
        for (pixel* p : pixelsList) { 
            delete p; 
        }
        pixelsList.clear();
        pixelsTree.clear();
    }

    // restarts the canvas to white blank slate
    void restart_canvas() {
        clear();

        int x = 0, y = 200;
        pixel* insertion;

        // sets positions and places pixels in canvas objects for later access
        for (int column = 1; column <= sideLen; column++) {
            std::map<int, pixel*> currentColumn;
            for (int cell = 1; cell <= sideLen; cell++) {
                if (x == sideLen * pixel_dim) {
                    x = 0;
                    y += pixel_dim;
                }
                insertion = new pixel(x, y, pixel_dim);
                pixelsList.push_back(insertion);
                currentColumn.emplace(cell, insertion);
                x += pixel_dim;
            }
            pixelsTree.emplace(column, currentColumn);
        }

        // sets each pixel's adjacent vector for filling operations
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

        current_mode = 0;
    }

    // generates random overlapping squares on the canvas
    void randomSquares() {
        restart_canvas();
        std::vector<sf::RectangleShape> squares;
        sf::RectangleShape temp;
        for (int c = 0; c < 50; c++) {
            temp.setSize(sf::Vector2f(rand() % 800 + 500, rand() % 800 + 500));
            temp.setPosition(sf::Vector2f(rand() % 900, rand() % 700 + 200));
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

    // collision logic to shade pixels within a circle's bounds
    bool intersects(sf::CircleShape c, sf::RectangleShape r)
    {
        auto circle = c.getPosition();
        auto square = r.getPosition();
        float dx = std::abs(circle.x - square.x);
        float dy = std::abs(circle.y - square.y);
        float distance = std::sqrt(dx * dx + dy * dy);
        return (distance <= c.getRadius() + r.getSize().x / 2.f);
    }

    // generates random overlapping circles on the canvas
    void randomCircles() {
        restart_canvas();
        std::vector<sf::CircleShape> circles;
        sf::CircleShape temp;
        for (int c = 0; c < 20; c++) {
            int radius = rand() % 500 + 100;
            temp.setRadius(radius);
            temp.setOrigin(radius, radius);
            temp.setPosition(sf::Vector2f(rand() % 900, rand() % 925 + 200));
            temp.setFillColor(sf::Color(rand(), rand(), rand()));
            circles.push_back(temp);
        }
        for (pixel* p : pixelsList) {
            for (sf::CircleShape circle : circles) {
                if (intersects(circle, p->getPixel())) {
                    p->setColor(circle.getFillColor());
                }
            }
        }
    }

    void simpleSquare() {
        restart_canvas();
        sf::RectangleShape sq;
        sq.setSize(sf::Vector2f(400, 400));
        sq.setOrigin(200, 200);
        sq.setPosition(450, 662.5);
        sq.setFillColor(sf::Color(rand(), rand(), rand()));
        for (pixel* p : pixelsList) {
            if (sq.getGlobalBounds().intersects(p->getPixel().getGlobalBounds())) {
                p->setColor(sq.getFillColor());
            }
        }
    }

    void simpleCircle() {
        restart_canvas();
        sf::CircleShape c;
        c.setRadius(250);
        c.setOrigin(250, 250);
        c.setPosition(450, 662.5);
        c.setFillColor(sf::Color(rand(), rand(), rand()));
        for (pixel* p : pixelsList) {
            if (intersects(c, p->getPixel())) {
                p->setColor(c.getFillColor());
            }
        }
    }
    
    void simpleTriangle() {
        restart_canvas();
        std::vector<sf::RectangleShape> squares;
        sf::RectangleShape temp;
        int color[3] = { rand(), rand(), rand() };
        for (int c = 1; c < 80; c++) {
            temp.setSize(sf::Vector2f(400 - (5 * c), 5));
            temp.setPosition(sf::Vector2f(250, 850 - (5 * c)));
            temp.setFillColor(sf::Color(color[0], color[1], color[2]));
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


    // generates a dense "maze"
    void dense_maze() {
        clear();

        int x = 0, y = 200;
        pixel* insertion;

        // sets positions and places pixels in canvas objects for later access
        for (int column = 1; column <= sideLen; column++) {
            std::map<int, pixel*> currentColumn;
            for (int cell = 1; cell <= sideLen; cell++) {
                if (x == sideLen * pixel_dim) {
                    x = 0;
                    y += pixel_dim;
                }
                insertion = new pixel(x, y, pixel_dim, 4);
                pixelsList.push_back(insertion);
                currentColumn.emplace(cell, insertion);
                x += pixel_dim;
            }
            pixelsTree.emplace(column, currentColumn);
        }

        // sets each pixel's adjacent vector for filling operations
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
    }

    // generates a sparse "maze"
    void sparse_maze() {
        clear();

        int x = 0, y = 200;
        pixel* insertion;

        // sets positions and places pixels in canvas objects for later access
        for (int column = 1; column <= sideLen; column++) {
            std::map<int, pixel*> currentColumn;
            for (int cell = 1; cell <= sideLen; cell++) {
                if (x == sideLen * pixel_dim) {
                    x = 0;
                    y += pixel_dim;
                }
                insertion = new pixel(x, y, pixel_dim, 3);
                pixelsList.push_back(insertion);
                currentColumn.emplace(cell, insertion);
                x += pixel_dim;
            }
            pixelsTree.emplace(column, currentColumn);
        }

        // sets each pixel's adjacent vector for filling operations
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
    }

    void preset_generation(int image[]) {
        clear();

        int x = 0, y = 200;
        pixel* insertion;

        // sets positions and places pixels in canvas objects for later access
        for (int column = 1; column <= sideLen; column++) {
            std::map<int, pixel*> currentColumn;
            for (int cell = 1; cell <= sideLen; cell++) {
                if (x == sideLen * pixel_dim) {
                    x = 0;
                    y += pixel_dim;
                }
                insertion = new pixel(x, y, pixel_dim, image);
                pixelsList.push_back(insertion);
                currentColumn.emplace(cell, insertion);
                x += pixel_dim;
            }
            pixelsTree.emplace(column, currentColumn);
        }

        // sets each pixel's adjacent vector for filling operations
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
    }


    // breadth-first fill that shifts through green gradient to illustrate each iteration -- pastel orange border to illustrate pixels in queue
    int breadth(bool baseColorFill, pixel* first_pixel, std::map<int, sf::Color> color_palette, std::map<int, std::vector<int>> color, sf::RenderWindow& window, int delay_time, int current_color_mode) {
        int r;
        int g;
        int b;
        sf::Color base;

        if (current_color_mode != 0) {
            r = color.at(current_color_mode)[0];
            g = color.at(current_color_mode)[1];
            b = color.at(current_color_mode)[2];
        }

        if (baseColorFill) {
            base = first_pixel->color();
        }


        int pixels_checked = 0; // stores the # of black pixels checked to return and record in statistics of process
        std::queue<pixel*> q; // queue to store pixels that need to be filled in
        q.push(first_pixel);

        int current_color_position = 0; // cycle through colors (0-7, 8 total colors) with each iteration, currently not in use but maybe for a rainbow mode?

        sf::Clock clock; // timer to keep track of current time for delaying visualization process
        sf::Time current_time = clock.getElapsedTime();

        for (pixel* current : pixelsList) { window.draw(current->getPixel()); }

        while (!q.empty()) {

            sf::Time delay_timer = clock.getElapsedTime(); // get present time to check for how long to delay iteration for

            if (delay_timer.asSeconds() - current_time.asSeconds()  > delay_time) {

                pixel* current = q.front();
                if (current_color_mode != 0) {
                    current->data.setFillColor(sf::Color(r, g, b)); // get the first pixel colored
                    // red
                    if (current_color_mode == 1) {
                        g, b += 1;

                        if (g > 240) {
                            g = 0;
                        }
                        if (b > 240) {
                            b = 0;
                        }
                    }
                        // orange
                    else if (current_color_mode == 2) {
                        g, b += 1;

                        if (g > 240) {
                            g = 100;
                            b = 0;
                        }
                    }
                        // yellow
                    else if (current_color_mode == 3) {
                        b += 1;

                        if (b > 240) {
                            b = 0;
                        }
                    }
                        // green
                    else if (current_color_mode == 4) {
                        r, b += 1;

                        if (r > 240) {
                            r = 0;
                        }
                        if (b > 240) {
                            b = 0;
                        }
                    }
                        // blue
                    else if (current_color_mode == 5) {
                        r += 1;

                        if (r > 240) {
                            r = 0;
                        }
                    }
                        // indigo
                    else if (current_color_mode == 6) {
                        r, g += 1;

                        if (r > 240) {
                            r = 0;
                        }
                        if (g > 240) {
                            g = 0;
                        }
                    }
                        // purple
                    else if (current_color_mode == 7) {
                        r, g += 1;

                        if (r > 240) {
                            r = 100;
                            g = 0;
                        }
                    }
                        // pink
                    else if (current_color_mode == 8) {
                        g += 1;

                        if (g > 240) {
                            g = 0;
                        }
                    }
                        // grey
                    else if (current_color_mode == 9) {
                        r, g, b += 1;

                        if (r > 240) {
                            r = 100;
                        }
                        if (g > 240) {
                            g = 100;
                        }
                        if (b > 240) {
                            b = 100;
                        }
                    }
                }
                else {
                    current->data.setFillColor(color_palette.at(current_color_position % 8));
                    current_color_position += 1;
                }

                q.pop();


                for (int i = 0; i < current->adjacent.size(); i++) {
                    if (baseColorFill) {
                        if (current->adjacent[i]->data.getFillColor() == base) {
                            current->adjacent[i]->data.setFillColor(color_palette.at(current_color_mode % 8)); // get the first pixel colored
                            q.push(current->adjacent[i]);
                        }
                        else {
                            pixels_checked += 1;
                        }
                        continue;
                    }
                    if (current->adjacent[i]->data.getFillColor() == sf::Color::White) {
                        current->adjacent[i]->data.setFillColor(color_palette.at(1)); // get the first pixel colored
                        q.push(current->adjacent[i]);
                    }
                    else if (current->adjacent[i]->data.getFillColor() == sf::Color::Black) {
                        pixels_checked += 1;
                    }
                }

                // drawing all pixels takes a while but prevents stuttering
                for (pixel* c : pixelsList) { window.draw(c->getPixel()) ;}
                //window.draw(current->getPixel());
                window.display();

                current_time = clock.getElapsedTime(); // keeps track of time at start of next loop to prepare for delay if added
            }

        }
        for (pixel* current : pixelsList) { window.draw(current->getPixel()); }

        return pixels_checked;
    }

    // depth-first fill that shifts through blue/purple gradient to illustrate each iteration -- pastel purple border to illustrate pixels in stack
    int depth(bool baseColorFill, pixel* first_pixel, std::map<int, sf::Color> color_palette, std::map<int, std::vector<int>> color, sf::RenderWindow& window, int delay_time, int current_color_mode) {
        int r;
        int g;
        int b;
        sf::Color base;

        if (current_color_mode != 0) {
            r = color.at(current_color_mode)[0];
            g = color.at(current_color_mode)[1];
            b = color.at(current_color_mode)[2];
        }

        if (baseColorFill) {
            base = first_pixel->color();
        }

        int pixels_checked = 0; // stores the # of black pixels checked to return and record in statistics of process
        std::stack<pixel*> s; // queue to store pixels that need to be filled in
        s.push(first_pixel);

        int current_color_position = 0; // cycle through colors (0-7, 8 total colors) with each iteration, currently not in use but maybe for a rainbow mode?

        sf::Clock clock; // timer to keep track of current time for delaying visualization process
        sf::Time current_time = clock.getElapsedTime();

        for (pixel* current : pixelsList) { window.draw(current->getPixel()); }

        while (!s.empty()) {

            sf::Time delay_timer = clock.getElapsedTime(); // get present time to check for how long to delay iteration for

            if (delay_timer.asSeconds() - current_time.asSeconds()  > delay_time) {

                pixel* current = s.top();

                if (current_color_mode != 0) {
                    current->data.setFillColor(sf::Color(r, g, b)); // get the first pixel colored
                    // red
                    if (current_color_mode == 1) {
                        g, b += 1;

                        if (g > 240) {
                            g = 0;
                        }
                        if (b > 240) {
                            b = 0;
                        }
                    }
                    // orange
                    else if (current_color_mode == 2) {
                        g, b += 1;

                        if (g > 240) {
                            g = 100;
                            b = 0;
                        }
                    }
                    // yellow
                    else if (current_color_mode == 3) {
                        b += 1;

                        if (b > 240) {
                            b = 0;
                        }
                    }
                    // green
                    else if (current_color_mode == 4) {
                        r, b += 1;

                        if (r > 240) {
                            r = 0;
                        }
                        if (b > 240) {
                            b = 0;
                        }
                    }
                    // blue
                    else if (current_color_mode == 5) {
                        r += 1;

                        if (r > 240) {
                            r = 0;
                        }
                    }
                    // indigo
                    else if (current_color_mode == 6) {
                        r, g += 1;

                        if (r > 240) {
                            r = 0;
                        }
                        if (g > 240) {
                            g = 0;
                        }
                    }
                    // purple
                    else if (current_color_mode == 7) {
                        r, g += 1;

                        if (r > 240) {
                            r = 100;
                            g = 0;
                        }
                    }
                    // pink
                    else if (current_color_mode == 8) {
                        g += 1;

                        if (g > 240) {
                            g = 0;
                        }
                    }
                    // grey
                    else if (current_color_mode == 9) {
                        r, g, b += 1;

                        if (r > 240) {
                            r = 100;
                        }
                        if (g > 240) {
                            g = 100;
                        }
                        if (b > 240) {
                            b = 100;
                        }
                    }
                }
                else {
                    current->data.setFillColor(color_palette.at(current_color_position % 8));
                    current_color_position += 1;
                }

                s.pop();

                // outlines all adjacent pixels that are to be checked next (all in stack)
                for (int i = 0; i < current->adjacent.size(); i++) {
                    if (baseColorFill) {
                        if (current->adjacent[i]->data.getFillColor() == base) {
                            current->adjacent[i]->data.setFillColor(color_palette.at(current_color_mode % 8)); // get the first pixel colored
                            s.push(current->adjacent[i]);
                        }
                        else {
                            pixels_checked += 1;
                        }
                        continue;
                    }
                    if (current->adjacent[i]->data.getFillColor() == sf::Color::White) {
                        current->adjacent[i]->data.setFillColor(color_palette.at(current_color_mode%8)); // get the first pixel colored
                        s.push(current->adjacent[i]);
                    }
                    else if (current->adjacent[i]->data.getFillColor() == sf::Color::Black) {
                        pixels_checked += 1;
                    }
                }

                // drawing all pixels takes a while but prevents stuttering
                for (pixel* c : pixelsList) { window.draw(c->getPixel()) ;}
                //window.draw(current->getPixel());
                window.display();

                current_time = clock.getElapsedTime(); // keeps track of time at start of next loop to prepare for delay if added
            }

        }
        for (pixel* current : pixelsList) { window.draw(current->getPixel()); }

        return pixels_checked;
    }

};