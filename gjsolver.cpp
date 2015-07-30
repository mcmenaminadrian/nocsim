#include <iostream>
#include <sstream>
#include <fstream>
#include <string>
#include <vector>
#include <cstdlib>


//Gauss-Jordan elimination

using namespace std;


int main()
{
	string path("./variables.csv");
	ifstream inputFile(path);

	vector<long> answers;
	//answer line
	string rawAnswer;
	getline(inputFile, rawAnswer);
	istringstream stringy(rawAnswer);
	string number;
	while(getline(stringy, number, ',')) {
		answers.push_back(atol(number.c_str()));
	}

	//now read in the system
	vector<vector<pair<long, long> > > lines;
	while(getline(inputFile, rawAnswer)) {
		istringstream stringy(rawAnswer);
		vector<pair<long, long> > innerLine;
		while (getline(stringy, number, ',')) {
			pair<long, long> addPair(atol(number.c_str()), 1);
			innerLine.push_back(addPair);
		}
		lines.push_back(innerLine);
	}

	for (int i = 0; i < lines.size(); i++) {
		


	return 0;
}
		
