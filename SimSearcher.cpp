#include "SimSearcher.h"

using namespace std;

SimSearcher::SimSearcher()
{
}

SimSearcher::~SimSearcher()
{
}

inline int my_abs(int x)
{
	if (x > 0) return x;
	else return -x;
}

inline int my_min(int a, int b)
{
	if (a < b) return a;
	else return b;
}

inline int my_max(int a, int b)
{
	if (a > b) return a;
	else return b;
}

inline int min_3(int a, int b, int c)
{
	return my_min(my_min(a,b), c);
}

inline unsigned hashh(const char* s, int len)
{
	unsigned h;
	h = 0;
	for (int i = 0; i < len; i++) {
		if (s[i] == ' ') break;
		h = h * 157 + (int)s[i];
	}
	return h;
}

void SimSearcher::insert(trie* rt, const char* s, int id, int tlim)
{
//	cout << "insert " << s << " for string " << id << endl;

//	cout << s << endl;
	for (int i = 0; i < tlim; i++){
		if (rt -> node[(int)s[i]] == NULL){
			rt->node[(int)s[i]] = new trie();
		}
		rt = rt -> node[(int)s[i]];
	}
	if (rt->ql != id){
		rt->qgram.push_back(id);
		rt->qsize++;
		rt->ql = id;
		//	cout << "insert at " << rt->num << " " << rt->qsize << endl;
	}
}

void SimSearcher::search(trie* rt, const char* s, int tlim)
{
	for (int i = 0; i < tlim; i++){
		if (rt->node[(int)s[i]] == NULL) return;
		rt = rt -> node[(int)s[i]];
	}
	int tid;
//	if (rt->sl == querytime) return;
	qsize++;
	qlists[qsize] = make_pair(rt->qsize, rt);
	rt->sl = querytime;
}

double SimSearcher::CalCulateJaccard(set<unsigned> &set1, set<unsigned> &set2)
{
	int tt = 0;
	for (auto l: set1) {
		if (set2.find(l) != set2.end())tt++;
	}
	return (double)(tt)/(double)(set1.size() + set2.size() - tt);
}

int SimSearcher::CalCulateED(const char* s1, int m, const char* s2, int n, int threshold)
{
    if (my_abs(m - n) > threshold)
        return threshold + 1;
    int f[m+1][n+1];
    for (int i = 0; i <= my_min(threshold, m); i++)f[i][0] = i;
    for (int i = 0; i <= my_min(threshold, n); i++)f[0][i] = i;
    for (int i = 1; i <= m; i++)
    {
        int limmin = my_max(i - threshold, 1);
        int limmax = my_min(i + threshold, n);
        if (limmin > limmax)break;
        for (int j = limmin; j <= limmax; j++)
        {
            int d1 = my_abs(i - 1 - j) > threshold ? threshold + 1 : f[i - 1][j];
            int d2 = my_abs(i - j + 1) > threshold ? threshold + 1 : f[i][j - 1];
            f[i][j] = min_3(d1 + 1, d2 + 1, f[i - 1][j - 1] + (s1[i-1] != s2[j-1]));
        }
    }
    return f[m][n];

}


void SimSearcher::BuildJaccard()
{
	minjac = 2147483647;
	string tempt;
	jacroot = new trie();
	int inx,ts;
	for (int i = 0; i < datasz; i++){
		jacset[i].clear();
		inx = 0;
		for (int j = 0; j < lendata[i]; j++){
			if (datastrings[i][j] == ' '){
				if (inx < j) {
					tempt = datastrings[i].substr(inx, j - inx);
					insert(jacroot, tempt.c_str(), i, j - inx);
					jacset[i].insert(hashh(tempt.c_str(), j - inx));
				}
				inx = j + 1;
			}
		}
		if (inx < lendata[i]) {
			tempt = datastrings[i].substr(inx, lendata[i] - inx);
			insert(jacroot, tempt.c_str(), i, lendata[i] - inx);
			jacset[i].insert(hashh(tempt.c_str(), lendata[i] - inx));
		}
		if (jacset[i].size() < minjac) minjac = jacset[i].size();
	}
//	cout << "build jaccard finished" << endl;
}

void SimSearcher::BuildQgram()
{
	string tempt;
	qroot = new trie();
	for (int i = 0; i < datasz; i++){
		for (int j = 1; j <= lendata[i] - qlimit + 1; j++){
//			cout << sb << endl;
			tempt = datastrings[i].substr(j-1, qlimit);
			insert(qroot, tempt.c_str(), i, qlimit);
		}
	}
//	cout << "build ed finished" << endl;
}

int SimSearcher::createIndex(const char *filename, unsigned q)
{
	char c;
	bool p;
	if (q == 0) qlimit = 1;
	else qlimit = q;
	datasz = 0;
	querytime = 0;
	dptime = 0;
//	timedp = 0;
//	timequery = 0;
	int len;
	ifstream fp;
	p = false;

	fp.open(filename);
	
	string temp;
	while (getline(fp, temp)) {
		len = temp.length();
		datastrings.push_back(temp);
		querycheck[datasz] = 0;
		lendata[datasz] = len;
		datasz++;
	}
	fp.close();

/*
	for (int i = 0; i < datasz; i++){
		cout << datastrings[i] << endl;
	}*/

	BuildQgram();
	BuildJaccard();
	return 0;
}

bool SimSearcher::check(trie* rt, int bz)
{
	int l,r,mid;
	l = 1;
	r = rt->qsize;
	while (l <= r) {
		mid = (l+r) / 2;
		if (rt->qgram[mid] < bz) l = mid + 1;
		else r = mid - 1;
	}
	if (l <= rt->qsize && rt->qgram[l] == bz )return true;
	return false;
}

int SimSearcher::searchJaccard(const char *query, double threshold, vector<pair<unsigned, double> > &result)
{
	result.clear();

	string tempt(query);
	int inx,len,listdec;
	vector<int> shortlist;
	set<string> queryword;
	set<unsigned> jacquery;

	querytime++;
	qsize = 0;
	len = strlen(query);
	lquery = len;
	inx = 0;
	
	int k,t;
	string ts;

	inx = 0;
	for (int i = 0; i < len; i++){
		if (query[i] == ' '){
			if (inx < i) {
				ts = tempt.substr(inx, i - inx);
				queryword.insert(ts);
				jacquery.insert(hashh(ts.c_str(), i - inx));
			}
			inx = i + 1;
		}
	}	

	if (inx < len) {
		ts = tempt.substr(inx, len - inx);
		queryword.insert(ts);
		jacquery.insert(hashh(ts.c_str(), len - inx));
	}

	for (auto w: queryword) {
		search(jacroot, (char*)w.c_str(), w.length());
	}

	double fv;
	double thresh1,thresh2;
	thresh1 = jacquery.size() * threshold;
	thresh2 = (minjac + jacquery.size()) * threshold / (1.0 + threshold);
	if (ceil(thresh1) > ceil(thresh2)) qthresh = ceil(thresh1);
	else qthresh = ceil(thresh2);

	if (qthresh <= 0) {
		for (int i = 0; i < datasz; i++) {
			fv = CalCulateJaccard(jacset[i], jacquery);
			if (fv >= threshold) result.push_back(make_pair(i, fv)); 
		}
	}
	else {

	if (qthresh - 1 < 0) listdec = 0;
	else listdec = qthresh - 1;
	listdec = qsize - listdec;
	if (listdec < 0) return SUCCESS;
	sort(qlists + 1, qlists + 1 + qsize);
	
	for (int i = 1; i <= listdec; i++){
		for (int j = 1; j <= qlists[i].second->qsize; j++){
			t = qlists[i].second->qgram[j];
			if (querycheck[t] != querytime) {
				querycheck[t] = querytime;
				occurtime[t] = 0;
				shortlist.push_back(t);
			}
			occurtime[t]++;
		}
	}

	sort(shortlist.begin(), shortlist.end());
	int soc;

	for (int i = 0; i < shortlist.size(); i++){
		t = shortlist[i];
		soc = occurtime[t];
		for (int j = listdec + 1; j <= qsize; j++) {
			if (soc + qsize - j + 1 < qthresh) break;
			if (soc >= qthresh) break;
			if (check(qlists[j].second, t) == true) soc++;
		}
		if (soc >= qthresh) {
			fv = CalCulateJaccard(jacset[t], jacquery);
			if (fv >= threshold) result.push_back(make_pair(t, fv));
		}
	}

	}
/*
	for (int i = 0; i < result.size(); i++){
		printf("%d %.4lf\n", result[i].first, result[i].second);
	}*/

	return SUCCESS;
}

int SimSearcher::searchED(const char *query, unsigned threshold, vector<pair<unsigned, unsigned> > &result)
{
//	clock_t c1,c2;
//	c1 = clock();
	result.clear();

	int k,t;
	int len,qthresh,listdec;
	vector<int> shortlist;

	querytime++;	
	len = strlen(query);
	lquery = len;

	qsize = 0;

	
	for (int i = 1; i <= len - qlimit + 1; i++) {
		search(qroot, query + i - 1, qlimit);
	}
	
	qthresh = lquery - qlimit + 1 - threshold * qlimit;
	
	if (qthresh <= 0) {
		for (int i = 0; i < datasz; i++) {
			k = CalCulateED(datastrings[i].c_str(), lendata[i], query, len, threshold);
			if (k <= threshold) result.push_back(make_pair(i, k)); 
		}
	}

	else {

	if (qthresh - 1 < 0) listdec = 0;
	else listdec = qthresh - 1;
	listdec = qsize - listdec;
	if (listdec < 0) return SUCCESS;
	sort(qlists + 1, qlists + 1 + qsize);
	
	for (int i = 1; i <= listdec; i++){
		for (int j = 1; j <= qlists[i].second->qsize; j++){
			t = qlists[i].second->qgram[j];
			if (querycheck[t] != querytime) {
				querycheck[t] = querytime;
				occurtime[t] = 0;
				shortlist.push_back(t);
			}
			occurtime[t]++;
		}
	}

	sort(shortlist.begin(), shortlist.end());
	int soc;

	for (int i = 0; i < shortlist.size(); i++){
		t = shortlist[i];
		soc = occurtime[t];
		for (int j = listdec + 1; j <= qsize; j++) {
			if (soc >= qthresh) break;
			if (soc + qsize - j + 1 < qthresh) break;
			if (check(qlists[j].second, t) == true) soc++;
		}
		if (soc >= qthresh) {
			k = CalCulateED(datastrings[t].c_str(), lendata[t], query, len, threshold);
			if (k <= threshold) result.push_back(make_pair(t, k));
		}
	}

	}

/*
	for (int i = 0; i < result.size(); i++){
		printf("%d %d\n", result[i].first, result[i].second);
	}*/
	
//	c2 = clock();
//	timequery += (double)(c2-c1)/(double)CLOCKS_PER_SEC;
//	printf("time query %.4lf\n", timequery);
//	printf("time dp %.4lf\n", timedp);

	return SUCCESS;
}

