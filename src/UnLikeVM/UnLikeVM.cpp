//=========================================================
//
//
//UnLikeVM
//说明：UnLike虚拟机实现文件
//
//
//=========================================================
#include "UnLikeVM.h"
#include <iostream>

using namespace std;

extern UnLikeVM g_UnLikeVM;

//=========================================================
//函数：UnLikeVM
//说明：构造函数
//输入：
//输出：
//返回：
//日期：2013-11-4
//备注：
//=========================================================
UnLikeVM::UnLikeVM()
{
	m_pExeCode=NULL;

	//初始化字符串表
	m_StringList.Head.pNext=NULL;
	m_StringList.pLast=&m_StringList.Head;
	m_StringList.nCount=0;
	m_StringList.nIDCount=0;
}

//=========================================================
//函数：UnLikeVM
//说明：析构函数
//输入：
//输出：
//返回：
//日期：2013-11-4
//备注：
//=========================================================
UnLikeVM::~UnLikeVM()
{
	if(m_pExeCode);
		delete []m_pExeCode;
}

//=========================================================
//函数：NewString
//说明：新建字符串
//输入：
//输出：
//返回：
//日期：2013-11-13
//备注：
//=========================================================
int UnLikeVM::NewString()
{
	StringNode *pStrNode;

	pStrNode=new StringNode;

	if(!pStrNode)
	{
		return 0;
	}

	pStrNode->pNext=NULL;
	m_StringList.pLast->pNext=pStrNode;
	m_StringList.pLast=pStrNode;

	m_StringList.nCount++;
	pStrNode->nID=++m_StringList.nIDCount;

	return pStrNode->nID;
}


//=========================================================
//函数：GetString
//说明：获取字符串
//输入：nID:字符串ID
//输出：
//返回：
//日期：2013-11-13
//备注：
//=========================================================
SString *UnLikeVM::GetString(int nID)
{
	StringNode *pStrNode;

	pStrNode=m_StringList.Head.pNext;

	while(pStrNode)
	{
		if(pStrNode->nID==nID)
			return &pStrNode->str;
		pStrNode=pStrNode->pNext;
	}

	return NULL;
}

//=========================================================
//函数：DeleteString
//说明：删除字符串
//输入：
//输出：
//返回：
//日期：2013-11-12
//备注：
//=========================================================
void UnLikeVM::DeleteString(int nID)
{
	StringNode *pStrNode;
	StringNode *pDel;

	pStrNode=&m_StringList.Head;

	while(pStrNode->pNext)
	{
		if(pStrNode->pNext->nID==nID)
		{
			pDel=pStrNode->pNext;
			pStrNode->pNext=pDel->pNext;
			delete pDel;
			break;
		}
		pStrNode=pStrNode->pNext;
	}
}


//=========================================================
//函数：SetExeCode
//说明：设置运行代码
//输入：
//输出：
//返回：
//日期：2013-11-8
//备注：
//=========================================================
void UnLikeVM::SetExeCode(char *pExeCode,int nExeCodeLength)
{
	m_pExeCode=new char[nExeCodeLength+STACK_SIZE];
	memcpy(m_pExeCode,pExeCode,nExeCodeLength);
	m_nExeCodeLength=nExeCodeLength;
}

//=========================================================
//函数：Reset
//说明：虚拟机重启
//输入：
//输出：
//返回：
//日期：2013-11-8
//备注：
//=========================================================
void UnLikeVM::Reset()
{
	//初始化寄存器
	memset(m_Register,sizeof(int)*8,0);
	RB=m_nExeCodeLength;
	RT=m_nExeCodeLength+STACK_SIZE;
}

//=========================================================
//函数：RunACommand
//说明：读取并执行一个指令
//输入：
//输出：
//返回：
//日期：2013-11-8
//备注：
//=========================================================
void UnLikeVM::RunACommand()
{
	char chOpt;

	chOpt=MEMORY(RP);

	switch(chOpt&0xF0)
	{
	case 0x00://NOOP
		CmdNOOP();
		RP++;
		break;
	case 0x10:
		CmdMOV();
		RP+=9;
		break;
	case 0x20:
		CmdCMP();
		RP+=9;
		break;
	case 0x30:
		CmdJPC();
		RP+=6;
		break;
	case 0x40:
		CmdJMP();
		RP+=5;
		break;
	case 0x50:
		CmdPUSH();
		RP+=5;
		break;
	case 0x60:
		CmdPOP();
		RP+=5;
		break;
	case 0x70:
		CmdCALL();
		RP+=5;
		break;
	case 0x80:
		CmdRET();
		RP++;
		break;
	case 0x90:
		CmdIN();
		RP+=9;
		break;
	case 0xA0:
		CmdOUT();
		RP+=9;
		break;
	case 0xB0:
		CmdCAL();
		RP+=10;
		break;
	case 0xC0:
		CmdCALF();
		RP+=10;
		break;
	case 0xF0:
		CmdEXIT();
		RP++;
		break;
	}
}

//=========================================================
//函数：CmdNOOP
//说明：
//输入：
//输出：
//返回：
//日期：2013-11-9
//备注：
//=========================================================
void UnLikeVM::CmdNOOP()
{
}

//=========================================================
//函数：CmdMOV
//说明：
//输入：
//输出：
//返回：
//日期：2013-11-9
//备注：
//=========================================================
void UnLikeVM::CmdMOV()
{
	int nOpt1,nOpt2;
	char chOpt=MEMORY(RP);

	nOpt1=*(int *)&MEMORY(RP+1);
	nOpt2=*(int *)&MEMORY(RP+5);

	switch(chOpt&0x0F)
	{
	case 0x00://立即数-立即数
		*(int *)&MEMORY(nOpt1)=nOpt2;
		break;
	case 0x01://立即数-立即数间接寻址
		*(int *)&MEMORY(nOpt1)=*(int *)&MEMORY(nOpt2);
		break;
	case 0x02://立即数-寄存器
		*(int *)&MEMORY(nOpt1)=REGISTER(nOpt2);
		break;
	case 0x03://立即数-寄存器间接寻址
		*(int *)&MEMORY(nOpt1)=*(int *)&MEMORY(REGISTER(nOpt2));
		break;
	case 0x04://立即数间接寻址-立即数
		*(int *)&MEMORY(nOpt1)=nOpt2;
		break;
	case 0x05://立即数间接寻址-立即数间接寻址
		*(int *)&MEMORY(nOpt1)=*(int *)&MEMORY(nOpt2);
		break;
	case 0x06://立即数间接寻址-寄存器
		*(int *)&MEMORY(nOpt1)=REGISTER(nOpt2);
		break;
	case 0x07://立即数间接寻址-寄存器间接寻址
		*(int *)&MEMORY(nOpt1)=*(int *)&MEMORY(REGISTER(nOpt2));
		break;
	case 0x08://寄存器-立即数
		REGISTER(nOpt1)=nOpt2;
		break;
	case 0x09://寄存器-立即数间接寻址
		REGISTER(nOpt1)=*(int *)&MEMORY(nOpt2);
		break;
	case 0x0A://寄存器-寄存器
		REGISTER(nOpt1)=REGISTER(nOpt2);
		break;
	case 0x0B://寄存器-寄存器间接寻址
		REGISTER(nOpt1)=*(int *)&MEMORY(REGISTER(nOpt2));
		break;
	case 0x0C://寄存器间接寻址-立即数
		*(int *)&MEMORY(REGISTER(nOpt1))=nOpt2;
		break;
	case 0x0D://寄存器间接寻址-立即数间接寻址
		*(int *)&MEMORY(REGISTER(nOpt1))=*(int *)&MEMORY(nOpt2);
		break;
	case 0x0E://寄存器间接寻址-寄存器
		*(int *)&MEMORY(REGISTER(nOpt1))=REGISTER(nOpt2);
		break;
	case 0x0F://寄存器间接寻址-寄存器间接寻址
		*(int *)&MEMORY(REGISTER(nOpt1))=*(int *)&MEMORY(REGISTER(nOpt2));
		break;
	}
}

//=========================================================
//函数：CmdCMP
//说明：
//输入：
//输出：
//返回：
//日期：2013-11-9
//备注：
//=========================================================
void UnLikeVM::CmdCMP()
{
	int nOpt1,nOpt2;
	char chOpt=MEMORY(RP);

	nOpt1=*(int *)&MEMORY(RP+1);
	nOpt2=*(int *)&MEMORY(RP+5);

	switch(chOpt&0x0F)
	{
	case 0x00://立即数-立即数
		break;
	case 0x01://立即数-立即数间接寻址
		nOpt2=*(int *)&MEMORY(nOpt2);
		break;
	case 0x02://立即数-寄存器
		nOpt2=REGISTER(nOpt2);
		break;
	case 0x03://立即数-寄存器间接寻址
		nOpt2=*(int *)&MEMORY(REGISTER(nOpt2));
		break;
	case 0x04://立即数间接寻址-立即数
		nOpt1=*(int *)&MEMORY(nOpt1);
		break;
	case 0x05://立即数间接寻址-立即数间接寻址
		nOpt1=*(int *)&MEMORY(nOpt1);
		nOpt2=*(int *)&MEMORY(nOpt2);
		break;
	case 0x06://立即数间接寻址-寄存器
		nOpt1=*(int *)&MEMORY(nOpt1);
		nOpt2=REGISTER(nOpt2);
		break;
	case 0x07://立即数间接寻址-寄存器间接寻址
		nOpt1=*(int *)&MEMORY(nOpt1);
		nOpt2=*(int *)&MEMORY(REGISTER(nOpt2));
		break;
	case 0x08://寄存器-立即数
		nOpt1=REGISTER(nOpt1);
		break;
	case 0x09://寄存器-立即数间接寻址
		nOpt1=REGISTER(nOpt1);
		nOpt2=*(int *)&MEMORY(nOpt2);
		break;
	case 0x0A://寄存器-寄存器
		nOpt1=REGISTER(nOpt1);
		nOpt2=REGISTER(nOpt2);
		break;
	case 0x0B://寄存器-寄存器间接寻址
		nOpt1=REGISTER(nOpt1);
		nOpt2=*(int *)&MEMORY(REGISTER(nOpt2));
		break;
	case 0x0C://寄存器间接寻址-立即数
		nOpt1=*(int *)&MEMORY(REGISTER(nOpt1));
		break;
	case 0x0D://寄存器间接寻址-立即数间接寻址
		nOpt1=*(int *)&MEMORY(REGISTER(nOpt1));
		nOpt2=*(int *)&MEMORY(nOpt2);
		break;
	case 0x0E://寄存器间接寻址-寄存器
		nOpt1=*(int *)&MEMORY(REGISTER(nOpt1));
		nOpt2=REGISTER(nOpt2);
		break;
	case 0x0F://寄存器间接寻址-寄存器间接寻址
		nOpt1=*(int *)&MEMORY(REGISTER(nOpt1));
		nOpt2=*(int *)&MEMORY(REGISTER(nOpt2));
		break;
	}
	RF=0;
	if(nOpt1==0)//参数1为0
		RF|=1;
	if(nOpt1>nOpt2)//参数1大于参数2
		RF|=2;
	if(nOpt1==nOpt2)//参数1等于参数2
		RF|=4;
}

//=========================================================
//函数：CmdJPC
//说明：
//输入：
//输出：
//返回：
//日期：2013-11-9
//备注：
//=========================================================
void UnLikeVM::CmdJPC()
{
	char chFindOpt=MEMORY(RP+5);
	char chOpt=MEMORY(RP);
	int nOpt=*(int *)&MEMORY(RP+1);

	switch(chFindOpt)
	{
	case 0://立即数
		break;
	case 1://立即数间接寻址
		nOpt=*(int *)&MEMORY(nOpt);
		break;
	case 2://寄存器
		nOpt=REGISTER(nOpt);
		break;
	case 3://寄存器间接寻址
		nOpt=*(int *)&MEMORY(REGISTER(nOpt));
		break;
	}

	switch(chOpt&0x0F)
	{
	case 0x00://Z
		if(RF&1)//参数1为0
		{
			RP=nOpt;
			RP-=6;
		}
		break;
	case 0x01://B
		if((RF&2)==0 && (RF&4)==0)//参数1小于参数2
		{
			RP=nOpt;
			RP-=6;
		}
		break;
	case 0x02://BE
		if((RF&2)==0 || (RF&4))//参数1小于等于参数2
		{
			RP=nOpt;
			RP-=6;
		}
		break;
	case 0x03://A
		if(RF&2 && (RF&4)==0)//参数1大于参数2
		{
			RP=nOpt;
			RP-=6;
		}
		break;
	case 0x04://AE
		if((RF&2) || (RF&4))// 参数1大于等于参数2
		{
			RP=nOpt;
			RP-=6;
		}
		break;
	case 0x05://NZ
		if((RF&1)==0)//参数1非0
		{
			RP=nOpt;
			RP-=6;
		}
		break;
	case 0x06://E
		if(RF&4)//参数1等于参数2
		{
			RP=nOpt;
			RP-=6;
		}
		break;
	case 0x07://NE
		if((RF&4)==0)//参数1不等于参数2
		{
			RP=nOpt;
			RP-=6;
		}
		break;
	}
}

//=========================================================
//函数：CmdJMP
//说明：
//输入：
//输出：
//返回：
//日期：2013-11-9
//备注：
//=========================================================
void UnLikeVM::CmdJMP()
{
	char chOpt=MEMORY(RP);
	int nOpt=*(int *)&MEMORY(RP+1);

	switch(chOpt&0x0F)
	{
	case 0://立即数
		break;
	case 1://立即数间接寻址
		nOpt=*(int *)&MEMORY(nOpt);
		break;
	case 2://寄存器
		nOpt=REGISTER(nOpt);
		break;
	case 3://寄存器间接寻址
		nOpt=*(int *)&MEMORY(REGISTER(nOpt));
		break;
	}

	RP=nOpt;
	RP-=5;
}

//=========================================================
//函数：CmdPUSH
//说明：
//输入：
//输出：
//返回：
//日期：2013-11-9
//备注：
//=========================================================
void UnLikeVM::CmdPUSH()
{
	char chOpt=MEMORY(RP);
	int nOpt=*(int *)&MEMORY(RP+1);

	switch(chOpt&0x0F)
	{
	case 0://立即数
		break;
	case 1://立即数间接寻址
		nOpt=*(int *)&MEMORY(nOpt);
		break;
	case 2://寄存器
		nOpt=REGISTER(nOpt);
		break;
	case 3://寄存器间接寻址
		nOpt=*(int *)&MEMORY(REGISTER(nOpt));
		break;
	}

	*(int *)&MEMORY(RB)=nOpt;
	RB+=4;
}

//=========================================================
//函数：CmdPOP
//说明：
//输入：
//输出：
//返回：
//日期：2013-11-9
//备注：
//=========================================================
void UnLikeVM::CmdPOP()
{
	char chOpt=MEMORY(RP);
	int nOpt=*(int *)&MEMORY(RP+1);
	int nPop;

	RB-=4;
	nPop=*(int *)&MEMORY(RB);

	switch(chOpt&0x0F)
	{
	case 0://立即数
		break;
	case 1://立即数间接寻址
		*(int *)&MEMORY(nOpt)=nPop;
		break;
	case 2:;//寄存器
		REGISTER(nOpt)=nPop;
		break;
	case 3://寄存器间接寻址
		*(int *)&MEMORY(REGISTER(nOpt))=nPop;
		break;
	}
}

//=========================================================
//函数：CmdCALL
//说明：
//输入：
//输出：
//返回：
//日期：2013-11-9
//备注：
//=========================================================
void UnLikeVM::CmdCALL()
{
	char chOpt=MEMORY(RP);
	int nOpt=*(int *)&MEMORY(RP+1);

	switch(chOpt&0x0F)
	{
	case 0://立即数
		break;
	case 1://立即数间接寻址
		nOpt=*(int *)&MEMORY(nOpt);
		break;
	case 2://寄存器
		nOpt=REGISTER(nOpt);
		break;
	case 3://寄存器间接寻址
		nOpt=*(int *)&MEMORY(REGISTER(nOpt));
		break;
	}

	RT-=4;
	*(int *)&MEMORY(RT)=RP+5;
	RP=nOpt;
	RP-=5;
}

//=========================================================
//函数：CmdRET
//说明：
//输入：
//输出：
//返回：
//日期：2013-11-9
//备注：
//=========================================================
void UnLikeVM::CmdRET()
{
	RP=*(int *)&MEMORY(RT);
	RT+=4;
	RP-=1;
}

//=========================================================
//函数：CmdIN
//说明：
//输入：
//输出：
//返回：
//日期：2013-11-9
//备注：
//=========================================================
void UnLikeVM::CmdIN()
{
	int nOpt1,nOpt2;
	char chOpt=MEMORY(RP);

	nOpt1=*(int *)&MEMORY(RP+1);
	nOpt2=*(int *)&MEMORY(RP+5);

	switch(chOpt&0x0F)
	{
	case 0x00://立即数-立即数
		break;
	case 0x01://立即数-立即数间接寻址
		nOpt2=*(int *)&MEMORY(nOpt2);
		break;
	case 0x02://立即数-寄存器
		nOpt2=REGISTER(nOpt2);
		break;
	case 0x03://立即数-寄存器间接寻址
		nOpt2=*(int *)&MEMORY(REGISTER(nOpt2));
		break;
	case 0x04://立即数间接寻址-立即数
		nOpt1=*(int *)&MEMORY(nOpt1);
		break;
	case 0x05://立即数间接寻址-立即数间接寻址
		nOpt1=*(int *)&MEMORY(nOpt1);
		nOpt2=*(int *)&MEMORY(nOpt2);
		break;
	case 0x06://立即数间接寻址-寄存器
		nOpt1=*(int *)&MEMORY(nOpt1);
		nOpt2=REGISTER(nOpt2);
		break;
	case 0x07://立即数间接寻址-寄存器间接寻址
		nOpt1=*(int *)&MEMORY(nOpt1);
		nOpt2=*(int *)&MEMORY(REGISTER(nOpt2));
		break;
	case 0x08://寄存器-立即数
		nOpt1=REGISTER(nOpt1);
		break;
	case 0x09://寄存器-立即数间接寻址
		nOpt1=REGISTER(nOpt1);
		nOpt2=*(int *)&MEMORY(nOpt2);
		break;
	case 0x0A://寄存器-寄存器
		nOpt1=REGISTER(nOpt1);
		nOpt2=REGISTER(nOpt2);
		break;
	case 0x0B://寄存器-寄存器间接寻址
		nOpt1=REGISTER(nOpt1);
		nOpt2=*(int *)&MEMORY(REGISTER(nOpt2));
		break;
	case 0x0C://寄存器间接寻址-立即数
		nOpt1=*(int *)&MEMORY(REGISTER(nOpt1));
		break;
	case 0x0D://寄存器间接寻址-立即数间接寻址
		nOpt1=*(int *)&MEMORY(REGISTER(nOpt1));
		nOpt2=*(int *)&MEMORY(nOpt2);
		break;
	case 0x0E://寄存器间接寻址-寄存器
		nOpt1=*(int *)&MEMORY(REGISTER(nOpt1));
		nOpt2=REGISTER(nOpt2);
		break;
	case 0x0F://寄存器间接寻址-寄存器间接寻址
		nOpt1=*(int *)&MEMORY(REGISTER(nOpt1));
		nOpt2=*(int *)&MEMORY(REGISTER(nOpt2));
		break;
	}

	switch(nOpt2)
	{
	case 0x00://申请字符串句柄
		R3=-NewString();
		break;
	case 0x01://删除申请的字符串句柄
		DeleteString(nOpt1);
		break;
	case 0x02://字符串赋值
		if(R3>=0)
			*GetString(-nOpt1)=(char *)&MEMORY(R3);
		else
			*GetString(-nOpt1)=*GetString(-R3);
		break;
	case 0x03:
		break;
	case 0x04:
		break;
	case 0x05:
		break;
	case 0x06:
		break;
	case  0x07:
		break;
	}
}

//=========================================================
//函数：CmdOUT
//说明：
//输入：
//输出：
//返回：
//日期：2013-11-9
//备注：
//=========================================================
void UnLikeVM::CmdOUT()
{
	int nOpt1,nOpt2;
	char chOpt=MEMORY(RP);

	nOpt1=*(int *)&MEMORY(RP+1);
	nOpt2=*(int *)&MEMORY(RP+5);

	switch(chOpt&0x0F)
	{
	case 0x00://立即数-立即数
		break;
	case 0x01://立即数-立即数间接寻址
		nOpt2=*(int *)&MEMORY(nOpt2);
		break;
	case 0x02://立即数-寄存器
		nOpt2=REGISTER(nOpt2);
		break;
	case 0x03://立即数-寄存器间接寻址
		nOpt2=*(int *)&MEMORY(REGISTER(nOpt2));
		break;
	case 0x04://立即数间接寻址-立即数
		nOpt1=*(int *)&MEMORY(nOpt1);
		break;
	case 0x05://立即数间接寻址-立即数间接寻址
		nOpt1=*(int *)&MEMORY(nOpt1);
		nOpt2=*(int *)&MEMORY(nOpt2);
		break;
	case 0x06://立即数间接寻址-寄存器
		nOpt1=*(int *)&MEMORY(nOpt1);
		nOpt2=REGISTER(nOpt2);
		break;
	case 0x07://立即数间接寻址-寄存器间接寻址
		nOpt1=*(int *)&MEMORY(nOpt1);
		nOpt2=*(int *)&MEMORY(REGISTER(nOpt2));
		break;
	case 0x08://寄存器-立即数
		nOpt1=REGISTER(nOpt1);
		break;
	case 0x09://寄存器-立即数间接寻址
		nOpt1=REGISTER(nOpt1);
		nOpt2=*(int *)&MEMORY(nOpt2);
		break;
	case 0x0A://寄存器-寄存器
		nOpt1=REGISTER(nOpt1);
		nOpt2=REGISTER(nOpt2);
		break;
	case 0x0B://寄存器-寄存器间接寻址
		nOpt1=REGISTER(nOpt1);
		nOpt2=*(int *)&MEMORY(REGISTER(nOpt2));
		break;
	case 0x0C://寄存器间接寻址-立即数
		nOpt1=*(int *)&MEMORY(REGISTER(nOpt1));
		break;
	case 0x0D://寄存器间接寻址-立即数间接寻址
		nOpt1=*(int *)&MEMORY(REGISTER(nOpt1));
		nOpt2=*(int *)&MEMORY(nOpt2);
		break;
	case 0x0E://寄存器间接寻址-寄存器
		nOpt1=*(int *)&MEMORY(REGISTER(nOpt1));
		nOpt2=REGISTER(nOpt2);
		break;
	case 0x0F://寄存器间接寻址-寄存器间接寻址
		nOpt1=*(int *)&MEMORY(REGISTER(nOpt1));
		nOpt2=*(int *)&MEMORY(REGISTER(nOpt2));
		break;
	}

	switch(nOpt1)
	{
	case 0x00:
		cout<<nOpt2<<endl;
		break;
	case 0x01:
		if(nOpt2>=0)
			cout<<&MEMORY(nOpt2)<<endl;
		else
			cout<<GetString(-nOpt2)->c_str()<<endl;
		break;
	case 0x02:
		break;
	case 0x03:
		break;
	case 0x04:
		break;
	case 0x05:
		break;
	case 0x06:
		break;
	case 0x07:
		break;
	}
}

//=========================================================
//函数：CmdCAL
//说明：
//输入：
//输出：
//返回：
//日期：2013-11-9
//备注：
//=========================================================
void UnLikeVM::CmdCAL()
{
	char chSym;//标识加减乘除取余
	char chOpt;
	int nOpt1;
	int nOpt2;
	int nResult;

	chSym=*(int *)&MEMORY(RP);

	chOpt=MEMORY(RP+9);

	nOpt1=*(int *)&MEMORY(RP+1);
	nOpt2=*(int *)&MEMORY(RP+5);

	switch(chOpt&0x0F)
	{
	case 0x00://立即数-立即数
		break;
	case 0x01://立即数-立即数间接寻址
		nOpt2=*(int *)&MEMORY(nOpt2);
		break;
	case 0x02://立即数-寄存器
		nOpt2=REGISTER(nOpt2);
		break;
	case 0x03://立即数-寄存器间接寻址
		nOpt2=*(int *)&MEMORY(REGISTER(nOpt2));
		break;
	case 0x04://立即数间接寻址-立即数
		nOpt1=*(int *)&MEMORY(nOpt1);
		break;
	case 0x05://立即数间接寻址-立即数间接寻址
		nOpt1=*(int *)&MEMORY(nOpt1);
		nOpt2=*(int *)&MEMORY(nOpt2);
		break;
	case 0x06://立即数间接寻址-寄存器
		nOpt1=*(int *)&MEMORY(nOpt1);
		nOpt2=REGISTER(nOpt2);
		break;
	case 0x07://立即数间接寻址-寄存器间接寻址
		nOpt1=*(int *)&MEMORY(nOpt1);
		nOpt2=*(int *)&MEMORY(REGISTER(nOpt2));
		break;
	case 0x08://寄存器-立即数
		nOpt1=REGISTER(nOpt1);
		break;
	case 0x09://寄存器-立即数间接寻址
		nOpt1=REGISTER(nOpt1);
		nOpt2=*(int *)&MEMORY(nOpt2);
		break;
	case 0x0A://寄存器-寄存器
		nOpt1=REGISTER(nOpt1);
		nOpt2=REGISTER(nOpt2);
		break;
	case 0x0B://寄存器-寄存器间接寻址
		nOpt1=REGISTER(nOpt1);
		nOpt2=*(int *)&MEMORY(REGISTER(nOpt2));
		break;
	case 0x0C://寄存器间接寻址-立即数
		nOpt1=*(int *)&MEMORY(REGISTER(nOpt1));
		break;
	case 0x0D://寄存器间接寻址-立即数间接寻址
		nOpt1=*(int *)&MEMORY(REGISTER(nOpt1));
		nOpt2=*(int *)&MEMORY(nOpt2);
		break;
	case 0x0E://寄存器间接寻址-寄存器
		nOpt1=*(int *)&MEMORY(REGISTER(nOpt1));
		nOpt2=REGISTER(nOpt2);
		break;
	case 0x0F://寄存器间接寻址-寄存器间接寻址
		nOpt1=*(int *)&MEMORY(REGISTER(nOpt1));
		nOpt2=*(int *)&MEMORY(REGISTER(nOpt2));
		break;
	}

	switch(chSym&0x0F)
	{
	case 0x00:
		nOpt1+=nOpt2;
		break;
	case 0x01:
		nOpt1-=nOpt2;
		break;
	case 0x02:
		nOpt1*=nOpt2;
		break;
	case 0x03:
		nOpt1/=nOpt2;
		break;
	case 0x04:
		nOpt1%=nOpt2;
		break;
	}

	nResult=nOpt1;
	nOpt1=*(int *)&MEMORY(RP+1);
	nOpt2=*(int *)&MEMORY(RP+5);

	switch(chOpt&0x0F)
	{
	case 0x00://立即数-立即数
		break;
	case 0x01://立即数-立即数间接寻址
		break;
	case 0x02://立即数-寄存器
		break;
	case 0x03://立即数-寄存器间接寻址
		break;
	case 0x04://立即数间接寻址-立即数
		*(int *)&MEMORY(nOpt1)=nResult;
		break;
	case 0x05://立即数间接寻址-立即数间接寻址
		*(int *)&MEMORY(nOpt1)=nResult;
		break;
	case 0x06://立即数间接寻址-寄存器
		*(int *)&MEMORY(nOpt1)=nResult;
		break;
	case 0x07://立即数间接寻址-寄存器间接寻址
		*(int *)&MEMORY(nOpt1)=nResult;
		break;
	case 0x08://寄存器-立即数
		REGISTER(nOpt1)=nResult;
		break;
	case 0x09://寄存器-立即数间接寻址
		REGISTER(nOpt1)=nResult;
		break;
	case 0x0A://寄存器-寄存器
		REGISTER(nOpt1)=nResult;
		break;
	case 0x0B://寄存器-寄存器间接寻址
		REGISTER(nOpt1)=nResult;
		break;
	case 0x0C://寄存器间接寻址-立即数
		*(int *)&MEMORY(REGISTER(nOpt1))=nResult;
		break;
	case 0x0D://寄存器间接寻址-立即数间接寻址
		*(int *)&MEMORY(REGISTER(nOpt1))=nResult;
		break;
	case 0x0E://寄存器间接寻址-寄存器
		*(int *)&MEMORY(REGISTER(nOpt1))=nResult;
		break;
	case 0x0F://寄存器间接寻址-寄存器间接寻址
		*(int *)&MEMORY(REGISTER(nOpt1))=nResult;
		break;
	}
}

//=========================================================
//函数：CmdCALF
//说明：
//输入：
//输出：
//返回：
//日期：2013-11-9
//备注：
//=========================================================
void UnLikeVM::CmdCALF()
{
	char chSym;//标识加减乘除取余
	char chOpt;
	float fOpt1,fOpt2;
	int nOpt1,nOpt2;
	float fResult;

	chSym=*(int *)&MEMORY(RP);

	chOpt=MEMORY(RP+9);

	nOpt1=*(int *)&MEMORY(RP+1);
	nOpt2=*(int *)&MEMORY(RP+5);

	switch(chOpt&0x0F)
	{
	case 0x00://立即数-立即数
		break;
	case 0x01://立即数-立即数间接寻址
		fOpt2=*(float *)&MEMORY(nOpt2);
		break;
	case 0x02://立即数-寄存器
		fOpt2=*(float *)&REGISTER(nOpt2);
		break;
	case 0x03://立即数-寄存器间接寻址
		fOpt2=*(float *)&MEMORY(REGISTER(nOpt2));
		break;
	case 0x04://立即数间接寻址-立即数
		fOpt1=*(float *)&MEMORY(nOpt1);
		break;
	case 0x05://立即数间接寻址-立即数间接寻址
		fOpt1=*(float *)&MEMORY(nOpt1);
		fOpt2=*(float *)&MEMORY(nOpt2);
		break;
	case 0x06://立即数间接寻址-寄存器
		fOpt1=*(float *)&MEMORY(nOpt1);
		fOpt2=*(float *)&REGISTER(nOpt2);
		break;
	case 0x07://立即数间接寻址-寄存器间接寻址
		fOpt1=*(float *)&MEMORY(nOpt1);
		fOpt2=*(float *)&MEMORY(REGISTER(nOpt2));
		break;
	case 0x08://寄存器-立即数
		fOpt1=*(float *)&REGISTER(nOpt1);
		break;
	case 0x09://寄存器-立即数间接寻址
		fOpt1=*(float *)&REGISTER(nOpt1);
		fOpt2=*(float *)&MEMORY(nOpt2);
		break;
	case 0x0A://寄存器-寄存器
		fOpt1=*(float *)&REGISTER(nOpt1);
		fOpt2=*(float *)REGISTER(nOpt2);
		break;
	case 0x0B://寄存器-寄存器间接寻址
		fOpt1=*(float *)&REGISTER(nOpt1);
		fOpt2=*(float *)&MEMORY(REGISTER(nOpt2));
		break;
	case 0x0C://寄存器间接寻址-立即数
		fOpt1=*(float *)&MEMORY(REGISTER(nOpt1));
		fOpt2=*(float *)&(nOpt2);
		break;
	case 0x0D://寄存器间接寻址-立即数间接寻址
		fOpt1=*(float *)&MEMORY(REGISTER(nOpt1));
		fOpt2=*(float *)&MEMORY(nOpt2);
		break;
	case 0x0E://寄存器间接寻址-寄存器
		fOpt1=*(float *)&MEMORY(REGISTER(nOpt1));
		fOpt2=*(float *)&REGISTER(nOpt2);
		break;
	case 0x0F://寄存器间接寻址-寄存器间接寻址
		fOpt1=*(float *)&MEMORY(REGISTER(nOpt1));
		fOpt2=*(float *)&MEMORY(REGISTER(nOpt2));
		break;
	}

	switch(chSym&0x0F)
	{
	case 0x00:
		fOpt1+=fOpt2;
		break;
	case 0x01:
		fOpt1-=fOpt2;
		break;
	case 0x02:
		fOpt1*=fOpt2;
		break;
	case 0x03:
		fOpt1/=fOpt2;
		break;
	case 0x04:
		break;
	}

	fResult=fOpt1;
	fOpt1=*(float *)&MEMORY(RP+1);
	fOpt2=*(float *)&MEMORY(RP+5);

	switch(chOpt&0x0F)
	{
	case 0x00://立即数-立即数
		break;
	case 0x01://立即数-立即数间接寻址
		break;
	case 0x02://立即数-寄存器
		break;
	case 0x03://立即数-寄存器间接寻址
		break;
	case 0x04://立即数间接寻址-立即数
		*(float *)&MEMORY(nOpt1)=fResult;
		break;
	case 0x05://立即数间接寻址-立即数间接寻址
		*(float *)&MEMORY(nOpt1)=fResult;
		break;
	case 0x06://立即数间接寻址-寄存器
		*(float *)&MEMORY(nOpt1)=fResult;
		break;
	case 0x07://立即数间接寻址-寄存器间接寻址
		*(float *)&MEMORY(nOpt1)=fResult;
		break;
	case 0x08://寄存器-立即数
		REGISTER(nOpt1)=*(int *)&fResult;
		break;
	case 0x09://寄存器-立即数间接寻址
		REGISTER(nOpt1)=*(int *)&fResult;
		break;
	case 0x0A://寄存器-寄存器
		REGISTER(nOpt1)=*(int *)&fResult;
		break;
	case 0x0B://寄存器-寄存器间接寻址
		REGISTER(nOpt1)=*(int *)&fResult;
		break;
	case 0x0C://寄存器间接寻址-立即数
		*(float *)&MEMORY(REGISTER(nOpt1))=fResult;
		break;
	case 0x0D://寄存器间接寻址-立即数间接寻址
		*(float *)&MEMORY(REGISTER(nOpt1))=fResult;
		break;
	case 0x0E://寄存器间接寻址-寄存器
		*(float *)&MEMORY(REGISTER(nOpt1))=fResult;
		break;
	case 0x0F://寄存器间接寻址-寄存器间接寻址
		*(float *)&MEMORY(REGISTER(nOpt1))=fResult;
		break;
	}
}

//=========================================================
//函数：CmdEXIT
//说明：
//输入：
//输出：
//返回：
//日期：2013-11-9
//备注：
//=========================================================
void UnLikeVM::CmdEXIT()
{
	RP=m_nExeCodeLength;
}

//=========================================================
//函数：Start
//说明：虚拟机重新开始运行
//输入：
//输出：
//返回：
//日期：2013-11-15
//备注：
//=========================================================
void UnLikeVM::Start()
{
	Reset();
	Run();
}

//=========================================================
//函数：Run
//说明：虚拟机运行
//输入：
//输出：
//返回：
//日期：2013-11-8
//备注：
//=========================================================
void UnLikeVM::Run()
{
	while(GetRP()<GetExeCodeLength())
		RunACommand();
}

//=========================================================
//函数：Stop
//说明：虚拟机暂停
//输入：
//输出：
//返回：
//日期：2013-11-8
//备注：
//=========================================================
void UnLikeVM::Stop()
{
}

//=========================================================
//函数：GetRP
//说明：获取当前运行指针位置
//输入：
//输出：
//返回：
//日期：2013-11-9
//备注：
//=========================================================
int UnLikeVM::GetRP()
{
	return RP;
}

//=========================================================
//函数：GetExeCodeLength
//说明：获取执行代码长度
//输入：
//输出：
//返回：
//日期：2013-11-9
//备注：
//=========================================================
int UnLikeVM::GetExeCodeLength()
{
	return m_nExeCodeLength;
}

