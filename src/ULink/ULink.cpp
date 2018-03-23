//=========================================================
//ULink
//说明：连接器类实现文件
//
//编写：方寸
//
//
//日期：2013-10-2
//=========================================================
#include <iostream>
#include <stdlib.h>
#include "ULink.h"

using namespace std;

//=========================================================
//函|数：ULink
//说明：构造函数
//输入：
//输出：
//返回：
//日期：2013-10-2
//备注：
//=========================================================
ULink::ULink()
{
	m_strOutputFile="";
	m_fpOutput=NULL;
	m_LabelHead.pNext=NULL;
	m_WriteBackAddressHead.pNext=NULL;
}

//=========================================================
//函数：ULink
//说明：析构函数
//输入：
//输出：
//返回：
//日期：2013-10-2
//备注：
//=========================================================
ULink::~ULink()
{
	ClearCompileLists();
}



//=========================================================
//函数：Analyse
//说明：分析读入内存的源代码
//输入：
//输出：
//返回：
//日期：2013-10-2
//备注：
//=========================================================
void ULink::Analyse()
{
	CompileInit();
	m_fpOutput=fopen(m_strOutputFile.c_str(),"wb");

	if(!m_fpOutput)
	{
		cout<<"Error:创建目标文件"<<m_strOutputFile.c_str()
			<<"失败！"<<endl;
		exit(1);
	}

    m_Token.SetNegative(true);
	LangProgram();

	WriteBackLabel();

	fclose(m_fpOutput);
}

//=========================================================
//函|数：CompileInit
//说明：编译初始化
//输入：
//输出：
//返回：
//日期：2013-10-3
//备注：
//=========================================================
void ULink::CompileInit()
{
	m_nBinCodeAddress=0;//初始化目标代码位置计数
	ClearCompileLists();
}


//=========================================================
//函|数：ClearCompileLists
//说明：清空编译产生的链表
//输入：
//输出：
//返回：
//日期：2013-10-3
//备注：
//=========================================================
void ULink::ClearCompileLists()
{
	LabelNode *plNode;
	WriteBackLabelNode *pwNode;
	WriteBackLabelNode::WriteBackAddressNode *pwaNode;

	if(m_LabelHead.pNext)
	{
		plNode=m_LabelHead.pNext;
		while(plNode)
		{
			m_LabelHead.pNext=plNode->pNext;
			delete plNode;
			plNode=m_LabelHead.pNext;
		}
	}

	if(m_WriteBackAddressHead.pNext)
	{
		pwNode=m_WriteBackAddressHead.pNext;
		while(pwNode)
		{
			pwaNode=pwNode->WriteBackAddress.pNext;
			while(pwaNode)
			{
				pwNode->WriteBackAddress.pNext=pwaNode->pNext;
				delete pwaNode;
				pwaNode=pwNode->WriteBackAddress.pNext;
			}
			m_WriteBackAddressHead.pNext=pwNode->pNext;
			delete pwNode;
			pwNode=m_WriteBackAddressHead.pNext;
		}
	}
}


//=========================================================
//函数：LangStatement
//说明：语法分析语句
//输入：
//输出：
//返回：
//日期：2013-10-2
//备注：
//=========================================================
void ULink::LangStatement()
{
	SString strToken;

	m_Token.Token();
	strToken=m_Token.GetToken();
	strToken.ToUpper();

	if(strToken=="NOOP")//空指令
	{
		LangNOOP();
	}
	else if(strToken=="MOV")
	{
		LangMOV();
	}
	else if(strToken=="CMP")
	{
		LangCMP();
	}
	else if(strToken=="JPC")
	{
		LangJPC();
	}
	else if(strToken=="JMP")
	{
		LangJMP();
	}
	else if(strToken=="PUSH")
	{
		LangPUSH();
	}
	else if(strToken=="POP")
	{
		LangPOP();
	}
	else if(strToken=="CALL")
	{
		LangCALL();
	}
	else if(strToken=="RET")
	{
		LangRET();
	}
	else if(strToken=="IN")
	{
		LangIN();
	}
	else if(strToken=="OUT")
	{
		LangOUT();
	}
	else if(strToken=="CAL")
	{
		LangCAL();
	}
	else if(strToken=="CALF")
	{
		LangCALF();
	}
	else if(strToken=="EXIT")
	{
		LangEXIT();
	}
	else if(strToken=="DATA")
	{
		LangDATA();
	}
	else if(strToken=="BLOCK")
	{
        LangBLOCK();
	}
	else if(strToken==";")//注释
	{
		m_Token.SkipToLineEnd();
	}
	else if(m_Token.GetTokenType()==SToken::TYPE_ENDLINE)//换行
	{
		m_Token.GoBack();
	}
	else if(m_Token.GetTokenType()==SToken::TYPE_LABEL)//标签
	{
		LangLABEL();
	}
	else if(m_Token.GetTokenType()==SToken::TYPE_ENDTEXT)
	{
		m_Token.GoBack();
	}
	else
	{
		cout<<"Error(1) Line("<<m_Token.GetCurrentLineCount()<<"):"
			<<"未知指令："<<m_Token.GetToken().c_str()<<endl;
		exit(1);
	}

	m_Token.Token();
	if(m_Token.GetToken()==";")
		m_Token.SkipToLineEnd();
	else
		m_Token.GoBack();
}

//=========================================================
//函数：LangProgram
//说明：语法分析源程序
//输入：
//输出：
//返回：
//日期：2013-10-2
//备注：
//=========================================================
void ULink::LangProgram()
{
	m_Token.SetTokenType(SToken::TYPE_ENDLINE);
	while(m_Token.GetTokenType()!=SToken::TYPE_ENDTEXT)
	{
		LangStatement();
		m_Token.Token();

		if(m_Token.GetTokenType()!=SToken::TYPE_ENDTEXT && m_Token.GetTokenType()!=SToken::TYPE_ENDLINE)
		{
			cout<<"Error(2) Line("<<m_Token.GetCurrentLineCount()
				<<"):非法语句："<<m_Token.GetToken().c_str()<<endl;
			exit(1);
		}
	}
}

//=========================================================
//函数：LangNOOP
//说明：空指令
//输入：
//输出：
//返回：
//日期：2013-10-2
//备注：
//=========================================================
void ULink::LangNOOP()
{
	char Byte;

	Byte=0x00;

	fwrite(&Byte,1,1,m_fpOutput);
	AddBinCodeAddress(1);
}


//=========================================================
//函数：LangMOV
//说明：MOV指令
//输入：
//输出：
//返回：
//日期：2013-10-2
//备注：
//=========================================================
void ULink::LangMOV()
{
	char BinCode;
	int nAddress;
	int nOldAddress;

	nAddress=GetBinCodeAddress();//保存用来回填指令代码
	AddBinCodeAddress(1);
	BinCode=0x10;
	fwrite(&BinCode,1,1,m_fpOutput);
	LangOperatorDual(&BinCode);
	nOldAddress=GetBinCodeAddress();
	fseek(m_fpOutput,nAddress,SEEK_SET);
	fwrite(&BinCode,1,1,m_fpOutput);
	fseek(m_fpOutput,nOldAddress,SEEK_SET);
}


//=========================================================
//函数：LangCMP
//说明：CMP指令
//输入：
//输出：
//返回：
//日期：2013-10-4
//备注：
//=========================================================
void ULink::LangCMP()
{
	char BinCode;
	int nAddress;
	int nOldAddress;

	nAddress=GetBinCodeAddress();//保存用来回填指令代码
	AddBinCodeAddress(1);
	BinCode=0x20;
	fwrite(&BinCode,1,1,m_fpOutput);
	LangOperatorDual(&BinCode);
	nOldAddress=GetBinCodeAddress();
	fseek(m_fpOutput,nAddress,SEEK_SET);
	fwrite(&BinCode,1,1,m_fpOutput);
	fseek(m_fpOutput,nOldAddress,SEEK_SET);
}

//=========================================================
//函数：LangJPC
//说明：JPC指令
//输入：
//输出：
//返回：
//日期：2013-10-4
//备注：
//=========================================================
void ULink::LangJPC()
{
	int nFindAddress;
	char BinCode;
	SString strToken;

	AddBinCodeAddress(1);
	BinCode=0x30;

	m_Token.Token();
	strToken=m_Token.GetToken();
	strToken.ToUpper();

	if(strToken=="Z")//0
	{
		BinCode|=0x00;
	}
	else if(strToken=="B")//小于
	{
		BinCode|=0x01;
	}
	else if(strToken=="BE")//小于等于
	{
		BinCode|=0x02;
	}
	else if(strToken=="A")//大于
	{
		BinCode|=0x03;
	}
	else if(strToken=="AE")//大于等于
	{
		BinCode|=0x04;
	}
	else if(strToken=="NZ")//非0
	{
		BinCode|=0x05;
	}
	else if(strToken=="E")
	{
		BinCode|=0x06;
	}
	else if(strToken=="NE")
	{
		BinCode|=0x07;
	}
	else
	{
		cout<<"Error(8) Line("<<m_Token.GetCurrentLineCount()
			<<"):"<<m_Token.GetToken().c_str()<<"是非法的逻辑判定符！"<<endl;
		exit(1);
	}

	fwrite(&BinCode,1,1,m_fpOutput);

	nFindAddress=LangHowToFindAddress();
	fwrite(&nFindAddress,1,1,m_fpOutput);
	AddBinCodeAddress(1);
}

//=========================================================
//函数：LangJMP
//说明：JMP指令
//输入：
//输出：
//返回：
//日期：2013-10-4
//备注：
//=========================================================
void ULink::LangJMP()
{
	char BinCode;
	int nAddress;
	int nOldAddress;
	int nFindAddress;

	nOldAddress=GetBinCodeAddress();
	AddBinCodeAddress(1);
	BinCode=0x40;
	fwrite(&BinCode,1,1,m_fpOutput);

	nFindAddress=LangHowToFindAddress();
	nAddress=GetBinCodeAddress();

	switch(nFindAddress)
	{
	case 0://立即数寻址
		BinCode|=0x00;
		break;
	case 1://立即数间接寻址
		BinCode|=0x01;
		break;
	case 2://寄存器寻址
		BinCode|=0x02;
		break;
	case 3://寄存器间接寻址
		BinCode|=0x03;
		break;
	}

	fseek(m_fpOutput,nOldAddress,SEEK_SET);
	fwrite(&BinCode,1,1,m_fpOutput);
	fseek(m_fpOutput,nAddress,SEEK_SET);
}

//=========================================================
//函数：LangPUSH
//说明：PUSH指令
//输入：
//输出：
//返回：
//日期：2013-10-2
//备注：
//=========================================================
void ULink::LangPUSH()
{
	char BinCode;
	int nAddress;
	int nOldAddress;
	int nFindAddress;

	nOldAddress=GetBinCodeAddress();
	AddBinCodeAddress(1);
	BinCode=0x50;
	fwrite(&BinCode,1,1,m_fpOutput);

	nFindAddress=LangHowToFindAddress();
	nAddress=GetBinCodeAddress();
	BinCode|=nFindAddress;

	fseek(m_fpOutput,nOldAddress,SEEK_SET);
	fwrite(&BinCode,1,1,m_fpOutput);
	fseek(m_fpOutput,nAddress,SEEK_SET);
}

//=========================================================
//函数：LangPOP
//说明：POP指令
//输入：
//输出：
//返回：
//日期：2013-10-5
//备注：
//=========================================================
void ULink::LangPOP()
{
	char BinCode;
	int nAddress;
	int nOldAddress;
	int nFindAddress;

	nOldAddress=GetBinCodeAddress();
	AddBinCodeAddress(1);
	BinCode=0x60;
	fwrite(&BinCode,1,1,m_fpOutput);

	nFindAddress=LangHowToFindAddress();
	nAddress=GetBinCodeAddress();
	if(nFindAddress==0)
	{
		cout<<"Error(9) Line("<<m_Token.GetCurrentLineCount()
			<<"):POP后的寻址方式不能为立即数！"<<endl;
		exit(1);
	}
	BinCode|=nFindAddress;

	fseek(m_fpOutput,nOldAddress,SEEK_SET);
	fwrite(&BinCode,1,1,m_fpOutput);
	fseek(m_fpOutput,nAddress,SEEK_SET);
}

//=========================================================
//函数：LangCALL
//说明：CALL指令
//输入：
//输出：
//返回：
//日期：2013-10-5
//备注：
//=========================================================
void ULink::LangCALL()
{
	char BinCode;
	int nAddress;
	int nOldAddress;
	int nFindAddress;

	nOldAddress=GetBinCodeAddress();
	AddBinCodeAddress(1);
	BinCode=0x70;
	fwrite(&BinCode,1,1,m_fpOutput);

	nFindAddress=LangHowToFindAddress();
	nAddress=GetBinCodeAddress();
	BinCode|=nFindAddress;

	fseek(m_fpOutput,nOldAddress,SEEK_SET);
	fwrite(&BinCode,1,1,m_fpOutput);
	fseek(m_fpOutput,nAddress,SEEK_SET);
}

//=========================================================
//函数：LangRET
//说明：RET指令
//输入：
//输出：
//返回：
//日期：2013-10-5
//备注：
//=========================================================
void ULink::LangRET()
{
	int BinCode;

	AddBinCodeAddress(1);
	BinCode=0x80;
	fwrite(&BinCode,1,1,m_fpOutput);
}


//=========================================================
//函数：LangIN
//说明：IN指令
//输入：
//输出：
//返回：
//日期：2013-10-5
//备注：
//=========================================================
void ULink::LangIN()
{
	char BinCode;
	int nAddress;
	int nOldAddress;

	nAddress=GetBinCodeAddress();//保存用来回填指令代码
	AddBinCodeAddress(1);
	BinCode=(char)0x90;
	fwrite(&BinCode,1,1,m_fpOutput);
	LangOperatorDual(&BinCode);
	nOldAddress=GetBinCodeAddress();
	fseek(m_fpOutput,nAddress,SEEK_SET);
	fwrite(&BinCode,1,1,m_fpOutput);
	fseek(m_fpOutput,nOldAddress,SEEK_SET);
}

//=========================================================
//函数：LangOUT
//说明：OUT指令
//输入：
//输出：
//返回：
//日期：2013-10-5
//备注：
//=========================================================
void ULink::LangOUT()
{
	char BinCode;
	int nAddress;
	int nOldAddress;

	nAddress=GetBinCodeAddress();//保存用来回填指令代码
	AddBinCodeAddress(1);
	BinCode=(char)0xA0;
	fwrite(&BinCode,1,1,m_fpOutput);
	LangOperatorDual(&BinCode);
	nOldAddress=GetBinCodeAddress();
	fseek(m_fpOutput,nAddress,SEEK_SET);
	fwrite(&BinCode,1,1,m_fpOutput);
	fseek(m_fpOutput,nOldAddress,SEEK_SET);
}

//=========================================================
//函数：LangCAL
//说明：CAL指令
//输入：
//输出：
//返回：
//日期：2013-10-5
//备注：
//=========================================================
void ULink::LangCAL()
{
	SString strToken;
	char BinCode;

	AddBinCodeAddress(1);
	BinCode=(char)0xB0;

	m_Token.Token();
	strToken=m_Token.GetToken();
	strToken.ToUpper();

	if(strToken=="ADD")
	{
		BinCode|=0x00;
	}
	else if(strToken=="SUB")
	{
		BinCode|=0x01;
	}
	else if(strToken=="MUL")
	{
		BinCode|=0x02;
	}
	else if(strToken=="DIV")
	{
		BinCode|=0x03;
	}
	else if(strToken=="MOV")
	{
		BinCode|=0x04;
	}
	else
	{
		cout<<"Error(10) Line("<<m_Token.GetCurrentLineCount()
			<<"):"<<strToken.c_str()<<"不是算术运算类型！"<<endl;
		exit(1);
	}

	fwrite(&BinCode,1,1,m_fpOutput);
	BinCode=0x00;
	LangOperatorDual(&BinCode);
	fwrite(&BinCode,1,1,m_fpOutput);
	AddBinCodeAddress(1);
}

//=========================================================
//函数：LangCALF
//说明：CALF指令
//输入：
//输出：
//返回：
//日期：2013-10-5
//备注：
//=========================================================
void ULink::LangCALF()
{
	SString strToken;
	char BinCode;

	AddBinCodeAddress(1);
	BinCode=(char)0xC0;

	m_Token.Token();
	strToken=m_Token.GetToken();
	strToken.ToUpper();

	if(strToken=="ADD")
	{
		BinCode|=0x00;
	}
	else if(strToken=="SUB")
	{
		BinCode|=0x01;
	}
	else if(strToken=="MUL")
	{
		BinCode|=0x02;
	}
	else if(strToken=="DIV")
	{
		BinCode|=0x03;
	}
	else if(strToken=="MOV")
	{
		BinCode|=0x04;
	}
	else
	{
		cout<<"Error(10) Line("<<m_Token.GetCurrentLineCount()
			<<"):"<<strToken.c_str()<<"不是算术运算类型！"<<endl;
		exit(1);
	}

	fwrite(&BinCode,1,1,m_fpOutput);
	LangOperatorDual(&BinCode);
}

//=========================================================
//函数：LangDATA
//说明：DATA伪指令
//输入：
//输出：
//返回：
//日期：2013-10-5
//备注：
//=========================================================
void ULink::LangDATA()
{
	LabelNode *plNode;
	SString strToken;

	m_Token.Token();

	if(m_Token.GetTokenType()!=SToken::TYPE_LABEL)
	{
		cout<<"Error(11) Line("<<m_Token.GetCurrentLineCount()
			<<"):DATA后应为标识符！"<<endl;
		exit(1);
	}

	//将标签加入
	strToken=m_Token.GetToken();
	strToken.ToUpper();
	plNode=new LabelNode;
	plNode->nAddress=GetBinCodeAddress();
	plNode->strLabel=strToken;
	plNode->pNext=m_LabelHead.pNext;
	m_LabelHead.pNext=plNode;

	LangDATAParameter();
}

//=========================================================
//函数：LangDATAParameter
//说明：DATA伪指令参数
//输入：
//输出：
//返回：
//日期：2013-10-5
//备注：
//=========================================================
void ULink::LangDATAParameter()
{
	int iInteger;
	float fFloat;

	m_Token.Token();

	if(m_Token.GetTokenType()==SToken::TYPE_NUMBER)
	{
		iInteger=atoi(m_Token.GetToken().c_str());
		fwrite(&iInteger,sizeof(int),1,m_fpOutput);
		AddBinCodeAddress(sizeof(int));
	}
	else if(m_Token.GetTokenType()==SToken::TYPE_STRING)
	{
		fwrite(m_Token.GetToken().c_str(),
		m_Token.GetToken().GetLength()+1,1,m_fpOutput);
		AddBinCodeAddress(m_Token.GetToken().GetLength()+1);
	}
	else if(m_Token.GetTokenType()==SToken::TYPE_FLOAT)
	{
		fFloat=atof(m_Token.GetToken().c_str());
		fwrite(&fFloat,sizeof(float),1,m_fpOutput);
		AddBinCodeAddress(sizeof(float));
	}
	else
	{
		cout<<"Error(12) Line("<<m_Token.GetCurrentLineCount()
			<<"):DATA参数类型错误！"<<endl;
		exit(1);
	}

	m_Token.Token();

	while(m_Token.GetToken()==",")
	{
		m_Token.Token();

		if(m_Token.GetTokenType()==SToken::TYPE_NUMBER)
		{
			iInteger=atoi(m_Token.GetToken().c_str());
			fwrite(&iInteger,4,1,m_fpOutput);
			AddBinCodeAddress(4);
		}
		else if(m_Token.GetTokenType()==SToken::TYPE_STRING)
		{
			fwrite(m_Token.GetToken().c_str(),
				m_Token.GetToken().GetLength()+1,1,m_fpOutput);
			AddBinCodeAddress(m_Token.GetToken().GetLength()+1);
		}
		else
		{
			cout<<"Error(12) Line("<<m_Token.GetCurrentLineCount()
				<<"):DATA参数类型错误！"<<endl;
			exit(1);
		}

		m_Token.Token();
	}

	m_Token.GoBack();
}


//=========================================================
//函数：LangBLOCK
//说明：BLOCK伪指令
//输入：
//输出：
//返回：
//日期：2013-10-5
//备注：
//=========================================================
void ULink::LangBLOCK()
{
	LabelNode *plNode;
	SString strToken;
	int nLength;
	unsigned char chInit;
	int nTemp;
	int i;

	strToken=m_Token.Token();
	strToken.ToUpper();

	//将标签加入
	strToken=m_Token.GetToken();
	strToken.ToUpper();
	plNode=new LabelNode;
	plNode->nAddress=GetBinCodeAddress();
	plNode->strLabel=strToken;
	plNode->pNext=m_LabelHead.pNext;
	m_LabelHead.pNext=plNode;

	m_Token.Token();

	if(m_Token.GetTokenType()!=SToken::TYPE_NUMBER)
	{
		cout<<"Error(13) Line("<<m_Token.GetCurrentLineCount()
			<<"):Block伪指令后跟应该是数字类型！"<<endl;
		exit(1);
	}

	nLength=atoi(m_Token.GetToken().c_str());

	m_Token.Token();

	if(m_Token.GetTokenType()!=SToken::TYPE_NUMBER)
	{
		cout<<"Error(14) Line("<<m_Token.GetCurrentLineCount()
			<<"):Block伪指令数据只能为数字类型！"<<endl;
		exit(1);
	}

	nTemp=atoi(m_Token.GetToken().c_str());
	if(nTemp>255)
	{
		cout<<"Error(15) Line("<<m_Token.GetCurrentLineCount()
			<<"):Block数据初值的范围在0~255！"<<endl;
		exit(1);
	}

	chInit=nTemp;

	for(i=0;i<nLength;i++)
	{
		fputc(chInit,m_fpOutput);
	}

	AddBinCodeAddress(nLength);
}

//=========================================================
//函数：LangEND
//说明：结束指令
//输入：
//输出：
//返回：
//日期：2013-10-2
//备注：
//=========================================================
void ULink::LangEXIT()
{
	char Byte;

	Byte=(char)0xF0;

	fwrite(&Byte,1,1,m_fpOutput);
	AddBinCodeAddress(1);
}

//=========================================================
//函数：LangLABEL
//说明：标签定义
//输入：
//输出：
//返回：
//日期：2013-10-5
//备注：
//=========================================================
void ULink::LangLABEL()
{
	LabelNode *pNewNode;
	SString strLabel;

	strLabel=m_Token.GetToken();
	strLabel.ToUpper();

	m_Token.Token();

	if(m_Token.GetToken()!=":")
	{
		cout<<"Error(17) Line("<<m_Token.GetCurrentLineCount()
			<<"):非法操作符"<<strLabel.c_str()<<endl;
		exit(1);
	}

	pNewNode=new LabelNode;
	if(!pNewNode)
	{
		cout<<"Error(18) Line("<<m_Token.GetCurrentLineCount()
			<<"):标签存储内存申请失败！"<<endl;
		exit(1);
	}

	pNewNode->nAddress=GetBinCodeAddress();
	pNewNode->strLabel=strLabel;
	pNewNode->pNext=m_LabelHead.pNext;
	m_LabelHead.pNext=pNewNode;
}

//=========================================================
//函数：LangOperatorDual
//说明：双操作数
//输入：
//输出：
//返回：
//日期：2013-10-2
//备注：
//=========================================================
void ULink::LangOperatorDual(char *pBinCode)
{
	int nFindAddress[2];

	nFindAddress[0]=LangHowToFindAddress();
	m_Token.Token();

	if(m_Token.GetToken()!=",")
	{
		cout<<"Error(7) Line("<<m_Token.GetCurrentLineCount()
			<<"):缺少一个操作数！"<<endl;
		exit(1);
	}

	nFindAddress[1]=LangHowToFindAddress();

	if(nFindAddress[0]==0 && nFindAddress[1]==0)
	{
		*pBinCode|=0x00;
	}
	else if(nFindAddress[0]==0 && nFindAddress[1]==1)
	{
		*pBinCode|=0x01;
	}
	else if(nFindAddress[0]==0 && nFindAddress[1]==2)
	{
		*pBinCode|=0x02;
	}
	else if(nFindAddress[0]==0 && nFindAddress[1]==3)
	{
		*pBinCode|=0x03;
	}
	else if(nFindAddress[0]==1 && nFindAddress[1]==0)
	{
		*pBinCode|=0x04;
	}
	else if(nFindAddress[0]==1 && nFindAddress[1]==1)
	{
		*pBinCode|=0x05;
	}
	else if(nFindAddress[0]==1 && nFindAddress[1]==2)
	{
		*pBinCode|=0x06;
	}
	else if(nFindAddress[0]==1 && nFindAddress[1]==3)
	{
		*pBinCode|=0x07;
	}
	else if(nFindAddress[0]==2 && nFindAddress[1]==0)
	{
		*pBinCode|=0x08;
	}
	else if(nFindAddress[0]==2 && nFindAddress[1]==1)
	{
		*pBinCode|=0x09;
	}
	else if(nFindAddress[0]==2 && nFindAddress[1]==2)
	{
		*pBinCode|=0x0a;
	}
	else if(nFindAddress[0]==2 && nFindAddress[1]==3)
	{
		*pBinCode|=0x0b;
	}
	else if(nFindAddress[0]==3 && nFindAddress[1]==0)
	{
		*pBinCode|=0x0c;
	}
	else if(nFindAddress[0]==3 && nFindAddress[1]==1)
	{
		*pBinCode|=0x0d;
	}
	else if(nFindAddress[0]==3 && nFindAddress[1]==2)
	{
		*pBinCode|=0x0e;
	}
	else if(nFindAddress[0]==3 && nFindAddress[1]==3)
	{
		*pBinCode|=0x0f;
	}
}

//=========================================================
//函数：LangHowToFindAddress
//说明：寻址方式
//输入：
//输出：
//返回：寻址方式代号
//0：立即数，1：立即数间接寻址，2：寄存器，3：寄存器间接寻址
//日期：2013-10-2
//备注：
//=========================================================
int ULink::LangHowToFindAddress()
{
	SString strToken;
	int nInteger;
	char ch;
	int nRet;

	m_Token.Token();
	if(m_Token.GetTokenType()==SToken::TYPE_LABEL)//直接寻址(标识符)
	{
		strToken=m_Token.GetToken();
		strToken.ToUpper();

		if(strToken.StartWith("R"))//可能是寄存器
		{
			ch=strToken.c_str()[1];
			switch(ch)
			{
			case 'P'://指令寄存器
				nInteger=0x00;
				nRet=2;
				break;
			case 'F'://标志寄存器
				nInteger=0x01;
				nRet=2;
				break;
			case 'T'://栈寄存器
				nInteger=0x02;
				nRet=2;
				break;
			case 'B'://栈寄存器
				nInteger=0x03;
				nRet=2;
				break;
			//4个通用寄存器
			case '0':
			case '1':
			case '2':
			case '3':
				nInteger=ch-'0'+4;
				nRet=2;
				break;
			default://不是寄存器是标识符
				nInteger=0;
				AddWriteBackLabel(strToken,GetBinCodeAddress());
				nRet=0;
				break;
			}
			fwrite(&nInteger,4,1,m_fpOutput);
		}
		else//标识符
		{
			nInteger=0;
			AddWriteBackLabel(strToken,GetBinCodeAddress());
			fwrite(&nInteger,4,1,m_fpOutput);
			nRet=0;
		}
	}
	else if(m_Token.GetToken()=="-")
	{
		m_Token.Token();
		if(m_Token.GetTokenType()!=SToken::TYPE_NUMBER)
		{
			cout<<"Error(19) Line("<<m_Token.GetCurrentLineCount()
				<<"):-号后应为数字！"<<endl;
			exit(1);
		}
		strToken=m_Token.GetToken();
		nInteger=-1*atoi(strToken.c_str());
		fwrite(&nInteger,4,1,m_fpOutput);
		nRet=0;
	}
	else if(m_Token.GetTokenType()==SToken::TYPE_NUMBER)//直接寻址(立即数)
	{
		strToken=m_Token.GetToken();
		nInteger=atoi(strToken.c_str());
		fwrite(&nInteger,4,1,m_fpOutput);
		nRet=0;
	}
	else if(m_Token.GetToken()=="[")//间接寻址
	{
		m_Token.Token();
		if(m_Token.GetTokenType()==SToken::TYPE_LABEL)
		{
			strToken=m_Token.GetToken();
			strToken.ToUpper();

			if(strToken.StartWith("R"))//可能是寄存器
			{
				ch=strToken.c_str()[1];
				switch(ch)
				{
				case 'P'://指令寄存器
					nInteger=0x00;
					nRet=3;
					break;
				case 'F'://标志寄存器
					nInteger=0x01;
					nRet=3;
					break;
				case 'T'://栈寄存器
					nInteger=0x02;
					nRet=3;
					break;
				case 'B'://栈寄存器
					nInteger=0x03;
					nRet=3;
					break;
					//4个通用寄存器
				case '0':
				case '1':
				case '2':
				case '3':
					nInteger=ch-'0'+4;
					nRet=3;
					break;
				default://不是寄存器是标识符
					nInteger=0;
					AddWriteBackLabel(strToken,GetBinCodeAddress());
					nRet=1;
					break;
				}
				fwrite(&nInteger,4,1,m_fpOutput);
			}
			else//标识符
			{
				nInteger=0;
				AddWriteBackLabel(strToken,GetBinCodeAddress());
				fwrite(&nInteger,4,1,m_fpOutput);
				nRet=1;
			}
		}
		else
		{
			cout<<"Error(4) Line("<<m_Token.GetCurrentLineCount()<<
			"):间接寻址的对象只能是标识符或寄存器"<<endl;
			exit(1);
		}

		m_Token.Token();

		if(m_Token.GetToken()!="]")
		{
			cout<<"Error(6) Line("<<m_Token.GetCurrentLineCount()<<
				"):"<<strToken.c_str()<<"后应为]！"<<endl;
			exit(1);
		}
	}
	else
	{
		cout<<"Error(3) Line("<<m_Token.GetCurrentLineCount()<<
			"):寻址方式错误！"<<endl;
		exit(1);
	}

	AddBinCodeAddress(4);
	return nRet;
}

//=========================================================
//函数：WriteBackLabel
//说明：回填标签
//输入：
//输出：
//返回：
//日期：2013-10-6
//备注：
//=========================================================
void ULink::WriteBackLabel()
{
	SString strLabel;
	int nAddress;
	LabelNode *plNode;
	WriteBackLabelNode *pwNode;
	WriteBackLabelNode::WriteBackAddressNode *pwaNode;



	pwNode=m_WriteBackAddressHead.pNext;

	while(pwNode)
	{
		pwaNode=pwNode->WriteBackAddress.pNext;
		strLabel=pwNode->strLabel;

		//在标签链中查找标签
		plNode=m_LabelHead.pNext;

		while(plNode)
		{
			if(strLabel==plNode->strLabel)//找到标签
			{
				nAddress=plNode->nAddress;
				break;
			}

			plNode=plNode->pNext;
		}

		if(strLabel!=plNode->strLabel)
		{
			cout<<"Error(16):标签"<<strLabel.c_str()<<"未找到！"
				<<endl;
			exit(1);
		}

		while(pwaNode)
		{
			fseek(m_fpOutput,pwaNode->nWriteBackAddress,SEEK_SET);
			fwrite(&nAddress,4,1,m_fpOutput);

			pwaNode=pwaNode->pNext;
		}

		pwNode=pwNode->pNext;
	}
}

//=========================================================
//函数：AddLabel
//说明：向标签链中添加一个标签
//输入：strLabel：标签名，nAddress：标签地址
//输出：
//返回：
//日期：2013-10-3
//备注：
//=========================================================
void ULink::AddLabel(SString &strLabel,int nAddress)
{
	LabelNode *pNode;

	pNode=new LabelNode;

	if(!pNode)
	{
		cout<<"Error(5) Line("<<m_Token.GetCurrentLineCount()
			<<"):申请标签信息储存内存失败！"<<endl;
		exit(1);
	}

	pNode->strLabel=strLabel;
	pNode->nAddress=nAddress;
	pNode->pNext=m_LabelHead.pNext;
	m_LabelHead.pNext=pNode;
}

//=========================================================
//函数：AddWriteBackLabel
//说明：添加一个待回填标签
//输入：strLabel：标签名，nAddress：待回填地址
//输出：
//返回：
//日期：2013-10-3
//备注：
//=========================================================
void ULink::AddWriteBackLabel(SString &strLabel,int nAddress)
{
	WriteBackLabelNode *pwNode;
	WriteBackLabelNode::WriteBackAddressNode *paNode;

	pwNode=m_WriteBackAddressHead.pNext;

	//遍历待回填标签链，查找要添加的标签，若不存在则添加一个
	while(pwNode)
	{
		if(pwNode->strLabel==strLabel)//找到标签
		{
			paNode=new WriteBackLabelNode::WriteBackAddressNode;
			if(!paNode)
			{
				cout<<"Error(6) Line("<<m_Token.GetCurrentLineCount()
					<<"):申请标签回填信息存储内存失败！"<<endl;
				exit(1);
			}
			paNode->nWriteBackAddress=nAddress;
			paNode->pNext=pwNode->WriteBackAddress.pNext;
			pwNode->WriteBackAddress.pNext=paNode;
			return;
		}
		pwNode=pwNode->pNext;
	}
	//找不到标签，添加一个

	pwNode=new WriteBackLabelNode;

	pwNode->strLabel=strLabel;
	pwNode->WriteBackAddress.pNext=new WriteBackLabelNode::WriteBackAddressNode;
	pwNode->WriteBackAddress.pNext->pNext=NULL;
	pwNode->WriteBackAddress.pNext->nWriteBackAddress=nAddress;

	pwNode->pNext=m_WriteBackAddressHead.pNext;
	m_WriteBackAddressHead.pNext=pwNode;
}

//=========================================================
//函数：GetBinCodeAddress
//说明：获取当前目标代码地址
//输入：
//输出：
//返回：
//日期：2013-10-4
//备注：
//=========================================================
int ULink::GetBinCodeAddress()
{
	return m_nBinCodeAddress;
}

//=========================================================
//函数：AddBinCodeAddress
//说明：增加目标代码计数
//输入：nCount：要增加的数量
//输出：
//返回：寻址方式代号
//日期：2013-10-2
//备注：
//=========================================================
void ULink::AddBinCodeAddress(int nCount)
{
	m_nBinCodeAddress+=nCount;
}

//=========================================================
//函数：LoadSourceFile
//说明：载入源代码文件
//输入：
//输出：
//返回：
//日期：2013-10-2
//备注：
//=========================================================
bool ULink::LoadSourceFile(const char *strFileName)
{
	FILE *fp;
	long nFileLength;
	char *pStr;

	fp=fopen(strFileName,"rb");

	if(!fp)
		return false;

	fseek(fp,0,SEEK_END);
	nFileLength=ftell(fp);
	fseek(fp,0,SEEK_SET);

	pStr=new char[nFileLength+1];

	if(!pStr)
		return false;

	fread(pStr,nFileLength,1,fp);

	pStr[nFileLength]='\0';

	m_Token.SetText(pStr);
	delete []pStr;

	fclose(fp);

	return true;
}


//=========================================================
//函数：SetOutputFile
//说明：设置输出文件路径
//输入：
//输出：
//返回：
//日期：2013-10-2
//备注：
//=========================================================
void ULink::SetOutputFile(const char *strFileName)
{
	m_strOutputFile=strFileName;
}
