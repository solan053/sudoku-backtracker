#include <fstream>
#include <iostream>
#include <sstream>  
#include <algorithm>
#include <limits>
#include <fstream>
#include <vector>
#include <string>
#include <map>
#include <cmath>
#include <iomanip>
#include <ctime> 
#include <cstdlib>
#include <chrono>
#pragma GCC diagnostic ignored "-Wc++11-extensions"

class board {
public:
	std::vector<std::vector<int> > cell_value; // 2D array of number in a spot - 0 indicates it is empty, else there is a placement
	std::vector<std::vector<bool> > is_input; // 2D array of whether a number is an input (and thus immutable)
	std::string outfile; // Used to generate output filename
	std::vector<std::vector<bool> > boxs; // boxs[i] is a vector tracking which numbers have been placed in the ith box
	/*
	Box0: 0<=row<=2, 0<=col<=2, index = (row/3)*3 + (col/3) = 0
	Box1: 0<=row<=2, 3<=col<=5, index = (row/3)*3 + (col/3) = 1
	Box2: 0<=row<=2, 6<=col<=8, index = (row/3)*3 + (col/3) = 2
	*/
	std::vector<std::vector<bool> > rows; // rows[i] is a vector tracking which numbers have been placed in the ith row
	std::vector<std::vector<bool> > cols; // cols[i] is a vector tracking which numbers have been placed in the ith row
	
	board(void) {
		std::vector<int> tempint;
		std::vector<bool> tempbool;
		for(int i=0; i<9; i++) {
			tempint.push_back(0);
			tempbool.push_back(false);
		}
		// Must account for values in boxs, rows and cols, being 1-9, not 0 - 8
		std::vector<bool> tempbool2 = tempbool;
		tempbool2.push_back(false);		
		for(int i=0; i<9; i++) {
			cell_value.push_back(tempint);
			is_input.push_back(tempbool);
			boxs.push_back(tempbool2);
			rows.push_back(tempbool2);
			cols.push_back(tempbool2);
		}
	}
	
	void place_input(int value,int row,int col) {
		is_input[row][col] = true;
		place_value(value,row,col);
	}
	void place_value(int value,int row,int col) {
		cell_value[row][col] = value;
		rows[row][value] = true;
		cols[col][value] = true;
		boxs[(row/3)*3 + (col/3)][value] = true;
	}
	void remove_value(int value,int row,int col) {
		int old_value = cell_value[row][col];
		rows[row][old_value] = false;
		cols[col][old_value] = false;
		boxs[(row/3)*3 + (col/3)][old_value] = false;
		cell_value[row][col] = 0;
	}
	bool is_legal_place(int value,int row,int col) {
		//if (is_input[row][col]) return false;
		//bool one = rows[row][value]; // If true, then we already have this value
		//bool two = cols[col][value];
		//bool thr = boxs[(row/3)*3 + (col/3)][value];
		// If any one is true, we know placing value in the (row,col) spot is an illegal move and return false
		//return (!one) && (!two) && (!thr);
		return (!rows[row][value]) && (!cols[col][value]) && (!boxs[(row/3)*3 + (col/3)][value]);
	}
	void set_outfile(std::string name) {
		int ind = name.find('.');
		outfile = name.substr(0,ind)+"_sol"+name.substr(ind);
	}
};


board sudoku;

void parse_file(std::string);
void display_board();
void display_stats();
void print_board();
bool backtracker(int,int);


int main(int argc, const char * argv[]) { 
	if(argc != 3) {
		std::cout<<"You did not enter proper arguments.\nTerminated without parsing.\n";
		return 0;
	}
	parse_file(argv[1]);
	display_board();
	if(std::string(argv[2]) == "b") {
		auto start = std::chrono::high_resolution_clock::now();
		if(backtracker(0,0)) {
			auto finish = std::chrono::high_resolution_clock::now();
			std::chrono::duration<double> elapsed = finish - start;
			std::cout<<"Backtracking worked! The solution will be printed"<<std::endl;
			display_board();
			std::cout <<"Runtime time for Backtracking: " << elapsed.count() <<"s"<<std::endl;
			print_board();
		}
		else std::cout<<"Backtracking did not work :("<<std::endl;
	}
	return 0;
}	

void parse_file(std::string infile) {
    std::ifstream file(infile);
    std::string str;
	sudoku.set_outfile(infile);
	
	for(int i=0; i<9; i++) {
		std::getline(file, str);
		str.erase(remove(str.begin(), str.end(), ' '), str.end()); // Remove spaces from string
		for(int j=0; j<9; j++) {
			int value = str.at(j) - '0';
			if(value != 0) sudoku.place_input(value,i,j);
		}
	}
	file.close();
}

// backtracking algorithm - test all valid decision paths till you find the solution
bool backtracker(int row,int col) {
	// Base case for recursion 
	if(row==9) return true; 
	// Find empty cell to test values in
	if (sudoku.is_input[row][col]) {
		return backtracker(row + (col+1)/9 , (col+1)%9);
	}
	// Test values from 1 through 9 for the current cell and see if rest of the sudoku is solvable ahead
	for (int value = 1; value<10; value++) {
		if(sudoku.is_legal_place(value,row,col)) {
			sudoku.place_value(value,row,col);
			if(backtracker(row + (col+1)/9 , (col+1)%9)) {
				return true;
			}
			sudoku.remove_value(value,row,col);
		}
	}
	return false;
}

void display_board() {
	std::cout<<std::endl;
	for(int i=0; i<9; i++) {
		for(int j=0; j<9; j++) {
			std::cout<<sudoku.cell_value[i][j]<<" ";
		}
		std::cout<<std::endl;
	}
	std::cout<<std::endl;
}

void display_stats() {
	std::cout<<"      1 2 3 4 5 6 7 8 9"<<std::endl;
	for(int i=0; i<9; i++) {
		std::cout<<"Box"<<i<<": ";
		for(int j=1; j<10; j++) {
			std::cout<<sudoku.boxs[i][j]<<" ";
		}
		std::cout<<std::endl;
		std::cout<<"Row"<<i<<": ";
		for(int j=1; j<10; j++) {
			std::cout<<sudoku.rows[i][j]<<" ";
		}
		std::cout<<std::endl;
		std::cout<<"Col"<<i<<": ";
		for(int j=1; j<10; j++) {
			std::cout<<sudoku.cols[i][j]<<" ";
		}
		std::cout<<std::endl;
	}
	std::cout<<std::endl;
}

void print_board() {
    std::ofstream myfile;
    myfile.open(sudoku.outfile);
	for(int i=0; i<9; i++) {
		for(int j=0; j<9; j++) {
			myfile<<sudoku.cell_value[i][j]<<" ";
		}
		myfile<<std::endl;
	}
	myfile.close();
}
