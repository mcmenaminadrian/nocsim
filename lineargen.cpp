#include <iostream>
#include <fstream>
#include <random>

#define RANGE 1024

using namespace std;

default_random_engine generator;
uniform_int_distribution<long> distribution(-1024, 1024);

int main()
{
	vector<long> variables;
	ofstream outfile("varaibles.csv", ios::out);
	//generate variables
	for (int i = 0; i < RANGE; i++) {
		if (i > 0) {
			outfile << ",";
		}
		variables.push_back(distribution(generator));
		outfile << variables[i];
	}
	outfile << endl;
	//generate linear equations
	
	for (int i = 0; i < RANGE; i++) {
		long total = 0;
		for (int j = 0; j < RANGE; j++) {
			if (j > 0) {
				outfile << ",";
			}
			long multiple = distribution(generator);
			total += multiple * variables[j];
			outfile << multiple;
		}
		outfile << "," << total << endl;
	}
	outfile.close();
}
		
