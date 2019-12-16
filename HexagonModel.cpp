#include "HexagonModel.hpp"
#include <stdlib.h>
#include <time.h>
#include "Util.hpp"


HexagonModel::HexagonModel(const int maxSides) : m_maxSides(maxSides)
{
    passedObs=false;
    numObs=0;//AZIZ
    m_level=0;//Aziz
    m_level2=0;//Aziz

    m_gameSpeed = 1;

    m_playerPosition = 0.5;
    m_playerSpeed = 2;
    m_playerDirection = 0;

    m_curDistance = 0;
    m_obsDistance = 8;
    m_obsSpeed = 6;

    m_curTime = 0;
    m_timeDue = 0;

    m_rotation = 0;
    m_rotationSpeed = 0;
    m_nextRotationSpeedChange = 0;

    m_invincible = false;
    m_gameOver = false;

    m_numSides = 6;

    m_lastPattern = -1;

    m_obstacles = new Obstacle*[m_maxSides];

    for(int i = 0; i < m_maxSides; ++i) {
        m_obstacles[i] = 0;
    }

    srand(time(0));
}

HexagonModel::~HexagonModel()
{
    for(int i = 0; i < m_maxSides; ++i) {
        if(m_obstacles[i] != 0) {
	    //cout<<"  3aaawedna    +++++++++++"<<endl;
            delete m_obstacles[i];
        }
    }

    delete[] m_obstacles;
}

void HexagonModel::Simulate(const double dt)
{
    m_timeDue += dt;

    const double tickSize = 1/100.0;

    while(!IsGameOver() && m_timeDue >= tickSize) {
        Tick(tickSize * m_gameSpeed);
        m_timeDue -= tickSize;
        m_curTime += tickSize * m_gameSpeed;
    }
}

void HexagonModel::Tick(const double dt)
{
    //Rotate game
    if(m_curTime >= m_nextRotationSpeedChange) {
        m_nextRotationSpeedChange = m_curTime + 5 + rand() % 10;
        m_rotationSpeed = 60 + rand() % 60;
        if(rand() % 2) {
            m_rotationSpeed = -m_rotationSpeed;
        }
    }

    m_rotation += m_rotationSpeed * dt;


    //Move player sideways
    const double playerPosDelta = (m_playerDirection * m_playerSpeed * m_numSides * dt);
    const double newPlayerPos = Cycle(m_playerPosition + playerPosDelta, 0, m_numSides);

    if(!WillCollide(newPlayerPos)) {
        m_playerPosition = newPlayerPos;
    }


    //Move forward
    m_curDistance += m_obsSpeed * dt;

    //Check for collisions
    if(!m_invincible && WillCollide(m_playerPosition)) {
        m_gameOver = true;
    }

    //Delete old obstacles
    for(int i = 0; i < m_maxSides; ++i) {
        Obstacle* obs = m_obstacles[i];
        if(obs && obs->end < m_curDistance - 1) {
            m_obstacles[i] = obs->next;
	    numObs++;
            delete obs;
	    passedObs=true;
        }
    }
    //Check for level
    bool test=false;
    for(int i = 0; i < m_maxSides; ++i) {
        Obstacle* obs = GetObstacle(i);
	while(obs){
		if(obs && obs->level==m_level2) {
		    test=true;
		    break;
		}
	        obs=obs->next;
	}
	if(test)break;
    }
    if(!test) m_level2++;
    //Add new obstacles if needed
    while(m_obsDistance - m_curDistance < 10) {
        AddPattern();
    }
}

void HexagonModel::AddObstacle(const int side, const double start, const double length)
{
    Obstacle* obs = GetObstacle(side);
    Obstacle* lastObs = 0;

    while(obs) {
        lastObs = obs;
        obs = obs->next;
    }

    Obstacle* newObs = new Obstacle;
    newObs->start = start;
    newObs->end = start + length;
    newObs->next = 0;
    newObs->level=m_level;    

    if(lastObs) {
        lastObs->next = newObs;
    } else {
        m_obstacles[side] = newObs;
    }

    m_obsDistance = Max(m_obsDistance, start + length);
}

void HexagonModel::AddPattern()
{
    const double base = m_obsDistance + 2;

    //Direction to spin the pattern in
    const int direction = rand() % 2;

    if(m_numSides == 6) {
	m_level++;
        //Never repeat the same pattern consecutively
        int pattern = m_lastPattern;
        while(pattern == m_lastPattern) {
            pattern = rand() % 7;
        }
        m_lastPattern = pattern;
	pattern=0;
        if(pattern == 0) {

            //Opposite hexagons
            const int iterations = 1 + rand() % 5;
            int lastSide = -1;

            //Add each hexagon
            for(int i = 0; i < iterations; ++i) {

                //Never put the gap on the same side consecutively
                int side = lastSide;
                while(side == lastSide) {
                    side = rand() % 6;
                }
                lastSide = side;

                //Construct the hexagon
                for(int s = 0; s < 6; ++s) {
                    if(s != side) {
                        AddObstacle(s, 1 + base + i * 3, 0.5);
                    }
                }
            }

        }
    } else {
        const int side = rand() % m_numSides;
        AddObstacle(side, m_obsDistance + 1, m_obsDistance + 1.5);
    }

}

const bool HexagonModel::WillCollide(const double position) const
{
    const int side = position;

    Obstacle* obs = GetObstacle(side);

    while(obs) {
        if(obs->start <= m_curDistance && obs->end > m_curDistance) {
            return true;
        }
        obs = obs->next;
    }

    return false;
}



const double HexagonModel::getDistanceFromNearestObs(const double position) const{//AZIZ
    const int side = position;
    Obstacle* obs = GetObstacle(side);
    double dis=9.9;
    while(obs) {
	double aux=obs->start - m_curDistance;
        if(aux < dis && (obs->start <= m_curDistance && obs->end > m_curDistance || obs->start>=m_curDistance)) {
            dis=aux;
        }
        obs = obs->next;
    }
   
    return dis<0?0:dis;
}

double disFromLeft(double a,double b,int max_s){//Aziz
	if (b<=a) return a-b;
        else return (a+max_s-b);
}

double disFromRight(double a,double b,int max_s){//Aziz
	if (b>=a) return b-a;
        else return (max_s-a+b);
}

const bool HexagonModel::isNearestObs(const double dis) const{//AZIZ
    for(int i = 0; i < m_numSides; ++i) {
        int side=i;
        Obstacle* obs = GetObstacle(side);
	while(obs){
	    double aux=obs->start-m_curDistance;
	    if(aux<dis && aux>=0)
		return false;
	    obs = obs->next;
	}
    }
    return true;
}



const int HexagonModel::directionToFarestObs(const double dis,double pos) const{//AZIZ
    double farest_dis=dis;
    int pos_=pos;
    for(int i = 0; i < m_numSides; ++i) {
        double dis_=getDistanceFromNearestObs(i);
	if (farest_dis<dis_){
		farest_dis=dis_;
		pos_=i;
	}else if(farest_dis==dis_ && pos_!=i){
		if(min(disFromLeft(pos,pos_,m_numSides),disFromRight(pos,pos_,m_numSides))>min(disFromLeft(pos,i,m_numSides),disFromRight(pos,i,m_numSides)))
			farest_dis=dis_,pos_=i;
	}
    }
    if(pos_==(int)pos) return 1;
    else{
	int a=pos+1,b=pos_;
	if(a==m_numSides) a=0;
	while(a!=b){
		if(WillCollide(a)) return 0;
		a++;
		if(a==m_numSides) a=0;
	}
	a=pos-1,b=pos_;
	if(a<0) a=m_numSides-1;
	while(a!=b){
		if(WillCollide(a)) return 2;
		a--;
		if(a==-1) a=m_numSides-1;
	}
	return disFromLeft(pos,pos_,m_maxSides)<disFromRight(pos,pos_,m_maxSides)?0:2;
    }
}

void HexagonModel::SetPlayerDirection(const int direction)
{
    if(direction > 0) {
        m_playerDirection = 1;
    } else if(direction < 0) {
        m_playerDirection = -1;
    } else {
        m_playerDirection = 0;
    }
}

void HexagonModel::SetGameSpeed(const double speed)
{
    m_gameSpeed = speed;
}

void HexagonModel::SetNumSides(const int numSides)
{
    m_numSides = Clamp(numSides, 4, m_maxSides);
}

void HexagonModel::SetInvincibility(const bool invincibility)
{
    m_invincible = invincibility;
}

Obstacle* HexagonModel::GetObstacle(const int side) const
{
    if(side < 0 || side >= m_maxSides)
        return 0;

    return m_obstacles[side];
}

const int HexagonModel::GetNumSides() const
{
    return m_numSides;
}

const double HexagonModel::GetPlayerPosition() const
{
    return m_playerPosition;
}

const double HexagonModel::GetPlayerDistance() const
{
    return m_curDistance;
}

const int HexagonModel::GetPlayerDirection() const
{
    return m_playerDirection;
}

const double HexagonModel::GetTime() const
{
    return m_curTime;
}

const bool HexagonModel::IsPlayerInvincible() const
{
    return m_invincible;
}

const bool HexagonModel::IsGameOver() const
{
    return m_gameOver;
}

const double HexagonModel::GetRotation() const
{
    return m_rotation;
}
const int    HexagonModel::GetLevel() const{
    return m_level2;
}
void         HexagonModel::SetPlayerSpeed(const int speed){
    m_playerSpeed=speed;
}

