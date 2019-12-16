#include "Game.hpp"
#include "iostream"
#include "HexagonView.hpp"



using namespace std;

bool compare(pair<int,int>p1,pair<int,int>p2){
     return (p1.second<p2.second);
}

Game::Game()
{
    m_model = 0;
    m_window = 0;
    //q-learning hyperparameters 
    alpha=0.1; //learning rate
    gamma=0.7;	//discount factor
    epsilon=0.2; //exploration variable, used for testing new paths
}

Game::~Game()
{
    if(m_model)
        delete m_model;

    if(m_window)
        delete m_window;
}

//q-table initialization
void Game::init_q(){
    for (int i=0;i<6;i++){
    	long long int st;
	st=2*pow(10,i);
	for(int j=0;j<6;j++) if(j!=i) st+=pow(10,j);
	long long int aux=st;
	for(int j=0;j<3;j++){
    	    for(int k=0;k<3;k++){
		st=aux;
	    	st+=j*pow(10,6);
		st+=k*pow(10,7);
		vector<double>tab(5,0);
		q_table[st]=tab;
	    }
	}
    }
    numLevels=0; //static variable to count total number of levels passed by the model
}

void Game::Run()
{   
    int dirr; //variable that can take 5 values (-1:left,0:no direction,1:right,2:slow left,3: slow right)  
    vector<pair<int,int>>states;//vector which contains the paths taken by the model to pass an obstacle, cleared each time an obs is passed
    int dir;//direction taken by the model
    int state;//variable which stores the current state

    m_model = new HexagonModel();

    HexagonView view;
    view.SetModel(m_model);
    view.SetBaseHue(0.3);
    view.SetHueRange(0.3);
    view.SetHueSpeed(0.5);

    m_window = new sf::RenderWindow(sf::VideoMode(1920,1080), "Hexagon");
    m_window->setFramerateLimit(60);

    bool paused = false;

    sf::Clock clock;
    bool test=true;

    while(test)
    {
        state=0;
	if(m_model->IsGameOver()){
	    test=false;
	    m_window->close();
	}
        sf::Event e;
        while(m_window->pollEvent(e))
        {
            if(e.type == sf::Event::Closed)
                m_window->close();

            if(e.type == sf::Event::KeyPressed)
            {
                if(e.key.code == sf::Keyboard::Space) {
                    paused = !paused;
                }

                if(e.key.code == sf::Keyboard::R) {
                    delete m_model;
                    m_model = new HexagonModel();
                    view.SetModel(m_model);
                }

            }

        }
        if(sf::Keyboard::isKeyPressed(sf::Keyboard::Escape)) {
            m_window->close();
        }
        
        {
	    dir = 0;

	    //if player wanna play manually
            /*if(sf::Keyboard::isKeyPressed(sf::Keyboard::Right) ||
               sf::Keyboard::isKeyPressed(sf::Keyboard::D)) {
                dir += 1;
            }

            if(sf::Keyboard::isKeyPressed(sf::Keyboard::Left) ||
               sf::Keyboard::isKeyPressed(sf::Keyboard::A)) {
                dir -= 1;
            }*/


	    srand(time(NULL));
 	    double randnum = (double)rand() / ((double)RAND_MAX + 1);

	    //find the current state
	    /*
		this part of the code is about to find the current state
		A state is calculated as follows: the first 6 numbers describe if a wall is present in each of the six positions, the first number always referes to the player, than follow the 				adjacent to it going from the right
		The two remaining numbers describe resp the position of the player in its position (left,right side or in the middle) and its distance from the obs (left,right side or in the middle)
	    */
	    

	    //first six numbers
	    int pos=m_model->GetPlayerPosition();
	    vector<pair<int,int>>tab;
	    for (int i=pos;i<6;i++){
		tab.push_back(make_pair(i,m_model->getDistanceFromNearestObs(i)));
	    }
	    for (int i=0;i<pos;i++){
		tab.push_back(make_pair(i,m_model->getDistanceFromNearestObs(i)));
	    }
	    sort(tab.begin(),tab.end(),compare);
            map<int,int>ord;
	    for (int i=0;i<tab.size();i++) ord[tab[i].first]=i;
	    for (int i=pos;i<6;i++){
		if(ord[i]==5)
		    state+=(2)*pow(10,i-pos);
		else state+=(1)*pow(10,i-pos);
	    }
	    for (int i=0;i<pos;i++){
		if(ord[i]==5)
		    state+=(2)*pow(10,tab.size()-pos+i);
		else state+=(1)*pow(10,tab.size()-pos+i);
	    }

	     
	    //dis and position calculation
	    double pos_=m_model->GetPlayerPosition();
	    pos=pos_;
	    double dis_=m_model->getDistanceFromNearestObs(pos);
	    int posL=pos_<(double)pos+0.3?0:1;
	    posL=pos_>(double)pos+0.6?2:posL;
	    int dis=dis_;
	    int disL=dis_<(double)dis+0.3?0:1;
	    disL=dis_>(double)dis+0.6?2:disL;
	    
	    state+=posL*pow(10,6)+disL*pow(10,7);

	
	    if (randnum<=epsilon){//Explore Action Space
		const double range_from  = 0.1;
			const double range_to    = 4.9;
			std::random_device                  rand_dev;
			std::mt19937                        generator(rand_dev());
			std::uniform_int_distribution<int>  distr(range_from, range_to);

			dir=(int)distr(generator)-1;
	    }else{//Exploit learned values
		double aux=q_table[state][0];
		
		if(aux==q_table[state][1] && aux==q_table[state][2] && aux==q_table[state][3] && aux==q_table[state][4] ){
			const double range_from  = 0.1;
			const double range_to    = 4.9;
			std::random_device                  rand_dev;
			std::mt19937                        generator(rand_dev());
			std::uniform_int_distribution<int>  distr(range_from, range_to);

			dir=(int)distr(generator)-1;
		}else{// take the action that has the max q-value
			dir=-1;
			if(q_table[state][1]>aux){
				aux=q_table[state][1];
				dir=0;
			}
			if(q_table[state][2]>aux){
				aux=q_table[state][2];
				dir=1;
			}
			if(q_table[state][3]>aux){
				aux=q_table[state][3];
				dir=2;
			}
			if(q_table[state][4]>aux){
				dir=3;
			}
		}
	    }
		
	    //update speed according to the chosen direction
	    dirr=dir;
	    m_model->SetPlayerSpeed(3);
	    if(dir==2)dir=-1,m_model->SetPlayerSpeed(1.5);
	    else if(dir==3)dir=1,m_model->SetPlayerSpeed(1.5);

            m_model->SetPlayerDirection(dir);

	    
        }

        if(sf::Keyboard::isKeyPressed(sf::Keyboard::Up) ||
           sf::Keyboard::isKeyPressed(sf::Keyboard::W)) {
            m_model->SetGameSpeed(2.0);
        } else if(sf::Keyboard::isKeyPressed(sf::Keyboard::Down) ||
                  sf::Keyboard::isKeyPressed(sf::Keyboard::S)) {
            m_model->SetGameSpeed(0.2);
        } else {
            m_model->SetGameSpeed(1.2);        }
		

        double dt = clock.restart().asSeconds();
	

        //More than a second? We must be debugging.
        if(dt > 1.0) {
            dt = 0.01;
        }
	dt=0.0167;



        if(!paused) {
            m_model->Simulate(dt);
	    if(m_model->IsGameOver()){
		numLevels+=m_model->GetLevel();
		numObs=m_model->numObs;
	    }
        }
	//cout<<"DTTTTTT: "<<dt<<endl;
        m_window->clear();
        view.Draw(m_window);
        m_window->display();
    }
}

void Game::Train()
{   int dirr; //variable that can take 5 values (-1:left,0:no direction,1:right,2:slow left,3: slow right)  
    vector<pair<int,int>>states;//vector which contains the paths taken by the model to pass an obstacle, cleared each time an obs is passed
    int dir;//direction taken by the model
    int state;//variable which stores the current state

    m_model = new HexagonModel();

    bool paused = false;

    sf::Clock clock;
    bool test=true;
    while(test)
    {
        state=0;
	if(m_model->IsGameOver()){
	    test=false;
	    //m_window->close();
	}
        sf::Event e;
        
        {
	    dir = 0;


	    srand(time(NULL));
 	    double randnum = (double)rand() / ((double)RAND_MAX + 1);

	    //find the current state
	    /*
		this part of the code is about to find the current state
		A state is calculated as follows: the first 6 numbers describe if a wall is present in each of the six positions, the first number always referes to the player, than follow the 				adjacent to it going from the right
		The two remaining numbers describe resp the position of the player in its position (left,right side or in the middle) and its distance from the obs (left,right side or in the middle)
	    */
	    

	    //first six numbers
	    int pos=m_model->GetPlayerPosition();
	    vector<pair<int,int>>tab;
	    for (int i=pos;i<6;i++){
		tab.push_back(make_pair(i,m_model->getDistanceFromNearestObs(i)));
	    }
	    for (int i=0;i<pos;i++){
		tab.push_back(make_pair(i,m_model->getDistanceFromNearestObs(i)));
	    }
	    sort(tab.begin(),tab.end(),compare);
            map<int,int>ord;
	    for (int i=0;i<tab.size();i++) ord[tab[i].first]=i;
	    for (int i=pos;i<6;i++){
		if(ord[i]==5)
		    state+=(2)*pow(10,i-pos);
		else state+=(1)*pow(10,i-pos);
	    }
	    for (int i=0;i<pos;i++){
		if(ord[i]==5)
		    state+=(2)*pow(10,tab.size()-pos+i);
		else state+=(1)*pow(10,tab.size()-pos+i);
	    }

	     
	    //dis and position calculation
	    double pos_=m_model->GetPlayerPosition();
	    pos=pos_;
	    double dis_=m_model->getDistanceFromNearestObs(pos);
	    int posL=pos_<(double)pos+0.3?0:1;
	    posL=pos_>(double)pos+0.6?2:posL;
	    int dis=dis_;
	    int disL=dis_<(double)dis+0.3?0:1;
	    disL=dis_>(double)dis+0.6?2:disL;
	    
	    state+=posL*pow(10,6)+disL*pow(10,7);

	
	    if (randnum<=epsilon){//Explore Action Space
		const double range_from  = 0.1;
			const double range_to    = 4.9;
			std::random_device                  rand_dev;
			std::mt19937                        generator(rand_dev());
			std::uniform_int_distribution<int>  distr(range_from, range_to);

			dir=(int)distr(generator)-1;
	    }else{//Exploit learned values
		double aux=q_table[state][0];
		
		if(aux==q_table[state][1] && aux==q_table[state][2] && aux==q_table[state][3] && aux==q_table[state][4] ){
			const double range_from  = 0.1;
			const double range_to    = 4.9;
			std::random_device                  rand_dev;
			std::mt19937                        generator(rand_dev());
			std::uniform_int_distribution<int>  distr(range_from, range_to);

			dir=(int)distr(generator)-1;
		}else{// take the action that has the max q-value
			dir=-1;
			if(q_table[state][1]>aux){
				aux=q_table[state][1];
				dir=0;
			}
			if(q_table[state][2]>aux){
				aux=q_table[state][2];
				dir=1;
			}
			if(q_table[state][3]>aux){
				aux=q_table[state][3];
				dir=2;
			}
			if(q_table[state][4]>aux){
				dir=3;
			}
		}
	    }
		
	    //update speed according to the chosen direction
	    dirr=dir;
	    m_model->SetPlayerSpeed(2.5);
	    if(dir==2)dir=-1,m_model->SetPlayerSpeed(1.5);
	    else if(dir==3)dir=1,m_model->SetPlayerSpeed(1.5);

            m_model->SetPlayerDirection(dir);

	    
        }

        if(sf::Keyboard::isKeyPressed(sf::Keyboard::Up) ||
           sf::Keyboard::isKeyPressed(sf::Keyboard::W)) {
            m_model->SetGameSpeed(2.0);
        } else if(sf::Keyboard::isKeyPressed(sf::Keyboard::Down) ||
                  sf::Keyboard::isKeyPressed(sf::Keyboard::S)) {
            m_model->SetGameSpeed(0.2);
        } else {
            m_model->SetGameSpeed(3);        }
		

        double dt = clock.restart().asSeconds();
	

        //More than a second? We must be debugging.
        if(dt > 1.0) {
            dt = 0.01;
        }
	dt=0.0167;



        if(!paused) {
           m_model->Simulate(dt);

	   //if we pass an obstacle we update all the (states,actions) taken to pass that obs since we're sure that these are good choices
	   //this is a little update of the q-learning algorithm, we have figured out that the algorithm has to keep in memory good choiced, a sequence is a good choice, not an action
	   if(m_model->passedObs){
		m_model->passedObs=false;
		if(states.size()>=1)
			for(int i=0;i<states.size()-1;i++)
				if(q_table[states[i].first][states[i].second]!=q_table[states[i+1].first][states[i+1].second])
					q_table[states[i].first][states[i].second]=(1 - alpha) * q_table[states[i].first][states[i].second] + alpha * (20 + gamma * q_table[states[i+1].first][states[i+1].second]);
		states.clear();}
	    else states.push_back(make_pair(state,dirr+1));// if still obs still not passed, add the current state ,action

	    //New state calculation, same approach as above
	    int new_state=0;
	    int new_pos=m_model->GetPlayerPosition();
	    vector<pair<int,int>>tab;
	    for (int i=new_pos;i<6;i++){
		tab.push_back(make_pair(i,m_model->getDistanceFromNearestObs(i)));
	    }
	    for (int i=0;i<new_pos;i++){
		tab.push_back(make_pair(i,m_model->getDistanceFromNearestObs(i)));
	    }
	    sort(tab.begin(),tab.end(),compare);
            map<int,int>ord;
	    for (int i=0;i<tab.size();i++) ord[tab[i].first]=i;
	    for (int i=new_pos;i<6;i++){
		if(ord[i]==5)
		    new_state+=(2)*pow(10,i-new_pos);
		else new_state+=(1)*pow(10,i-new_pos);
	    }
	    for (int i=0;i<new_pos;i++){
		if(ord[i]==5)
		    new_state+=(2)*pow(10,tab.size()-new_pos+i);
		else new_state+=(1)*pow(10,tab.size()-new_pos+i);
	    }
	    
	    double pos_=m_model->GetPlayerPosition();
	    int pos=pos_;
	    double dis_=m_model->getDistanceFromNearestObs((int)pos_);
	    int posL=pos_<(double)pos+0.3?0:1;
	    posL=pos_>(double)pos+0.6?2:posL;
	    int dis=dis_;
	    int disL=dis_<(double)dis+0.3?0:1;
	    disL=dis_>(double)dis+0.6?2:disL;
	    
	    new_state+=posL*pow(10,6)+disL*pow(10,7);


	    int reward;
	    
	    if(m_model->IsGameOver()){
	    	states.clear();
                numLevels+=m_model->GetLevel();
		numObs=m_model->numObs;
		reward=-10;
	    }
            else reward=1;

	    //update q-table
            double old_value=q_table[state][dirr+1];
            double new_max=max(std::max (std::max (q_table[new_state][0],q_table[new_state][1]),max (q_table[new_state][2],q_table[new_state][3])), q_table[new_state][4]);
	    double new_value=(1 - alpha) * old_value + alpha * (reward + gamma * new_max);
            q_table[state][dirr+1] = new_value;
        }
    }
}
