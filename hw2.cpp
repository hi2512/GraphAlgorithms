#include <iostream>
#include <fstream>
#include <string>
#include <string.h>
#include <stdlib.h>
#include <vector>
#include <tuple>
#include <algorithm>
#include <cmath>

using namespace std;


struct PageNode {
	int number;
	int label;
	int outLinks;
	double curVal;
	double prevVal;
};

bool terminateThreshold(vector<PageNode> pn, double threshold) {
	for(auto it = pn.begin(); it != pn.end(); ++it) {
		PageNode cur = *it;
		if(abs(abs(cur.curVal) - abs(cur.prevVal)) > threshold) {
			return false;
		}
	}
	return true;
}

void printPageVals(vector<PageNode> & pn) {
	double sum = 0;
	for(PageNode n : pn) {
		cout << "Node number: " << n.number << " Label: " << n.label << " Out Links: " << n.outLinks << endl;
		cout << "Cur Val: " << n.curVal << " Prev Val: " << n.prevVal << endl;
		sum += n.curVal;
	}
	cout << "SUM: " << sum << endl;
}

void doPageRank(std::vector<int> & rp, std::vector<int> & ci, std::vector<int> & ai, std::vector<int> nodeLabels, int nodeNum, double damp, double threshold) {
	vector<PageNode> pns;
	for(int i = 1; i <= nodeNum; i++) {
		pns.push_back(PageNode{i, i, 0, 0.0, 1.0 / nodeLabels.size()});
	}
	//set outlinks
	for(int i = 0; i < rp.size(); i++) {
		if(i == (rp.size() - 1)) {
			pns.at(nodeLabels.at(i) - 1).outLinks = ci.size() + 1 - rp.at(i);
		} else {
			pns.at(nodeLabels.at(i) - 1).outLinks = rp.at(i + 1) - rp.at(i);
		}
	}

	//do inital iteration
	for(PageNode& n : pns) {
		//check for nodes going to this one
		double inSum = 0;
		int rowCount = 0;
		for(int i = 0; i < ci.size(); i++) {
			if((rowCount+ 1 < rp.size()) && (rp.at(rowCount + 1) - 1 == i)) {
				rowCount++;
			}
			if(ci.at(i) == n.number) {
				cout << "insum for "   << n.label << ": " << pns.at(nodeLabels.at(rowCount) - 1).prevVal / pns.at(nodeLabels.at(rowCount) - 1).outLinks << " link from " << nodeLabels.at(rowCount);
				cout << " val: " << pns.at(nodeLabels.at(rowCount) - 1).prevVal << " outlinks: " << pns.at(nodeLabels.at(rowCount) - 1).outLinks << endl;
				inSum += pns.at(nodeLabels.at(rowCount) - 1).prevVal / pns.at(nodeLabels.at(rowCount) - 1).outLinks;
			}
		}
		n.curVal = ((1.0 - damp) / nodeNum) + (inSum * damp);
		//n.curVal = inSum;
	}
	/*
	printPageVals(pns);
	for(PageNode& n : pns) {
			n.prevVal = n.curVal;
	}
	for(PageNode& n : pns) {
		//check for nodes going to this one
		double inSum = 0;
		int rowCount = 0;
		for(int i = 0; i < ci.size(); i++) {
			if((rowCount+ 1 < rp.size()) && (rp.at(rowCount + 1) - 1 == i)) {
				rowCount++;
			}
			if(ci.at(i) == n.number) {
				cout << "insum for "   << n.label << ": " << pns.at(nodeLabels.at(rowCount) - 1).prevVal / pns.at(nodeLabels.at(rowCount) - 1).outLinks << " link from " << nodeLabels.at(rowCount);
				cout << " val: " << pns.at(nodeLabels.at(rowCount) - 1).prevVal << " outlinks: " << pns.at(nodeLabels.at(rowCount) - 1).outLinks << endl;
				inSum += pns.at(nodeLabels.at(rowCount) - 1).prevVal / pns.at(nodeLabels.at(rowCount) - 1).outLinks;
			}
		}
		n.curVal = ((1.0 - damp) / rp.size()) + (inSum * damp);
		//n.curVal = inSum;
	}
	*/
	while(!terminateThreshold(pns, threshold)) {
		for(PageNode& n : pns) {
			n.prevVal = n.curVal;
		}
	for(PageNode& n : pns) {
		//check for nodes going to this one
		double inSum = 0;
		int rowCount = 0;
		for(int i = 0; i < ci.size(); i++) {
			if((rowCount+ 1 < rp.size()) && (rp.at(rowCount + 1) - 1 == i)) {
				rowCount++;
			}
			if(ci.at(i) == n.number) {
				cout << "insum for "   << n.label << ": " << pns.at(nodeLabels.at(rowCount) - 1).prevVal / pns.at(nodeLabels.at(rowCount) - 1).outLinks << " link from " << nodeLabels.at(rowCount);
				cout << " val: " << pns.at(nodeLabels.at(rowCount) - 1).prevVal << " outlinks: " << pns.at(nodeLabels.at(rowCount) - 1).outLinks << endl;
				inSum += pns.at(nodeLabels.at(rowCount) - 1).prevVal / pns.at(nodeLabels.at(rowCount) - 1).outLinks;
			}
		}
		n.curVal = ((1.0 - damp) / nodeNum) + (inSum * damp);
		//n.curVal = inSum;
	}
	}
	cout << "FINAL" << endl;
	printPageVals(pns);


}

void dimacsToCSR(ifstream & dimacfile, std::vector<int> & rp, std::vector<int> & ci, std::vector<int> & ai, std::vector<int> & nodeLabels) {
	std::vector<tuple<int, int, int>> res;
	string line;
	char fline[50];
	int nodeFrom, nodeTo, weight;
	while(getline(dimacfile, line)) {
		strcpy(fline, line.c_str());
		strtok(fline, " ");
		nodeFrom = atoi(strtok(NULL, " "));
		nodeTo = atoi(strtok(NULL, " "));
		weight = atoi(strtok(NULL, " "));
		res.push_back(tuple<int, int, int>(nodeFrom, nodeTo, weight));
	}
	//arent they already sorted though?
	sort(res.begin(), res.end(), [](tuple<int, int, int> x, tuple<int, int, int> y) {return get<0>(x) < get<0>(y);});
	int curNode = 0;
	for(auto t : res) {
		cout << get<0>(t) << " " << get<1>(t) << " " << get<2>(t) << endl;
		if(!count(nodeLabels.begin(), nodeLabels.end(), get<0>(t))) {
			nodeLabels.push_back(get<0>(t));
		}
		if(curNode != get<0>(t)) {
			rp.push_back(ci.size() + 1);
		}
		curNode = get<0>(t);
		ci.push_back(get<1>(t));
		ai.push_back(get<2>(t));
	}
}

void csrToDimacs(ofstream & outfile, std::vector<int> & rp, std::vector<int> & ci, std::vector<int> & ai, std::vector<int> & nodeLabels, int nodeNum) {
	outfile << "p sp " << nodeNum << " " << ci.size() << endl;
	int rIndex = 0;
	for(int i = 0; i < ci.size(); i++) {
		outfile << "a ";
		if((rIndex + 1 < rp.size()) && (rp.at(rIndex + 1) - 1 == i)) {
			rIndex++;
		}
		outfile << nodeLabels.at(rIndex) << " ";
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
	vector<int> nodeLabels;
	dimacsToCSR(dimacfile, rp, ci, ai, nodeLabels);
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
	for(auto i : nodeLabels) {
		cout << i << " ";
	}
	cout << endl;

	ofstream outfile;
	outfile.open("1.dimacs");
	csrToDimacs(outfile, rp, ci, ai, nodeLabels, nodes);
	outfile.close();

	doPageRank(rp, ci, ai, nodeLabels, nodes, 0.85, 0.001);

	return 0;
}