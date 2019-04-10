#include "SimSearcher.h"

using namespace std;

int main(int argc, char **argv)
{
	SimSearcher searcher;

	vector<pair<unsigned, unsigned> > resultED;
	vector<pair<unsigned, double> > resultJaccard;

	unsigned q = 3, edThreshold = 2;
	double jaccardThreshold = 0.85;
/*
	searcher.createIndex(argv[1], q);
	searcher.searchJaccard("query", jaccardThreshold, resultJaccard);
	searcher.searchED("query", edThreshold, resultED);
*/
	searcher.createIndex("sample.txt", 2);
	
	string query;
	cin >> query;
	
	searcher.searchED(query.c_str(), 6, resultED);
	cout << endl;
	searcher.searchJaccard(query.c_str(), 0.4, resultJaccard);
	cout << endl;
	searcher.searchED(query.c_str(), 7, resultED);
	cout << endl;
	searcher.searchJaccard(query.c_str(), 0.2, resultJaccard);


	return 0;
}
