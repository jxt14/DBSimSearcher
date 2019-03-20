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
	qsize++;
	qlists[qsize] = make_pair(rt->qsize, rt);
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

/*
	for (int i = 1; i <= datasz; i++){
		cout << datastrings[i] << endl;
	}
*/
//	cout << "input succeed" << endl;
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
	result.clear();
	string tq,tb;
	int len,qthresh,listdec;

	querytime++;
	tq = query;
	len = tq.length();

	qsize = 0;
	shortsz = 0;

	for (int i = 1; i <= len - qlimit + 1; i++) {
		tb = tq.substr(i-1, qlimit);
		search(qroot, tb);
	}
	
	qthresh = qsize - qlimit + 1 - threshold * qlimit;
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

	filtsz = 0;

	bz = 1;
	while (bz <= shortsz){
		next = bz;
		while (shortlist[next] == shortlist[bz] && next <= shortsz)next++;
		soc = next - bz;
		if (soc >= qthresh) {
			filtsz++;
			filtans[filtsz] = shortlist[bz];
		}
		else{
			for (int j = listdec + 1; j <= qsize; j++) {
				if (check(qlists[j].second, shortlist[bz]) == true) soc++;
				if (soc >= qthresh) break;
				if (soc + qsize - j + 1 < qthresh) break;
			}
			if (soc >= qthresh) {
				filtsz++;
				filtans[filtsz] = shortlist[bz];
			}
		}
		bz = next;
	}

	sort(filtans + 1, filtans + 1 + filtsz);
	int k;
	for (int i = 1; i <= filtsz; i++){
		k = CalCulateED(datastrings[filtans[i]], tq);
		if (k <= threshold) result.push_back(make_pair(filtans[i], k));
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

