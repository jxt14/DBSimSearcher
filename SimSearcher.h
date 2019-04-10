#ifndef DFD
#define DFD
#include <vector>
#include <utility>
#include <iostream>
#include <fstream>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <string>
#include <algorithm>
#include <set>
#include <cmath>
#include <map>

const int SUCCESS = 0;
const int FAILURE = 1;


struct trie
{
	int qsize,ql,sl;
	trie* node[129];
	std::vector<int> qgram;
	trie()
	{
		qsize = 0;
		ql = -1;
		sl = -1;
		for(int i = 0; i <= 128; i++)node[i] = NULL;
		qgram.clear();
		qgram.push_back(0);
	}
};

typedef std::pair<int,trie*> pii;

class SimSearcher
{
public:
	int datasz,qgramsz;
	std::vector<std::string> datastrings;
	int lendata[200011],lquery;
	unsigned qlimit;
	trie* qroot;
	trie* jacroot;
	std::map<int, std::set<unsigned>> jacset;
	int minjac;
	SimSearcher();
	~SimSearcher();
	
	int querytime,qthresh,dptime;

	pii qlists[200011];
	int querycheck[200011],occurtime[200011];
	int qsize;//the qgram size of the query string

	double timebuild,timequery,timedp;

	bool check(trie*, int);
	void BuildJaccard();
	void BuildQgram();
	double CalCulateJaccard(std::set<unsigned>&, std::set<unsigned>&);
	int CalCulateED(const char*, int, const char*, int, int);
	void insert(trie*, const char*, int, int);
	void search(trie*, const char*, int);
	int createIndex(const char *filename, unsigned q);
	int searchJaccard(const char *query, double threshold, std::vector<std::pair<unsigned, double> > &result);
	int searchED(const char *query, unsigned threshold, std::vector<std::pair<unsigned, unsigned> > &result);
};

#endif