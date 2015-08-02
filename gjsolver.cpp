#include <iostream>
#include <sstream>
#include <fstream>
#include <string>
#include <vector>
#include <cstdlib>
#include <utility>


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
		pair<long, long> pivot = (lines[i])[i];
		pair<long, long> factor(pivot.second, pivot.first);
		for (long j = i + 1; j < lines.size(); j++) {
			pair<long, long> multiple(
				(lines[j])[i].first * factor.first,
				(lines[j])[i] * factor.second);
			(lines[j])[i] = 0;
			for (long k = i + 1; k <= lines.size(); k++) {
				(lines[j])[k] = pair( /* (lines[j])[k] - multiple * (lines[i][k] * factor) */
					(lines[j])[k].first * multiple.second * (lines[i])[k].second - multiple.first * (lines[i])[k].first * (lines[j])[k].second,
					(lines[j])[k].second * multiple.first * (lines[i])[k].first - multiple.second * (lines[i])[k].second * (lines[j])[k].first);
			}
		}
	}
	cout << "DIAGONAL FORM" << endl;
	for (int i = 0; i < lines.size(); i++) {
		for (int j = 0; j < lines.size(); j++) {
			cout << lines[i][j].first << "/" << lines[i][j].second << " , ";
		}
		cout << " == " << lines[i][lines.size()].first << " / " << lines[i][lines.size()] << end;
	}

}
		
