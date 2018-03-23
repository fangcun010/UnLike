#define DEBUG_UNLIKE

#include <iostream>
#include "UnLike.h"

using namespace std;

int main(int argc,char *argv[])
{
    UnLike ul;

	if(argc==3)
	{
		ul.SetOutputFile(argv[2]);
		cout<<"编译中..."<<endl;
		ul.LoadCodeFromFile(argv[1]);
		ul.Compile();
		ul.Output();
	}
	else
	{
		ul.SetOutputFile("test.unl");
		ul.LoadCodeFromFile("test.lik");
		ul.Compile();
		ul.Output();
		cout<<ul.m_TypeStack.size()<<endl;
		cout<<ul.m_FuncCallStack.size()<<endl;
	}

    return 0;
}
