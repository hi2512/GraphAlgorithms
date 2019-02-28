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

void scaleValues(vector<PageNode> * pn) {
	double sum = 0;
	for(PageNode n : *pn) {
		sum += n.curVal;
	}
	for(PageNode& n : *pn) {
		n.curVal /= sum;
	}
}

// Checks to see if change below the threshold has occured
bool terminateThreshold(vector<PageNode>* pn, double threshold) {
	for(auto it = pn->begin(); it != pn->end(); ++it) {
		PageNode cur = *it;
		if(abs(abs(cur.curVal) - abs(cur.prevVal)) > threshold) {
			return false;
		}
	}
	return true;
}

void printSum(vector<PageNode>* pn) {
	double sum = 0;
	for(PageNode n : *pn) {
		sum += n.curVal;
	}
	cout << "SUM: " << sum << endl;
}

void printPageVals(vector<PageNode>* pn) {
	double sum = 0;
	for(PageNode n : *pn) {
		cout << "Node number: " << n.number << " Cur Val: " << n.curVal << endl;
		sum += n.curVal;
	}
	cout << "SUM: " << sum << endl;
}

// Calculates page rank values on a set of data
vector<PageNode>* doPageRank(std::vector<int> & rp, std::vector<int> & ci, std::vector<int> & ai, std::vector<int> nodeLabels, int nodeNum, double damp, double threshold) {
	vector<PageNode>* pns = new vector<PageNode>();
	for(int i = 1; i <= nodeNum; i++) {
		pns->push_back(PageNode{i, i, 0, 1.f / nodeNum, 1.f});
	}
	// Set outlinks
	for(int i = 0; i < rp.size(); i++) {
		if(i == (rp.size() - 1)) {
			pns->at(nodeLabels.at(i) - 1).outLinks = ci.size() + 1 - rp.at(i);
		} else {
			pns->at(nodeLabels.at(i) - 1).outLinks = rp.at(i + 1) - rp.at(i);
		}
	}
	do {
		// Set previous value to value of last iteration
		for(PageNode& n : *pns) {
			n.prevVal = n.curVal;
			n.curVal = 0;
		}
		// Gather sum
		for(int i = 0; i < rp.size(); i++) {
			int ciIndex = rp.at(i) - 1;
			int linkNum = pns->at(nodeLabels.at(i) - 1).outLinks;
			// Run through all links
			for(int j = 0; j < linkNum; j++) {
				PageNode& fromNode = pns->at(nodeLabels.at(i) - 1);
				PageNode& toNode =  pns->at(ci.at(ciIndex + j) - 1);
				toNode.curVal += /*ai.at(ciIndex + j) * */
					fromNode.prevVal / fromNode.outLinks;
			}
		}
		for(PageNode& n : *pns) {
			n.curVal = ((1.0 - damp) / nodeNum) + (n.curVal * damp);
		}
	} while(!terminateThreshold(pns, threshold));
	scaleValues(pns);
	printPageVals(pns);

}

bool tupleOrder(tuple<int, int, int> x, tuple<int, int, int> y) {
	if((get<0>(x) == get<0>(y)) && (get<1>(x) == get<1>(y))) {
		return get<2>(x) > get<2>(y);
	} else if(get<0>(x) == get<0>(y)) {
		return get<1>(x) < get<1>(y);
	} else {
		return get<0>(x) < get<0>(y);
	}
}

// Converts from DIMACS format to CSR format
void dimacsToCSR(ifstream & dimacfile, std::vector<int> & rp, std::vector<int> & ci, std::vector<int> & ai, std::vector<int> & nodeLabels,
				std::vector<tuple<int, int, int>> & res) {
	string line;
	char fline[50];
	int nodeFrom, nodeTo, weight;

	// Read in from file
	while(getline(dimacfile, line)) {
		strcpy(fline, line.c_str());
		strtok(fline, " ");
		nodeFrom = atoi(strtok(NULL, " "));
		nodeTo = atoi(strtok(NULL, " "));
		weight = atoi(strtok(NULL, " "));
		res.push_back(tuple<int, int, int>(nodeFrom, nodeTo, weight));
	}

	sort(res.begin(), res.end(), tupleOrder);

	// Keeps only highest weight duplicates
	res.erase(unique(res.begin(), res.end(), [](tuple<int, int, int> x, tuple<int, int, int> y) {return (get<0>(x) == get<0>(y)) && (get<1>(x) == get<1>(y));}), res.end());
	int curNode = 0;
	for(auto t : res) {
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

// Converts from CSR format to DIMACS format
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

// Outputs the histogram to a file
void output_histogram(ofstream& outfile, std::vector<int> & rp, std::vector<int> & ci, std::vector<int> & ai, std::vector<int> nodeLabels, int nodeNum) {
	vector<PageNode>* pns = new vector<PageNode>();
	for(int i = 1; i <= nodeNum; i++) {
		pns->push_back(PageNode{i, i, 0, 1.f / nodeNum, 1.f});
	}
	// Analyze the outgoing edges from each node
	// Keep trakc of the max number of outgoing edges
	int maxLinks = -1;
	for(int i = 0; i < rp.size(); i++) {
		if(i == (rp.size() - 1)) {
			int numLinks = ci.size() + 1 - rp.at(i);
			pns->at(nodeLabels.at(i) - 1).outLinks = numLinks;
			if(numLinks > maxLinks) {
				maxLinks = numLinks;
			}
		} else {
			int numLinks = rp.at(i + 1) - rp.at(i);
			pns->at(nodeLabels.at(i) - 1).outLinks = numLinks;
			if(numLinks > maxLinks) {
				maxLinks = numLinks;
			}
		}
	}

	// Determine the frequency of nodes for a given number of outgoing edges
	vector<int> links(maxLinks + 1, 0);
	for(int j = 0; j < pns->size(); ++j) {
		++links[(*pns)[j].outLinks];
	}

	// Output to file
	for(int k = 0; k < links.size(); ++k) {
		outfile << k << " " << links[k] << endl;
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
	// p
	strtok(fline, " ");
	// sp
	strtok(NULL, " ");
	tok = strtok(NULL, " ");
	int nodes = atoi(tok);
	tok = strtok(NULL, " ");
	int edges = atoi(tok);

	// COO format data
	vector<int>* rp = new vector<int>();
	vector<int>* ci = new vector<int>();
	vector<int>* ai = new vector<int>();
	vector<int>* nodeLabels = new vector<int>();
	vector<tuple<int, int, int>>* res = new vector<tuple<int, int, int>>();

	// Converting to CSR
	dimacsToCSR(dimacfile, *rp, *ci, *ai, *nodeLabels, *res);
	ofstream lfile;
	lfile.open("labels.txt");
	for(int i = 0; i < nodeLabels->size(); i++) {
		lfile << i + 1 << " " << nodeLabels->at(i) << endl;
	}
	lfile.close();

	// Histogram
	cout << "Creating Histogram:\n";
	ofstream histogram_outfile;
	histogram_outfile.open("histogram.txt");
	output_histogram(histogram_outfile, *rp, *ci, *ai, *nodeLabels, nodes);
	histogram_outfile.close();
	cout << "Done Creating Histogram\n";

	// Output to dimacs
	ofstream outfile;
	outfile.open("out.dimacs");
	csrToDimacs(outfile, *rp, *ci, *ai, *nodeLabels, nodes);
	outfile.close();

	delete(doPageRank(*rp, *ci, *ai, *nodeLabels, nodes, 0.85, 0.001));

	return 0;
}