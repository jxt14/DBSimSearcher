#include "SimSearcher.h"

using namespace std;

SimSearcher::SimSearcher()
{
}

SimSearcher::~SimSearcher()
{
}

void SimSearcher::insert(trie* rt, string s,int id)
{
	int l = s.length();
//	cout << "insert " << s << " for string " << id << endl;

//	cout << s << endl;
	for (int i = 0; i < l; i++){
		if (rt -> node[(int)s[i]] == NULL){
			rt->node[(int)s[i]] = new trie();
			rt->node[(int)s[i]]->num = ++qgramsz;
		}
		rt = rt -> node[(int)s[i]];
	}
	if (rt->qgram == NULL){
//		cout << "NULL " << endl;
		rt->qgram = new int[datasz + 10];
		rt->qsize = 0;
	}
	if (rt->ql != id){
		qsize[id]++;
		rt->qgram[++rt->qsize] = id;
		rt->ql = id;
		//	cout << "insert at " << rt->num << " " << rt->qsize << endl;
	}
}

void SimSearcher::search(trie* rt, string s)
{
	int l = s.length();
	for (int i = 0; i < l; i++){
		if (rt->node[(int)s[i]] == NULL) return;
		rt = rt -> node[(int)s[i]];
	}
	int tid;
	if (rt->sl == querytime) return;
	for (int i = 1; i <= rt->qsize ; i++){
		tid = rt->qgram[i];
		if (forcal[tid] != querytime){
			forcal[tid] = querytime;
			occurtime[tid] = 0;
		}
		occurtime[tid]++;
		if (occurtime[tid] >= qsize[tid] + qthresh){
			filtsz++;
			filtans[filtsz] = tid;
			occurtime[tid] = -20000000;
		}
	}
	rt->sl = querytime;
}

int SimSearcher::CalCulateED(string s1, string s2)
{
	int l1,l2;
	l1 = s1.length();
	l2 = s2.length();
	for (int i = 0; i <= l1; i++)
		for (int j = 0; j <= l2; j++)f[i][j] = -1;
	for (int i = 0; i <= max(l1,l2); i++){
		if (i <= l1)f[i][0] = i;
		if (i <= l2)f[0][i] = i;
	}
	int tempmin;
	for (int i = 1; i <= l1; i++)
		for (int j = 1; j <= l2; j++){
			tempmin = -1;
			if (f[i-1][j] != -1) tempmin = f[i-1][j] + 1;
			if (f[i][j-1] != -1 && f[i][j-1] + 1 < tempmin) tempmin = f[i][j-1] + 1;
			if (f[i-1][j-1] != -1){
				if (s1[i-1] == s2[j-1] && f[i-1][j-1] < tempmin) tempmin = f[i-1][j-1];
				if (s1[i-1] != s2[j-1] && f[i-1][j-1] + 1 < tempmin) tempmin = f[i-1][j-1] + 1;
			}
			if (tempmin != -1){
				if (f[i][j] == -1 || tempmin < f[i][j])f[i][j] = tempmin;
			}
		}
	return f[l1][l2];

}

void SimSearcher::BuildQgram()
{
	string st,sb;
	qgramsz = 0;
	qroot = new trie();
	qroot -> num = 0;
	for (int i = 0; i < datasz; i++){
		st = datastrings[i];
		qsize[i] = 0;
	//	cout << st << endl;
		for (int j = 1; j <= st.length() - qlimit + 1; j++){
			sb = st.substr(j-1, qlimit);
//			cout << sb << endl;
			insert(qroot, sb, i);
		}
	}
//	cout << "Build QGram Succeed" << endl;
}

int SimSearcher::createIndex(const char *filename, unsigned q)
{
	char c;
	bool p;
	string temp;
	if (q == 0) qlimit = 1;
	else qlimit = q;
	datasz = 0;
	qgramsz = 0;
	querytime = 0;
	maxlength = 0;

	ifstream fp;
	p = false;
	char tp[300011];

	fp.open(filename);
	
	while (getline(fp, temp)) {
		qgramsz += (temp.length() - qlimit + 1);
		datastrings[datasz] = temp;		
		datasz++;
		if (temp.length() > maxlength) maxlength = temp.length();
	}
	fp.close();

	for (int i = 0; i < datasz; i++)forcal[i] = -1;
/*
	for (int i = 1; i <= datasz; i++){
		cout << datastrings[i] << endl;
	}
*/
//	cout << "input succeed" << endl;
	BuildQgram();
	return 0;
}


int SimSearcher::searchJaccard(const char *query, double threshold, vector<pair<unsigned, double> > &result)
{
	result.clear();
	return SUCCESS;
}

int SimSearcher::searchED(const char *query, unsigned threshold, vector<pair<unsigned, unsigned> > &result)
{
	result.clear();
	string tq,tb;
	int len;

	filtsz = 0;

	querytime++;
	tq = query;
	len = tq.length();
	qthresh = - qlimit + 1 - threshold * qlimit;
	
	for (int i = 1; i <= len - qlimit + 1; i++) {
		tb = tq.substr(i-1, qlimit);
		search(qroot, tb);
	}
	
	sort(filtans+1, filtans+1+filtsz);

	int k;
	for (int i = 1; i <= filtsz; i++){
	//	cout << filtans[i] << endl;
		k = CalCulateED(datastrings[filtans[i]], tq);
		if (k <= threshold) {
			result.push_back(make_pair(filtans[i], k));
		}
	}

/*
	int k;
	for (int i = 0; i < datasz; i++){
		k = CalCulateED(datastrings[i], tq);
		if (k <= threshold) {
			result.push_back(make_pair(i, k));
		}
	}
*/
/*
	for (int i = 0; i < result.size(); i++){
		printf("%d %d\n", result[i].first, result[i].second);
	}*/
	return SUCCESS;
}

