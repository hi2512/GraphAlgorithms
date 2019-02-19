#include <iostream>
#include <fstream>
#include <string>
#include <string.h>
#include <stdlib.h>
#include <vector>
#include <tuple>
#include <algorithm>

using namespace std;


void dimacsToCSR(ifstream & dimacfile, std::vector<int> & rp, std::vector<int> & ci, std::vector<int> & ai) {
	std::vector<tuple<int, int, int>> res;
	string line;
	char fline[50];
	char * tok;
	int nodeFrom, nodeTo, weight;
	while(getline(dimacfile, line)) {
		strcpy(fline, line.c_str());
		strtok(fline, " ");
		tok = strtok(NULL, " ");
		nodeFrom = atoi(tok);
		tok = strtok(NULL, " ");
		nodeTo = atoi(tok);
		tok = strtok(NULL, " ");
		weight = atoi(tok);
		res.push_back(tuple<int, int, int>(nodeFrom, nodeTo, weight));
	}
	//arent they already sorted though?
	sort(res.begin(), res.end(), [](tuple<int, int, int> x, tuple<int, int, int> y) {return get<0>(x) < get<0>(y);});
	int curNode = 0;
	for(auto t : res) {
		cout << get<0>(t) << " " << get<1>(t) << " " << get<2>(t) << endl;
		if(curNode != get<0>(t)) {
			rp.push_back(ci.size() + 1);
		}
		curNode = get<0>(t);
		ci.push_back(get<1>(t));
		ai.push_back(get<2>(t));
	}
}

void csrToDimacs(ofstream & outfile, std::vector<int> & rp, std::vector<int> & ci, std::vector<int> & ai) {
	outfile << "p sp " << rp.size() << " " << ci.size() << endl;
	int rIndex = 0;
	for(int i = 0; i < ci.size(); i++) {
		outfile << "a ";
		if((i + 1) > rp.at(rIndex)) {
			rIndex++;
		}
		outfile << rIndex + 1 << " ";
		outfile << ci.at(i) << " " << ai.at(i) << endl;
	}
}

int main(int argc, char * argv[], char * env[]) {

	ifstream dimacfile(argv[1]);
	if(!dimacfile) {
		cout << "could not open file" << endl;
	}
	string firstline;
	getline(dimacfile, firstline);
	char fline[50];
	strcpy(fline, firstline.c_str());

	char * tok;
	//p
	strtok(fline, " ");
	//sp
	strtok(NULL, " ");
	tok = strtok(NULL, " ");
	int nodes = atoi(tok);
	tok = strtok(NULL, " ");
	int edges = atoi(tok);

	vector<int> rp;
	vector<int> ci;
	vector<int> ai;
	dimacsToCSR(dimacfile, rp, ci, ai);
	for(auto i : rp) {
		cout << i << " ";
	}
	cout << endl;
	for(auto i : ci) {
		cout << i << " ";
	}
	cout << endl;
	for(auto i : ai) {
		cout << i << " ";
	}
	cout << endl;

	ofstream outfile;
	outfile.open("1.dimacs");
	csrToDimacs(outfile, rp, ci, ai);
	outfile.close();

	return 0;
}