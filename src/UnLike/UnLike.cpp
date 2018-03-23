#include "UnLike.h"
#include <stdlib.h>
#include <assert.h>

//=========================================================
//编译开始
//=========================================================

//=========================================================
//函数：CompileInit
//说明：
//输入：
//输出：
//返回：
//日期：2015-2-22
//备注：
//=========================================================
void UnLike::CompileInit()
{
	m_GlobalVarMap.clear();//初始化全局变量映射
	m_GlobalVars.clear();

	m_pCurToken=&m_Token;

	m_bInFunc=false;
	m_nConstString=0;

	m_strControlCode="";
	m_strFunctionCode="";
	m_strDataCode="";

	m_nJMPID=0;

	while(!m_TypeStack.empty())//清空类型栈
	{
		m_TypeStack.pop();
	}
}

//=========================================================
//函数：InitLocal
//说明：初始化局部环境
//输入：
//输出：
//返回：
//日期：2015-2-22
//备注：
//=========================================================
inline void UnLike::InitLocal()
{
	m_LocalVarMap.clear();
	m_LocalVars.clear();
	m_nLocalSize=0;
	m_nCurLocalLocation=0;

	m_CurFuncInfo.ParamMap.clear();
	m_CurFuncInfo.Params.clear();
}

//=========================================================
//编译结束
//=========================================================

//=========================================================
//编译相关
//=========================================================

//=========================================================
//函数：Compile
//说明：编译
//输入：
//输出：
//返回：
//日期：2015-2-22
//备注：
//=========================================================
void UnLike::Compile()
{
	CompileInit();
	try
	{
		AddCode("in 0,0\nmov [strtemp],r3\n");
		AddDataCode("data strtemp 0\n");
		gFileBlock(m_Token);
		AddCode("exit\n");
	}
	catch(CompileError ce)
	{
		cout<<ce.GetErrorInfo().c_str()<<endl;
	}
}

//=========================================================
//函数：LoadCodeFromFile
//说明：从文件载入代码
//输入：
//输出：
//返回：
//日期：2015-2-22
//备注：
//=========================================================
void UnLike::LoadCodeFromFile(const char *strFileName)
{
    FILE *fp;
    char *pText=NULL;

    fp=fopen(strFileName,"rb");

	assert(fp);

	m_strCurFileName=strFileName;

	fseek(fp,0,SEEK_END);
	long nFileLength=ftell(fp);

	fseek(fp,0,SEEK_SET);

	pText=new char[nFileLength+1];

	fread(pText,nFileLength,1,fp);
	pText[nFileLength]='\0';

	m_Token.SetText(pText);

	delete []pText;

	fclose(fp);
}

//=========================================================
//函数：SetOutputFile
//说明：设置输出文件
//输入：strFileName
//输出：
//返回：
//日期：2015-3-30
//备注：
//=========================================================
void UnLike::SetOutputFile(const char *strFileName)
{
    m_strOutputFile=strFileName;
}

//=========================================================
//函数：Output
//说明：输出文件
//输入：
//输出：
//返回：
//日期：2015-3-30
//备注：
//=========================================================
bool UnLike::Output()
{
	FILE *fp;

	fp=fopen(m_strOutputFile.c_str(),"wb");

	if(!fp)
	{
		return false;
	}

	fwrite(m_strControlCode.c_str(),m_strControlCode.GetLength(),1,fp);
	fwrite(m_strFunctionCode.c_str(),m_strFunctionCode.GetLength(),1,fp);
	fwrite(m_strDataCode.c_str(),m_strDataCode.GetLength(),1,fp);

	fclose(fp);
}

//=========================================================
//函数：MakeError
//说明：产生编译错误
//输入：nCode:错误代码
//输出：
//返回：
//日期：2015-2-22
//备注：
//=========================================================
inline void UnLike::MakeError(int nCode)
{
	CompileError re;

	re.SetError(nCode,m_pCurToken->GetCurrentLineCount(),m_strCurFileName.c_str());

	throw re;
}

//=========================================================
//函数：代码段添加
//说明：
//输入：
//输出：
//返回：
//日期：2015-2-21
//备注：
//=========================================================
void UnLike::AddDataCode(const char *strCode)
{
	m_strDataCode+=strCode;
}

void UnLike::AddControlCode(const char *strCode)
{
	m_strControlCode+=strCode;
}

void UnLike::AddFunctionCode(const char *strCode)
{
	m_strFunctionCode+=strCode;
}

void UnLike::AddCode(const char *strCode)
{
	if(m_bInFunc)
		AddFunctionCode(strCode);
	else
		AddControlCode(strCode);
}

void UnLike::cPush(const char *strName,bool bIn=false)
{
	SString strCode;

	strCode="push ";
	if(bIn)
		strCode+="[";

	strCode+=strName;

	if(bIn)
		strCode+="]";

	strCode+="\n";

	AddCode(strCode.c_str());
}

void UnLike::cPop(const char *strName,bool bIn=false)
{
	SString strCode;

	strCode="pop ";
	if(bIn)
		strCode+="[";

	strCode+=strName;

	if(bIn)
		strCode+="]";

	strCode+="\n";

	AddCode(strCode.c_str());
}

int UnLike::GetJMPID()
{
	return m_nJMPID;
}

void UnLike::AddJMPID()
{
	m_nJMPID++;
}

//=========================================================
//函数：语法分析
//说明：
//输入：
//输出：
//返回：
//日期：2015-2-22
//备注：
//=========================================================


//=========================================================
//函数：gFileBlock
//说明：文件块
//输入：
//输出：
//返回：
//日期：2015-2-22
//备注：
//=========================================================
void UnLike::gFileBlock(SToken &Token)
{
	CompileError ce;
	int nType;

	while(Token.GetTokenType()!=SToken::TYPE_ENDTEXT)
	{
		gStatement(Token);

		Token.Token();
		nType=Token.GetTokenType();

		if(nType==SToken::TYPE_SYMBOL && Token.GetToken()=="@")
		{
			Token.SkipToLineEnd();
			Token.Token();
		}
		else if(nType!=SToken::TYPE_ENDLINE && nType!=SToken::TYPE_ENDTEXT)
		{
			MakeError(0);
		}
	}
}

//=========================================================
//函数：gStatement
//说明：语句
//输入：
//输出：
//返回：
//日期：2015-2-22
//备注：
//=========================================================
void UnLike::gStatement(SToken &Token)
{
	SString strToken=Token.Token();
	int nType=Token.GetTokenType();

	if(strToken==KEYWORD_IMPORT)
	{
		gImport(Token);
	}
	else if(strToken==KEYWORD_FUNC)
	{
		gFunc(Token);
	}
	else if(strToken==KEYWORD_WHILE)
	{
		gWhile(Token);
	}
	else if(strToken==KEYWORD_IF)
	{
		gIf(Token);
	}
	else if(strToken==KEYWORD_ASM)
	{
		gAsm(Token);
	}
	else if(nType==SToken::TYPE_SYMBOL && strToken=="@")
	{
		Token.SkipToLineEnd();
	}
	else if(nType==SToken::TYPE_ENDLINE || nType==SToken::TYPE_ENDTEXT)
	{
		Token.GoBack();
	}
	else if(nType==SToken::TYPE_LABEL)
	{
		if(GetTypeID(strToken)!=VAR_TYPE_NONE)//变量定义
		{
			gVariableDefine(Token,strToken);
		}
		else if(m_GlobalVarMap.count(strToken))//全局变量,可能的变量赋值
		{
			Token.GoBack();
			gAssign(Token);
		}
		else if(m_FuncMap.count(strToken))//函数调用
		{
			FuncCallInfo fci;

			fci.bRet=false;
			PushFuncCall(fci);
			gCallFunc(Token,strToken);
		}
	}
	else
	{
		gExpression(Token);
	}
}

//=========================================================
//函数：gImport
//说明：import语句
//输入：
//输出：
//返回：
//日期：2015-2-22
//备注：
//=========================================================
void UnLike::gImport(SToken &Token)
{
	int nType;
	long nFileLength;
	char *pText=NULL;
	SToken LocalToken;
	SString strCode;
    FILE *fp;

    SString strFileName=Token.Token();

    nType=Token.GetTokenType();

    if(nType!=SToken::TYPE_STRING)
    {
		MakeError(1);
    }

	fp=fopen(strFileName.c_str(),"rb");

	if(!fp)
	{
		MakeError(2);
	}
	fseek(fp,0,SEEK_END);
	nFileLength=ftell(fp);
	fseek(fp,0,SEEK_SET);
	pText=new char[nFileLength+1];
	fread(pText,nFileLength,1,fp);
	pText[nFileLength]='\0';

	LocalToken.SetText(pText);

	delete []pText;

	fclose(fp);

	strCode=";=======================================\n";
	strCode+=";源文件:";
	strCode+=strFileName;
	strCode+="\n;=======================================\n";
	AddControlCode(strCode.c_str());
	AddDataCode(strCode.c_str());
	AddFunctionCode(strCode.c_str());

	gFileBlock(LocalToken);
}

//=========================================================
//函数：gAsm
//说明：内嵌汇编
//输入：
//输出：
//返回：
//日期：2015-2-22
//备注：
//=========================================================
void UnLike::gAsm(SToken &Token)
{
	SString strCode;

	Token.Token();

	if(Token.GetTokenType()!=SToken::TYPE_STRING)
	{
		MakeError(31);
	}

	strCode=Token.GetToken();
	strCode+="\n";
	AddCode(strCode.c_str());
}

//=========================================================
//函数：gWhile
//说明：while语句
//输入：
//输出：
//返回：
//日期：2015-2-22
//备注：
//=========================================================
void UnLike::gWhile(SToken &Token)
{
	int nStartID,nOverID;

	SString strCode;

	nStartID=GetJMPID();
	AddJMPID();
	nOverID=GetJMPID();
	AddJMPID();

	strCode="jmp_";
	strCode+=nStartID;
	strCode+=":\n";

	AddCode(strCode.c_str());

	gLogicalExpression(Token);

	strCode="pop r1\ncmp r1,0\njpc e jmp_";
	strCode+=nOverID;
	strCode+="\n";
	AddCode(strCode.c_str());

	Token.Token();

	if(Token.GetTokenType()!=SToken::TYPE_ENDLINE)
	{
		MakeError(11);
	}

	while(!gWhileStatement(Token))
	{
		Token.Token();

		if(Token.GetTokenType()!=SToken::TYPE_ENDLINE)
		{
			MakeError(11);
		}
	}

	strCode="jmp jmp_";
	strCode+=nStartID;
	strCode+="\njmp_";
	strCode+=nOverID;
	strCode+=":\n";

	AddCode(strCode.c_str());
}

//=========================================================
//函数：gWhileStatement
//说明：while内部语句
//输入：
//输出：
//返回：
//日期：2015-2-24
//备注：
//=========================================================
bool UnLike::gWhileStatement(SToken &Token)
{
	SString strToken=Token.Token();

	if(strToken==KEYWORD_IF)
	{
	}
	else if(strToken==KEYWORD_WHILE)
	{
		gWhile(Token);
	}
	else if(strToken==KEYWORD_WEND)
	{
		return true;
	}
	else if(strToken==KEYWORD_ASM)
	{
		gAsm(Token);
	}
	else if(strToken==KEYWORD_RETURN && m_bInFunc)
	{
		gFuncReturn(Token);
	}
    else if(Token.GetTokenType()==SToken::TYPE_ENDLINE)
    {
		Token.GoBack();
    }
    else if(Token.GetTokenType()==SToken::TYPE_LABEL)
    {
		if(GetTypeID(strToken)!=VAR_TYPE_NONE)//变量定义
		{
			gVariableDefine(Token,strToken);
		}
		else if(m_LocalVarMap.count(strToken))
		{
			Token.GoBack();
			gAssign(Token);
		}
		else if(m_GlobalVarMap.count(strToken))//全局变量,可能的变量赋值
		{
			Token.GoBack();
			gAssign(Token);
		}
		else if(m_FuncMap.count(strToken))//函数调用
		{
			FuncCallInfo fci;

			fci.bRet=false;
			PushFuncCall(fci);
			gCallFunc(Token,strToken);
		}
		else
		{
            MakeError(32);
		}
    }
    else
    {
		MakeError(3);
    }

    return false;
}

//=========================================================
//函数：gIf
//说明：If语句
//输入：
//输出：
//返回：
//日期：2015-2-22

//备注：
//=========================================================
void UnLike::gIf(SToken &Token)
{
	VarTypeInfo vartype;
	SString strCode;
	int nEndID;
	int nOverID;

	nEndID=GetJMPID();
	AddJMPID();

	nOverID=GetJMPID();
	AddJMPID();

	gLogicalExpression(Token);

	vartype=PopType();

	strCode="pop r1\ncmp r1,0\njpc e jmp_";
	strCode+=nOverID;
	strCode+="\n";

	AddCode(strCode.c_str());

	Token.Token();

	if(Token.GetToken()!=KEYWORD_THEN)
	{
		MakeError(13);
	}

	Token.Token();

	if(Token.GetTokenType()!=SToken::TYPE_ENDLINE)
	{
		MakeError(11);
	}

	while(!gIfStatement(Token,nOverID,nEndID))
	{
		Token.Token();

		if(Token.GetTokenType()!=SToken::TYPE_ENDLINE)
		{
			MakeError(11);
		}
	}

	strCode="jmp_";
	strCode+=nOverID;
	strCode+=":\njmp_";
	strCode+=nEndID;
	strCode+=":\n";
	AddCode(strCode.c_str());
}

//=========================================================
//函数：gIfStatement
//说明：if内部语句
//输入：
//输出：
//返回：
//日期：2015-2-25
//备注：
//=========================================================
bool UnLike::gIfStatement(SToken &Token,int &nOverID,int nEndID)
{
	SString strCode;

	SString strToken=Token.Token();

	if(strToken==KEYWORD_IF)
	{
		gIf(Token);
	}
	else if(strToken==KEYWORD_WHILE)
	{
		gWhile(Token);
	}
	else if(strToken==KEYWORD_ELIF)
    {
		strCode="jmp jmp_";
		strCode+=nEndID;
		strCode+="\n";
		AddCode(strCode.c_str());
		gIfElIf(Token,nOverID,nEndID);
	}
	else if(strToken==KEYWORD_ELSE)
	{
		strCode="jmp jmp_";
		strCode+=nEndID;
		strCode+="\n";
		AddCode(strCode.c_str());
		gIfElse(Token,nOverID);
		return true;
	}
	else if(strToken==KEYWORD_END)
	{
		Token.Token();

		if(Token.GetToken()!=KEYWORD_IF)
		{
			MakeError(14);
		}

		return true;
	}
	else if(strToken==KEYWORD_ASM)
	{
		gAsm(Token);
	}
	else if(strToken==KEYWORD_RETURN && m_bInFunc)
	{
		gFuncReturn(Token);
	}
	else if(Token.GetTokenType()==SToken::TYPE_ENDLINE)
	{
		Token.GoBack();
	}
	else if(Token.GetTokenType()==SToken::TYPE_LABEL)
    {
		if(GetTypeID(strToken)!=VAR_TYPE_NONE)//变量定义
		{
			gVariableDefine(Token,strToken);
		}
		else if(m_GlobalVarMap.count(strToken))//全局变量,可能的变量赋值
		{
			Token.GoBack();
			gAssign(Token);
		}
		else if(m_FuncMap.count(strToken))//函数调用
		{
			FuncCallInfo fci;

			fci.bRet=false;
			PushFuncCall(fci);
			gCallFunc(Token,strToken);
		}
		else
		{
            MakeError(32);
		}
    }
	else
	{
		MakeError(3);
	}

	return false;
}

//=========================================================
//函数：gIfElIf
//说明：ElIf语句
//输入：
//输出：
//返回：
//日期：2015-2-26
//备注：
//=========================================================
void UnLike::gIfElIf(SToken &Token,int &nOverID,int nEndID)
{
	SString strCode;
	VarTypeInfo vartype;

	strCode="jmp_";
	strCode+=nOverID;
	strCode+=":\n";

	AddCode(strCode.c_str());

	nOverID=GetJMPID();
	AddJMPID();

	strCode="jmp_";
	strCode+=nOverID;
	strCode+=":\n";

	nOverID=GetJMPID();
	AddJMPID();

	gLogicalExpression(Token);

	vartype=PopType();

	strCode="pop r1\ncmp r1,0\njpc e jmp_";
	strCode+=nOverID;
	strCode+="\n";

	AddCode(strCode.c_str());

	if(Token.Token()!=KEYWORD_THEN)
	{
		MakeError(13);
	}
}

//=========================================================
//函数：gIfElse
//说明：Else语句
//输入：
//输出：
//返回：
//日期：2015-2-26
//备注：
//=========================================================
void UnLike::gIfElse(SToken &Token,int &nOverID)
{
	SString strCode;
    Token.Token();

	strCode="jmp_";
	strCode+=nOverID;
	strCode+=":\n";

	nOverID=GetJMPID();
	AddJMPID();

	AddCode(strCode.c_str());

    if(Token.GetTokenType()!=SToken::TYPE_ENDLINE)
    {
		MakeError(11);
    }

	while(!gIfElseStatement(Token))
	{
		Token.Token();

		if(Token.GetTokenType()!=SToken::TYPE_ENDLINE)
		{
            MakeError(11);
		}
	}
}

//=========================================================
//函数：gIfElseStatement()
//说明：ElIf语句
//输入：
//输出：
//返回：
//日期：2015-2-26
//备注：
//=========================================================
bool UnLike::gIfElseStatement(SToken &Token)
{
	SString strToken=Token.Token();

	if(strToken==KEYWORD_IF)
	{
		gIf(Token);
	}
	else if(strToken==KEYWORD_WHILE)
	{
		gWhile(Token);
	}
	else if(strToken==KEYWORD_END)
	{
		if(Token.Token()!=KEYWORD_IF)
		{
			MakeError(14);
		}

		return true;
	}
	else if(strToken==KEYWORD_ASM)
	{
		gAsm(Token);
	}
	else if(Token.GetTokenType()==SToken::TYPE_ENDLINE)
	{
		Token.GoBack();
	}
	else if(Token.GetTokenType()==SToken::TYPE_LABEL)
    {
		if(GetTypeID(strToken)!=VAR_TYPE_NONE)//变量定义
		{
			gVariableDefine(Token,strToken);
		}
		else if(m_GlobalVarMap.count(strToken))//全局变量,可能的变量赋值
		{
			Token.GoBack();
			gAssign(Token);
		}
		else if(m_FuncMap.count(strToken))//函数调用
		{
			FuncCallInfo fci;

			fci.bRet=false;
			PushFuncCall(fci);
			gCallFunc(Token,strToken);
		}
		else
		{
            MakeError(32);
		}
    }
	else
	{
		MakeError(3);
	}

	return false;
}

//=========================================================
//函数：gUnit
//说明：单元
//输入：
//输出：
//返回：
//日期：2015-2-22
//备注：
//=========================================================
int UnLike::gUnit(SToken &Token)
{
	VarTypeInfo vartype;
	int nVarType;
	SString strCode;
	VariableInfo *pVarInfo=NULL;//变量信息
	FuncInfo *pFuncInfo=NULL;//函数信息
	SString strToken;
	int nType,nLabelType;
	SString strName;

	Token.Token();

	nType=Token.GetTokenType();

	if(nType==SToken::TYPE_LABEL)
	{
		strName=Token.GetToken();

		if(m_bInFunc && m_LocalVarMap.count(strName))//局部变量
		{
			int nID=m_LocalVarMap[strName];
			pVarInfo=&m_LocalVars[nID];

			char str[256];

			if(pVarInfo->bArray)//数组
			{
				if(Token.Token()!="(")
				{
					MakeError(27);
				}
				gArrayParams(Token,*pVarInfo);
				if(Token.Token()!=")")
				{
					MakeError(28);
				}
			}

			strCode="mov r3,rt\n";

			strCode+="cal add r3,";
			sprintf(str,"%d",pVarInfo->nLocation);
			strCode+=str;
			strCode+="\n";

			if(pVarInfo->bArray)
			{
				strCode+="pop r1\ncal add r3,r1\n";
			}

			AddCode(strCode.c_str());
			cPush("r3",true);

			nVarType=pVarInfo->nType;
		}
		else if(m_GlobalVarMap.count(strName))//全局变量
		{
			int nID=m_GlobalVarMap[strName];
			pVarInfo=&m_GlobalVars[nID];
			char str[256];

			if(pVarInfo->bArray)//数组
			{
				if(Token.Token()!="[")
				{
					MakeError(10);
				}
				gArrayParams(Token,*pVarInfo);

				if(Token.Token()!="]")
				{
					MakeError(10);
				}
			}

			strCode="";
			if(pVarInfo->bArray)
			{
				strCode+="pop r1\n";
			}

			strCode+="mov r3,var_";
			strCode+=pVarInfo->strName;
			strCode+="\n";

			if(pVarInfo->bArray)
			{
				strCode+="cal add r3,r1\n";
			}

			AddCode(strCode.c_str());
			cPush("r3",true);
			nVarType=pVarInfo->nType;
		}
		else if(m_FuncMap.count(strName))//函数
		{
			FuncInfo *fi=&m_Funcs[m_FuncMap[strName]];

			if(fi->bHaveRetValue)
            {
				FuncCallInfo  fci;

				fci.bRet=true;

				PushFuncCall(fci);
				gCallFunc(Token,strName);
			}
			else
			{
				MakeError(30);
			}
			nVarType=fi->nRetType;
		}
		else
		{
			MakeError(26);
		}
	}
	else if(nType==SToken::TYPE_NUMBER)
	{
		cPush(Token.GetToken().c_str());

		nVarType=VAR_TYPE_INTEGER;
	}
	else if(nType==SToken::TYPE_FLOAT)
	{
		cPush(Token.GetToken().c_str());

		nVarType=VAR_TYPE_FLOAT;
	}
	else if(nType==SToken::TYPE_STRING)
	{
		unsigned int nID;
		nID=AddConstString(Token.GetToken().c_str());
		char str[256];

		sprintf(str,"const_str%d",nID);

		cPush(str);
		nVarType=VAR_TYPE_STRING;
	}
	else
	{
		MakeError(25);
	}

	vartype.nType=nVarType;
	PushType(vartype);
}

//=========================================================
//函数：gAUnit
//说明：一个单元
//输入：
//输出：
//返回：
//日期：2015-3-13
//备注：
//=========================================================
void UnLike::gAUnit(SToken &Token)
{
	SString strName;

    Token.Token();

    if(Token.GetTokenType()==SToken::TYPE_LABEL)
    {
		strName=Token.GetToken();

		Token.Token();

		if(Token.GetToken()=="(")//函数调用
		{
			gCallFunc(Token,strName);
		}
		else if(Token.GetToken()=="[")//函数调用
		{
			gCallArray(Token,strName);
		}
		else
		{
			Token.GoBack();
		}
	}
	else
	{
		MakeError(25);
	}
}

//=========================================================
//函数：gCallFunc
//说明：函数调用
//输入：
//输出：
//返回：
//日期：2015-3-13
//备注：
//=========================================================
void UnLike::gCallFunc(SToken &Token,SString &strName)
{
	FuncCallInfo fci;
	VarTypeInfo vartype;
	SString strCode;
	FuncInfo *fi=&m_Funcs[m_FuncMap[strName]];
	int fsize=fi->Params.size();

	if(Token.Token()!="(")
	{
        MakeError(10);
	}

	for(int i=0;i<fsize;i++)
	{
		gCallFuncParam(Token,fi->Params[i]);

		if(i!=fsize-1)
		{
			Token.Token();

			if(Token.GetToken()!=",")
			{
				MakeError(24);
			}
		}
	}

	if(Token.Token()!=")")
	{
		MakeError(10);
	}

	strCode="call func_";
	strCode+=strName;
	strCode+="\n";

	fci=PopFuncCall();

	if(!fci.bRet && fi->bHaveRetValue)
	{
		strCode+="pop r1\n";
	}

	if(fci.bRet && !fi->bHaveRetValue)
	{
		MakeError(30);
	}

	AddCode(strCode.c_str());
}

//=========================================================
//函数：gCallFuncParam
//说明：函数调用参数
//输入：
//输出：
//返回：
//日期：2015-3-13
//备注：
//=========================================================
void UnLike::gCallFuncParam(SToken &Token,ParamInfo &pi)
{
	VarTypeInfo vartype;
	gExpression(Token);

	vartype=PopType();
}


//=========================================================
//函数：gCallArray
//说明：数组调用
//输入：
//输出：
//返回：
//日期：2015-3-9
//备注：
//=========================================================
void UnLike::gCallArray(SToken &Token,SString &strName)
{
	while(1)
	{
		gCallArrayParam(Token);
		Token.Token();

		if(Token.GetToken()==")")
		{
			break;
		}
		else if(Token.GetToken()!=",")
		{
			MakeError(24);
		}
	}
}

//=========================================================
//函数：gCallArrayParam
//说明：数组调用参数
//输入：
//输出：
//返回：
//日期：2015-3-13
//备注：
//=========================================================
void UnLike::gCallArrayParam(SToken &Token)
{
	gExpression(Token);
}

//=========================================================
//函数：gExpression
//说明：表达式
//输入：
//输出：
//返回：
//日期：2015-3-9
//备注：
//=========================================================
void UnLike::gExpression(SToken &Token)
{
    gExpressionT(Token);
    gExpressionF(Token);
}

//=========================================================
//函数：gExpressionT
//说明：表达式
//输入：
//输出：
//返回：
//日期：2015-3-9
//备注：
//=========================================================
void UnLike::gExpressionT(SToken &Token)
{
	gExpressionG(Token);
	gExpressionH(Token);
}

//=========================================================
//函数：gExpressionF
//说明：表达式
//输入：
//输出：
//返回：
//日期：2015-3-9
//备注：
//=========================================================
void UnLike::gExpressionF(SToken &Token)
{
	VarTypeInfo vartype,vartype1,vartype2;
	SString strToken;

	strToken=Token.Token();

	if(strToken=="+")//加法
	{
		gExpressionT(Token);

		vartype2=PopType();
		vartype1=PopType();
		cPop("r2");
		cPop("r1");

		if(vartype1.nType==VAR_TYPE_INTEGER && vartype2.nType==VAR_TYPE_INTEGER)//整形<->整形
		{
			AddCode("cal add r1,r2\npush r1\n");

			vartype.nType=VAR_TYPE_INTEGER;
			PushType(vartype);
		}
		else if(vartype1.nType==VAR_TYPE_FLOAT && vartype2.nType==VAR_TYPE_FLOAT)
		{
			AddCode("calf sub r1,r2\npush r1\n");

			vartype.nType=VAR_TYPE_FLOAT;
			PushType(vartype);
		}

		gExpressionF(Token);
	}
	else if(strToken=="-")//减法
	{
		gExpressionT(Token);

		vartype2=PopType();
		vartype1=PopType();
		cPop("r2");
		cPop("r1");

		if(vartype1.nType==VAR_TYPE_INTEGER && vartype2.nType==VAR_TYPE_INTEGER)//整形<->整形
		{
			AddCode("cal sub r1,r2\npush r1\n");

			vartype.nType=VAR_TYPE_INTEGER;
			PushType(vartype);
		}
		else if(vartype1.nType==VAR_TYPE_FLOAT && vartype2.nType==VAR_TYPE_FLOAT)
		{
			AddCode("calf sub r1,r2\npush r1\n");

			vartype.nType=VAR_TYPE_FLOAT;
			PushType(vartype);
		}

		gExpressionF(Token);
	}
	else
	{
		Token.GoBack();
	}
}

//=========================================================
//函数：gExpressionG
//说明：表达式
//输入：
//输出：
//返回：
//日期：2015-3-9
//备注：
//=========================================================
void UnLike::gExpressionG(SToken &Token)
{
	Token.Token();

	if(Token.GetToken()=="(")
	{
		gExpressionT(Token);
        gExpressionF(Token);
        Token.Token();
        if(Token.GetToken()!=")")
        {
			MakeError(20);
        }
	}
	else
	{
		Token.GoBack();
		Token.SetNegative(true);
		gUnit(Token);
		Token.SetNegative(false);
	}
}

//=========================================================
//函数：gExpressionH
//说明：表达式
//输入：
//输出：
//返回：
//日期：2015-3-9
//备注：
//=========================================================
void UnLike::gExpressionH(SToken &Token)
{
	VarTypeInfo vartype,vartype1,vartype2;
	SString strToken;

	strToken=Token.Token();

	if(strToken=="*")
	{
		gExpressionG(Token);

		vartype2=PopType();
		vartype1=PopType();
		cPop("r2");
		cPop("r1");

		if(vartype1.nType==VAR_TYPE_INTEGER && vartype2.nType==VAR_TYPE_INTEGER)//整形<->整形
		{
			AddCode("cal mul r1,r2\npush r1\n");

			vartype.nType=VAR_TYPE_INTEGER;
			PushType(vartype);
		}
		else if(vartype1.nType==VAR_TYPE_FLOAT && vartype2.nType==VAR_TYPE_FLOAT)
		{
			AddCode("calf sub r1,r2\npush r1\n");

			vartype.nType=VAR_TYPE_FLOAT;
			PushType(vartype);
		}

		gExpressionH(Token);
	}
	else if(strToken=="/")
	{
		gExpressionG(Token);

		vartype2=PopType();
		vartype1=PopType();
		cPop("r2");
		cPop("r1");

		if(vartype1.nType==VAR_TYPE_INTEGER && vartype2.nType==VAR_TYPE_INTEGER)//整形<->整形
		{
			AddCode("cal div r1,r2\npush r1\n");

			vartype.nType=VAR_TYPE_INTEGER;
			PushType(vartype);
		}
		else if(vartype1.nType==VAR_TYPE_FLOAT && vartype2.nType==VAR_TYPE_FLOAT)
		{
			AddCode("calf sub r1,r2\npush r1\n");

			vartype.nType=VAR_TYPE_FLOAT;
			PushType(vartype);
		}

		gExpressionH(Token);
	}
	else if(strToken=="%")
	{
		gExpressionG(Token);

		vartype2=PopType();
		vartype1=PopType();
		cPop("r2");
		cPop("r1");

		if(vartype1.nType==VAR_TYPE_INTEGER && vartype2.nType==VAR_TYPE_INTEGER)//整形<->整形
		{
			AddCode("cal mod r1,r2\npush r1\n");

			vartype.nType=VAR_TYPE_INTEGER;
			PushType(vartype);
		}
		else if(vartype1.nType==VAR_TYPE_FLOAT && vartype2.nType==VAR_TYPE_FLOAT)
		{
			AddCode("calf sub r1,r2\npush r1\n");

			vartype.nType=VAR_TYPE_FLOAT;
			PushType(vartype);
		}

		gExpressionH(Token);
	}
	else
	{
		Token.GoBack();
	}
}

//=========================================================
//函数：gLogicalExpression
//说明：逻辑表达式
//输入：
//输出：
//返回：
//日期：2015-3-25
//备注：
//=========================================================
void UnLike::gLogicalExpression(SToken &Token)
{
    gLogicalExpressionT(Token);
    gLogicalExpressionF(Token);
}

//=========================================================
//函数：gLogicalExpressionT
//说明：逻辑表达式
//输入：
//输出：
//返回：
//日期：2015-3-25
//备注：
//=========================================================
void UnLike::gLogicalExpressionT(SToken &Token)
{
	gLogicalExpressionG(Token);
	gLogicalExpressionH(Token);
}

//=========================================================
//函数：gLogicalExpressionF
//说明：逻辑表达式
//输入：
//输出：
//返回：
//日期：2015-3-25
//备注：
//=========================================================
void UnLike::gLogicalExpressionF(SToken &Token)
{
	SString strCode;
	VarTypeInfo vartype1,vartype2;

	Token.Token();

	if(Token.GetToken()=="or")
	{
		int nFalseID,nFalseID2,nOverID;

		vartype1=PopType();

		nFalseID=GetJMPID();AddJMPID();
		nFalseID2=GetJMPID();AddJMPID();
		nOverID=GetJMPID();AddJMPID();


		if(vartype1.nType==VAR_TYPE_INTEGER)
		{
			strCode="pop r1\ncmp r1,0\njpc e jmp_";
			strCode+=nFalseID;
			strCode+="\npush 1\njmp jmp_";
			strCode+=nOverID;
			strCode+="\njmp_";
			strCode+=nFalseID;
			strCode+=":\n";
		}

		AddCode(strCode.c_str());

		gLogicalExpressionT(Token);
		gLogicalExpressionF(Token);

		vartype2=PopType();

		if(vartype1.nType==VAR_TYPE_INTEGER && vartype2.nType==VAR_TYPE_INTEGER)
		{
			strCode="pop r1\ncmp r1,0\njpc e jmp_";
			strCode+=nFalseID2;
			strCode+="\npush 1\njmp jmp_";
			strCode+=nOverID;
			strCode+="\njmp_";
			strCode+=nFalseID2;
			strCode+=":\npush 0\n";

			PushType(vartype1);
		}

		strCode+="jmp_";
		strCode+=nOverID;
		strCode+=":\n";

		AddCode(strCode.c_str());
	}
	else
	{
		Token.GoBack();
	}
}

//=========================================================
//函数：gLogicalExpressionG
//说明：逻辑表达式
//输入：
//输出：
//返回：
//日期：2015-3-25
//备注：
//=========================================================
void UnLike::gLogicalExpressionG(SToken &Token)
{
	VarTypeInfo vartype1,vartype2;
	SString strCode;
	SString strToken;
	SString strE;
	bool bE;

	gLogicalExpressionI(Token);

	strToken=Token.Token();

	if(strToken=="<")
	{
		int nTrueID,nOverID;

		nTrueID=GetJMPID();
		AddJMPID();
		nOverID=GetJMPID();
		AddJMPID();

		strE=Token.Token();

		if(strE=="=")
		{
			bE=true;
		}
		else
		{
			bE=false;
			Token.GoBack();
		}

		gLogicalExpressionI(Token);

		vartype2=PopType();
		vartype1=PopType();
		strCode="pop r2\npop r1\ncmp r1,r2\n";

		if(bE)
		{
			strCode+="jpc be jmp_";
			strCode+=nTrueID;
		}
		else
		{
			strCode+="jpc b jmp_";
			strCode+=nTrueID;
		}
		strCode+="\npush 0\njmp jmp_";
		strCode+=nOverID;
		strCode+="\n";
		strCode+="jmp_";
		strCode+=nTrueID;
		strCode+=":\npush 1\njmp_";
		strCode+=nOverID;
		strCode+=":\n";

		AddCode(strCode.c_str());
	}
	else if(strToken==">")
	{
		int nTrueID,nOverID;

		nTrueID=GetJMPID();
		AddJMPID();
		nOverID=GetJMPID();
		AddJMPID();

		strE=Token.Token();

		if(strE=="=")
		{
			bE=true;
		}
		else
		{
			bE=false;
			Token.GoBack();
		}
		gLogicalExpressionI(Token);

		vartype2=PopType();
		vartype1=PopType();
		strCode="pop r2\npop r1\ncmp r1,r2\n";

		if(bE)
		{
			strCode+="jpc ae jmp_";
			strCode+=nTrueID;

		}
		else
		{
			strCode+="jpc a jmp_";
			strCode+=nTrueID;
		}
		strCode+="\npush 0\njmp jmp_";
		strCode+=nOverID;
		strCode+="\n";
		strCode+="jmp_";
		strCode+=nTrueID;
		strCode+=":\npush 1\njmp_";
		strCode+=nOverID;
		strCode+=":\n";

		AddCode(strCode.c_str());
	}
	else if(strToken=="=")
	{
		int nTrueID;
		int nOverID;

		nTrueID=GetJMPID();
		AddJMPID();
		nOverID=GetJMPID();
		AddJMPID();

		gLogicalExpression(Token);

		vartype2=PopType();
		vartype1=PopType();

		strCode="pop r2\npop r1\ncmp r1,r2\njpc e jmp_";
		strCode+=nTrueID;
		strCode+="\npush 0\njmp jmp_";
		strCode+=nOverID;
		strCode+="\njmp_";
		strCode+=nTrueID;
		strCode+=":\npush 1\njmp_";
		strCode+=nOverID;
		strCode+=":\n";

		AddCode(strCode.c_str());
	}
	else
	{
		Token.GoBack();
	}
}

//=========================================================
//函数：gLogicalExpressionH
//说明：逻辑表达式
//输入：
//输出：
//返回：
//日期：2015-3-25
//备注：
//=========================================================
void UnLike::gLogicalExpressionH(SToken &Token)
{
	VarTypeInfo vartype1,vartype2;
	SString strCode;

	Token.Token();

	if(Token.GetToken()=="and")
	{
		int nTrueID,nFalseID,nOverID;

		vartype1=PopType();

		nTrueID=GetJMPID();AddJMPID();
		nFalseID=GetJMPID();AddJMPID();
		nOverID=GetJMPID();AddJMPID();


		if(vartype1.nType==VAR_TYPE_INTEGER)
		{
			strCode="pop r1\ncmp r1,0\njpc e jmp_";
			strCode+=nFalseID;
			strCode+="\n";
		}

		AddCode(strCode.c_str());

		gLogicalExpressionG(Token);
		gLogicalExpressionH(Token);

		vartype2=PopType();

		if(vartype1.nType==VAR_TYPE_INTEGER && vartype2.nType==VAR_TYPE_INTEGER)
		{
			strCode="pop r1\ncmp r1,0\njpc e jmp_";
			strCode+=nFalseID;
			strCode+="\npush 1\njmp jmp_";
			strCode+=nOverID;
			strCode+="\n";
			PushType(vartype1);
		}

		strCode+="jmp_";
		strCode+=nFalseID;
		strCode+=":\npush 0\n";
		strCode+="jmp_";
		strCode+=nOverID;
		strCode+=":\n";

		AddCode(strCode.c_str());

	}
	else
	{
		Token.GoBack();
	}
}

//=========================================================
//函数：gLogicalExpressionI
//说明：逻辑表达式
//输入：
//输出：
//返回：
//日期：2015-3-25
//备注：
//=========================================================
void UnLike::gLogicalExpressionI(SToken &Token)
{
	int nJMPID;
	SString strCode;
	VarTypeInfo vartype;

	Token.Token();

	if(Token.GetToken()=="not")
	{
        gLogicalExpression(Token);

        vartype=PopType();
        strCode="pop r1\n";

        if(vartype.nType==VAR_TYPE_INTEGER)
        {
			nJMPID=GetJMPID();

			AddJMPID();
			AddJMPID();
            strCode+="cmp r1,0\n";
            strCode+="jpc ne jmp_";
            strCode+=nJMPID+1;
            strCode+="\npush 1\njmp jmp_";
            strCode+=nJMPID+1;
			strCode+="\njmp_";
			strCode+=nJMPID;
			strCode+=":\npush 0\njmp_";
			strCode+=nJMPID+1;
			strCode+=":\n";

			AddCode(strCode.c_str());
			PushType(vartype);
        }
	}
	else if(Token.GetToken()=="(")
	{
		gLogicalExpression(Token);
		if(Token.Token()!=")")
		{
			MakeError(10);
		}
	}
	else
	{
		Token.GoBack();
		gExpression(Token);
	}
}


//=========================================================
//函数：gFunc
//说明：Func语句
//输入：
//输出：
//返回：
//日期：2015-2-22
//备注：
//=========================================================
void UnLike::gFunc(SToken &Token)
{
	SString strBakFuncCode;
	SString strBak;
	SString strCode;
	SString strType;
	SString strName=Token.Token();

	m_bInFunc=true;

	InitLocal();

	m_nFuncOverID=GetJMPID();
	AddJMPID();

	strBakFuncCode=m_strFunctionCode;
	m_strFunctionCode="";
	if(Token.GetTokenType()!=SToken::TYPE_LABEL)
	{
		MakeError(8);
	}

	if(m_FuncMap.count(strName))
	{
		MakeError(6);
	}

	m_CurFuncInfo.strName=strName;

	Token.Token();

	if(Token.GetToken()!="(")
	{
		MakeError(9);
	}

	gFuncParams(Token);

	if(Token.GetToken()!=")")
	{
		MakeError(10);
	}

    Token.Token();

	if(Token.GetToken()==KEYWORD_AS)
	{
        Token.Token();

        if(Token.GetTokenType()!=SToken::TYPE_LABEL)
        {
            MakeError(4);
        }

        strType=Token.GetToken();

        int nRetType=GetTypeID(strType);

        if(nRetType==VAR_TYPE_NONE)
        {
			MakeError(17);
        }

        m_CurFuncInfo.bHaveRetValue=true;
        m_CurFuncInfo.nRetType=nRetType;

        Token.Token();
	}
	else
	{
		m_CurFuncInfo.bHaveRetValue=false;
	}

	AddCode(strCode.c_str());

    if(Token.GetTokenType()!=SToken::TYPE_ENDLINE)
    {
		MakeError(11);
    }
	while(!gFuncStatement(Token))
	{
		Token.Token();
		if(Token.GetTokenType()!=SToken::TYPE_ENDLINE)
		{
			MakeError(0);
		}
	}


	if(m_CurFuncInfo.bHaveRetValue)
	{
		AddCode("push 0\n");
	}

	//恢复rt寄存器
	strCode="jmp_";
	strCode+=m_nFuncOverID;
	strCode+=":\n";

	int sz=m_LocalVars.size();

	for(int i=0;i<sz;i++)
	{
		VariableInfo &vi=m_LocalVars[i];

		if(vi.nType==VAR_TYPE_STRING)
		{
			strCode+="mov r1,rt\ncal add r1,";
			strCode+=vi.nLocation;
			strCode+="\n";
			if(vi.bArray)
			{
				int asz=vi.nSize>>2;

				for(int j=0;j<asz;j++)
				{
					if(j)
					{
						strCode+="cal add r1,4\n";
					}
					strCode+="in [r1],1\n";
				}
			}
			else
			{
				strCode+="in [r1],1\n";
			}
		}
	}

	AddCode(strCode.c_str());

	if(m_nLocalSize)
	{
		char str[256];
		strCode="cal add rt,";
		sprintf(str,"%d",m_nLocalSize);
		strCode+=str;
		strCode+="\n";

		AddCode(strCode.c_str());
	}

	AddCode("ret\n");

	m_bInFunc=false;
	m_FuncMap[strName]=m_Funcs.size();
	m_Funcs.push_back(m_CurFuncInfo);

	strCode=";=======================================\n";
	strCode+=";Function:";
	strCode+=strName;

	if(m_CurFuncInfo.bHaveRetValue)
	{
		strCode+="\n;Return:";
		strCode+=strType;
	}

	strCode+="\n;=======================================\n";

	strCode+="func_";
	strCode+=m_CurFuncInfo.strName;
	strCode+=":\n";

	if(m_nLocalSize)
	{
		char str[256];

		strCode+="cal sub rt,";
		sprintf(str,"%d",m_nLocalSize);
		strCode+=str;
		strCode+="\n";
	}

	strBak=m_strFunctionCode;
	m_strFunctionCode=strBakFuncCode;
	m_strFunctionCode+=strCode;
	m_strFunctionCode+=strBak;
}

//=========================================================
//函数：gFuncParams
//说明：函数参数
//输入：
//输出：
//返回：
//日期：2015-2-22
//备注：
//=========================================================
void UnLike::gFuncParams(SToken &Token)
{
	SString strCode;
	Token.Token();

	if(Token.GetToken()==")")
		return;

	Token.GoBack();

	while(true)
	{
		gFuncParam(Token);
		Token.Token();
		if(Token.GetToken()!=",")
			break;
	}

	int psize;
	vector<ParamInfo> &ps=m_CurFuncInfo.Params;
	psize=ps.size();

	strCode="";
	for(int i=psize-1;i>=0;i--)
	{
			strCode+="mov r2,rt\ncal add r2,";
			strCode+=m_LocalVars[m_LocalVarMap[ps[i].strName]].nLocation;

			if(ps[i].nType!=VAR_TYPE_STRING)
			{
				strCode+="\npop [r2]\n";
			}
			else
			{
				strCode+="\nin 0,0\nmov [r2],r3\n";
                strCode+="pop r3\nin [r2],2\n";
			}
	}

	AddCode(strCode.c_str());
}

//=========================================================
//函数：gFuncParam
//说明：函数内部参数
//输入：
//输出：
//返回：
//日期：2015-2-24
//备注：
//=========================================================
void UnLike::gFuncParam(SToken &Token)
{
	SString strType;
	VariableInfo vi;
	ParamInfo pi;
	int nType;

	Token.Token();

	if(Token.GetTokenType()!=SToken::TYPE_LABEL)
	{
		MakeError(9);
	}

	strType=Token.GetToken();
	nType=GetTypeID(strType);

	if(nType==VAR_TYPE_NONE)
	{
		MakeError(17);
	}
	pi.nType=nType;
	vi.nType=nType;
	Token.Token();

	if(Token.GetTokenType()!=SToken::TYPE_LABEL)
	{
		MakeError(9);
	}

	pi.strName=Token.GetToken();
	vi.strName=pi.strName;
	vi.nSize=GetTypeSize(vi.nType);
	vi.bArray=false;

	int nLastID=m_LocalVars.size()-1;
	if(nLastID!=-1)
		vi.nLocation=m_LocalVars[nLastID].nLocation+m_LocalVars[nLastID].nSize;
	else
		vi.nLocation=0;

	if(m_LocalVarMap.count(vi.strName))
	{
		MakeError(19);
	}

	m_LocalVarMap[vi.strName]=m_LocalVars.size();
	m_LocalVars.push_back(vi);
	m_nLocalSize+=vi.nSize;

	m_CurFuncInfo.Params.push_back(pi);
}

//=========================================================
//函数：gFuncReturn
//说明：函数返回
//输入：
//输出：
//返回：
//日期：2015-3-22
//备注：
//=========================================================
void UnLike::gFuncReturn(SToken &Token)
{
	VarTypeInfo vartype;

	if(m_CurFuncInfo.bHaveRetValue)
	{
		SString strCode;

		gExpression(Token);

		vartype=PopType();

		if(vartype.nType!=m_CurFuncInfo.nRetType)
		{
			MakeError(29);
		}

		strCode="jmp jmp_";
		strCode+=m_nFuncOverID;
		strCode+="\n";
		AddCode(strCode.c_str());
	}
	else
	{
		AddCode("ret\n");
	}
}

//=========================================================
//函数：gFuncStatement
//说明：函数内部语句
//输入：
//输出：
//返回：
//日期：2015-2-24
//备注：
//=========================================================
bool UnLike::gFuncStatement(SToken &Token)
{
	SString strToken=Token.Token();

	if(strToken==KEYWORD_WHILE)
	{
		gWhile(Token);
	}
	else if(strToken==KEYWORD_FOR)
	{
	}
	else if(strToken==KEYWORD_IF)
	{
		gIf(Token);
	}
	else if(strToken==KEYWORD_RETURN)
	{
		gFuncReturn(Token);
	}
	else if(strToken==KEYWORD_END)
	{
        Token.Token();

        if(Token.GetToken()!=KEYWORD_FUNC)
        {
			MakeError(12);
        }

        return true;
	}
	else if(strToken==KEYWORD_ASM)
	{
		gAsm(Token);
	}
	else if(Token.GetTokenType()==SToken::TYPE_ENDLINE)
	{
		Token.GoBack();
	}
	else if(Token.GetTokenType()==SToken::TYPE_LABEL)
	{
		if(GetTypeID(strToken)!=VAR_TYPE_NONE)//变量定义
		{
			gVariableDefine(Token,strToken);
		}
		else if(m_LocalVarMap.count(strToken) || m_GlobalVarMap.count(strToken))//可能的变量赋值
		{
			Token.GoBack();
			gAssign(Token);
		}
		else if(m_FuncMap.count(strToken))//函数调用
		{
			FuncCallInfo fci;

			fci.bRet=false;
			PushFuncCall(fci);
			gCallFunc(Token,strToken);
		}
	}
	else
	{
		MakeError(3);
	}

	return false;
}

//=========================================================
//函数：gVariableDefine
//说明：变量定义
//输入：
//输出：
//返回：
//日期：2015-2-26
//备注：
//=========================================================
void UnLike::gVariableDefine(SToken &Token,SString &strName)
{
    int nType=GetTypeID(strName);
    int nSize;

    if(nType==VAR_TYPE_NONE)
    {
		MakeError(17);
    }

    nSize=GetTypeSize(nType);

	while(1)
	{
		gVariableDefineParam(Token,nType,nSize);

		Token.Token();

		if(Token.GetTokenType()==SToken::TYPE_ENDLINE)
		{
			break;
		}

		if(Token.GetToken()!=",")
		{
			MakeError(15);
		}
	}

	Token.GoBack();
}

//=========================================================
//函数：gVariableDefineParam
//说明：变量定义参数
//输入：
//输出：
//返回：
//日期：2015-2-26
//备注：
//=========================================================
void UnLike::gVariableDefineParam(SToken &Token,int nType,int nSize)
{
	VariableInfo vi;
	SString strCode;

    Token.Token();

    if(Token.GetTokenType()!=SToken::TYPE_LABEL)
    {
		MakeError(16);
    }

    SString strName=Token.GetToken();

	vi.strName=strName;
	vi.nType=nType;
	Token.Token();

	if(Token.GetToken()=="[")
	{
		int tSize;
		int nCount=1;
		vi.bArray=true;

		while(1)
		{
			Token.Token();
			if(Token.GetTokenType()!=SToken::TYPE_NUMBER)
			{
				MakeError(21);
			}
			tSize=atoi(Token.GetToken().c_str());
			vi.Dimens.push_back(tSize);
			nCount*=tSize;

			Token.Token();

			if(Token.GetToken()=="]")
			{
				break;
			}
			else if(Token.GetToken()!=",")
			{
				MakeError(22);
			}
		}
		nSize*=nCount;
	}
	else
	{
		vi.bArray=false;
		vi.nSize=GetTypeSize(vi.nType);
		Token.GoBack();
	}

	vi.nSize=nSize;

    if(m_bInFunc)//在函数中
    {
		if(m_LocalVarMap.count(strName))
		{
			MakeError(19);
		}
		m_LocalVarMap[strName]=m_LocalVars.size();
		int nLastID=m_LocalVars.size()-1;
		if(nLastID!=-1)
			vi.nLocation=m_LocalVars[nLastID].nLocation+m_LocalVars[nLastID].nSize;
		else
			vi.nLocation=0;
		m_LocalVars.push_back(vi);
		m_nLocalSize+=vi.nSize;

		if(vi.nType==VAR_TYPE_STRING)
		{
			strCode="mov r1,rt\ncal add r1,";
			strCode+=vi.nLocation;
			strCode+="\n";

			if(vi.bArray)
			{
				int nCount=vi.nSize/4;
                for(int i=0;i<nCount;i++)
                {
					if(i!=0)
					{
						strCode+="cal add r1,4\n";
					}
					strCode+="in 0,0\nmov [r1],r3\n";
                }
			}
			else
			{
				strCode+="in 0,0\nmov [r1],r3\n";
			}

			AddCode(strCode.c_str());
		}
    }
    else
    {
		if(m_GlobalVarMap.count(strName))
		{
			MakeError(19);
		}
		m_GlobalVarMap[strName]=m_GlobalVars.size();
		m_GlobalVars.push_back(vi);

		if(vi.bArray)//数组
		{
			char strNum[256];
			strCode="block var_";
			strCode+=vi.strName;
			sprintf(strNum," %d",vi.nSize);
			strCode+=strNum;
			strCode+=" 0\n";

			AddDataCode(strCode.c_str());

			if(nType==VAR_TYPE_STRING)
			{
				strCode="mov r0,var_";
				strCode+=vi.strName;
				strCode+="\n";
				int nCount=nSize>>2;

				for(int i=0;i<nCount;i++)
				{
					strCode+="in 0,0\nmov [r0],r3\n";
					if(i!=nCount-1)
					{
						strCode+="cal add r0,4\n";
					}
				}
				AddCode(strCode.c_str());
			}
		}
		else
		{
			strCode="data var_";
			strCode+=vi.strName;
			strCode+=" 0\n";

			AddDataCode(strCode.c_str());

			if(nType==VAR_TYPE_STRING)
			{
                strCode="in 0,0\nmov [var_";
                strCode+=vi.strName;
                strCode+="],r3\n";

                AddCode(strCode.c_str());
			}
		}
    }
}

//=========================================================
//函数：gAssign
//说明：赋值语句
//输入：
//输出：
//返回：
//日期：2015-2-26
//备注：
//=========================================================
void UnLike::gAssign(SToken &Token)
{
    SString strCode;
    SString strName;
    VarTypeInfo vartype;
    int nID;

    strName=Token.Token();
//=========================================================
//局部变量赋值
//=========================================================
    if(m_bInFunc && m_LocalVarMap.count(strName))
    {
        nID=m_LocalVarMap[strName];
        VariableInfo &vi=m_LocalVars[nID];
//=========================================================
//局部数组变量赋值
//=========================================================
        if(vi.bArray)//数组
        {
            if(Token.Token()!="[")
            {
                MakeError(27);
            }

            gArrayParams(Token,vi);

            if(Token.Token()!="]")
            {
                MakeError(28);
            }
            if(Token.Token()!="=")
            {
                MakeError(5);
            }
            gExpression(Token);

            cPop("r2");
            cPop("r1");

			char str[256];

            strCode="mov r3,rt\n";
			strCode+="cal add r3,";
			sprintf(str,"%d",vi.nLocation);
			strCode+=str;
			strCode+="\ncal add r1,r3\n";

			vartype=PopType();

			if(vi.nType==VAR_TYPE_INTEGER && vartype.nType==VAR_TYPE_INTEGER)
			{
				strCode+="mov [r1],r2\n";
				AddCode(strCode.c_str());
			}
			else if(vi.nType==VAR_TYPE_FLOAT && vartype.nType==VAR_TYPE_FLOAT)
			{
				strCode+="mov [r1],r2\n";
				AddCode(strCode.c_str());
			}
			else if(vi.nType==VAR_TYPE_STRING && vartype.nType==VAR_TYPE_STRING)
			{
                strCode+="mov r3,r2\nin [r1],2\n";
                AddCode(strCode.c_str());
			}
        }
//=========================================================
//局部变量赋值
//=========================================================
        else
        {
            if(Token.Token()!="=")
            {
                MakeError(5);
            }
            gExpression(Token);

            vartype=PopType();
            if(vi.nType==VAR_TYPE_INTEGER && vartype.nType==VAR_TYPE_INTEGER)
            {
                char str[256];

                cPop("r2");
                strCode="mov r3,rt\n";
                strCode+="cal add r3,";
                sprintf(str,"%d",vi.nLocation);
                strCode+=str;
                strCode+="\nmov [r3],r2\n";
                AddCode(strCode.c_str());
            }
            else if(vi.nType==VAR_TYPE_FLOAT && vartype.nType==VAR_TYPE_FLOAT)
            {
                char str[256];

                cPop("r2");
                strCode="mov r3,rt\n";
                strCode+="cal add r3,";
                sprintf(str,"%d",vi.nLocation);
                strCode+=str;
                strCode+="\nmov [r3],r2\n";
                AddCode(strCode.c_str());
            }
            else if(vi.nType==VAR_TYPE_STRING && vartype.nType==VAR_TYPE_STRING)
            {
                char str[256];

                cPop("r2");
                strCode="mov r3,rt\n";
                strCode+="cal add r3,";
                sprintf(str,"%d",vi.nLocation);
                strCode+=str;
                strCode+="\nmov r1,r3\nmov r3,r2\nin [r1],2\n";
                AddCode(strCode.c_str());
            }
        }
    }
//=========================================================
//全局变量赋值
//=========================================================
    else if(m_GlobalVarMap.count(strName))
    {
        VariableInfo &vi=m_GlobalVars[m_GlobalVarMap[strName]];
//=========================================================
//全局数组变量赋值
//=========================================================
        if(vi.bArray)//数组
        {
            if(Token.Token()!="[")
            {
                MakeError(27);
            }

            gArrayParams(Token,vi);

            if(Token.Token()!="]")
            {
                MakeError(28);
            }
            if(Token.Token()!="=")
            {
                MakeError(5);
            }
            gExpression(Token);

            cPop("r2");//表达式的值
            cPop("r3");//元素在数组中的位置
            strCode="mov r1,";
            strCode+="var_";
            strCode+=vi.strName;
            strCode+="\ncal add r1,r3\n";

            AddCode(strCode.c_str());

            vartype=PopType();

            if(vi.nType==VAR_TYPE_INTEGER && vartype.nType==VAR_TYPE_INTEGER)
            {
                AddCode("mov [r1],r2\n");
            }
            else if(vi.nType==VAR_TYPE_FLOAT && vartype.nType==VAR_TYPE_FLOAT)
            {
                AddCode("mov [r1],r2\n");
            }
            else if(vi.nType==VAR_TYPE_STRING && vartype.nType==VAR_TYPE_STRING)
            {
				AddCode("mov r3,r2\nin [r1],2\n");
            }
        }
//=========================================================
//全局变量赋值
//=========================================================
        else
        {
            if(Token.Token()!="=")
            {
                MakeError(5);
            }
            gExpression(Token);

            vartype=PopType();
            if(vi.nType==VAR_TYPE_INTEGER && vartype.nType==VAR_TYPE_INTEGER)
            {
                strCode="var_";
                strCode+=vi.strName;
                cPop(strCode.c_str(),true);
            }
            else if(vi.nType==VAR_TYPE_FLOAT && vartype.nType==VAR_TYPE_FLOAT)
            {
                strCode="var_";
                strCode+=vi.strName;
                cPop(strCode.c_str(),true);
            }
            else if(vi.nType==VAR_TYPE_STRING && vartype.nType==VAR_TYPE_STRING)
            {
				strCode="pop r3\n";
				strCode+="in ";
				strCode+="[var_";
				strCode+=vi.strName;
				strCode+="],2\n";
                AddCode(strCode.c_str());
            }
        }
    }
    else
    {
        MakeError(26);
    }
}
//=========================================================
//函数：gArrayParams
//说明：数组参数
//输入：
//输出：
//返回：
//日期：2015-3-21
//备注：
//=========================================================
void UnLike::gArrayParams(SToken &Token,VariableInfo &vi)
{
	SString strCode;
	char str[256];
	VarTypeInfo vartype;
	int dsize=vi.Dimens.size();

	for(int i=1;i<=dsize;i++)
	{
		gExpression(Token);

		if(i!=dsize && Token.Token()!=",")
		{
            MakeError(24);
		}
	}

	//计算元素位置
	int nBase;

	cPop("r1");
	nBase=vi.Dimens[dsize-1];
	for(int i=dsize-2;i>=0;i--)
	{
		vartype=PopType();

		if(vartype.nType!=VAR_TYPE_INTEGER)
		{
			MakeError(4);
		}

		cPop("r2");

		strCode="cal mul r2,";
		sprintf(str,"%d",nBase);
		strCode+=str;
		strCode+="\n";
		AddCode(strCode.c_str());
		AddCode("cal add r1,r2\n");

		nBase*=vi.Dimens[i];
	}
//========================================================
//此行代码极易因为机器的不同而发生错误
	AddCode("cal mul r1,4\n");
//========================================================
	cPush("r1");
}

//=========================================================
//函数：InitCurFuncInfo
//说明：初始化当前函数信息
//输入：
//输出：
//返回：
//日期：2015-2-26
//备注：
//=========================================================
inline void UnLike::InitCurFuncInfo()
{
	m_CurFuncInfo.bHaveRetValue=false;
	m_CurFuncInfo.Params.clear();
}

//=========================================================
//函数：GetTypeID
//说明：由名称获得类型ID
//输入：strName：要获得ID的类型名称
//输出：
//返回：VAR_TYPE_NONE：没有这个类型
//日期：2015-2-26
//备注：
//=========================================================
inline int UnLike::GetTypeID(SString &strName)
{
	if(strName==KEYWORD_INTEGER)
	{
		return VAR_TYPE_INTEGER;
	}
	else if(strName==KEYWORD_FLOAT)
	{
		return VAR_TYPE_FLOAT;
	}
	else if(strName==KEYWORD_STRING)
	{
		return VAR_TYPE_STRING;
	}

	return VAR_TYPE_NONE;
}

//=========================================================
//函数：GetLabelType
//说明：获取标识符类型
//输入：strName:标识符内容
//输出：
//返回：LABEL_TYPE_NONE：没有这个类型
//日期：2015-3-15
//备注：
//=========================================================
int UnLike::GetLabelType(SString &strName)
{
	if(m_bInFunc)
	{
		if(m_LocalVarMap.count(strName))
			return LABEL_TYPE_LOCAL_VARIABLE;
	}
	if(m_GlobalVarMap.count(strName))
		return LABEL_TYPE_GLOBAL_VARIABLE;
	if(m_FuncMap.count(strName))
		return LABEL_TYPE_FUNC;

	return LABEL_TYPE_NONE;
}

//=========================================================
//函数：GetTypeSize
//说明：获取类型大小
//输入：ID：类型ID
//输出：
//返回：
//日期：2015-3-8
//备注：
//=========================================================
inline int UnLike::GetTypeSize(int ID)
{
	switch(ID)
	{
		case VAR_TYPE_INTEGER:
		case VAR_TYPE_FLOAT:
		case VAR_TYPE_STRING:
			return 4;
			break;
	}
}

//=========================================================
//函数：GetLocalLocation
//说明：获取当前局部变量位置
//输入：
//输出：
//返回：
//日期：2015-3-8
//备注：
//=========================================================
inline int UnLike::GetLocalLocation()
{
	return m_nCurLocalLocation;
}

//=========================================================
//函数：AddLocalLocation
//说明：增加局部变量当前位置
//输入：
//输出：
//返回：
//日期：2015-3-8
//备注：
//=========================================================
void UnLike::AddLocalLocation(int nAdd)
{
	m_nCurLocalLocation+=nAdd;
}

//=========================================================
//函数：PushTYpe
//说明：压入类型
//输入：
//输出：
//返回：
//日期：2015-3-21
//备注：
//=========================================================
void UnLike::PushType(VarTypeInfo &vi)
{
	m_TypeStack.push(vi);
}

//=========================================================
//函数：PopTYpe
//说明：弹出类型
//输入：
//输出：
//返回：
//日期：2015-3-21
//备注：
//=========================================================
VarTypeInfo UnLike::PopType()
{
	VarTypeInfo vi;

	vi=m_TypeStack.top();
	m_TypeStack.pop();

	return vi;
}

//=========================================================
//函数：PushFuncCall
//说明：压入函数调用信息
//输入：
//输出：
//返回：
//日期：2015-3-21
//备注：
//=========================================================
void UnLike::PushFuncCall(FuncCallInfo &fci)
{
	m_FuncCallStack.push(fci);
}

//=========================================================
//函数：PopFuncCall
//说明：弹出函数调用信息
//输入：
//输出：
//返回：
//日期：2015-3-21
//备注：
//=========================================================
FuncCallInfo UnLike::PopFuncCall()
{
	FuncCallInfo fci;

	fci=m_FuncCallStack.top();
	m_FuncCallStack.pop();

	return fci;
}

//=========================================================
//函数：AddConstString
//说明：增加字符串常量
//输入：
//输出：
//返回：
//日期：2015-2-26
//备注：
//=========================================================
unsigned int UnLike::AddConstString(const char *str)
{
	char strNum[256];
	SString strCode;

	strCode="data const_str";
	sprintf(strNum,"%d",m_nConstString);
	strCode+=strNum;
	strCode+=" \"";
	strCode+=str;
	strCode+="\"\n";

	AddDataCode(strCode.c_str());

	m_nConstString++;

	return m_nConstString-1;
}

//=========================================================
//函数：GetCurFile
//说明：获取当前正在编译的文件文件名
//输入：
//输出：
//返回：
//日期：2015-2-26
//备注：
//=========================================================
const char *UnLike::GetCurFile() const
{
		return m_strCurFileName.c_str();
}
