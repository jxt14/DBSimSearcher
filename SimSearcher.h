#ifndef DFD
#define DFD
#include <vector>
#include <utility>
#include <iostream>
#include <fstream>
#include <cstdio>
#include <cstdlib>
#include <map>
#include <cstring>
#include <string>
#include <algorithm>

const int SUCCESS = 0;
const int FAILURE = 1;


struct trie
{
	int num,qsize,ql,sl;
	trie* node[129];
	int* qgram;
	trie()
	{
		qsize = 0;
		ql = -1;
		sl = -1;
		num = -1;
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
	unsigned qlimit;
	int f[311][311];
	trie* qroot;
	SimSearcher();
	~SimSearcher();
	
	int querytime,qthresh;

	pii qlists[200011];
	int shortlist[200011];
	int filtsz,shortsz,qsize;//the qgram size of the query string
	int filtans[200011];

	double timebuild,timequery,timedp;

	bool check(trie*, int);
	void BuildQgram();
	int CalCulateED(char*, const char*, int);
	void insert(trie*, char*, int);
	void search(trie*, char*);
	int createIndex(const char *filename, unsigned q);
	int searchJaccard(const char *query, double threshold, std::vector<std::pair<unsigned, double> > &result);
	int searchED(const char *query, unsigned threshold, std::vector<std::pair<unsigned, unsigned> > &result);
};

#endif