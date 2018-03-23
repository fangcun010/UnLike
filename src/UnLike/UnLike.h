//=========================================================
//UnLike
//编译器
//
//编写：方寸
//
//
//日期：2015-2-21
//=========================================================
#ifndef _UN_LIKE_H_
#define _UN_LIKE_H_

#include <iostream>
#include <map>
#include <vector>
#include <stack>

#include "../SToken.h"
#include "../SString.h"

using namespace std;

//=========================================================
//
//常量定义
//
//
//
//
//=========================================================

//=========================================================
//关键字
#define 							KEYWORD_IMPORT										"import"
#define								KEYWORD_FUNC											"func"
#define								KEYWORD_AS													"as"
#define								KEYWORD_SUB												"sub"
#define								KEYWORD_WHILE											"while"
#define								KEYWORD_WEND											"wend"
#define								KEYWORD_FOR												"for"
#define								KEYWORD_FEND											"fend"
#define								KEYWORD_IF													"if"
#define								KEYWORD_ELSE												"else"
#define								KEYWORD_ELIF												"elif"
#define								KEYWORD_THEN											"then"
#define								KEYWORD_RETURN										"return"
#define								KEYWORD_STATIC											"static"
#define								KEYWORD_END												"end"
#define 							KEYWORD_INTEGER										"integer"
#define								KEYWORD_FLOAT											"float"
#define								KEYWORD_STRING										"string"
#define 							KEYWORD_ASM												"asm"

//=========================================================
//变量

//大小
#define								VAR_SIZE_INTEGER										4
#define								VAR_SIZE_FLOAT											4
#define								VAR_SIZE_STRING											4
//类型
#define								VAR_TYPE_INTEGER										-1
#define								VAR_TYPE_FLOAT											-2
#define								VAR_TYPE_STRING										-3
#define								VAR_TYPE_NONE											-100

//=========================================================
//标识符类型

#define								LABEL_TYPE_GLOBAL_VARIABLE					0
#define								LABEL_TYPE_LOCAL_VARIABLE						1
#define								LABEL_TYPE_FUNC											2
#define								LABEL_TYPE_NONE											3

//=========================================================
//错误信息
static char strUnLikeCompileErrorInfo[][256]={"error(1):非法语句尾部!",
																							"error(2):import后没有发现文件名!",
																							"error(3):import的文件无法打开!",
																							"error(4):非法语句!",
																							"error(5):数组下标只能为整形!",
																							"error(6):此处应出现=号!",
																							"error(7):函数重定义!",
																							"error(8):函数无返回值!",
																							"error(9):函数名称不合法!",
																							"error(10):参数类型名不合法!",
																							"error(11):此处缺少括号!",
																							"error(12):此处缺少换行!",
																							"error(13):缺少func!",
																							"error(14):if语句缺少then",
																							"error(15):end后找不到if",
																							"error(16):变量定义未完!",
																							"error(17):变量名称不合法!",
																							"error(18):无法找到该变量类型!",
																							"error(19):参数名称不合法!",
																							"error(20):存在同名变量!",
																							"error(21):括号没有结尾!",
																							"error(22):数组长度应为整形数!",
																							"error(23):数组长度后应跟,号或!",
																							"error(24):此处应出现标识符!",
																							"error(25):缺少,号!",
																							"error(26):未定义表达式单元!",
																							"error(27):标识符不存在!",
																							"error(28):此处应为[",
																							"error(29):此处应为]",
																							"error(30):函数返回值类型错误!",
																							"error(31):调用的函数没有返回值!",
																							"error(32):此处应为字符串!",
																							"error(33):未定义标识符!"
																							};
class CompileError
{
private:
	int m_nErrCode;
	int m_nErrLineCount;
	SString m_strFileName;
public:
	void SetError(int nCode,int nErrLineCount,const char *strFileName)
	{
		m_nErrCode=nCode;
		m_strFileName=strFileName;
		m_nErrLineCount=nErrLineCount;
	}
	int GetErrCode()
	{
		return m_nErrCode;
	}
	SString GetErrorInfo()
	{
		char strTemp[512];
		SString strRet;

		sprintf(strTemp,"File(%s) Line(%d):",m_strFileName.c_str(),m_nErrLineCount);
		strRet+=strTemp;
		strRet+=strUnLikeCompileErrorInfo[m_nErrCode];

		return strRet;
	}
};

//=========================================================
//参数信息
struct ParamInfo
{
	SString strName;//参数名称
	int nType;//参数类型
};

//=========================================================
//函数信息
struct FuncInfo
{
	SString strName;//函数名称
	bool bHaveRetValue;//是否有返回值
	int nRetType;//返回类型
	vector<ParamInfo> Params;//函数参数
	map<SString,int> ParamMap;//参数映射
};

//=========================================================
//函数调用信息
struct FuncCallInfo
{
	bool bRet;//需要返回值
};

//=========================================================
//变量信息
//变量类型：-1,-2,-3分别是integer,float,string
struct VariableInfo
{
	SString strName;//变量名称
	int nType;//变量类型
	int nSize;//变量大小
	int nLocation;//变量开始位置
	bool bArray;//是否为数组
	vector<int> Dimens;//维度
};

//变量类型信息
struct VarTypeInfo
{
	int nType;//类型
};

class UnLike
{
//Data
#ifdef DEBUG_UNLIKE
public:
#else
private:
#endif
//=========================================================
//编译相关
	SString m_strOutputFile;//输出文件名
//=========================================================
//词法分析
	SToken m_Token;
	SToken *m_pCurToken;
//=========================================================
//编译状态
	SString m_strCurFileName;//当前编译文件名称
	bool m_bInFunc;//是否在函数中
//=========================================================
//变量
	map<SString,int> m_GlobalVarMap;//全局变量映射
	map<SString,int> m_LocalVarMap;//局部变量映射
	vector<VariableInfo> m_GlobalVars;//全局变量
	vector<VariableInfo> m_LocalVars;//局部变量
	int m_nLocalSize;//局部空间大小
	int m_nCurLocalLocation;//当前局部变量位置
	unsigned int m_nConstString;//字符串常量数量
	stack<VarTypeInfo> m_TypeStack;//类型栈
	stack<FuncCallInfo> m_FuncCallStack;//函数调用信息栈
//=========================================================
//函数
	map<SString,int> m_FuncMap;//函数映射
	vector<FuncInfo> m_Funcs;//函数信息
	FuncInfo m_CurFuncInfo;//当前函数信息
//=========================================================
//代码段
	SString m_strDataCode;//数据段代码
	SString m_strControlCode;//控制段代码
	SString m_strFunctionCode;//函数段代码
	int m_nJMPID;//跳转ID
	int m_nFuncOverID;//函数结束标签ID
//=========================================================
//Function
#ifdef DEBUG_UNLIKE
public:
#else
private:
#endif
//=========================================================
//编译
	void CompileInit();//编译初始化
	void InitLocal();//初始化局部环境
	void MakeError(int nCode);//产生编译错误
	void InitCurFuncInfo();//初始化当前函数信息
	int GetTypeID(SString &strName);//由名称获得类型ID
	int GetLabelType(SString &strName);//获取标识符类型
	int GetTypeSize(int ID);//获取类型大小
	int GetLocalLocation();//获取当前局部变量位置
	void AddLocalLocation(int nAdd);//增加局部变量当前位置
	void PushType(VarTypeInfo &vi);//压入类型
	void PushFuncCall(FuncCallInfo &fci);//压入函数调用信息
	VarTypeInfo PopType();//弹出类型
	FuncCallInfo PopFuncCall();//弹出函数调用信息
//=========================================================
//代码段添加
	void AddControlCode(const char *strCode);
	void AddDataCode(const char *strCode);
	void AddFunctionCode(const char *strCode);
	void AddCode(const char *strCode);//根据上下文增加代码
	int GetJMPID();//获取跳转ID
	void AddJMPID();//增加跳转ID
	unsigned int AddConstString(const char *str);
	void cPush(const char *strName,bool bIn);
	void cPop(const char *strName,bool bIn);
//=========================================================
public:
//=========================================================
//编译相关
	void Compile();//开始编译
	void LoadCodeFromFile(const char *str);//从文件载入代码
	void SetOutputFile(const char *strFileName);//设置输出文件
	bool Output();//输出文件
#ifdef DEBUG_UNLIKE
public:
#else
private:
#endif
//=========================================================
//语法分析
	void gFileBlock(SToken &Token);//文件块
	void gStatement(SToken &Token);//语句
	void gAsm(SToken &Token);//内嵌汇编
	void gFunc(SToken &Token);//函数
	void gFuncParams(SToken &Token);//函数参数
	void gFuncParam(SToken &Token);//函数参数
	bool gFuncStatement(SToken &Token);//函数内部语句
	void gFuncReturn(SToken &Token);//函数返回
	void gImport(SToken &Token);//import语句
	void gWhile(SToken &Token);//while语句
	bool gWhileStatement(SToken &Token);//while内部语句
	void gIf(SToken &Token);//if语句
	bool gIfStatement(SToken &Token,int &nOverID,int nEndID);//if内部语句
	void gIfElIf(SToken &Token,int &nOverID,int nEndID);//If内ElIf语句
	void gIfElse(SToken &Token,int &nOverID);//If内Else语句
	bool gIfElseStatement(SToken &Token);//If内Else内部语句
	void gVariableDefine(SToken &Token,SString &strName);//变量定义
	void gVariableDefineParam(SToken &Token,int nType,int nSize);//变量定义参数
	void gAssign(SToken &Token);//赋值语句
	void gArrayParams(SToken &Token,VariableInfo &vi);//数组参数
	int gUnit(SToken &Token);//单元
    void gAUnit(SToken &Token);//一个逻辑单元
    void gCallFunc(SToken &Token,SString &strName);//函数调用
    void gCallFuncParam(SToken &Token,ParamInfo &pi);//函数调用参数
    void gCallArray(SToken &Token,SString &strName);//数组调用
    void gCallArrayParam(SToken &Token);//数组调用参数
	void gExpression(SToken &Token);//表达式
	void gExpressionT(SToken &Token);
	void gExpressionF(SToken &Token);
	void gExpressionG(SToken &Token);
	void gExpressionH(SToken &Token);
	void gLogicalExpression(SToken &Token);//逻辑表达式
	void gLogicalExpressionT(SToken &Token);
	void gLogicalExpressionF(SToken &Token);
	void gLogicalExpressionG(SToken &Token);
	void gLogicalExpressionH(SToken &Token);
	void gLogicalExpressionI(SToken &Token);
	void gLogicalExpressionJ(SToken &Token);
//=========================================================
//其它
	const char *GetCurFile() const;//获取当前正在编译的文件文件名
};

#endif
