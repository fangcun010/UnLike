#include <iostream>
#include <GL/freeglut.h>
#include <GL/glext.h>
#include <glib.h>
#include "gUnLikeVM.h"
#include "Paint.h"

using namespace std;

#define								WND_W								800
#define								WND_H									600

GThread 							*g_RunThread;					//虚拟机运行线程
UnLikeVM							g_UnLikeVM;						//虚拟机
Paint									g_Paint;								//绘图相关信息

void Reshape(int w,int h);
void Display();
bool LoadExeCode(const char *strFileName);
void *Run(void *pData);

int main(int argc,char *argv[])
{
	if(argc!=2)
	{
		cout<<"No input file!"<<endl;

		return 1;
	}

	glutInit(&argc,argv);

	glutInitDisplayMode(GLUT_DOUBLE|GLUT_MULTISAMPLE);

    glutInitWindowSize(WND_W,WND_H);
    glutInitWindowPosition(50,50);

    glutCreateWindow("UnLikeVM");

	glutDisplayFunc(Display);
	glutReshapeFunc(Reshape);

	if(LoadExeCode(argv[1]))
		g_RunThread=g_thread_create(Run,NULL,false,NULL);
	else return 1;

	glutMainLoop();

	return 0;
}

void Reshape(int w,int h)
{
	glViewport(0,0,(GLsizei)w,(GLsizei)h);
	glMatrixMode(GL_PROJECTION);
	glOrtho(0,(GLfloat)w,0,(GLfloat)h,-100.0f,100.0f);
	glMatrixMode(GL_MODELVIEW);
}

void Display()
{
	glClear(GL_COLOR_BUFFER_BIT);

	glutSwapBuffers();
}

void *Run(void *pData)
{
	g_UnLikeVM.Start();
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

	delete []pBinCode;

	fclose(fp);

	return true;
}

