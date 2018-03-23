#include "UnLikeVM.h"
#include <iostream>

using namespace std;

//ȫ�ֱ���
UnLikeVM g_UnLikeVM;//UnLike�����ʵ��

//�ӳ�ʽ����
bool LoadExeCode(const char *strFileName);

//=========================================================
//�˵�
//=========================================================

//=========================================================
//������Main
//˵������������
//���룺
//�����
//���أ�
//���ڣ�2013-11-8
//��ע��
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
			cout<<"û��ָ�������ļ�!"<<endl;
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
//������AppInit
//˵���������ʼ��
//���룺
//�����
//���أ�
//���ڣ�2013-11-8
//��ע��
//=========================================================
void AppInit()
{
}


//=========================================================
//������LoadExeCode
//˵��������������
//���룺
//�����
//���أ�
//���ڣ�2013-3-30
//��ע��
//=========================================================
bool LoadExeCode(const char *strFileName)
{
	char *pBinCode;
	FILE *fp;
	long nFileLength;

	fp=fopen(strFileName,"rb");

	if(!fp)
	{
		cout<<"����������ʧ��!"<<endl;
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
