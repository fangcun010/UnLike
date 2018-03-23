//=========================================================
//
//
//UnLikeVM
//˵����UnLike����������ļ�
//
//
//=========================================================

#ifndef _S_UNLIKE_VM_H_
#define _S_UNLIKE_VM_H_

#include <GL/freeglut.h>
#include <GL/glext.h>
#include "../SString.h"

#define						UNLIKE_VM_WNDCLASS			TEXT("UnLikeVMWndClass")

//=========================================================
//��������
//=========================================================
//������Ĵ���
#define								REGISTER(x)							m_Register[x]
#define								RP									m_Register[0]
#define								RF									m_Register[1]
#define								RT									m_Register[2]
#define								RB									m_Register[3]
#define								R0									m_Register[4]
#define								R1									m_Register[5]
#define								R2									m_Register[6]
#define								R3									m_Register[7]
//�����ջ��С
#define								STACK_SIZE							1024000
#define								MEMORY(x)							m_pExeCode[x]

class UnLikeVM
{
	//Data
private:
	char *m_pExeCode;//������ִ�д���
	int m_nExeCodeLength;
	int m_Register[8];//������Ĵ���
	struct StringNode//�ַ�������
	{
		SString str;
		int nID;
		StringNode *pNext;
	};
	struct StringList//����Ŵ�1��ʼ
	{
		StringNode Head;
		StringNode *pLast;
		int nIDCount;
		int nCount;
	} m_StringList;
	//Function
private:
	//=========================================================
	//�ַ�������
	//=========================================================
	int NewString();//�½��ַ���
	SString *GetString(int nID);//��ȡ�ַ���
	void DeleteString(int nID);//ɾ���ַ���
	//=========================================================
	//�����ִ��
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
	void SwapBuffers();//���ƻ����еĻ��浽���������
	//=========================================================
	//VM���ú���
	//=========================================================
	void SetExeCode(char *pExeCode,int nExeCodeLength);//�������д���
	//=========================================================
	//���������
	//=========================================================
	void Reset();//���������
	void Start();//��������¿�ʼ����
	void Run();//���������
	void Stop();//�������ͣ
	void RunACommand();//��ȡ��ִ��һ��ָ��
	//=========================================================
	//�����״̬
	//=========================================================
	int GetRP();//��ȡ��ǰ����ָ��λ��
	int GetExeCodeLength();//��ȡִ�д��볤��
};

#endif
