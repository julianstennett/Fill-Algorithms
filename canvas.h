#pragma once
#include <iostream>
#include <chrono>
#include "pixel.h"
#include "interface.h"

class canvas {
    std::map<int, std::map<int, pixel*>> pixelsTree; // nested tree used for positional search over specified pixel log(n^(1/2)) + log(n^(1/2)) = O(log(n))
    std::vector<pixel*> pixelsList; // general list used for iteration over all pixels O(n)
    int sideLen = 0;
    int pixel_dim = 30;
    bool BreadthFill = true;

    sf::Event event;
    sf::Mouse mouse;


    bool drawing = false; // keeps track of if drawing mode is on or not
    int current_mode; // to keep track of what to display for interface current outline -> 0 = N/A (first start program)
    // 1 = "Random Rectangles", 2 = "Random Circles", 3 = "Random Sparse Maze", 4 = "Random Dense Maze", 5 = Simple Triangle
    // 6 - Simple Circle, 7 = Simple Rectangle, 8 = Surprise, 9 = Drawing Mode;

    sf::RenderWindow window;

    int duration = -1;
    int iteration_delay; // for speed of traversal change
    int current_pixels_checked = -1;

    interface ui;

public:
    // parameterized constructor taking in a single screen dimension Side x Side
    canvas(int screenDimension) {
        current_mode = 0;
        iteration_delay = 0;

        this->sideLen = screenDimension;
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
    void click(sf::Vector2i pos, bool BFS, sf::Color color, sf::RenderWindow& window) {
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

                int i = (x - 1) % 30 + (y - 1) * 30;
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
            current_pixels_checked = breadth(found, ui.custom_colors, window, iteration_delay);

        else
            //found->depth(color, window);
            current_pixels_checked = depth(found, ui.custom_colors, window, iteration_delay);


        auto end = std::chrono::high_resolution_clock::now();
        auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count();
        std::cout << "Algorithm time: " << duration << " milliseconds. \n";

        ui.update_stats(window, duration, iteration_delay, current_pixels_checked);
        this->duration = duration;
        quick_update();
    }

    void display() { // SFML display and controls for the window and user interface

        bool updateWindow = true;
        window.setFramerateLimit(60);
        while (window.isOpen()) {
            ui.display_interface(window);

            while (window.pollEvent(event)) {

                if (event.type == sf::Event::Closed) {

                    window.close();
                }
                if (event.type == sf::Event::MouseButtonPressed) {
                    if (event.key.code == sf::Mouse::Left) {
                        updateWindow = true;
                        auto pos = mouse.getPosition(window);

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

                        click(pos, BreadthFill, sf::Color::Blue, window);
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

                    if (event.key.code == sf::Keyboard::R) {
                        current_mode = 1;
                        ui.update_interface(window, current_mode, BreadthFill);

                        std::cout << "random squares...\n";
                        randomSquares();
                        updateWindow = true;
                    }

                    if (event.key.code == sf::Keyboard::C) {
                        current_mode = 2;
                        ui.update_interface(window, current_mode, BreadthFill);

                        std::cout << "random circles...\n";
                        randomCircles();
                        updateWindow = true;
                    }

                    // activates drawing mode
                    if (event.key.code == sf::Keyboard::D) {
                        drawing = !drawing;
                        if (drawing) {
                            current_mode = 9;
                        }
                        else {
                            current_mode = 0;
                        }
                        ui.update_interface(window, current_mode, BreadthFill);
                        quick_update();
                    }

                    // decreases how long an iteration of either traversal takes to execute (waits until amount of time has passed)
                    if (event.key.code == sf::Keyboard::Down) {
                        if (iteration_delay >= 1) {
                            iteration_delay -= 1;
                            ui.update_stats(window, duration, iteration_delay, current_pixels_checked);
                            quick_update();
                        }
                    }

                    // increases how long an iteration of either traversal takes to execute (waits until amount of time has passed)
                    if (event.key.code == sf::Keyboard::Up) {
                        iteration_delay += 1;
                        ui.update_stats(window, duration, iteration_delay, current_pixels_checked);
                        quick_update();
                    }

                    // restarts the canvas with a blank slate
                    if (event.key.code == sf::Keyboard::BackSpace) {
                        restart_canvas();
                        ui.update_stats(window, duration, iteration_delay, current_pixels_checked);
                        quick_update();
                    }

                    // activates randomized sparse "maze" mode
                    if (event.key.code == sf::Keyboard::S) {
                        sparse_maze();
                        ui.update_stats(window, duration, iteration_delay, current_pixels_checked);
                        quick_update();
                    }

                    // activates randomized dense "maze" mode
                    if (event.key.code == sf::Keyboard::A) {
                        dense_maze();
                        ui.update_stats(window, duration, iteration_delay, current_pixels_checked);
                        quick_update();
                    }

                    // TO-DO: WORK ON PRESETS WILL RETURN TO FINISH

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

    // restarts the canvas to white blank slate
    void restart_canvas() {
        pixelsList.clear();
        pixelsTree.clear();

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

    // these don't work kinda rn because I have the BFS/DFS filling only white spaces -- will modify back to the original color of the first pixel when selected
    // when I get back to working on this (@8/4/2023 - 3:59AM EST)
    // definitely work on the circles function when you get the chance
    void randomSquares() {
        std::vector<sf::RectangleShape> squares;
        sf::RectangleShape temp;
        for (int c = 0; c < 10; c++) {
            temp.setSize(sf::Vector2f(rand() % sideLen * 5 + 20, rand() % sideLen * 5 + 20));
            temp.setPosition(sf::Vector2f(rand() % sideLen * 2, rand() % sideLen * 3));
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

    void randomCircles() { // currently broken and only showing rectangles
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

    // generates a dense "maze"
    void dense_maze() {
        pixelsList.clear();
        pixelsTree.clear();

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

        current_mode = 4;
    }

    // generates a sparse "maze"
    void sparse_maze() {
        pixelsList.clear();
        pixelsTree.clear();

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

        current_mode = 3;
    }


    // breadth-first fill that shifts through green gradient to illustrate each iteration -- pastel orange border to illustrate pixels in queue
    int breadth(pixel* first_pixel, std::map<int, sf::Color> color_palette, sf::RenderWindow& window, int delay_time) {
        int color_saturation = 255;


        int pixels_checked = 1; // stores the # of pixels checked to return and record in statistics of process
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
                current->data.setFillColor(sf::Color(color_saturation, 255, 0)); // get the first pixel colored
                //current->fill(color_palette.at(current_color_position % 8));
                q.pop();
                color_saturation -= 1; // how the gradient occurs, goes into overflow for cool effect


                for (int i = 0; i < current->adjacent.size(); i++) {
                    if (current->adjacent[i]->data.getFillColor() == sf::Color::White) {
                        current->adjacent[i]->data.setFillColor(color_palette.at(1)); // get the first pixel colored
                        q.push(current->adjacent[i]);
                    }
                    pixels_checked += 1;
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
    int depth(pixel* first_pixel, std::map<int, sf::Color> color_palette, sf::RenderWindow& window, int delay_time) {
        int color_saturation = 255;

        int pixels_checked = 1; // stores the # of pixels checked to return and record in statistics of process
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
                current->data.setFillColor(sf::Color(color_saturation, 0, 255)); // get the first pixel colored
                //current->fill(color_palette.at(current_color_position % 8));
                s.pop();
                color_saturation -= float(1)/float(3);


                for (int i = 0; i < current->adjacent.size(); i++) {
                    if (current->adjacent[i]->data.getFillColor() == sf::Color::White) {
                        current->adjacent[i]->data.setFillColor(color_palette.at(6)); // get the first pixel colored
                        s.push(current->adjacent[i]);
                    }
                    pixels_checked += 1;
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