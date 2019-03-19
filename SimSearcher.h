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
	int num,qsize,ql,sl,kl;
	trie* node[129];
	int* qgram;
	trie()
	{
		qsize = 0;
		ql = -1;
		sl = -1;
		kl = -1;
		num = -1;
		for(int i = 0; i <= 128; i++)node[i] = NULL;
		qgram = NULL;
	}
};

class SimSearcher
{
public:
	int datasz,qgramsz,maxlength;
	std::string datastrings[80011];
	int qsize[80011];
	unsigned qlimit;
	int f[311][311];
	trie* qroot;
	SimSearcher();
	~SimSearcher();
	
	int forcal[200011];
	int occurtime[200011];
	int querytime,filtsz,qthresh;
	int filtans[200011];

	void BuildQgram();
	int CalCulateED(std::string, std::string);
	void insert(trie*, std::string, int);
	void search(trie*, std::string);
	int createIndex(const char *filename, unsigned q);
	int searchJaccard(const char *query, double threshold, std::vector<std::pair<unsigned, double> > &result);
	int searchED(const char *query, unsigned threshold, std::vector<std::pair<unsigned, unsigned> > &result);
};

#endif