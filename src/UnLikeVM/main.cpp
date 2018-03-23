#include "UnLikeVM.h"
#include <iostream>

using namespace std;

//全局变量
UnLikeVM g_UnLikeVM;//UnLike虚拟机实例

//子程式声明
bool LoadExeCode(const char *strFileName);

//=========================================================
//菜单
//=========================================================

//=========================================================
//函数：Main
//说明：主调函数
//输入：
//输出：
//返回：
//日期：2013-11-8
//备注：
//=========================================================
int main(int argc,char *argv[])
{
	if(argc==2)
	{
		if(LoadExeCode(argv[1]))
		{
			g_UnLikeVM.Start();
		}
		else
		{
			cout<<"没有指明程序文件!"<<endl;
		}
	}
	else
	{
		LoadExeCode("test.ike");
		g_UnLikeVM.Start();
	}

	return 0;
}

//=========================================================
//函数：AppInit
//说明：程序初始化
//输入：
//输出：
//返回：
//日期：2013-11-8
//备注：
//=========================================================
void AppInit()
{
}


//=========================================================
//函数：LoadExeCode
//说明：载入程序代码
//输入：
//输出：
//返回：
//日期：2013-3-30
//备注：
//=========================================================
bool LoadExeCode(const char *strFileName)
{
	char *pBinCode;
	FILE *fp;
	long nFileLength;

	fp=fopen(strFileName,"rb");

	if(!fp)
	{
		cout<<"载入程序代码失败!"<<endl;
		return false;
	}

	fseek(fp,0,SEEK_END);

	nFileLength=ftell(fp);
	fseek(fp,0,SEEK_SET);

	pBinCode=new char[nFileLength];
	fread(pBinCode,nFileLength,1,fp);

	g_UnLikeVM.SetExeCode(pBinCode,nFileLength);

	fclose(fp);

	return true;
}
