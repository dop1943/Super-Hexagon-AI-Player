#include <bits/stdc++.h> 
#pragma once

using namespace std;

struct Obstacle;

class HexagonModel
{
public:
    HexagonModel(const int maxSides = 16);
    ~HexagonModel();

    void            Simulate(const double dt);
    void            SetPlayerDirection(const int direction);
    void            SetGameSpeed(const double speed);
    void            SetNumSides(const int numSides);
    void            SetInvincibility(const bool invincibility);
    void            SetPlayerSpeed(const int speed);

    Obstacle*       GetObstacle(const int side)                 const;
    const int       GetNumSides()                               const;
    const double    GetPlayerPosition()                         const;
    const double    getDistanceFromNearestObs(const double position)                 const; //Aziz
    const double    GetPlayerDistance()                         const;
    const int       GetPlayerDirection()                        const;
    const double    GetTime()                                   const;
    const bool      IsGameOver()                                const;
    const bool      IsPlayerInvincible()                        const;
    const double    GetRotation()                               const;
    const bool      WillCollide(const double position)          const;
    const bool 	    isNearestObs(const double dis) 		const;//AZIZ
    const int 	    directionToFarestObs(const double dis,const double pos) 	const;//AZIZ
    const int       GetLevel()                        		const;
    int			numObs;
    bool          passedObs;

private:
    void            Tick(const double dt);
    void            AddObstacle(const int side, const double start, const double length);
    void            AddPattern();


    double          m_gameSpeed;

    double          m_curTime;

    double          m_playerPosition;   //Position from 0..m_numSides
    double          m_playerSpeed;      //Speed player can move sideways
    double          m_curDistance;      //Current distance through obstacles
    double          m_obsDistance;      //Distance to end of last obstacle
    double          m_obsSpeed;         //Speed obstacles approach at
    double          m_timeDue;          //Time left to simulate 

    double          m_rotation;
    double          m_rotationSpeed;
    double          m_nextRotationSpeedChange;

    bool            m_invincible;
    bool            m_gameOver;
    int             m_playerDirection;

    const int       m_maxSides;
    int             m_numSides;

    int             m_lastPattern;
    int		    m_level; //Aziz
    int             m_level2;//AZIZ
    Obstacle**      m_obstacles;


};


struct Obstacle
{
    double          start;
    double          end;
    int 	    level;
    Obstacle*       next;
};
