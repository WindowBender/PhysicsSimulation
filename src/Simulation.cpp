#include "../include/Simulation.h"
#include "SFML/Graphics/CircleShape.hpp"
#include "SFML/Graphics/Color.hpp"
#include "SFML/Graphics/RectangleShape.hpp"
#include "SFML/Graphics/RenderTarget.hpp"
#include "SFML/System/Vector2.hpp"
#include <__utility/integer_sequence.h>
#include <cstddef>
#include <iostream>
#include <iterator>
#include <sstream>
#include <thread>
#include <vector>


Simulation::~Simulation()
{
}

Simulation::Simulation()
{


    m_objects.reserve(10000);
    initText();
    m_mouseColShape.setRadius(m_mouseColRad);
    m_mouseColShape.setPointCount(50);
    m_mouseColShape.setFillColor(sf::Color::Transparent);
    m_mouseColShape.setOutlineThickness(1);
    m_mouseColShape.setOutlineColor(sf::Color::White);
    
}

const void Simulation::setWindow( sf::RenderWindow& window )
{
    m_window = &window;
}
const void Simulation::setSubSteps( int substeps )
{
    m_subStepNumber = substeps;
}
const float Simulation::getSubDeltaTime( ) const
{
    return m_deltaTime / static_cast<float>(m_subStepNumber);
}
const int Simulation::getSubSteps( ) const
{
    return m_subStepNumber;
}

const float Simulation::getTime() const
{
    return m_time;
}

Object& Simulation::addNewObject( sf::Vector2f startPos, float r, bool pinned )
{
    return m_objects.emplace_back(startPos, r, m_objects.size(), pinned); 
}

void Simulation::initText()
{
    if(!m_font.loadFromFile("./res/fonts/open-sans/OpenSans-Semibold.ttf"))
    {
        std::cout << "ERROR LOADING FILE IN SIMULATION CLASS" << '\n';
    }

    m_debugText.setFillColor(sf::Color::White);
    m_debugText.setFont(m_font);
    m_debugText.setCharacterSize(20);
    m_debugText.setPosition(10,10);
    m_debugText.setString("NULL");
}


void Simulation::setDeltaTime()
{
     m_deltaTime = m_deltaTimeClock.restart().asSeconds() * MULT;
}
// UPDATING
void Simulation::updateText()
{
    std::stringstream ss;
    ss 
        << "SIM TIME: " << m_simUpdateClock.restart().asMilliseconds() << "ms" << '\n'
        << "BALLS: " << m_objects.size() << '\n'
        << "GRAVITY: " << m_gravityActive << '\n';
        ;
    m_debugText.setString(ss.str());
}

void Simulation::getInput()
{
    if(InputHandler::isLeftMouseClicked())
    {
        if(mouseHoveringBall())
            m_grabbingBall = true;

    }
    else {
        if(m_grabbingBall)
        {
            for(auto &obj : m_objects)
            {
                if(obj.isGrabbed)
                {
                    obj.isGrabbed = false;
                    obj.outlineThic = 0;
                    // this prevents the velocity shooting the ball when paused and grabbing a ball
                    if(!m_paused)
                        obj.addVelocity(m_mouseVelocity, getSubDeltaTime());
                    else
                        obj.setVelocity(sf::Vector2f(0,0), getSubDeltaTime());

                }

            }

        }
        m_grabbingBall = false;
    }

    if(InputHandler::isRightMouseClicked())
        m_mouseColActive = true;
    else
        m_mouseColActive = false;


    if(InputHandler::isCClicked())
    {
        if(!m_isKeyHeld)
        {
            m_isKeyHeld = true;
            m_objects.clear();
        }

    }
    else if(InputHandler::isSpaceClicked())
    {
        if(!m_isKeyHeld)
        {
            m_isKeyHeld = true;
            for(auto& obj : m_objects)
                obj.setVelocity({0,0}, getSubDeltaTime());
            m_paused = !m_paused;
        }
    }
    else if(InputHandler::isGClicked())
    {
        if(!m_isKeyHeld)
        {
            m_isKeyHeld = true;
            m_gravityActive = !m_gravityActive;
        }
    }
    else{
        m_isKeyHeld = false;
    }

    if(InputHandler::isQClicked())
    {
        if(!m_isMouseHeld && m_grabbingBall)
        {
            m_isMouseHeld = true;
            for(auto &obj : m_objects)
            {
                if(obj.isGrabbed)
                    obj.togglePinned();

            }
        }
    }
    else{
        m_isMouseHeld = false;
    }

}

void Simulation::changeMouseRadius( float change )
{
    if(m_mouseColActive)
    {
        m_mouseColRad += change;
        if(m_mouseColRad < m_mouseColMinRad)
            m_mouseColRad = m_mouseColMinRad;

        if(m_mouseColRad > m_mouseColMaxRad)
            m_mouseColRad = m_mouseColMaxRad;

    }
    else if(m_grabbingBall){
        for(auto &obj : m_objects)
        {
            if(obj.isGrabbed)
            {
                obj.radius += change;
                if(obj.radius < m_mouseColMinRad)
                    obj.radius = m_mouseColMinRad;

                if(obj.radius > m_mouseColMaxRad)
                    obj.radius = m_mouseColMaxRad;
            }
        }
    }

}


void Simulation::updateMousePos()
{
    m_mousePosView = m_window->mapPixelToCoords(sf::Mouse::getPosition(*m_window));
}

void Simulation::calcMouseVelocity()
{
    // useful so that when grabbing a ball, then letting go, depending on how you throw the mouse, depends on how the ball is thrown
    m_mouseVelocity = m_mousePosView - m_mouseOldPos;
    m_mouseOldPos = m_mousePosView;
}

void Simulation::startSim( )
{
    m_updateThread = std::thread(&Simulation::simulate, this);
}

void Simulation::simulate( )
{
    while(m_window->isOpen())
    {
            m_time+= m_deltaTime;
            updateText();
            setDeltaTime();
            float subStepDT = getSubDeltaTime();
            for(int i{getSubSteps()}; i > 0; --i)
            {
                if(m_window->hasFocus() && !m_paused)
                {
                    if(m_gravityActive)
                        applyGravityToObjects();
                    demoSpawner();
                    updateObjects( subStepDT );

                }
                checkConstraints();
                checkCollisions();
                ballGrabbedMovement();
            }


    }



}

void Simulation::updateUI()
{
    if(m_window->hasFocus())
    {
        getInput();
    }
    updateMousePos();
    calcMouseVelocity();
    
}

void Simulation::joinUpdateThread()
{
    m_updateThread.join();
}

bool Simulation::mouseHoveringBall()
{
    for(auto &obj: m_objects)
    {
        sf::Vector2f axis = m_mousePosView - obj.currentPos;
        float dist = sqrt(axis.x * axis.x + axis.y * axis.y);
        
        if(dist < obj.radius && !m_grabbingBall)
        {
            obj.isGrabbed = true;
            obj.outlineThic = 1;
            return true;
        }
    }

    return false;
}

void Simulation::ballGrabbedMovement()
{
    for(auto &obj : m_objects)
    {
        if(obj.isGrabbed)
        {
            if(obj.isPinned)
                obj.outlineColor = sf::Color::Green;
            else
                obj.outlineColor = sf::Color::White;
            obj.outlineThic = 1;
            obj.currentPos = m_mousePosView;
        }
    }
}

sf::Color Simulation::getRainbowColors( float time )
{
    float red = sin(time);
    float green = sin(time + 0.3f * 2);
    float blue = sin(time + 0.6f * 2.0);


    return sf::Color( 
        static_cast<unsigned>(red * red * 255),
        static_cast<unsigned>(green * green * 255),
        static_cast<unsigned>(blue * blue * 255)
            );
}

void Simulation::demoSpawner()
{
    sf::Vector2f spawnPos = {m_window->getSize().x * 0.5f, m_window->getSize().y * 0.25f};
    int maxBalls = 650;
    float spawnDelay = 0.05f;
    float spawnSpeed = 40;
    int minRad = 4;
    int maxRad = 14;

    if(m_objects.size() < maxBalls && m_clock.getElapsedTime().asSeconds() >= spawnDelay)
    {
        m_clock.restart().asSeconds();
        Object& ob = addNewObject(spawnPos, (rand() % maxRad) + minRad);

        float time = getTime();
        
        float angle =  time * 3.1415936 * 0.05;

        ob.addVelocity( spawnSpeed * sf::Vector2f(cos(angle), sin(angle)), getSubDeltaTime());
        ob.color = getRainbowColors(time);

    }

}

void Simulation::checkConstraints()
{
    for(auto &obj : m_objects)
    {
        float winWidth = m_window->getSize().x;
        float winHeight = m_window->getSize().y;
        sf::Vector2f veloc = obj.currentPos - obj.oldPos;
        if(obj.currentPos.x > winWidth - obj.radius)
        {
            obj.currentPos.x = winWidth - obj.radius;
            //obj.oldPos.x = obj.currentPos.x + veloc.x * obj.friction;
        }
        if(obj.currentPos.x < obj.radius)
        {
            obj.currentPos.x = obj.radius;
            // obj.oldPos.x = obj.currentPos.x + veloc.x * obj.friction;
        }
        if(obj.currentPos.y < obj.radius)
        {
            obj.currentPos.y = obj.radius;
            // obj.oldPos.y = obj.currentPos.y + veloc.y * obj.friction;
        }
        if(obj.currentPos.y > winHeight - obj.radius)
        {
            obj.currentPos.y = winHeight - obj.radius;
            // obj.oldPos.y = obj.currentPos.y + veloc.y * obj.friction;
        }
    }
}

void Simulation::checkCollisions()
{

    for(std::size_t i = 0; i < m_objects.size(); ++i)
    {
        Object& obj1 = m_objects[i];
        for(std::size_t j = i + 1; j < m_objects.size(); ++j)
        {
            Object& obj2 = m_objects[j];
            sf::Vector2f axis = obj1.currentPos - obj2.currentPos;
            float distanceBtw = sqrt(axis.x * axis.x + axis.y * axis.y);
            float minAllowedDist = obj1.radius + obj2.radius;
            if(distanceBtw < minAllowedDist)
            {
                float moveAmount = minAllowedDist - distanceBtw;
                float percentage = (moveAmount / distanceBtw) * 0.5;
                sf::Vector2f offsetAmount = axis * percentage;

                if(!obj1.isPinned)
                    obj1.currentPos += offsetAmount;
                if(!obj2.isPinned)
                    obj2.currentPos -= offsetAmount;
            }
        }
    }

    mouseCollisionsBall();
}

void Simulation::mouseCollisionsBall()
{
    if(m_mouseColActive)
    {
        for(auto &obj : m_objects)
        {
            sf::Vector2f axis = m_mousePosView - obj.currentPos;
            float dist = sqrt(axis.x * axis.x + axis.y * axis.y);
            float minDist = m_mouseColRad + obj.radius;
            if(dist < minDist)
            {
                if(!obj.isPinned)
                {
                    float moveAmount = minDist - dist;
                    float perc = (moveAmount / dist) * 0.5;
                    sf::Vector2f off = axis * perc;
                    obj.currentPos -= off;
                }
            }

        }
    }
}

void Simulation::updateObjects( float subDeltaTime )
{
    for( auto &obj : m_objects)
    {
        if(!obj.isPinned)
            obj.update(subDeltaTime);
    }
}

void Simulation::applyGravityToObjects( )
{
    if(m_gravityActive)
    {
        for(auto &obj : m_objects)
        {
            obj.accelerate( obj.mass * GRAVITY);
        }

    }
}


// RENDERING
void Simulation::render( sf::RenderTarget &target )
{

    sf::CircleShape circleS;
    circleS.setPointCount(50);
    for(auto &obj : m_objects)
    {
        circleS.setRadius(obj.radius);
        circleS.setOrigin(obj.radius, obj.radius);
        circleS.setFillColor(obj.color);
        circleS.setPosition(obj.currentPos);
        circleS.setOutlineColor(obj.outlineColor);
        circleS.setOutlineThickness(obj.outlineThic);

        target.draw(circleS);
    }

    if(m_mouseColActive)
    {
        float newRad = m_mouseColRad - m_mouseColShape.getOutlineThickness();
        m_mouseColShape.setRadius(newRad);
        m_mouseColShape.setOrigin(newRad, newRad);
        m_mouseColShape.setPosition(m_mousePosView);
        target.draw(m_mouseColShape);
    }
}

void Simulation::renderUI( sf::RenderTarget &target )
{
    target.draw(m_debugText);
}
