#include<SFML/Graphics.hpp>

#include<SFML/Window.hpp>

#include <random>

#include<iostream>

#include<vector>

#include<assert.h>

#define PI 3.141592

float to180(float rot) { //360-type rotation to -180 180 mode rotation
    if (rot > 180) {
        rot -= 360;
    }
    return rot;
}

class Bird { // class for a bird (shape + vision cone)

private:
    sf::RectangleShape rshape; //bird shape
    sf::ConvexShape cone; //cone of vision

public:
    Bird(float x = 0, float y = 0, float rot = 0) { // initialize pos and rot for bird
        rshape = sf::RectangleShape(sf::Vector2f(7, 4));

        rshape.setPosition(sf::Vector2f(x, y));
        rshape.setRotation(rot);
        rshape.setFillColor(sf::Color::Red);
        rshape.setOrigin(sf::Vector2f(rshape.getLocalBounds().width / 2, rshape.getLocalBounds().height / 2));

        cone.setOrigin(rshape.getOrigin());

        cone.setPointCount(3);
        cone.setPoint(0, sf::Vector2f(5, 2));
        cone.setPoint(1, sf::Vector2f(5 + 17, 7));

        cone.setPoint(2, sf::Vector2f(5 + 17, -3));

        cone.setFillColor(sf::Color::Blue);
        cone.setPosition(sf::Vector2f(x, y));

    }

    sf::RectangleShape* getShape() {
        return &rshape;
    }

    //regular functions...
    float getRot() {
        return rshape.getRotation();
    }

    sf::Vector2f getPos() {
        return rshape.getPosition();
    }

    void draw(sf::RenderWindow& window) {
        window.draw(rshape);
        window.draw(cone);

    }

    void setRotVal(float rot) {
        rshape.setRotation(rot);
        cone.setRotation(rot);
    }

    //rotation adjustement
    float adjsustRot(float finalRot) {

        //test shortest path to adjust rot
        float error = finalRot - getRot(); //360

        if (error > 10 || error < -10) {
            return error / 20;
        }
        else {
            return error / -30;
        }

    }

    void move(float amount) { //calculate y and x movement according to rotation 
        float rot = getRot() * (PI / 180);
        float xMove = amount * cos(rot);
        float yMove = amount * sin(rot);

        rshape.move(sf::Vector2f(xMove, yMove));
        cone.move(sf::Vector2f(xMove, yMove));

    }

    float defaultBehaviorFunction(float x) {
        return pow(x, 3) / abs(x * 5 / x);
    }

    char touchScreen(sf::Vector2u size) {

        sf::Vector2f p1 = cone.getTransform().transformPoint(cone.getPoint(1));
        sf::Vector2f p2 = cone.getTransform().transformPoint(cone.getPoint(2));

        float rot = getRot();

        //x
        if (p1.x >= size.x || p2.x >= size.x) {
            if (rot <= 180) {
                return 'r';
            }
            else {
                return 'l';
            }
        }
        else if (p1.x <= 0 || p2.x <= 0) {
            if (rot >= 180) {
                return 'r';
            }
            else {
                return 'l';
            }
        }

        //y
        else if (p1.y >= size.y || p2.y >= size.y) {
            if (rot <= 90 || rot >= 270) {
                return 'l';
            }
            else {
                return 'r';
            }

            return 'r';
        }
        else if (p1.y <= 0 || p2.y <= 0) {
            if (rot <= 90 || rot >= 270) {
                return 'r';
            }
            else {
                return 'l';
            }
        }
        else {
            return '0';
        }

    }

    sf::FloatRect getGlobalBounds() {
        return rshape.getGlobalBounds();
    }

    Bird* detectOther(std::vector < Bird >& birbVec) {
        for (auto i = birbVec.begin(); i != birbVec.end(); i++) {
            if (cone.getGlobalBounds().intersects(i->getGlobalBounds()) && i->getShape() != &rshape)
                return &*i;

        }
        return nullptr;
    }

    void defaultBehavior(sf::Time& deltaTime, sf::Vector2u size, std::vector < Bird >& birbVec) {

        float newRot = defaultBehaviorFunction(((float)rand() / (RAND_MAX)) * 2 - 1);
        float newMove = defaultBehaviorFunction((float)rand() / (RAND_MAX) * 15 - 7.5) + 100;

        switch (touchScreen(size)) {
        case 'r':

            newRot = 10;
            break;
        case 'l':
            newRot = -10;
            break;
        default:
            break;
        }

        Bird* detected = detectOther(birbVec);
        if (detected != nullptr) {

            rshape.setFillColor(sf::Color::Green);
            newRot += adjsustRot(detected->getRot()) * (deltaTime.asMilliseconds());

        }
        else {
            rshape.setFillColor(sf::Color::Red);
        }

        setRotVal(getRot() + newRot);
        move(newMove * deltaTime.asSeconds());
    }
};

int main() {

    sf::Time deltaTime;
    sf::Clock clock;

    sf::RenderWindow window(sf::VideoMode(750, 750), "lézoizo cé rigolo");

    sf::Event ev;

    std::vector < Bird > birdList;

    for (int i = 0; i < 100; i++) {
        float x = ((float)rand() / (RAND_MAX)) * window.getSize().x;
        float y = ((float)rand() / (RAND_MAX)) * window.getSize().y;
        float rot = ((float)rand() / (RAND_MAX)) * 360;
        birdList.push_back(Bird(x, y, rot));
    }
    //birdList.push_back(Bird(15, 250, 0));
    //birdList.push_back(Bird(250, 250, 180));

    //Bird birb(15, 250, 45);
    //birb.setRotVal(0);

    float rot = 0;
    sf::Font arial;
    assert(arial.loadFromFile("font/arial.ttf"), "cant load font");

    sf::Text fps("FPS: 0", arial, 15);
    fps.setFillColor(sf::Color::White);

    while (window.isOpen()) {
        deltaTime = clock.restart();

        while (window.pollEvent(ev)) {
            if (ev.type == sf::Event::EventType::Closed) {
                window.close();
            }
        }

        window.clear(sf::Color::Black);

        for (auto i = birdList.begin(); i != birdList.end(); i++) {
            i->defaultBehavior(deltaTime, window.getSize(), birdList);
            i->draw(window);
        }

        fps.setString("fps " + std::to_string(1 / deltaTime.asSeconds()));
        window.draw(fps);

        window.display();
    }

}