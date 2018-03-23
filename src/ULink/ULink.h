//=========================================================
//ULink
//说明：连接器类
//
//编写：方寸
//
//
//日期：2013-10-7
//=========================================================

#ifndef			_U_LINK_H_
#define			_U_LINK_H_

#include "../SToken.h"

class ULink
{
	//Data
private:
	struct LabelNode//标签结点
	{
		SString strLabel;//标签字符串
		int nAddress;//地址
		LabelNode *pNext;
	} m_LabelHead;
	struct WriteBackLabelNode//待填标签结点
	{
		struct WriteBackAddressNode
		{
			int nWriteBackAddress;//要回填的地址
			WriteBackAddressNode *pNext;
		};
		SString strLabel;
		WriteBackAddressNode WriteBackAddress;
		WriteBackLabelNode *pNext;
	} m_WriteBackAddressHead;
	SToken								m_Token;				//词法分析器
	SString								m_strOutputFile;		//输出文件路径
	FILE								*m_fpOutput;			//输出文件句柄
	int									m_nBinCodeAddress;		//二进制代码位置计数
	//Function
private:
	void CompileInit();//编译初始化
	void ClearCompileLists();//清空编译产生的链表信息
	void AddLabel(SString &strLabel,int nAddress);//向标签链中添加一个标签
	void AddWriteBackLabel(SString &strLabel,int nAddress);//添加一个待回填标签
	//=========================================================
	//语法分析
	//=========================================================
	void LangProgram();//源程序
	void LangStatement();//语句
	void LangOperatorDual(char *pBinCode);//双操作数
	void LangDATAParameter();//DATA伪指令参数
	int LangHowToFindAddress();//寻址方式
	void LangNOOP();//空指令
	void LangMOV();
	void LangCMP();
	void LangJPC();
	void LangJMP();
	void LangPUSH();
	void LangPOP();
	void LangCALL();
	void LangRET();
	void LangIN();
	void LangOUT();
	void LangCAL();
	void LangCALF();
	void LangDATA();
	void LangBLOCK();
	void LangLABEL();
	void LangEXIT();
	//=========================================================
	void WriteBackLabel();//回填标签
	void AddBinCodeAddress(int nCount);//增加目标代码位置计数
	int GetBinCodeAddress();//获取目标代码位置计数
public:
	ULink();
	~ULink();
	bool LoadSourceFile(const char *strFileName);//载入源代码文件
	void SetOutputFile(const char *strFileName);//设置输出文件路径
	void Analyse();//分析读入内存的源代码
};

#endif
