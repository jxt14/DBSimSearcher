#include <cstdio>
#include <cstdlib>
#include <iostream>
#include <cstring>
#include <fstream>

using namespace std;

int main()
{
    char* s;
    s = new char[301];
    string ss;
    ifstream fp;
    fp.open("sample.txt");
    while(fp.getline(s, 300)){
        ss = s;
        cout << strlen(s) << endl;
    }
    fp.close();
}
