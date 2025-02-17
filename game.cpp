#include <SFML/Graphics.hpp>
#include <cmath>
#include <stdlib.h>
#include <stdio.h>

class Paddle
{
public:
    sf::RenderWindow &window;
    sf::RectangleShape shape = sf::RectangleShape(sf::Vector2f(150, 30));
    float dx;

    Paddle(sf::RenderWindow &window) : window(window)
    {
        shape.setFillColor(sf::Color::Magenta);
        reset();
        
    }
    void reset()
    {
        shape.setPosition(sf::Vector2f(window.getSize().x / 2, window.getSize().y - 50));
        dx = 0;
    }

    void handleWallCollision()
    {
        sf::Vector2f pos = shape.getPosition();
        sf::Vector2u windowSize = window.getSize();

        if (pos.x + shape.getSize().x > windowSize.x)
        {
            dx = 0;
            shape.setPosition(sf::Vector2f(windowSize.x - shape.getSize().x, pos.y));
        }
        else if (pos.x < 0)
        {
            dx = 0;
            shape.setPosition(sf::Vector2f(0, pos.y));
        }
    }
    void onKeyPressed(sf::Event::KeyPressed const *key)
    {
        if (key->code == sf::Keyboard::Key::Left)
        {
            dx = -0.05;
        }
        else if (key->code == sf::Keyboard::Key::Right)
        {
            dx = 0.05;
        }
    }
    void onKeyReleased(sf::Event::KeyReleased const *key)
    {
        dx = 0;
    }
    void draw()
    {
        handleWallCollision();
        shape.setPosition(sf::Vector2f(shape.getPosition().x + dx, shape.getPosition().y));
        window.draw(shape);
    }
};

class Ball
{
public:
    sf::RenderWindow &window;
    sf::RectangleShape shape = sf::RectangleShape(sf::Vector2f(15, 15));
    float dx;
    float dy;

    Ball(sf::RenderWindow &window) : window(window)
    {
        shape.setFillColor(sf::Color::Red);
        reset();
    }

    void handleWallCollision()
    {
        sf::Vector2f pos = shape.getPosition();
        sf::Vector2u windowSize = window.getSize();

        if (pos.x + shape.getSize().x > windowSize.x || pos.x < 0)
            dx = -dx;
        if (pos.y + shape.getSize().y > windowSize.y || pos.y < 0)
            dy = -dy;
    }

    void reset()
    {
        shape.setPosition(sf::Vector2f(window.getSize().x / 2, window.getSize().y / 2));
        dx = 0.05;
        dy = 0.05;
    }

    void draw()
    {
        handleWallCollision();
        sf::Vector2f pos = shape.getPosition();
        shape.setPosition(sf::Vector2f(pos.x += dx, pos.y += dy));
        window.draw(shape);
    }
};

class Brick
{
public:
    sf::RenderWindow &window;
    sf::RectangleShape shape = sf::RectangleShape(sf::Vector2f(140, 40));
    bool shouldRender = true;

    Brick(sf::RenderWindow &window, float x, float y) : window(window)
    {
        shape.setFillColor(sf::Color::Blue);
        shape.setPosition(sf::Vector2f(x, y));
    };
    void reset()
    {
        shouldRender = true;
    }
    void draw()
    {
        if (shouldRender)
        {
            window.draw(shape);
        }
    }
};

std::vector<Brick> createBricks(sf::RenderWindow &window)
{
    std::vector<Brick> bricks;
    bricks.reserve(24);
    float currentBrickX = 10;
    float currentBrickY = 10;
    for (int y = 0; y < 4; y++)
    {
        for (int x = 0; x < 6; x++)
        {
            bricks.push_back(Brick(window, currentBrickX, currentBrickY));
            currentBrickX += 170;
        }
        currentBrickY += 70;
        currentBrickX = 10;
    }
    return bricks;
}


int main()
{
    sf::RenderWindow window(sf::VideoMode({1010, 800}), "C++ Breakout");

    sf::Font arial("/usr/share/fonts/open-sans/OpenSans-Regular.ttf");
    sf::Text gameOverText(arial);
    gameOverText.setString("Game Over!");
    sf::Vector2f center = gameOverText.getGlobalBounds().size / 2.f;
    sf::Vector2f localBounds = center + gameOverText.getLocalBounds().position;
    gameOverText.setOrigin(localBounds);
    gameOverText.setPosition(sf::Vector2f{window.getSize() / 2u});

    sf::Text scoreText(arial);
    scoreText.setString("0");
    scoreText.setPosition(sf::Vector2f(20, window.getSize().y-50));

    int score = 0;

    std::vector<Brick> bricks = createBricks(window);

    Paddle paddle(window);
    Ball ball(window);

    while (window.isOpen())
    {
        while (const std::optional event = window.pollEvent())
        {
            if (event->is<sf::Event::Closed>())
            {
                window.close();
            }
            else if (const auto *key = event->getIf<sf::Event::KeyPressed>())
            {
                paddle.onKeyPressed(key);
                if (key->code == sf::Keyboard::Key::R)
                {
                    ball.reset();
                    paddle.reset();
                    for (auto &brick : bricks)
                    {
                        brick.reset();
                    }
                    score = 0;
                    scoreText.setString(std::to_string(score));
                }
            }
            else if (const auto *key = event->getIf<sf::Event::KeyReleased>())
            {
                paddle.onKeyReleased(key);
            }
            else if (const auto *resized = event->getIf<sf::Event::Resized>())
            {
                // update the view to the new size of the window
                sf::FloatRect visibleArea({0.f, 0.f}, sf::Vector2f(resized->size));
                window.setView(sf::View(visibleArea));
            }
        }
        window.clear(sf::Color::Black);

        sf::Vector2f ballPos = ball.shape.getPosition();
        sf::Vector2f paddlePos = paddle.shape.getPosition();
        if (ballPos.y > paddlePos.y - paddle.shape.getSize().y && ballPos.x + ball.shape.getSize().x > paddlePos.x &&
            ballPos.x < paddlePos.x + paddle.shape.getSize().x)
        {
            ball.dy = -ball.dy;
        }

        if (ballPos.y > paddlePos.y)
        {
            window.draw(gameOverText);
            ball.dx = 0;
            ball.dy = 0;
        }

        ball.draw();
        paddle.draw();
        for (auto &brick : bricks)
        {
            brick.draw();

            sf::Vector2f brickPos = brick.shape.getPosition();
            if (ballPos.y > brickPos.y - brick.shape.getSize().y && ballPos.x + ball.shape.getSize().x > brickPos.x &&
                ballPos.x < brickPos.x + brick.shape.getSize().x && ballPos.y < brickPos.y)
            {
                if (brick.shouldRender) {
                    brick.shouldRender = false;
                    score++;
                    scoreText.setString(std::to_string(score));
                }
            }
        }
        window.draw(scoreText);

        window.display();
    }
}