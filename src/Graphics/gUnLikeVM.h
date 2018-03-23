//=========================================================
//
//
//gUnLikeVM
//说明：UnLike虚拟机声明文件
//
//
//=========================================================

#ifndef _S_G_UNLIKE_VM_H_
#define _S_G_UNLIKE_VM_H_

#include <GL/freeglut.h>
#include <GL/glext.h>
#include "../SString.h"

#define						UNLIKE_VM_WNDCLASS			TEXT("UnLikeVMWndClass")

//=========================================================
//虚拟机相关
//=========================================================
//虚拟机寄存器
#define								REGISTER(x)							m_Register[x]
#define								RP									m_Register[0]
#define								RF									m_Register[1]
#define								RT									m_Register[2]
#define								RB									m_Register[3]
#define								R0									m_Register[4]
#define								R1									m_Register[5]
#define								R2									m_Register[6]
#define								R3									m_Register[7]
//虚拟机栈大小
#define								STACK_SIZE							1024000
#define								MEMORY(x)							m_pExeCode[x]

class UnLikeVM
{
	//Data
private:
	char *m_pExeCode;//二进制执行代码
	int m_nExeCodeLength;
	int m_Register[8];//虚拟机寄存器
	struct StringNode//字符串表结点
	{
		SString str;
		int nID;
		StringNode *pNext;
	};
	struct StringList//结点编号从1开始
	{
		StringNode Head;
		StringNode *pLast;
		int nIDCount;
		int nCount;
	} m_StringList;
	//Function
private:
	//=========================================================
	//字符串管理
	//=========================================================
	int NewString();//新建字符串
	SString *GetString(int nID);//获取字符串
	void DeleteString(int nID);//删除字符串
	//=========================================================
	//虚拟机执行
	//=========================================================
	void CmdNOOP();
	void CmdMOV();
	void CmdCMP();
	void CmdJPC();
	void CmdJMP();
	void CmdPUSH();
	void CmdPOP();
	void CmdCALL();
	void CmdRET();
	void CmdIN();
	void CmdOUT();
	void CmdCAL();
	void CmdCALF();
	void CmdEXIT();
public:
	UnLikeVM();
	~UnLikeVM();
	void SwapBuffers();//绘制缓冲中的画面到虚拟机窗口
	//=========================================================
	//VM设置函数
	//=========================================================
	void SetExeCode(char *pExeCode,int nExeCodeLength);//设置运行代码
	//=========================================================
	//虚拟机控制
	//=========================================================
	void Reset();//重启虚拟机
	void Start();//虚拟机重新开始运行
	void Run();//虚拟机运行
	void Stop();//虚拟机暂停
	void RunACommand();//读取并执行一个指令
	//=========================================================
	//虚拟机状态
	//=========================================================
	int GetRP();//获取当前运行指针位置
	int GetExeCodeLength();//获取执行代码长度
};

#endif
