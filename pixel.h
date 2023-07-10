#pragma once

class pixel {
    sf::RectangleShape data;
    std::vector<pixel*> adjacent;
public:

    // parameratized constructor taking in positional arguements
    pixel(int x, int y) {
        data.setFillColor(sf::Color::Black);
        data.setSize(sf::Vector2f(5, 5));
        data.setPosition(x, y);
    }

    void border(pixel* adj) {
        adjacent.push_back(adj);
    }

    void breadth(sf::Color color, sf::RenderWindow& window) {
        sf::Color base = data.getFillColor();
        if (base == color) {
            return;
        }
        std::queue<pixel*> q;
        q.push(this);
        while (!q.empty()) {
            int level = q.size();
            while (level--) {
                pixel* curr = q.front();
                curr->data.setFillColor(color);
                for (pixel* adj : curr->adjacent) { // adds adjacent pixels with a matching color to the queue
                    if (adj->data.getFillColor() == base) {
                        adj->data.setFillColor(color);
                        q.push(adj);
                    }
                    else {
                        window.draw(adj->data);
                    }
                }
                window.draw(curr->data);
                q.pop();
            }
            window.display(); // displays color changes by level
        }

    }

    void depth(sf::Color color, sf::RenderWindow& window) {
        return;
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