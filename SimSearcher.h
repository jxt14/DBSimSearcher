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
	int* qgram;
	trie()
	{
		qsize = 0;
		ql = -1;
		sl = -1;
		for(int i = 0; i <= 128; i++)node[i] = NULL;
		qgram = NULL;
	}
};

typedef std::pair<int,trie*> pii;

class SimSearcher
{
public:
	int datasz,qgramsz;
	char* datastrings[200011];
	int lendata[200011],lquery;
	unsigned qlimit;
	int f[2][311];
	trie* qroot;
	trie* jacroot;
	std::map<int, std::set<std::string>> jacset;
	std::set<std::string> jacquery;
	int minjac;
	SimSearcher();
	~SimSearcher();
	
	int querytime,qthresh,dptime;

	pii qlists[200011];
	int shortlist[200011];
	int filtsz,shortsz,qsize;//the qgram size of the query string
	int filtans[200011];

	double timebuild,timequery,timedp;

	bool check(trie*, int);
	void BuildJaccard();
	void BuildQgram();
	double CalCulateJaccard(int);
	int CalCulateED(char*, const char*, int, int);
	void insert(trie*, char*, int, int);
	void search(trie*, char*, int);
	int createIndex(const char *filename, unsigned q);
	int searchJaccard(const char *query, double threshold, std::vector<std::pair<unsigned, double> > &result);
	int searchED(const char *query, unsigned threshold, std::vector<std::pair<unsigned, unsigned> > &result);
};

#endif