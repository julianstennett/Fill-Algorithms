// TO-DO: Remove fill image button since there is active screen clicking instead
// ALSO will probably improve UI general aesthetic when back
// class for initializing all SFML text, textures, and sprites for the user interface
class interface {
    bool bfs_on = true; // tracks if bfs is on in canvas mode, else dfs -- for sprite texture purposes
    int current_selected = 0; // tracks what current outline should say

    std::map<int, std::string> modes; // holds all the names of modes that exist

    // initial stats of fill application: all default until action on canvas
    int current_time = -1;
    int iteration_delay = 0;
    int pixels_looked = -1;

    // all fonts to be loaded - using Lora
    sf::Font screen_font_normal;
    sf::Font screen_font_italicized;
    sf::Font screen_font_bold;

    // all text for interface
    sf::Text title;
    sf::Text current_outline;
    sf::Text current_alg;
    sf::Text speed_of_search;
    sf::Text elapsed_time;
    sf::Text num_pixels_checked;

    // active text that changes throughout user interaction
    sf::Text selected_outline; // the current outline name based on shortcut pressed
    sf::Text time;
    sf::Text speed;
    sf::Text pixels_checked;

    // all textures and their associated sprites for interface
    sf::Texture bfs_texture;
    sf::Texture bfs_pressed_texture;
    sf::Sprite bfs_option;

    sf::Texture dfs_texture;
    sf::Texture dfs_pressed_texture;
    sf::Sprite dfs_option;

    sf::Texture fill_button_texture;
    sf::Texture fill_pressed_texture;
    sf::Sprite fill_button;

    sf::Texture help_texture;
    sf::Sprite help_button;

    sf::RenderWindow help_screen;

public:
    std::map<int, sf::Color> custom_colors; // colors that will be used for fill traversal visualization

    sf::FloatRect bfs_bounds;
    sf::FloatRect dfs_bounds;
    sf::FloatRect fill_bounds;
    sf::FloatRect help_bounds;

    // initialize all the SFML objects for interface
    interface() {


        custom_colors.emplace(0, sf::Color(255, 173, 173));
        custom_colors.emplace(1, sf::Color(255, 214, 165));
        custom_colors.emplace(2, sf::Color(253, 255, 182));
        custom_colors.emplace(3,sf::Color(202, 255, 191));
        custom_colors.emplace(4, sf::Color(155, 246, 255));
        custom_colors.emplace(5, sf::Color(160, 196, 255));
        custom_colors.emplace(6, sf::Color(189, 178, 255));
        custom_colors.emplace(7, sf::Color(255, 198, 255));


        screen_font_normal.loadFromFile("files/text_files/Lora-Regular.ttf");
        screen_font_italicized.loadFromFile("files/text_files/Lora-Italic.ttf");
        screen_font_bold.loadFromFile("files/text_files/Lora-Bold.ttf");


        title.setFont(screen_font_bold);
        current_outline.setFont(screen_font_normal);
        current_alg.setFont(screen_font_normal);
        speed_of_search.setFont(screen_font_normal);
        elapsed_time.setFont(screen_font_normal);
        num_pixels_checked.setFont(screen_font_normal);
        selected_outline.setFont(screen_font_italicized);
        time.setFont(screen_font_italicized);
        speed.setFont(screen_font_italicized);
        pixels_checked.setFont(screen_font_italicized);


        title.setString("Visual Vortex");
        current_outline.setString("Current Outline:");
        current_alg.setString("Current Algorithm:");
        speed_of_search.setString("Speed of Traversal");
        elapsed_time.setString("Elapsed Time:");
        num_pixels_checked.setString("# of Pixels Checked:");
        selected_outline.setString("N/A");
        time.setString("N/A");
        speed.setString("0 seconds of delay");
        pixels_checked.setString("N/A");

        title.setCharacterSize(96);
        current_outline.setCharacterSize(36);
        current_alg.setCharacterSize(36);
        speed_of_search.setCharacterSize(36);
        elapsed_time.setCharacterSize(36);
        num_pixels_checked.setCharacterSize(36);
        selected_outline.setCharacterSize(36);
        time.setCharacterSize(30);
        speed.setCharacterSize(30);
        pixels_checked.setCharacterSize(30);


        title.setFillColor(sf::Color::Black);
        current_outline.setFillColor(sf::Color::Black);
        current_alg.setFillColor(sf::Color::Black);
        speed_of_search.setFillColor(sf::Color::Black);
        elapsed_time.setFillColor(sf::Color::Black);
        num_pixels_checked.setFillColor(sf::Color::Black);
        selected_outline.setFillColor(sf::Color::Black);
        time.setFillColor(sf::Color::Black);
        speed.setFillColor(sf::Color::Black);
        pixels_checked.setFillColor(sf::Color::Black);


        title.setPosition(10, 17);
        current_outline.setPosition(20, 120);
        current_alg.setPosition(1002, 218);
        speed_of_search.setPosition(1002, 512);
        elapsed_time.setPosition(1002, 597);
        num_pixels_checked.setPosition(1002, 690);
        selected_outline.setPosition(300, 120);
        time.setPosition(1002, 640);
        speed.setPosition(1002, 555);
        pixels_checked.setPosition(1002, 735);

        bfs_texture.loadFromFile("files/image_files/bfs.png");
        bfs_pressed_texture.loadFromFile("files/image_files/bfs_selected.png");
        dfs_texture.loadFromFile("files/image_files/dfs.png");
        dfs_pressed_texture.loadFromFile("files/image_files/dfs_selected.png");
        fill_button_texture.loadFromFile("files/image_files/fill_image_button.png");
        fill_pressed_texture.loadFromFile("files/image_files/fill_image_pressed.png");
        help_texture.loadFromFile("files/image_files/help_button.png");

        bfs_option.setTexture(bfs_pressed_texture);
        dfs_option.setTexture(dfs_texture);
        fill_button.setTexture(fill_button_texture);
        help_button.setTexture(help_texture);

        bfs_option.setPosition(1002, 273);
        dfs_option.setPosition(1281, 273);
        fill_button.setPosition(1039, 349);
        help_button.setPosition(1329, 1000);

        bfs_bounds = bfs_option.getGlobalBounds();
        dfs_bounds = dfs_option.getGlobalBounds();
        fill_bounds = fill_button.getGlobalBounds();
        help_bounds = help_button.getGlobalBounds();

        modes.emplace(0, "N/A");
        modes.emplace(1, "Random Rectangles");
        modes.emplace(2, "Random Circles");
        modes.emplace(3, "Random Sparse Maze");
        modes.emplace(4, "Random Dense Maze");
        modes.emplace(5, "Simple Triangle");
        modes.emplace(6, "Simple Circle");
        modes.emplace(7, "Simple Rectangle");
        modes.emplace(8, "Surprise!");
        modes.emplace(9, "Drawing Mode");

    }

    // displays the interface components of the screen
    void display_interface(sf::RenderWindow &main_screen) {
        main_screen.clear(sf::Color(237, 242, 255));
        main_screen.draw(title);
        main_screen.draw(current_outline);
        main_screen.draw(current_alg);
        main_screen.draw(speed_of_search);
        main_screen.draw(elapsed_time);
        main_screen.draw(num_pixels_checked);
        main_screen.draw(selected_outline);
        main_screen.draw(time);
        main_screen.draw(speed);
        main_screen.draw(pixels_checked);

        main_screen.draw(bfs_option);
        main_screen.draw(dfs_option);
        main_screen.draw(fill_button);
        main_screen.draw(help_button);

    }

    // updates current outline text and or bfs/dfs selection on interface
    void update_interface(sf::RenderWindow &main_screen, int current_mode, bool bfs) {
        if (bfs && !bfs_on) {
            bfs_option.setTexture(bfs_pressed_texture);
            dfs_option.setTexture(dfs_texture);

            bfs_on = true;
        }
        else if (!bfs && bfs_on) {
            bfs_option.setTexture(bfs_texture);
            dfs_option.setTexture(dfs_pressed_texture);

            bfs_on = false;
        }

        if (current_selected != current_mode) {
            selected_outline.setString(modes.at(current_mode));
            current_selected = current_mode;
        }

        display_interface(main_screen);
    }

    // updates the stats of the last bfs or dfs process run
    void update_stats(sf::RenderWindow &main_screen, int curr_time, int curr_speed, int curr_pixels_checked) {
        if (current_time != curr_time) {
            std::string temp = std::to_string(curr_time);
            time.setString(temp + " ms");

            current_time = curr_time;
        }

        if (iteration_delay != curr_speed) {
            std::string temp = std::to_string(curr_speed);
            speed.setString(temp + " seconds of delay");

            iteration_delay = curr_speed;
        }

        if (pixels_looked != curr_pixels_checked) {
            std::string temp = std::to_string(curr_pixels_checked);
            pixels_checked.setString(temp + " pixels");

            pixels_looked = curr_pixels_checked;
        }

        display_interface(main_screen);
    }

    // opens the help menu containing shortcuts/hotkeys that can be utilized
    void display_help() {
        help_screen.create({600, 600}, "Help Menu"); // creates the help menu screen to open on user side

        // using image created in Figma as shortcut rather than utilize multiple text objects and aligning them
        sf::Texture help_screen_texture;
        sf::Sprite help_screen_sprite;
        help_screen_texture.loadFromFile("files/image_files/help_menu.png");
        help_screen_sprite.setTexture(help_screen_texture);

        sf::Event event;
        while (help_screen.isOpen()) {
            help_screen.clear(sf::Color(237, 242, 255)); // background color of help menu
            while (help_screen.pollEvent(event)) { // only event possible is for user to close the menu to return to main program
                if (event.type == sf::Event::Closed) {
                    help_screen.close();
                }
            }

            help_screen.draw(help_screen_sprite);
            help_screen.display();
        }

    }







};
