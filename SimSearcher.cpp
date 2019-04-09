#include "SimSearcher.h"

using namespace std;

SimSearcher::SimSearcher()
{
}

SimSearcher::~SimSearcher()
{
}

void SimSearcher::insert(trie* rt, char* s,int id)
{
//	cout << "insert " << s << " for string " << id << endl;

//	cout << s << endl;
	for (int i = 0; i < qlimit; i++){
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
		rt->qgram[++rt->qsize] = id;
		rt->ql = id;
		//	cout << "insert at " << rt->num << " " << rt->qsize << endl;
	}
}

void SimSearcher::search(trie* rt, char* s)
{
	for (int i = 0; i < qlimit; i++){
		if (rt->node[(int)s[i]] == NULL) return;
		rt = rt -> node[(int)s[i]];
	}
	int tid;
	if (rt->sl == querytime) return;
	qsize++;
	qlists[qsize] = make_pair(rt->qsize, rt);
	rt->sl = querytime;
}

int SimSearcher::CalCulateED(char* s1, const char* s2, int threshold, int len1)
{
	int l1,l2;
//	clock_t c1,c2;
//	c1 = clock();
	dptime++;
	l1 = len1;
	l2 = lquery;
	if (abs(l1-l2) > threshold) return threshold + 1;
	int limmin,limmax,limmxl;
	for (int i = 0; i <= l2; i++)f[0][i] = i;
	for (int i = 1; i <= l1; i++){
		f[i&1][0] = i;
		limmin = 1;
		if (i - threshold > 1) limmin = i - threshold;
		limmax = l2;
		if (i + threshold < l2) limmax = i + threshold;
		limmxl = l2;
		if (i - 1 + threshold < l2) limmxl = i - 1 + threshold;
		for (int j = limmin; j <= limmax; j++){
			f[i&1][j] = threshold + 1;
			if((j-1 >= limmin || j-1 == 0) && f[i&1][j-1] + 1 < f[i&1][j])f[i&1][j] = f[i&1][j-1] + 1;
			if (j <= limmxl && f[(i&1)^1][j] + 1 < f[i&1][j])f[i&1][j] = f[(i&1)^1][j] + 1;
			if(f[(i&1)^1][j-1] + (s1[i-1] != s2[j-1]) < f[i&1][j])f[i&1][j] = f[(i&1)^1][j-1] + (s1[i-1] != s2[j-1]);
		}
	}
//	c2 = clock();
//	double thstime = (double)(c2-c1)/(double)CLOCKS_PER_SEC;
//	timedp += thstime;
	return f[l1 & 1][l2];

}

void SimSearcher::BuildQgram()
{
	char* tempt;
	int len;
	qgramsz = 0;
	qroot = new trie();
	qroot -> num = 0;
	for (int i = 0; i < datasz; i++){
		tempt = datastrings[i];
		len = strlen(datastrings[i]);
		lendata[i] = len;
		//cout << len << endl;
	//	cout << st << endl;
		for (int j = 1; j <= len - qlimit + 1; j++){
//			cout << sb << endl;
			insert(qroot, tempt, i);
			tempt++;
		}
	}
	//cout << "Build QGram Succeed" << endl;
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
	
	char* temp;
	temp = new char[301];
	while (fp.getline(temp, 300)) {
		len = strlen(temp);
		datastrings[datasz] = new char[len + 1];
		for (int i = 0; i < len; i++)datastrings[datasz][i] = temp[i];
		datastrings[datasz][len] = 0;
		datasz++;
	}
	fp.close();

/*
	for (int i = 1; i <= datasz; i++){
		cout << datastrings[i] << endl;
	}
*/
	BuildQgram();
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
	return SUCCESS;
}

int SimSearcher::searchED(const char *query, unsigned threshold, vector<pair<unsigned, unsigned> > &result)
{
//	clock_t c1,c2;
//	c1 = clock();
	result.clear();

	int len,qthresh,listdec;
	char* tempt;
	querytime++;	
	len = strlen(query);
	lquery = len;

	qsize = 0;
	shortsz = 0;

	tempt = (char*)query;
	for (int i = 1; i <= len - qlimit + 1; i++) {
		search(qroot, tempt);
		tempt++;
	}
	
	qthresh = qsize - threshold * qlimit;
	if (qthresh - 1 < 0) listdec = 0;
	else listdec = qthresh - 1;
	listdec = qsize - listdec;
	if (listdec < 0) return SUCCESS;
	sort(qlists + 1, qlists + 1 + qsize);
	
	for (int i = 1; i <= listdec; i++){
		for (int j = 1; j <= qlists[i].second->qsize; j++){
			shortsz++;
			shortlist[shortsz] = qlists[i].second->qgram[j];
		}
	}

	sort(shortlist + 1, shortlist + 1 + shortsz);
	int bz,next,soc;
	int k,t;

	filtsz = 0;

	bz = 1;
	while (bz <= shortsz){
		next = bz;
		t = shortlist[bz];
		while (shortlist[next] == t && next <= shortsz)next++;
		soc = next - bz;
		if (soc >= qthresh) {
			k = CalCulateED(datastrings[t], query, threshold, lendata[t]);
			if (k <= threshold) result.push_back(make_pair(t, k));
		}
		else{
			for (int j = listdec + 1; j <= qsize; j++) {
				if (check(qlists[j].second, t) == true) soc++;
				if (soc >= qthresh) break;
				if (soc + qsize - j + 1 < qthresh) break;
			}
			if (soc >= qthresh) {
				k = CalCulateED(datastrings[t], query, threshold, lendata[t]);
				if (k <= threshold) result.push_back(make_pair(t, k));
			}
		}
		bz = next;
	}

	//sort(filtans + 1, filtans + 1 + filtsz);
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
	
//	c2 = clock();
//	timequery += (double)(c2-c1)/(double)CLOCKS_PER_SEC;
//	printf("time query %.4lf\n", timequery);
//	printf("time dp %.4lf\n", timedp);

	return SUCCESS;
}

