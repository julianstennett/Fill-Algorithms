#pragma once

class pixel {
public:
    sf::RectangleShape data;
    std::vector<pixel*> adjacent;

    // "default" parameterized constructor taking in positional arguments
    pixel(float x, float y, int pixel_dim) {
        y = y - 200;


        int x_i = int(x) / pixel_dim;
        int y_i = int(y) / pixel_dim;

        x_i = x_i % 30;
        y_i = y_i * 30;
        int i = x_i + y_i;


        data.setFillColor(sf::Color::White);
        data.setSize(sf::Vector2f(pixel_dim, pixel_dim));
        data.setPosition(x, y+200);
    }
    // parameterized constructor for pixel dealing with presets -- to use for all preset modes
    pixel(float x, float y, int pixel_dim, int image[]) {
        y = y - 200;

        int x_i = int(x) / pixel_dim;
        int y_i = int(y) / pixel_dim;

        x_i = x_i % 30;
        y_i = y_i * 30;
        int i = x_i + y_i;

        if(image[i]== 0){
            data.setFillColor(sf::Color::White);
        }
        else if (image[i] == 1) {
            data.setFillColor(sf::Color::Black);
        }
        else if (image[i] == 2) {
            // dark grey 3
            data.setFillColor(sf::Color(102, 102, 102));
        }
        else {
            // light magenta 3
            data.setFillColor(sf::Color(234, 209, 220));
        }

        data.setSize(sf::Vector2f(pixel_dim, pixel_dim));
        data.setPosition(x, y+200);
    }

    // pixel constructor for mazes - for reference current_mode = 3 -> sparse & 4 -> dense
    pixel(float x, float y, int pixel_dim, int current_mode) {
        int occurence_value = 3; // set initially for sparse, increases or decreases chances of white:black pixels
        if (current_mode == 3) {
            occurence_value = 8;
        }

        if (rand() % occurence_value) {
            data.setFillColor(sf::Color::White);
        }
        else {
            data.setFillColor(sf::Color::Black);
        }

        y = y - 200;

        int x_i = int(x) / pixel_dim;
        int y_i = int(y) / pixel_dim;

        x_i = x_i % 30;
        y_i = y_i * 30;
        int i = x_i + y_i;

        data.setSize(sf::Vector2f(pixel_dim, pixel_dim));
        data.setPosition(x, y+200);
    }

    void border(pixel* adj) {
        adjacent.push_back(adj);
    }

    sf::RectangleShape getPixel() {
        return data;
    }

    sf::Color color() {
        return data.getFillColor();
    }

    void setColor(sf::Color color) {
        data.setFillColor(color);
    }

    void setColor(std::vector<int> newColor) {
        data.setFillColor(sf::Color(newColor[1], newColor[2], newColor[3]));
    }
};
