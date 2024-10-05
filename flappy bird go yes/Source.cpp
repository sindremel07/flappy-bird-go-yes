#include <SFML/Graphics.hpp>
#include <SFML/Window.hpp>
#include <SFML/System.hpp>
#include <iostream>
#include <random>
#include <cmath>
#include <vector>

// struct of pipes
struct PipePair {
    sf::RectangleShape topPipe;
    sf::RectangleShape bottomPipe;
    float xPosition;
    bool givenScore;
};

// function to shoot cookie
void shoot(std::vector<sf::Sprite>& cookies, sf::Sprite& bird, sf::Texture cookieTexture) {
    float shootPosY = bird.getPosition().y - (bird.getScale().y / 2);
    float shootPosX = bird.getPosition().x + bird.getScale().x;

    sf::Sprite newCookie;
    newCookie.setPosition(shootPosX, shootPosY);

    cookies.push_back(newCookie);
}

// checking if and what part of a pipe was hit by cookie
int checkCookieHit(const sf::FloatRect& topBounds, const sf::FloatRect& bottomBounds, const sf::FloatRect& cookieBounds) {
    if (cookieBounds.intersects(topBounds)) {
        return 1;
    }
    else if (cookieBounds.intersects(bottomBounds)) {
        return 2;
    }
}

// checking if player hit pipe
bool checkCollision(const sf::FloatRect& spriteBounds, const sf::FloatRect& topBounds, const sf::FloatRect& bottomBounds) {
    return spriteBounds.intersects(topBounds) || spriteBounds.intersects(bottomBounds);
}

// main function
int main() {
    // random height generation
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<> distrib1(150, 350);
    std::uniform_int_distribution<> distrib2(50, 250);

    // random variables
    float pipeSpeed = 130.0f; // start speed of pipes
    int scoreInt = 0;
    float posY = 100;
    float velocity = 0;
    float gravity = 0.5f;
    float jumpStrength = -7.0f;
    bool spacePressed = false;
    bool cookieShot = false;
    float scrollSpeed = 10.0f;

    // create window
    sf::RenderWindow window(sf::VideoMode(800, 600), "flappy bird go yes");
    window.setFramerateLimit(60);

    // getting textures and font
    sf::Texture bird;
    if (!bird.loadFromFile("bird.png")) {
        std::cout << "Error loading bird texture!" << std::endl;
        return 1;
    }
    sf::Texture cookieTexture;
    if (!cookieTexture.loadFromFile("cookie.png")) {
        std::cout << "Error loading cookie texture!" << std::endl;
        return 1;
    }
    sf::Font font;
    if (!font.loadFromFile("baijamjuree.ttf")) {
        std::cout << "Error loading font!" << std::endl;
        return 1;
    }
    sf::Texture backgroundTexture;
    if (!backgroundTexture.loadFromFile("background.png")) {
        return -1;
    }

    // displaying score text and bird sprite
    sf::Text score;
    score.setFont(font);
    score.setFillColor(sf::Color::White);
    score.setCharacterSize(25);
    score.setPosition(50, 20);
    score.setString("Score: " + std::to_string(scoreInt));

    sf::Sprite sprite;
    sprite.setTexture(bird);
    sprite.setPosition(100, posY);

    // creating two background sprites for infinite loop
    sf::Sprite background1;
    sf::Sprite background2;
    background1.setTexture(backgroundTexture);
    background2.setTexture(backgroundTexture);
    background1.setPosition(0, 0);
    background2.setPosition(backgroundTexture.getSize().x -1, 0);


    // random height containers
    int height1 = distrib1(gen);
    int height2 = distrib2(gen); 

    // array to contain the pipes
    std::vector<PipePair> pipes = {
        {sf::RectangleShape(sf::Vector2f(50, height1)), sf::RectangleShape(sf::Vector2f(50, 600 - height1 - 200)), 900},
        {sf::RectangleShape(sf::Vector2f(50, height2)), sf::RectangleShape(sf::Vector2f(50, 600 - height2 - 200)), 1300}
    };

    // array to contain the cookies
    std::vector<sf::Sprite> cookies;

    // displaying the pipes in the array
    for (auto& pipe : pipes) {
        pipe.topPipe.setFillColor(sf::Color(0, 150, 0));
        pipe.bottomPipe.setFillColor(sf::Color(0, 150, 0));
        pipe.bottomPipe.setPosition(pipe.xPosition, pipe.topPipe.getSize().y + 200);
    }

    sf::Clock clock;
    while (window.isOpen()) {
        sf::Time deltaTime = clock.restart();
        float delta = deltaTime.asSeconds();

        sf::Event event;
        while (window.pollEvent(event)) {
            if (event.type == sf::Event::Closed)
                window.close();
            else if (event.type == sf::Event::KeyPressed && event.key.code == sf::Keyboard::Space) {
                if (!spacePressed) {
                    velocity = jumpStrength;
                    spacePressed = true;
                }
            }
            else if (event.type == sf::Event::KeyReleased && event.key.code == sf::Keyboard::Space) {
                spacePressed = false;
            }

            if (event.type == sf::Event::MouseButtonPressed && event.key.code == sf::Mouse::Left) {
                if (!cookieShot) {
                    shoot(cookies, sprite, cookieTexture);
                    cookieShot = true;
                }
            }
            else if (event.type == sf::Event::MouseButtonReleased && event.key.code == sf::Mouse::Left) {
                cookieShot = false;
            }
        }

        // move both background sprites to the left
        background1.move(-scrollSpeed * delta, 0);
        background2.move(-scrollSpeed * delta, 0);

        // infinite loop of background
        if (background1.getPosition().x + backgroundTexture.getSize().x <= 0) {
            background1.setPosition(background2.getPosition().x + backgroundTexture.getSize().x -1, 0);
        }
        if (background2.getPosition().x + backgroundTexture.getSize().x <= 0) {
            background2.setPosition(background1.getPosition().x + backgroundTexture.getSize().x -1, 0);
        }

        // physics for bird
        velocity += gravity;
        posY += velocity;

        pipeSpeed += 0.1f; // increasing pipe speed overtime

        // moving all the pipes and updating score if player passes
        for (auto& pipe : pipes) {
            float yPosTop = pipe.topPipe.getPosition().y;
            float yPosBottom = pipe.bottomPipe.getPosition().y;
            pipe.xPosition -= pipeSpeed * delta;
            pipe.topPipe.setPosition(pipe.xPosition, yPosTop);
            pipe.bottomPipe.setPosition(pipe.xPosition, yPosBottom);

            if (pipe.xPosition <= -50) {
                int randomHeight = distrib1(gen);
                pipe.xPosition = 900;
                pipe.givenScore = false;
                pipe.topPipe.setSize(sf::Vector2f(50, randomHeight));
                pipe.bottomPipe.setSize(sf::Vector2f(50, 600 - randomHeight - 200));
                pipe.topPipe.setPosition(pipe.xPosition, 0);
                pipe.bottomPipe.setPosition(pipe.xPosition, pipe.topPipe.getSize().y + 200);
            }// put 90-100, since it skips some positions when pipes goes faster
            if (std::round(pipe.xPosition) > 90 && std::round(pipe.xPosition) <= 100) {
                if (!pipe.givenScore) {
                    scoreInt += 1;
                    pipe.givenScore = true;
                    score.setString("Score: " + std::to_string(scoreInt));
                }
            }
        }

        // moving all the cookies that got shot and deleting when they go off screen
        for (auto it = cookies.begin(); it != cookies.end(); ) {
            float currentPosCookieX = it->getPosition().x;
            float currentPosCookieY = it->getPosition().y;

            currentPosCookieX += 300.0f * delta;
            it->setPosition(currentPosCookieX, currentPosCookieY);
            it->setTexture(cookieTexture);

            if (std::round(it->getPosition().x) >= (800.0f + it->getScale().x)) {
                score.setString("Score: " + std::to_string(scoreInt));

                it = cookies.erase(it);
            }
            else {
                ++it;
            }
        }

        // close game if bird falls down 
        if (posY > 600 - sprite.getGlobalBounds().height) {
            window.close();
        }

        // moves bird
        sprite.setPosition(100, posY);

        // checking for collisions between bird and pipes
        sf::FloatRect spriteBounds = sprite.getGlobalBounds();
        for (const auto& pipe : pipes) {
            sf::FloatRect topBounds = pipe.topPipe.getGlobalBounds();
            sf::FloatRect bottomBounds = pipe.bottomPipe.getGlobalBounds();
            if (checkCollision(spriteBounds, topBounds, bottomBounds)) {
                window.close();
            }
        }

        // checking for collision between pipes and cookie
        for (const auto& cookie : cookies) {
            for (auto& pipe : pipes) {
                sf::FloatRect topBounds = pipe.topPipe.getGlobalBounds();
                sf::FloatRect bottomBounds = pipe.bottomPipe.getGlobalBounds();
                sf::FloatRect cookieBounds = cookie.getGlobalBounds();

                int pipeHit = checkCookieHit(topBounds, bottomBounds, cookieBounds);

                if (pipeHit && pipeHit == 1) {
                    pipe.topPipe.setPosition(pipe.xPosition, -1000);
                }
                else if (pipeHit && pipeHit == 2) {
                    pipe.bottomPipe.setPosition(pipe.xPosition, 1000);
                }
            }
        }


        window.clear();

        window.draw(background1);
        window.draw(background2);

        // draws all the items on screen
        window.draw(sprite);
        for (const auto& pipe : pipes) {
            window.draw(pipe.topPipe);
            window.draw(pipe.bottomPipe);
        }

        for (const auto& cookie : cookies) {
            window.draw(cookie);
        }
        window.draw(score);
        window.display();
    }

    return 0;
}
