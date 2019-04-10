#include <cstdio>
#include <cstdlib>
#include <iostream>
#include <cstring>
#include <fstream>
#include <set>
#include <string>

using namespace std;

int main()
{
    string s1;
    char *s;
    s = new char[10];
    s[0] = 'a';
    s[1] = 'b';
    s[2] = 0;
    s[3] = 'c';
    s1 = s;
    cout << s1 << endl;
}
