# Super-Hexagon-AI-Player

## Description
Modeling and Developement of a Reinforcement Learning system for the Super Hexagon Game


## Algorithm: Q-Learning
The Q Learning algorithm had to be modified by storing all good actions that have lead to passing an obstacle then given each corresponding state an extra reward. This has been figured out because the algorithm couldn't learn the fact that the player has to do a sequenced actions to be able to pass the obstacle.

## Hyperparameter tuning:
Grid-search was performed to detect all best hyperparameters.

## Most important files
Main.cpp
HexagonModel.cpp
Game.cpp (contains Q Learning implementation)

## Source Code Of The Game
The source code of the game was developped as a clone of the original game by Harry Jeffery 
link ==> https://github.com/eXeC64/Hexagon


