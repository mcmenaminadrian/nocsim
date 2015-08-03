#include <iostream>
#include <sstream>
#include <fstream>
#include <string>
#include <vector>
#include <cstdlib>
#include <utility>


//Gauss-Jordan elimination

using namespace std;

void gcd(pair<long, long>& input)
{
	long first = abs(input.first);
	long second = abs(input.second);
	if (first > second) {
		first = second;
		second = abs(input.first);
	}
	while (second != 0) {
		long temp = second;
		second = first%second;
		first = temp;
	}
	input.first /= first;
	input.second /= first;
	cout << " GCD: " << first << "   ";
}

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
		pair<long, long> factor(lines[i][i].second, lines[i][i].first);
		for (int j = i; j <= lines.size(); j++) {
			lines[i][j].first *= factor.first;
			lines[i][j].second *= factor.second;
		}
		for (long j = i + 1; j < lines.size(); j++) {
			pair<long, long> multiple(lines[j][i].first, lines[j][i].second);
			cout << i << ":" << j << " --- factor is " << multiple.first << " / " << multiple.second << endl;
			lines[j][i] = pair<long, long>(0, 0);
			for (long k = i + 1; k <= lines.size(); k++) {
				lines[j][k] = pair<long, long>( /* (lines[j])[k] - multiple * (lines[i][k] * factor) */
					lines[j][k].first * multiple.second * lines[i][k].second - lines[i][k].first * multiple.first * lines[j][k].second,
					lines[j][k].second * multiple.first * lines[i][k].first - lines[i][k].second * multiple.second * lines[i][k].first);
				cout << "BEFORE: " << lines[j][k].first << " / " << lines[j][k].second;
				gcd(lines[j][k]);
				cout << "     AFTER: " << lines[j][k].first << " / " << lines[j][k].second << endl;
			}
		}
	}
	cout << "DIAGONAL FORM" << endl;
	for (int i = 0; i < lines.size(); i++) {
		for (int j = 0; j < lines.size(); j++) {
			cout << lines[i][j].first << "/" << lines[i][j].second << " , ";
		}
		cout << " == " << lines[i][lines.size()].first << " / " << lines[i][lines.size()].second << endl;
	}

}
		
