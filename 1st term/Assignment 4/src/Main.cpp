#include <iostream>
#include <string>
#include <fstream>
#include <sstream>
#include "LetterNode.h"
using namespace std;

bool willDeleted = false; // bool for deleting the item in delete function.

vector <string> splitString(string& str, const char& c) { //function for splitting a string and return a vector for that.
	stringstream sstr1(str);
	vector<string> sVector;
	string segment;

	while (getline(sstr1, segment, c)) {

		sVector.push_back(segment);
	}
	return sVector;
}
int binarySearch(vector<LetterNode*>&vec, int l, int r, char x) //Binary search method for where to insert new item. If item found then return -1
{																//If the item is already in the list then return it's position to be inserted.
	if (r >= l) {
		int mid = l + (r - l) / 2;
		if (vec[mid]->letter== x)
			return -1;

		if (vec[mid]->letter > x)
			return binarySearch(vec, l, mid - 1, x);


		return binarySearch(vec, mid + 1, r, x);
	}
	if (l < vec.size()) {
		if (x < vec[l]->letter)
			return l;
		else
			return l + 2;
	}
	else {
		return l;
	}
}

void searchWord(int i, string key, LetterNode*& node, fstream& outputFile) {
	char letter;
	if (i < key.length()) {
		letter = key[i];
		bool nextFound = false;
		LetterNode* nextnode;
		for (size_t j = 0; j < node->children.size(); j++) {
			if (node->children[j]->letter == letter) {
				nextnode = node->children[j];
				nextFound = true;
			}
		}
		if (nextFound == true) {
			i++;
			searchWord(i, key, nextnode, outputFile);
		}
		else {
			if (i == 0) { // if item is not referenced by root.
				outputFile << "\"no record\"" << endl;
				return;
			}
			else { //if item is referenced by root but remainder is not.
				 outputFile<< "\"incorrect Dothraki word\"" << endl;
				return;
			}
		}
	
	}

	else {
		if (node->val == "+") { //If input exists but it's value is empty.
			outputFile << "\"not enough Dothraki word\"" << endl;
			return;
		}
		else {
			outputFile << "\"The English equivalent is " << node->val <<"\""<< endl;
			return;
		}
	}
		
		

}
void insertWord(int i, string key, string val, LetterNode*& node, fstream& outputFile) {
	if (i < key.length()) {
		char letter = key[i];
		bool nextFound = false;
		LetterNode* nextnode;
		for (size_t j = 0; j < node->children.size(); j++) {
			if (node->children[j]->letter == letter) {
				nextnode = node->children[j];
				nextFound = true;
			}
		}
		if (nextFound == true) { // if child is found then keep going.
			i++;
			insertWord(i, key, val,nextnode, outputFile);
		}
		else { //if child is not found then create a new node and add it properly by using binary search method.
			LetterNode* newNode = new LetterNode(letter);
			if (node->children.size()==0)
				node->children.push_back(newNode);
			else {
				int size = static_cast<int>(node->children.size());
				int pos = binarySearch(node->children, 0,  size- 1, letter);
				if (pos == 0) 
					node->children.insert(node->children.begin(), newNode);
				else if (pos == size)
					node->children.push_back(newNode);
				else {
					node->children.insert(node->children.begin() + pos, newNode);
				}
			}
			i++;
			insertWord(i, key, val, newNode, outputFile);
		}
	}
	else { // if reading the word is finished.
		if (node->val == "+") { //if the node's value is empty.
			node->val = val;
			outputFile <<"\""<< key << "\" was added" << endl;
		}
		else {
			if (node->val == val) { //if the value already exists.
				outputFile <<"\""<< key << "\" already exist" << endl;
			}
			else { // if not then update it.
				node->val = val;
				outputFile <<"\""<< key << "\" was updated" << endl;
			}
		}
	}
}
void listWords(LetterNode*& node, string substr, int tabcount, fstream& outputFile) {
	string defaultstr = substr; //when we go to another child use default string and tab count.
	int defaultcount = tabcount;
	size_t i = 0;
	while (i < node->children.size()) {
		LetterNode*& nextnode = node->children[i];
		substr += nextnode->letter; //update the substring.
		if (nextnode->children.size() > 1) { // if the node will be splitted into branches.
			outputFile << string(tabcount, '\t') << "-" << substr;
			if (nextnode->val.compare("+") != 0) {
				outputFile << "(" << nextnode->val << ")";
			}
			outputFile << endl;
				
			tabcount++; //increase the tab count.
		}
		else if (nextnode->children.size() <=1 && nextnode->val.compare("+")!=0) { //if not then just print the key and do nothing.
			outputFile << string(tabcount, '\t')<< "-"<< substr<< "(" << nextnode->val << ")" << endl;
		}
		listWords(nextnode, substr,  tabcount, outputFile);
		substr = defaultstr;
		tabcount = defaultcount;
		i++;
	}
}
void deleteWord(int i, string key, LetterNode* prevnode,  LetterNode*& node, fstream& outputFile) {
	char letter;
	string val;
	if (i < key.length()) {
		letter = key[i];
		bool nextFound = false;
		LetterNode* nextnode;
		for (size_t j = 0; j < node->children.size(); j++) {
			if (node->children[j]->letter == letter) {
				nextnode = node->children[j];
				nextFound = true;
			}
		}
		if (nextFound) {
			i++;
			deleteWord(i, key, node,nextnode, outputFile);
		}
		else {
			if (i == 0) { // if item is not referenced by root.
				outputFile << "\"no record\"" << endl;
				willDeleted = false;
				return;
			}
			else { //if item is referenced by root but remainder is not.
				outputFile << "\"incorrect Dothraki word\"" << endl;
				willDeleted = false;
				return;
			}
		}

	}

	else {
		if (node->val == "+") { //If input exists but it's value is empty.
			outputFile << "\"not enough Dothraki word\"" << endl;
			willDeleted = false;
			return;
		}
		else {
			willDeleted = true; //start deleting nodes.
			outputFile << "\"" << key << "\" deletion is successful" << endl;
			val = node->val;
		}
	}
	if (willDeleted) { // if we need to delete the node then keep deleting while turning back to initial calling of recursive function. 
					   //Else do nothing.
		size_t index = 0;
		for (size_t j = 0; j < prevnode->children.size(); j++) {
			if (prevnode->children[j]->letter== node->letter) {
				index = j;
			}
		}
		if (node->children.size() == 0 && (node->val=="+" || node->val == val) && prevnode->letter != '*') { //keep deleting it.
			delete prevnode->children[index];
			prevnode->children.erase(prevnode->children.begin() + index);
		}
		else if (node->children.size()!=0) {//if the node is splitted into branches then stop deleting.
			node->val = "+";
			willDeleted = false;
		}
		else {
			willDeleted = false;
		}

	}

}
void readAndExecute(string inputName, string outputName) {
	LetterNode* root = new LetterNode();
	fstream outputFile(outputName, std::fstream::out);
	fstream inputFile(inputName, std::fstream::in);
	string text;

	while (getline(inputFile, text)) {
		if (text.length() > 4) {

			if (text.substr(0, 6) == "insert") { //if "insert" is written in the line.
				string params = text.substr(text.find('(')+1, text.length());
				params.pop_back();
				vector <string> keyval = splitString(params, ',');
				insertWord(0, keyval[0], keyval[1], root, outputFile);
			}
			else if (text.substr(0, 6) == "search") { //if "search" is written in the line.
				string key = text.substr(text.find('(')+1, text.length());
				key.pop_back();
				searchWord(0, key, root, outputFile);
			}
			else if (text.substr(0, 6) == "delete") { //if "delete" is written in the line.
				string key = text.substr(text.find('(') + 1, text.length());
				key.pop_back();
				willDeleted = false;
				deleteWord(0, key, nullptr, root, outputFile);
			}
		}
		else { //if "list" is written in the line.
			listWords(root,"",0, outputFile);
		}
	}
	inputFile.close();
	outputFile.close();

}

int main(int argc, char** argv) {
	readAndExecute(argv[1], argv[2]);
	

}