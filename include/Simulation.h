#ifndef SIMULATION_H
#define SIMULATION_H
#include "SFML/Graphics/RenderWindow.hpp"
#include <SFML/Graphics.hpp>
#include <cstddef>
#include <vector>
#include <list>
#include <iostream>
#include <cmath>
#include "SFML/Graphics/Text.hpp"
#include <sstream>
#include "SFML/System/Vector2.hpp"
#include "Time.h"
#include "Object.h"
#pragma once

class Simulation
{
    private:
        sf::RenderWindow* m_window;

        int m_subStepNumber;
        float m_deltaTime;
        float m_subDeltaTime;
        float m_time;

        sf::Text m_debugText;
        sf::Font m_font;

        std::vector<Object> m_objects;
        std::vector<Object*> m_ptrObjects;

        const sf::Vector2f GRAVITY = { 0.f, 20.f };

        sf::Clock m_clock;



    private:
        void initText( );

        void updateObjects( float subDeltaTime );
        void updateText( );
        void applyGravityToObjects( );
        void checkConstraints( );
        void checkCollisions( );

        void demoSpawner( );

    public:

    public:
        Simulation();
        ~Simulation();

        void render( sf::RenderTarget& target );
        void renderUI( sf::RenderTarget& target );

        void update( float& deltaTime );

        Object& addNewObject( sf::Vector2f startPos, float r, bool pinned = false);

        const void setWindow( sf::RenderWindow& window );
        const void setSubSteps( int substeps );

        const float getSubDeltaTime( ) const;
        const int getSubSteps( ) const;
        const float getTime( ) const;


};

#endif // !DEBUG