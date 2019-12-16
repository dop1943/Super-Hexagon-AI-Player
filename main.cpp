#include "Game.hpp"
#include <cstdlib> 
#include<vector>
#include <iostream>

using namespace std;
map<int,vector<double>>Game::q_table;
int Game::numLevels;
int Epochs=6000;

//read q-table from text file
void readQTable(int iterNum){
	double x;
	string fileName="test"+to_string(iterNum);
	ifstream myfile;
	myfile.open (fileName);
	int i=0,st=0;
	Game::init_q();
	vector<double>tab(5);
	while(myfile>>x){
		if(i==0) st=x;
		else tab[i-1]=x;
		i++;
		if(i==6){
			Game::q_table[st]=tab;
			i=0;
		}
	}
	cout<<Game::q_table[1111112][1]<<endl;
}

//print q-table in text file named "test(Numiter)", print iteration results (eval) in another text file
void printResults(int iterNum,int num_episodes,double levelRatio,double obsRatio){
	ofstream myfile,myfile1;
	string fileName="test"+to_string(iterNum);
	string fileRes="results"+to_string(iterNum);
	myfile1.open(fileRes);
  	myfile.open (fileName);

	//Printing Final Results
	myfile1<<"**********************************"<<endl<<"*****************************************"<<endl;
	myfile1<<"#########################################"<<endl;
	myfile1<<"FINAL RESULTS: IterNumber: "<<iterNum<<endl;
	myfile1<<"Number of episodes: "<<num_episodes<<endl;
	myfile1<<"Level Ratio : "<<fixed<<setprecision(3)<<levelRatio<<endl;
	myfile1<<"Obs Ratio : "<<fixed<<setprecision(3)<<obsRatio<<endl;
	myfile1<<"#########################################"<<endl;
	myfile1<<"**********************************"<<endl<<"*****************************************"<<endl;
  	
	//to reduced !!!!!!!!!!!!!!!!!!!!!!!!!!!!
	for (map<int,vector<double>>::iterator it=Game::q_table.begin();it!=Game::q_table.end();++it){
		myfile<<it->first<<" ";
		for(int i=0;i<5;++i) myfile<<it->second[i]<<" ";
		myfile<<endl;
	}
	myfile.close();
}
void GridSearch(double alpha,double gamma,double epsilon1,double epsilon2,double epsilon3,int iterNum){
	//variables
	int num_episodes=1;
	double obsRatio,levelRatio;
        int numObs=0;
	
	//Initialization
	//Game::init_q();
	readQTable(12);
	int max_=0,test=0;
	while(num_episodes<Epochs){
		//Instanciation and setting hyperparameters
		Game game;
		game.alpha=alpha;
		game.gamma=gamma;
		game.epsilon=-1;
		/*if(num_episodes>=1000)	game.epsilon=epsilon2;
		if(num_episodes>=2000) game.epsilon=epsilon3;*/

		//game.Train();
		game.Run();
		//Evaluation metrics calculation
		numObs+=game.numObs;
		obsRatio=(double)numObs/(double)num_episodes;
		levelRatio=(double)Game::numLevels/(double)num_episodes;

		//Verbose
		if(num_episodes%10==0){
			cout<<"#########################################"<<endl;
			cout<<"Number of episodes: "<<num_episodes<<endl;
			cout<<"Level Ratio : "<<fixed<<setprecision(3)<<levelRatio<<endl;
        		cout<<"Obs Ratio : "<<fixed<<setprecision(3)<<obsRatio<<endl;
			cout<<"Obs : "<<numObs<<endl;
			cout<<"#########################################"<<endl;
		}
		num_episodes++;	

		//Improvement ?
		if(max_<obsRatio)max_=obsRatio,test=0;
		else test++;
		//cout<<numObs<<endl;
		//if( obsRatio>500)break;
	}
	printResults(iterNum,num_episodes,levelRatio,obsRatio);
}
int main(int argc, char** argv)
{
	//Aziz
	/*GridSearch(0.1,0.8,0.8,0.4,0,11);*/
	GridSearch(0.1,0.7,0.2,0.1,0,12);/*
	GridSearch(0.1,0.5,0.2,0.1,0,13);
	GridSearch(0.2,0.8,0.2,0.1,0,14);
	GridSearch(0.2,0.7,0.2,0.1,0,15);
	GridSearch(0.2,0.5,0.2,0.1,0,16);
	GridSearch(0.3,0.8,0.2,0.1,0,7);
	GridSearch(0.3,0.7,0.2,0.1,0,8);
	GridSearch(0.3,0.5,0.2,0.1,0,9);
	
	GridSearch(0.1,0.8,0.3,0.15,0,110);
	GridSearch(0.1,0.7,0.3,0.15,0,111);
	GridSearch(0.1,0.5,0.3,0.15,0,112);
	GridSearch(0.2,0.8,0.3,0.15,0,113);
	GridSearch(0.2,0.7,0.3,0.15,0,114);
	GridSearch(0.2,0.5,0.3,0.15,0,15);
	GridSearch(0.3,0.8,0.3,0.15,0,16);
	GridSearch(0.3,0.7,0.3,0.15,0,17);
	GridSearch(0.3,0.5,0.3,0.15,0,18);*/
	//GridSearch(0.05,0.5,0.2,0.1,0,1333);
	return 0;
}
/*
alpha: 0.1/0.2/0.3
gamma: 0.8/0.7/0.5
epsilon: 0.2->0/0.3->0
epochs: while no improvement for 100 iterations, stop training the model
*/

