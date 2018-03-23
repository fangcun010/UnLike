#include <iostream>
#include <stdlib.h>
#include "ULink.h"

using namespace std;

void Help();

int main(int argc,char *argv[])
{
	ULink ulink;

	if(argc!=3)
	{
		Help();
		exit(1);
	}

	cout<<"连接中......"<<endl;

	if(!ulink.LoadSourceFile(argv[1]))
	{
		cout<<argv[1]<<"文件无法打开！"<<endl;
		exit(1);
	}

	ulink.SetOutputFile(argv[2]);
   	ulink.Analyse();

	cout<<"连接成功！"<<endl;

	return 0;
}

void Help()
{
	cout<<"ULink v1.0"<<endl;
	cout<<"Usage:ULink SourceFile DestFile"<<endl;
}
