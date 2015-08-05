#include <iostream>
#include <sstream>
#include <fstream>
#include <string>
#include <vector>
#include <cstdlib>
#include <utility>
#include <gmpxx.h>



//Gauss-Jordan elimination

using namespace std;

void gcd(pair<mpz_class, mpz_class>& input)
{
	mpz_class first = abs(input.first);
	mpz_class second = abs(input.second);
	if (first > second) {
		first = second;
		second = abs(input.first);
	}
	while (second != 0) {
		mpz_class temp = second;
		second = first%second;
		first = temp;
	}
	input.first /= first;
	input.second /= first;
	if (input.second < 0) {
		input.first *= -1;
		input.second *= -1;
	}
}

int main()
{
	string path("./variables.csv");
	ifstream inputFile(path);

	vector<mpz_class> answers;
	//answer line
	string rawAnswer;
	getline(inputFile, rawAnswer);
	istringstream stringy(rawAnswer);
	string number;
	while(getline(stringy, number, ',')) {
		answers.push_back(atol(number.c_str()));
	}

	//now read in the system
	vector<vector<pair<mpz_class, mpz_class> > > lines;
	while(getline(inputFile, rawAnswer)) {
		istringstream stringy(rawAnswer);
		vector<pair<mpz_class, mpz_class> > innerLine;
		while (getline(stringy, number, ',')) {
			pair<mpz_class, mpz_class>
				addPair(atol(number.c_str()), 1);
			innerLine.push_back(addPair);
		}
		lines.push_back(innerLine);
	}

	for (int i = 0; i < lines.size(); i++) {
		pair<mpz_class, mpz_class> pivot(lines[i][i].second,
			lines[i][i].first);
		if (lines[i][i].first != 0) {
			lines[i][i].first = 1;
			lines[i][i].second = 1;
		} else {
			continue;
		}
		for (int j = i + 1; j <= lines.size(); j++) {
			lines[i][j].first *= pivot.first;
			lines[i][j].second *= pivot.second;
			gcd(lines[i][j]);
		}
		for (int j = i + 1; j < lines.size(); j++) {
			pair<mpz_class, mpz_class> multiple(lines[j][i].first,
				lines[j][i].second);	
			lines[j][i] = pair<mpz_class, mpz_class>(0, 1);
			for (int k = i + 1; k <= lines.size(); k++) {
				pair<mpz_class, mpz_class>
					factor(
					multiple.first * lines[i][k].first,
					multiple.second * lines[i][k].second);
				gcd(factor);
				lines[j][k] = pair<mpz_class, mpz_class>
					(lines[j][k].first * factor.second -
					factor.first * lines[j][k].second,
					lines[j][k].second * factor.second);
				gcd(lines[j][k]);
			}
			
		}
	}
	cout << "DIAGONAL FORM" << endl;
	for (int i = 0; i < lines.size(); i++) {
		for (int j = 0; j < lines.size(); j++) {
			if (lines[i][j].first == 0) {
				cout << "0 , ";
			} else {
				cout << lines[i][j].first << "/" << lines[i][j].second << " , ";
			}
		}
		cout << " == " << lines[i][lines.size()].first << " / " << lines[i][lines.size()].second << endl;
	}

}
		
