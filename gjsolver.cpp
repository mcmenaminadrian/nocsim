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
	vector<vector<long> > lines;
	while(getline(inputFile, rawAnswer)) {
		istringstream stringy(rawAnswer);
		vector<long> innerLine;
		while (getline(stringy, number, ',')) {
			 innerLine.push_back(atol(number.c_str()));
		}
		lines.push_back(innerLine);
	}

	for (int i = 0; i < lines.size(); i++) {
			cout << "Line " << i << " is ";
		for (int j = 0; j < (lines[i]).size(); j++) {
			cout << (lines[i])[j] << ",";
		}
		cout << endl;
	}

	return 0;
}
		
