#pragma once
#include <string>
#include "Transition.h"
#include <vector>
using namespace std;
class State {


public:
	string name; //name of the state.
	vector<Transition> transitions; //transitions of the state.

	State(string name="") { this->name = name; }
	void addTrans(Transition &t) {transitions.push_back(t);}
};