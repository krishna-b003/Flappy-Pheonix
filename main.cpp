
#include <SFML/Graphics.hpp>
#include <optional>
#include <cstdlib>
#include <vector>
#include <algorithm>
#include <cstdint>
#include <fstream>
#include <string>

// ---------------- COLLISION ----------------

bool overlaps(const sf::FloatRect& a, const sf::FloatRect& b) {
    return a.position.x < b.position.x + b.size.x &&
           a.position.x + a.size.x > b.position.x &&
           a.position.y < b.position.y + b.size.y &&
           a.position.y + a.size.y > b.position.y;
}

// ---------------- VILLAIN ----------------

struct Villain {
    sf::CircleShape body{16.f};
    sf::CircleShape leftEye{3.5f};
    sf::CircleShape rightEye{3.5f};
    sf::ConvexShape mouth{3};

    bool alive = true;
    bool dying = false;
    float deathTimer = 0.f;

    Villain() {
        body.setOrigin({16.f, 16.f});
        leftEye.setOrigin({3.5f, 3.5f});
        rightEye.setOrigin({3.5f, 3.5f});

        body.setFillColor(sf::Color(90, 30, 110));
        body.setOutlineThickness(2.f);
        body.setOutlineColor(sf::Color(45, 15, 65));

        leftEye.setFillColor(sf::Color(255, 35, 35));
        rightEye.setFillColor(sf::Color(255, 35, 35));

        // Vicious grin
        mouth.setPoint(0, {0.f, 0.f});
        mouth.setPoint(1, {14.f, 0.f});
        mouth.setPoint(2, {7.f, 7.f});
        mouth.setFillColor(sf::Color(255, 225, 190));
    }
};

// ---------------- PHOENIX DESIGN ----------------

void drawPhoenix(sf::RenderWindow& window,
                 const sf::CircleShape& hitbox,
                 float velocity) {
    sf::Vector2f p = hitbox.getPosition();
    float angle = std::clamp(velocity * 0.045f, -25.f, 45.f);

    // Tail feathers
    for (int i = 0; i < 3; i++) {
        sf::ConvexShape feather(3);
        feather.setPoint(0, {0.f, 0.f});
        feather.setPoint(1, {-20.f - i * 3.f, -5.f + i * 7.f});
        feather.setPoint(2, {-9.f, 5.f + i * 4.f});
        feather.setPosition({p.x + 12.f, p.y + 18.f});
        feather.setFillColor(
            i == 0 ? sf::Color(255, 60, 20) :
            i == 1 ? sf::Color(255, 135, 15) :
                     sf::Color(255, 205, 40)
        );
        feather.setRotation(sf::degrees(angle));
        window.draw(feather);
    }

    // Back wing
    sf::ConvexShape backWing(4);
    backWing.setPoint(0, {0.f, 0.f});
    backWing.setPoint(1, {-13.f, -24.f});
    backWing.setPoint(2, {5.f, -15.f});
    backWing.setPoint(3, {13.f, 2.f});
    backWing.setPosition({p.x + 15.f, p.y + 17.f});
    backWing.setFillColor(sf::Color(220, 65, 15));
    backWing.setRotation(sf::degrees(angle));
    window.draw(backWing);

    // Body
    sf::CircleShape body(17.f);
    body.setOrigin({17.f, 17.f});
    body.setScale({1.2f, 0.85f});
    body.setPosition({p.x + 18.f, p.y + 18.f});
    body.setFillColor(sf::Color(255, 135, 20));
    window.draw(body);

    // Golden chest
    sf::CircleShape chest(10.f);
    chest.setOrigin({10.f, 10.f});
    chest.setScale({0.8f, 1.f});
    chest.setPosition({p.x + 23.f, p.y + 23.f});
    chest.setFillColor(sf::Color(255, 210, 65));
    window.draw(chest);

    // Head
    sf::CircleShape head(10.f);
    head.setOrigin({10.f, 10.f});
    head.setPosition({p.x + 31.f, p.y + 10.f});
    head.setFillColor(sf::Color(255, 175, 25));
    window.draw(head);

    // Beak
    sf::ConvexShape beak(3);
    beak.setPoint(0, {0.f, 0.f});
    beak.setPoint(1, {15.f, 4.f});
    beak.setPoint(2, {1.f, 9.f});
    beak.setPosition({p.x + 36.f, p.y + 8.f});
    beak.setFillColor(sf::Color(255, 65, 10));
    window.draw(beak);

    // Eye and pupil
    sf::CircleShape eye(3.f);
    eye.setPosition({p.x + 29.f, p.y + 6.f});
    eye.setFillColor(sf::Color::White);
    window.draw(eye);

    sf::CircleShape pupil(1.5f);
    pupil.setPosition({p.x + 31.f, p.y + 7.f});
    pupil.setFillColor(sf::Color::Black);
    window.draw(pupil);

    // Front wing
    sf::ConvexShape wing(4);
    wing.setPoint(0, {0.f, 0.f});
    wing.setPoint(1, {-8.f, -23.f});
    wing.setPoint(2, {9.f, -17.f});
    wing.setPoint(3, {14.f, 3.f});
    wing.setPosition({p.x + 16.f, p.y + 17.f});
    wing.setFillColor(sf::Color(255, 90, 15));
    wing.setRotation(sf::degrees(angle + 12.f));
    window.draw(wing);
}

// ---------------- PILLARS ----------------

void drawPillar(sf::RenderWindow& window,
                const sf::RectangleShape& pillar,
                bool isUpper) {
    sf::Vector2f p = pillar.getPosition();
    sf::Vector2f size = pillar.getSize();

    sf::RectangleShape stone(size);
    stone.setPosition(p);
    stone.setFillColor(sf::Color(74, 133, 83));
    window.draw(stone);

    sf::RectangleShape shade({12.f, size.y});
    shade.setPosition({p.x + size.x - 12.f, p.y});
    shade.setFillColor(sf::Color(40, 94, 58));
    window.draw(shade);

    for (float y = 8.f; y < size.y; y += 28.f) {
        sf::RectangleShape seam({size.x - 12.f, 2.f});
        seam.setPosition({p.x, p.y + y});
        seam.setFillColor(sf::Color(47, 105, 61));
        window.draw(seam);

        sf::RectangleShape smallSeam({2.f, 13.f});
        smallSeam.setPosition({p.x + 27.f, p.y + y + 2.f});
        smallSeam.setFillColor(sf::Color(47, 105, 61));
        window.draw(smallSeam);
    }

    sf::RectangleShape cap({size.x + 12.f, 13.f});
    cap.setPosition({
        p.x - 6.f,
        isUpper ? size.y - 13.f : p.y
    });
    cap.setFillColor(sf::Color(125, 155, 113));
    window.draw(cap);
}

// ---------------- DIGIT DISPLAY ----------------

void drawDigit(sf::RenderWindow& window, int digit,
               float x, float y, float scale,
               sf::Color color) {
    const int patterns[10] = {
        0b1111110, 0b0110000, 0b1101101,
        0b1111001, 0b0110011, 0b1011011,
        0b1011111, 0b1110000, 0b1111111,
        0b1111011
    };

    const int segments[7] = {
        0b1000000, 0b0100000, 0b0010000,
        0b0001000, 0b0000100, 0b0000010,
        0b0000001
    };

    const sf::Vector2f positions[7] = {
        {3.f, 0.f}, {16.f, 3.f}, {16.f, 18.f},
        {3.f, 32.f}, {0.f, 18.f}, {0.f, 3.f},
        {3.f, 16.f}
    };

    const sf::Vector2f sizes[7] = {
        {10.f, 3.f}, {3.f, 13.f}, {3.f, 13.f},
        {10.f, 3.f}, {3.f, 13.f}, {3.f, 13.f},
        {10.f, 3.f}
    };

    for (int i = 0; i < 7; i++) {
        if (patterns[digit] & segments[i]) {
            sf::RectangleShape segment({
                sizes[i].x * scale,
                sizes[i].y * scale
            });

            segment.setPosition({
                x + positions[i].x * scale,
                y + positions[i].y * scale
            });

            segment.setFillColor(color);
            window.draw(segment);
        }
    }
}

void drawNumber(sf::RenderWindow& window, int number,
                float x, float y, float scale,
                sf::Color color) {
    std::string digits = std::to_string(number);

    for (char c : digits) {
        drawDigit(window, c - '0', x, y, scale, color);
        x += 23.f * scale;
    }
}

// ---------------- MAIN ----------------

int main() {
    constexpr float W = 900.f;
    constexpr float H = 600.f;

    sf::RenderWindow window(
        sf::VideoMode({900, 600}),
        "Flappy Phoenix"
    );

    window.setFramerateLimit(60);

    sf::CircleShape phoenix(18.f);
    phoenix.setFillColor(sf::Color::Transparent);
    phoenix.setPosition({150.f, 250.f});

    float velocity = 0.f;
    const float gravity = 1000.f;
    const float flapStrength = -380.f;

    const float pillarWidth = 65.f;
    const float gapHeight = 220.f;
    const float pillarSpeed = 220.f;

    float pillarX = 650.f;
    float gapY = 190.f;
    bool scoredThisPillar = false;

    sf::RectangleShape upperPillar;
    sf::RectangleShape lowerPillar;

    auto updatePillars = [&]() {
        upperPillar.setSize({pillarWidth, gapY});
        lowerPillar.setSize({
            pillarWidth, H - gapY - gapHeight
        });

        upperPillar.setPosition({pillarX, 0.f});
        lowerPillar.setPosition({
            pillarX, gapY + gapHeight
        });
    };

    updatePillars();

    std::vector<sf::CircleShape> fireballs;
    const float fireSpeed = 700.f;

    Villain villain;

    // FIX: Consistent body-centered positioning for all facial features.
    auto positionVillain = [&]() {
        float x = pillarX - 10.f;
        float y = gapY + gapHeight / 2.f;

        villain.body.setOrigin({16.f, 16.f});
        villain.body.setPosition({x, y});

        villain.leftEye.setOrigin({3.5f, 3.5f});
        villain.rightEye.setOrigin({3.5f, 3.5f});

        villain.leftEye.setPosition({x - 5.f, y - 4.f});
        villain.rightEye.setPosition({x + 5.f, y - 4.f});

        villain.mouth.setPosition({x - 7.f, y + 3.f});
    };

    struct Cloud {
        sf::Vector2f position;
        float speed;
        float size;
    };

    std::vector<Cloud> clouds = {
        {{100.f, 90.f}, 22.f, 1.f},
        {{390.f, 155.f}, 15.f, 0.8f},
        {{700.f, 65.f}, 27.f, 1.2f},
        {{820.f, 300.f}, 18.f, 0.9f}
    };

    auto drawCloud = [&](const Cloud& cloud) {
        auto puff = [&](float dx, float dy, float radius) {
            sf::CircleShape c(radius * cloud.size);
            c.setPosition({
                cloud.position.x + dx * cloud.size,
                cloud.position.y + dy * cloud.size
            });
            c.setFillColor(sf::Color(255, 255, 255, 220));
            window.draw(c);
        };

        puff(0.f, 9.f, 15.f);
        puff(17.f, 0.f, 20.f);
        puff(39.f, 8.f, 16.f);
        puff(20.f, 13.f, 18.f);
    };

    int score = 0;
    int highScore = 0;

    {
        std::ifstream file("highscore.txt");
        if (file >> highScore) {
            highScore = std::max(0, highScore);
        }
    }

    auto saveHighScore = [&]() {
        std::ofstream file("highscore.txt");
        if (file) {
            file << highScore;
        }
    };

    bool gameOver = false;

    auto resetGame = [&]() {
        phoenix.setPosition({150.f, 250.f});
        velocity = 0.f;

        pillarX = 650.f;
        gapY = 190.f;
        scoredThisPillar = false;
        score = 0;

        fireballs.clear();

        villain.alive = true;
        villain.dying = false;
        villain.deathTimer = 0.f;
        villain.body.setScale({1.f, 1.f});
        villain.leftEye.setScale({1.f, 1.f});
        villain.rightEye.setScale({1.f, 1.f});

        villain.body.setFillColor(sf::Color(90, 30, 110));
        villain.leftEye.setFillColor(sf::Color(255, 35, 35));
        villain.rightEye.setFillColor(sf::Color(255, 35, 35));

        gameOver = false;
        updatePillars();
        positionVillain();

        window.setTitle(
            "Flappy Phoenix | Score: 0 | Best: " +
            std::to_string(highScore)
        );
    };

    sf::Clock clock;

    while (window.isOpen()) {
        float dt = std::min(
            clock.restart().asSeconds(), 0.05f
        );

        while (const std::optional event = window.pollEvent()) {
            if (event->is<sf::Event::Closed>()) {
                window.close();
            }

            if (const auto* key =
                    event->getIf<sf::Event::KeyPressed>()) {

                if (key->code == sf::Keyboard::Key::Escape) {
                    window.close();
                }

                if (gameOver &&
                    key->code == sf::Keyboard::Key::R) {
                    resetGame();
                    continue;
                }

                if (!gameOver) {
                    if (key->code == sf::Keyboard::Key::Space) {
                        velocity = flapStrength;
                    }

                    if (key->code == sf::Keyboard::Key::F) {
                        sf::CircleShape fireball(7.f);
                        fireball.setFillColor(
                            sf::Color(255, 75, 15)
                        );

                        fireball.setPosition({
                            phoenix.getPosition().x + 30.f,
                            phoenix.getPosition().y + 10.f
                        });

                        fireballs.push_back(fireball);
                    }
                }
            }
        }

        if (!gameOver) {
            velocity += gravity * dt;
            phoenix.move({0.f, velocity * dt});

            pillarX -= pillarSpeed * dt;

            if (!scoredThisPillar &&
                pillarX + pillarWidth < phoenix.getPosition().x) {
                score++;
                scoredThisPillar = true;

                if (score > highScore) {
                    highScore = score;
                    saveHighScore();
                }

                window.setTitle(
                    "Flappy Phoenix | Score: " +
                    std::to_string(score) +
                    " | Best: " + std::to_string(highScore)
                );
            }

            if (pillarX + pillarWidth < 0.f) {
                pillarX = W;
                gapY = 100.f +
                    static_cast<float>(std::rand() % 230);

                scoredThisPillar = false;

                villain.alive = true;
                villain.dying = false;
                villain.deathTimer = 0.f;

                villain.body.setScale({1.f, 1.f});
                villain.leftEye.setScale({1.f, 1.f});
                villain.rightEye.setScale({1.f, 1.f});

                villain.body.setFillColor(sf::Color(90, 30, 110));
                villain.leftEye.setFillColor(sf::Color(255, 35, 35));
                villain.rightEye.setFillColor(sf::Color(255, 35, 35));
            }

            updatePillars();
            positionVillain();

            for (auto it = fireballs.begin();
                 it != fireballs.end();) {

                it->move({fireSpeed * dt, 0.f});
                bool hitVillain = false;

                if (villain.alive &&
                    overlaps(
                        it->getGlobalBounds(),
                        villain.body.getGlobalBounds()
                    )) {

                    villain.alive = false;
                    villain.dying = true;
                    villain.deathTimer = 0.f;
                    hitVillain = true;
                }

                if (hitVillain || it->getPosition().x > W) {
                    it = fireballs.erase(it);
                } else {
                    ++it;
                }
            }

            if (villain.dying) {
                villain.deathTimer += dt;

                float progress = std::min(
                    villain.deathTimer / 0.30f, 1.f
                );

                float scale = 1.f - progress;
                villain.body.setScale({scale, scale});
                villain.leftEye.setScale({scale, scale});
                villain.rightEye.setScale({scale, scale});

                auto alpha = static_cast<std::uint8_t>(
                    255.f * (1.f - progress)
                );

                villain.body.setFillColor(
                    sf::Color(90, 30, 110, alpha)
                );
                villain.leftEye.setFillColor(
                    sf::Color(255, 35, 35, alpha)
                );
                villain.rightEye.setFillColor(
                    sf::Color(255, 35, 35, alpha)
                );

                if (progress >= 1.f) {
                    villain.dying = false;
                }
            }

            sf::FloatRect birdBounds =
                phoenix.getGlobalBounds();

            if (birdBounds.position.y <= 0.f ||
                birdBounds.position.y + birdBounds.size.y >= H) {
                gameOver = true;
            }

            if (!gameOver &&
                (overlaps(
                    birdBounds, upperPillar.getGlobalBounds()
                ) ||
                overlaps(
                    birdBounds, lowerPillar.getGlobalBounds()
                ))) {
                gameOver = true;
            }

            if (!gameOver && villain.alive &&
                overlaps(
                    birdBounds, villain.body.getGlobalBounds()
                )) {
                gameOver = true;
            }

            if (gameOver) {
                saveHighScore();

                window.setTitle(
                    "GAME OVER | Score: " +
                    std::to_string(score) +
                    " | Best: " + std::to_string(highScore) +
                    " | Press R to Restart"
                );
            }

            for (auto& cloud : clouds) {
                cloud.position.x -= cloud.speed * dt;

                if (cloud.position.x < -100.f) {
                    cloud.position.x = W + 30.f;
                    cloud.position.y =
                        35.f + static_cast<float>(std::rand() % 300);
                }
            }
        }

        // ---------------- DRAW ----------------

        window.clear(sf::Color(115, 195, 240));

        for (const auto& cloud : clouds) {
            drawCloud(cloud);
        }

        drawPillar(window, upperPillar, true);
        drawPillar(window, lowerPillar, false);

        for (const auto& fireball : fireballs) {
            sf::CircleShape glow(10.f);
            glow.setPosition({
                fireball.getPosition().x - 3.f,
                fireball.getPosition().y - 3.f
            });
            glow.setFillColor(sf::Color(255, 130, 20, 100));
            window.draw(glow);
            window.draw(fireball);

            sf::CircleShape core(3.f);
            core.setPosition({
                fireball.getPosition().x + 4.f,
                fireball.getPosition().y + 4.f
            });
            core.setFillColor(sf::Color(255, 240, 100));
            window.draw(core);
        }

        // FIXED VILLAIN RENDERING:
        // Draw the circle and facial features while alive.
        // During death, draw the fading body and eyes.
        if (villain.alive || villain.dying) {
            window.draw(villain.body);

            if (villain.alive) {
                window.draw(villain.leftEye);
                window.draw(villain.rightEye);
                window.draw(villain.mouth);
            } else if (villain.dying) {
                window.draw(villain.leftEye);
                window.draw(villain.rightEye);
            }
        }

        drawPhoenix(window, phoenix, velocity);

        drawNumber(
            window, score, 25.f, 25.f, 1.2f,
            sf::Color::White
        );

        drawNumber(
            window, highScore, 740.f, 25.f, 0.8f,
            sf::Color(255, 235, 100)
        );

        if (gameOver) {
            sf::RectangleShape overlay({W, H});
            overlay.setFillColor(sf::Color(100, 0, 0, 100));
            window.draw(overlay);
        }

        window.display();
    }

    saveHighScore();
    return 0;
}
