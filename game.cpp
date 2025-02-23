#include <SFML/Graphics.hpp>
#include <cmath>
#include <stdlib.h>
#include <stdio.h>

sf::Color colors[] = {sf::Color::Red, sf::Color::Blue, sf::Color::Magenta, sf::Color::Cyan, sf::Color::Green};

class Paddle
{
public:
    const float speed = 0.15;
    const int width = 170;
    const int height = 25;

    float dx;
    sf::RenderWindow &window;
    sf::RectangleShape shape = sf::RectangleShape(sf::Vector2f(width, height));

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
            dx = -speed;
        }
        else if (key->code == sf::Keyboard::Key::Right)
        {
            dx = speed;
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
    const float speed = 0.05;
    const int diameter = 20;

    float dx;
    float dy;
    sf::RenderWindow &window;
    sf::CircleShape shape = sf::CircleShape(diameter / 2);

    Ball(sf::RenderWindow &window) : window(window)
    {
        shape.setFillColor(sf::Color::Red);
        reset();
    }

    void handleWallCollision()
    {
        sf::Vector2f pos = shape.getPosition();
        sf::Vector2u windowSize = window.getSize();

        if (pos.x + diameter > windowSize.x || pos.x < 0)
            dx = -dx;
        if (pos.y + diameter > windowSize.y || pos.y < 0)
            dy = -dy;
    }

    void reset()
    {
        shape.setPosition(sf::Vector2f(window.getSize().x / 2, window.getSize().y / 2));
        dx = speed;
        dy = -speed;
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
    const int width = 140;
    const int height = 40;

    bool shouldRender = true;
    sf::RenderWindow &window;
    sf::RectangleShape shape = sf::RectangleShape(sf::Vector2f(width, height));

    Brick(sf::RenderWindow &window, float x, float y) : window(window)
    {
        shape.setFillColor(colors[rand() % (sizeof(colors) / sizeof(colors)[0])]);
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

    sf::Text winText = gameOverText;
    winText.setString("You win!");

    sf::Text scoreText(arial);
    scoreText.setString("0");
    scoreText.setPosition(sf::Vector2f(20, window.getSize().y - 50));

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
        if (ballPos.y > paddlePos.y - paddle.height && ballPos.x + ball.diameter > paddlePos.x &&
            ballPos.x < paddlePos.x + paddle.width && ballPos.y < paddlePos.y)
        {
            ball.dy = -ball.dy;
        }

    

        ball.draw();
        paddle.draw();

        int bricksRemoved = 0;
        for (auto &brick : bricks)
        {
            brick.draw();
            sf::Vector2f brickPos = brick.shape.getPosition();
            if (ballPos.y > brickPos.y - brick.height && ballPos.x + ball.diameter > brickPos.x &&
                ballPos.x < brickPos.x + brick.width && ballPos.y - ball.diameter < brickPos.y)
            {
                if (brick.shouldRender)
                {
                    brick.shouldRender = false;
                    score++;
                    scoreText.setString(std::to_string(score));

                    ball.dy = -ball.dy;
                }
            }
            if (!brick.shouldRender) bricksRemoved++;
        }

        if (ballPos.y > paddlePos.y)
        {
            window.draw(gameOverText);
            ball.dx = 0;
            ball.dy = 0;
        }
        if (bricksRemoved >= bricks.size()) {
            window.draw(winText);
            ball.dx = 0;
            ball.dy = 0;
        }

        window.draw(scoreText);

        window.display();
    }
}