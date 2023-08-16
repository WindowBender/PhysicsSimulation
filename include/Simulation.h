#ifndef SIMULATION_H
#define SIMULATION_H
#include "IDVector.h"
#include <_types/_uint8_t.h>
#pragma once
#include "Global.h"
#include "SFML/Graphics/CircleShape.hpp"
#include "SFML/Graphics/RenderWindow.hpp"
#include <SFML/Graphics.hpp>
#include <cstddef>
#include <mutex>
#include <thread>
#include <vector>
#include <list>
#include <iostream>
#include <cmath>
#include "SFML/Graphics/Text.hpp"
#include <sstream>
#include "SFML/System/Vector2.hpp"
#include "Time.h"
#include "Object.h"
#include "Grid.h"
#include "InputHandler.h"
#include "Stick.h"
#include "StickMaker.h"

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

        int m_ballRad = BALL_RADIUS;

        const sf::Vector2f GRAVITY = { 0.f, 20.f };

        sf::Clock m_clock;
        sf::Clock m_simUpdateClock;


        
        bool m_grabbingBall = true;


        sf::Clock m_deltaTimeClock;
        float MULT  = 60;

        std::thread m_updateThread;

        // MOUSE
        sf::Vector2f m_mousePosView;
        sf::Vector2f m_mouseOldPos;
        sf::Vector2f m_mouseVelocity;
        float m_mouseColRad = 15;
        bool m_mouseColActive = false;
        sf::CircleShape m_mouseColShape;

        float m_mouseColMaxRad = 300;
        float m_mouseColMinRad = 4;


        bool m_isKeyHeld = false;
        bool m_isMouseHeld = false;
        bool m_buildKeyHeld = false;

        bool m_gravityActive = true;
        bool m_paused = false;

        bool m_buildModeActive = false;
        bool m_newBallPin = false;

        const int MAXBALLS = 1000;

        sf::Clock m_spawnClock;
        float m_spawnNewBallDelay = 0.15;
        float m_spawnNewBluePrintDelay = 0.4f;

        IDVector<Object> m_objects;
        IDVector<Stick> m_sticks;

        Builder::StickMaker m_stickMaker;


        bool m_demospawnerDone = false;

        static const int s_ballPointCount = 30;


    private:
        void initText( );
        float calcDistance( sf::Vector2f pos1, sf::Vector2f pos2 );


        void updateObjects( float subDeltaTime );
        void updateSticks( );
        void updateText( );
        void updateMousePos( );

        void applyGravityToObjects( );

        void checkConstraints( );

        void checkCollisions( );
        void mouseCollisionsBall( );
        void getInput( );


        void ballGrabbedMovement( );

        bool mouseHoveringBall( );
        bool mouseHoveringBall( int& deleteID );

        void calcMouseVelocity( );
        void setDeltaTime( );

        sf::Color getRainbowColors( float time );

        void nonBuildModeMouseControls();
        void buildModeMouseControls();

    public:

    public:
        Simulation();
        ~Simulation();

        void initSticks( );

        void render( sf::RenderTarget& target );
        void renderSticks( sf::RenderTarget& target );
        void renderUI( sf::RenderTarget& target );
        void renderBluePrints( sf::RenderTarget& target );

        void deleteBall( int& delID );

        void startSim( );
        void simulate( );

        void updateUI( );
        void demoSpawner( );

        Object& addNewObject( sf::Vector2f startPos, float r, bool pinned = false);
        Stick& addNewStick( int id1, int id2, float length );
        void makeNewStick( );
        void spawnStick( );

        void joinUpdateThread( );
        void changeMouseRadius( float change );

        const void setWindow( sf::RenderWindow& window );
        const void setSubSteps( int substeps );

        const float getSubDeltaTime( ) const;
        const int getSubSteps( ) const;
        const float getTime( ) const;


};

#endif // !DEBUG
