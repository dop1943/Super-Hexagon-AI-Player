#pragma once

#include <SFML/Graphics.hpp>

#include "HexagonModel.hpp"

using namespace std;
class Game
{
public:
    Game();
    ~Game();
    void                Run();
    void                Train();
  
    static		map<int,vector<double>>q_table;
    static void 	init_q();//Aziz
    double              alpha,gamma,epsilon; //Aziz
    static int          numLevels;
    int                 numObs;


private:
    HexagonModel*       m_model;
    sf::RenderWindow*   m_window;
    
};



