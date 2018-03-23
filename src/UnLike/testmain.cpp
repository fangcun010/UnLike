#include <iostream>
#include "UnLike.h"

using namespace std;
bool operator < (const SString &strA,const SString &strB)
{
	return strcmp(strA.c_str(),strB.c_str())<0?true:false;
}
int main()
{
    UnLike ul;

    ul.LoadCodeFromFile("test.txt");

    ul.Compile();

    return 0;
}

