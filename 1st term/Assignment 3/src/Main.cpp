#include <iostream>
#include <sstream>
#include <fstream>
#include <string>
#include <vector>
#include "Transition.h"
#include "State.h"
#include <stack>
using namespace std;

vector<State> states;
string initState;
vector<string> finalStates;
vector <string> inputAlphabet;
vector <string> outputAlphabet;

State& findState(string& name) {  //Find the state that matches from states vector with the given name and return it.
	for (size_t i = 0; i < states.size(); i++) {
		State& s = states[i];
		if (name == s.name) {
			return s;
		}
	}
}

vector <string> splitString(string &str, const char &c) { //function for splitting a string and return a vector for that.
	stringstream sstr1(str);
	vector<string> sVector;
	string segment;

	while (getline(sstr1, segment, c)) {

		sVector.push_back(segment);
	}
	return sVector;
}

void findAndAdd(string &name,Transition &t, string outputName) { 
	//this methods stands for checking whether in the transition line there is a matching error
	//if there isn't any error then find the related state and add it's new transition.
	bool isAlphaCorr=false, subBool1=false, subBool2=false, subBool3= false; // Does alphabet match with the transition's characters?
	bool stateFound = false, subBool4 = false, subBool5 = false; //Do states matches with the transition's states?
	for (string ch : inputAlphabet) {
		if (t.iRead == "e" || t.iRead == ch) {
			subBool1 = true;
			break;
		}
		
	}
	for (string ch : outputAlphabet) {
		if (t.iPop == "e" || t.iPop == ch) {
			subBool2 = true;
		}
		if (t.iPush == "e" || t.iPush == ch) {
			subBool3 = true;
		}
	}
	if (subBool1  && subBool2&& subBool3) { //if all of the characters found in alphabets then it's alphabetically correct.
		isAlphaCorr = true;
	}

	for (State s : states) { 
		if (s.name == name)
			subBool4 = true;
		if (s.name == t.nextState)
			subBool5 = true;
	}
	if (subBool4 && subBool5) // if current state and next state in the states vector then it's correct.
		stateFound = true;

	if (!isAlphaCorr || !stateFound) {
		fstream file;
		file.open(outputName, fstream::out);
		file << "Error [1]:DPDA description is invalid!";
		file.close();
		exit(-1);
	} // error checking is done.


	for (size_t i = 0; i < states.size(); i++) { //finding the state and adding new transition.
		State& s = states[i];
		if (name == s.name) {
			s.addTrans(t);
			break;
		}
	}
}
void isInFinal(fstream& outputFile, string stateStr) { //Method for checking whether the given state name matches with the final state names.
	bool isFinal = false;
	for (string f : finalStates) {
		if (f == stateStr) {
			outputFile << "ACCEPT" << endl << endl;
			isFinal = true;
			break;
		}
	}
	if (!isFinal)
		outputFile << "REJECT" << endl << endl; // if the state isn't in the final states then print REJECT.
}
void printStack(fstream& outputFile, stack<string>& outputStack) { //this is a recursive function for printing the stack in the reverse order.
	if (outputStack.empty())
		return;
	
	else {

		string top = outputStack.top();
		outputStack.pop();
		printStack(outputFile, outputStack);

		if (outputStack.size()==0)
			outputFile << top;
		else
			outputFile << ","+ top ;
		outputStack.push(top);
	}
}

void readFile(string dpdaName, string outputName) { //function for reading the dpda file.

	fstream File(dpdaName, std::fstream::in);
	string text;
	int li = 0; //line index

	while (getline(File, text)) {
		text.erase(0, 2);

		if (li == 0) {
			string statesString = text.substr(0, text.find("=>") - 1);
			string initAndFinals = text.substr(text.find("=>") + 3, text.length());
			string stateName;
			vector<string> statesVector = splitString(statesString, ',');
			vector<string> vector2 = splitString(initAndFinals, ',');

			for (size_t i = 0; i < statesVector.size(); i++) {
				State s(statesVector[i]);
				states.push_back(s); //fill the states.
			}

			for (size_t i = 0; i < vector2.size(); i++) {
				stateName = vector2[i].substr(1, vector2[i].length() - 2); //getting rid of parantheses.

				if (vector2[i][0] == '(') {
					initState = stateName; // assign the initial state.
				}

				else if (vector2[i][0] == '[') {
					finalStates.push_back(stateName); //fill the final states.
				}
				stateName.clear();
			}

		}
		else if (li == 1) {
			inputAlphabet = splitString(text, ','); //fill the input alphabet.

		}
		else if (li == 2) {
			outputAlphabet = splitString(text, ',');; //fill the output alphabet.
		}
		else {
			vector <string> transParams = splitString(text, ','); //parameters of transition.
			Transition t(transParams[1], transParams[2], transParams[3], transParams[4]); // create the new transition.
			findAndAdd(transParams[0], t, outputName); //finding the related state.
		}

		li++;
	}
	File.close();
	

}
void executeFile(string inputName, string outputName) {
	fstream inputFile(inputName, std::fstream::in);
	string text;
	fstream outputFile(outputName, std::fstream::out);

	while (getline(inputFile, text)) {
		if (text == "") {
			isInFinal(outputFile, initState);
			continue;
		}
		vector<string> inputVector = splitString(text, ',');
		State curr = findState(initState); //current state.
		string ch; //character of input.
		Transition lastTrans; //Last transition, it's necessary for checking last step.
		stack <string> outputStack; //Stack for printing the output.
		bool lastStep=false; //is it in its last step?
		bool tFound = false; //is the related transition found?
		size_t i = 0;
		while (i <= inputVector.size()) {
			tFound = false;
			if (i == inputVector.size()) //if the input is finished then character points to void.
				ch = "";
			else
				ch = inputVector[i];
			for (int j = 0; j < curr.transitions.size(); j++) { //iteration for finding related transition.
				 Transition t=curr.transitions[j];

				if ((t.iRead == ch || t.iRead == "e") && (t.iPop == "e" || outputStack.empty() || t.iPop==outputStack.top())) {
					tFound = true;
					lastTrans = t;
					if (t.iRead != "e") //increase the index if there is a character in item to read.
						i++;
					
					if (t.iPush != "e") //push the character if there is a character in item to push.
						outputStack.push(t.iPush);
					
					if (t.iPop != "e") //pop the character if there is a character in item to pop.
						outputStack.pop();
					
					outputFile << (curr.name + ",") << (t.iRead + ",") << (t.iPop + " ") << "=> "
						<< (t.nextState + ",") << (t.iPush + " [STACK]:"); //print the step.
					printStack(outputFile, outputStack);
					outputFile << endl;
					curr = findState(t.nextState); //update current state.
					break;

				}
			}
			if (i == inputVector.size()) {//I needed this condition scope for executing the loop once again.
				if (lastStep) 
					break;
				lastStep = true;
			}
			if (!tFound)
				break;
			
		}
		if (tFound) {
			isInFinal(outputFile, lastTrans.nextState);
		}
		else //if transition not found then print REJECT.
			outputFile << "REJECT" << endl << endl;
		
	}
	inputFile.close();
	outputFile.close();
}

int main(int argc, char** argv) {
	readFile(argv[1], argv[3]);    //run the program.
	executeFile(argv[2], argv[3]);
	return 1;
}