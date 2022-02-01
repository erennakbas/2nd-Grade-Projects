#pragma once
#include <vector>
#include <string>
using namespace std;

class LetterNode {
public:
	char letter;
	string val;
	vector<LetterNode*> children;
	LetterNode(char letter = '*', string val="+") { this->letter = letter, this->val = val; }

};