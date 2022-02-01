#pragma once
#include <string>
using namespace std;
class Transition {


public:
	string iRead; //item to read, pop, push
	string iPop;
	string iPush;
	string nextState;  //next state's name.

	Transition(string iRead="e", string iPop = "e", string nextState = "", string iPush = "e") {
		this->iRead = iRead; 
		this->iPop = iPop;
		this->nextState = nextState;
		this->iPush = iPush;
	}
};