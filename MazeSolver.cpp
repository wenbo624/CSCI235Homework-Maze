/*
Author: Wen Bo
Course: CSCI-235
Date: FALL 2018
Assignment: Project 4

The implementation for the class MazeSolver.
@file MazeSolver.cpp
*/

#include <iostream>
#include <fstream>
using namespace std;

#include "MazeSolver.h"


//constructor
//pre: input file is in correct format with first two values being integers
//      followed by valid maze characters in {'_', '*','$'}
//post: if inuput file cannot be read outputs "Cannot read from input_file_name"
//      otherwise sets maze_rows_ and maze_columns from first two values in input file
//      and allocates two 2-dimesional array of size maze_rows_ and maze_columns
//      both maze_ and solution_ are initialized with characters read from input
MazeSolver::MazeSolver(std::string input_file)
{
	int row, col;
	ifstream infile;
	infile.open(input_file);
	if(infile.fail())
	{
		cerr << "Cannot read from " << input_file << endl;
		exit(1);
	}
	else
	{
		infile >> row >> col;
		
		initializeMaze(row, col);
		fillMaze(infile);
		initializeSolution();
		maze_ready = true;
	}
	
	infile.close();

}

//pre: rows and columns are positive integers
//post: allocates maze_ with rows and columns
//called by constructor
void MazeSolver::initializeMaze(int rows, int columns)
{
	if((rows > 0)&& (columns > 0))
	{
		maze_rows_ = rows;
		maze_columns_ = columns;
	}

	maze_ = new char* [maze_rows_];
	for(int i = 0; i < maze_rows_; i++)
	{
		maze_[i] = new char [maze_columns_];
	}
}

//pre: maze_ has been allocated with the correct number of rows and columns read from input file
//post: fills in maze_ with characters read from input file
//called by constructor
void MazeSolver::fillMaze(std::ifstream& input_stream)
{
	char ch;

	for(int x = 0; x < maze_rows_; x++)
	{
		for(int y = 0; y < maze_columns_; y++)
		{
			input_stream >> ch;
			maze_[x][y] = ch;
		}
	}

}

//pre: maze_ has been initialized with valid character values in {'_', '*','$'}
//     start position is always [0][0]
//post: initializes solution_ with a copy of maze_
//      initializes backtrack_stack_  with all viable paths from position [0][0]
//      and mark current position as visited ( > ) on solution_
//called by constructor
void MazeSolver::initializeSolution()
{
	copyMazetoSolution();
	Position start;
	start.row = 0;
	start.column = 0;
	backtrack_stack_.push(start);

}

//pre: maze_ has been properly initialized
//post: allocates solution_ to the correct number of rows and columns
//      and copies the contents of maze_ into solution_
//called by initializeSolution()
void MazeSolver::copyMazetoSolution()
{
	solution_ = new char* [maze_rows_];

	for(int i = 0; i < maze_rows_; i++)
	{
		solution_[i] = new char [maze_columns_];
	}

	for(int x = 0; x < maze_rows_; x++)
	{
		for(int y = 0; y < maze_columns_; y++)
		{
			solution_[x][y] = maze_[x][y];
		}
	}
}

// destructor
//post: deletes maze_ and solution_
MazeSolver::~MazeSolver()
{
	if(maze_ready)
	{
		for(int i = 0; i < maze_rows_; i++){
			delete [] maze_[i];

			delete [] solution_[i];
			
		}
		delete [] maze_;
		delete [] solution_;
	}
	
}

//return: true if maze has been initialized, false otherwise
bool MazeSolver::mazeIsReady()
{
	return maze_ready;
}

//pre: maze_ has been initialized with valid character values in {'_', '*','$'}
//post: solution_ has been marked with '>' signs along the path to the exit
//      prints "Found the exit!!!" if exit is found
//      or "This maze has no solution." if no exit could be found
//return: true if exit is found, false otherwise
bool MazeSolver::solveMaze()
{

	Position current_position = backtrack_stack_.top();
	while(!backtrack_stack_.empty())
	{

		if(maze_[current_position.row][current_position.column] == '$')
		{
			cout << "Found the exit!!!" << endl;
			return true;
		}
		else if(extendPath(current_position))
		{
			solution_[current_position.row][current_position.column] = '>';
			current_position.row = backtrack_stack_.top().row;
			current_position.column = backtrack_stack_.top().column;
			
		}
		else
		{
			maze_[current_position.row][current_position.column] = 'x';
			solution_[current_position.row][current_position.column]  = '@';
			backtrack_stack_.pop();

			if(!backtrack_stack_.empty())
			{
				current_position.row = backtrack_stack_.top().row;
				current_position.column = backtrack_stack_.top().column;
			}
			else
			{
				cout << "This maze has no solution." << endl;
				return false;
			}
		}
	}
	return false;
}

//post: prints the solution to the standard output stream
//      with single space character between each maze character
void MazeSolver::printSolution()
{
	cout << "The solution to this maze is:" << endl;
	for(int i = 0; i < maze_rows_; i++)
	{
		for(int j = 0; j < maze_columns_; j++)
		{
			cout << solution_[i][j] << " ";
		}
		cout << endl;
	}
	cout << endl;
}

//checks if the path can be extended in maze_ from position current_position in direction dir
//return: true if path can be extended given current_position and dir, false otherwise
//called by extendPath
bool MazeSolver::isExtensible(Position current_position, direction dir)
{
	if(dir == SOUTH)
	{
	
		if((current_position.row + 1 < maze_rows_) && ((maze_[current_position.row + 1][current_position.column] == '_') || (maze_[current_position.row + 1][current_position.column] == '$')))
			return true;
		
	}
	else if(dir == EAST)
	{
		if((current_position.column + 1 < maze_columns_) && ((maze_[current_position.row][current_position.column + 1] == '_') || (maze_[current_position.row][current_position.column + 1] == '$')))
			return true;

	}
	return false;
}

//pre: old_position is a Position initialized with row and column to valid positions in maze_ and it is extensible in direction dir
//return: a new Position on the maze moving in direction dir from old_position
//called by extendPath()
Position MazeSolver::getNewPosition(Position old_position, direction dir)
{
	Position new_position;
	if(isExtensible(old_position, dir)){
		if(dir == SOUTH)
		{
			new_position.row = old_position.row + 1;
			new_position.column = old_position.column;
		}
		else if(dir == EAST)
		{
			new_position.row = old_position.row;
			new_position.column = old_position.column + 1;
		}
		
	}
	return new_position;
		
}

//pre: current_position is a valid position on the maze_
//post: adds all positions extensible from current_position to backtrack_stack_
//return: true if path was extended, false otherwise
//called by solveMaze()
bool MazeSolver::extendPath(Position current_position)
{
	bool can_extend = false;
	Position new_position;

	if(isExtensible(current_position, SOUTH))
	{
		new_position = getNewPosition(current_position, SOUTH);
		backtrack_stack_.push(new_position);
		can_extend = true;

	}
	if(isExtensible(current_position, EAST))
	{
		new_position = getNewPosition(current_position, EAST);
		backtrack_stack_.push(new_position);
		can_extend = true;
	}
	return can_extend;
}