#include<cstdio>
#include<cstdlib>
#include<ctime>
#include<cstring>
#include<string>
#include<iostream>

using namespace std;

int main()
{
    freopen("sp.txt", "w", stdout);
    string s;
    char c;
    int k;
    srand(unsigned(time(NULL)));
    for(int i = 1; i <= 100; i++){
        for (int j = 1; j <= 20; j++){
            k = rand()%5;
            c = 'a' + k;
            if (j == 1) s = c;
            else s = s + c; 
        }
        cout << s << endl;
    }
}