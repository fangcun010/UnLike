//=========================================================
//
//
//UnLikeVM
//˵����UnLike�����ʵ���ļ�
//
//
//=========================================================
#include "UnLikeVM.h"
#include <iostream>

using namespace std;

extern UnLikeVM g_UnLikeVM;

//=========================================================
//������UnLikeVM
//˵�������캯��
//���룺
//�����
//���أ�
//���ڣ�2013-11-4
//��ע��
//=========================================================
UnLikeVM::UnLikeVM()
{
	m_pExeCode=NULL;

	//��ʼ���ַ�����
	m_StringList.Head.pNext=NULL;
	m_StringList.pLast=&m_StringList.Head;
	m_StringList.nCount=0;
	m_StringList.nIDCount=0;
}

//=========================================================
//������UnLikeVM
//˵������������
//���룺
//�����
//���أ�
//���ڣ�2013-11-4
//��ע��
//=========================================================
UnLikeVM::~UnLikeVM()
{
	if(m_pExeCode);
		delete []m_pExeCode;
}

//=========================================================
//������NewString
//˵�����½��ַ���
//���룺
//�����
//���أ�
//���ڣ�2013-11-13
//��ע��
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
//������GetString
//˵������ȡ�ַ���
//���룺nID:�ַ���ID
//�����
//���أ�
//���ڣ�2013-11-13
//��ע��
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
//������DeleteString
//˵����ɾ���ַ���
//���룺
//�����
//���أ�
//���ڣ�2013-11-12
//��ע��
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
//������SetExeCode
//˵�����������д���
//���룺
//�����
//���أ�
//���ڣ�2013-11-8
//��ע��
//=========================================================
void UnLikeVM::SetExeCode(char *pExeCode,int nExeCodeLength)
{
	m_pExeCode=new char[nExeCodeLength+STACK_SIZE];
	memcpy(m_pExeCode,pExeCode,nExeCodeLength);
	m_nExeCodeLength=nExeCodeLength;
}

//=========================================================
//������Reset
//˵�������������
//���룺
//�����
//���أ�
//���ڣ�2013-11-8
//��ע��
//=========================================================
void UnLikeVM::Reset()
{
	//��ʼ���Ĵ���
	memset(m_Register,sizeof(int)*8,0);
	RB=m_nExeCodeLength;
	RT=m_nExeCodeLength+STACK_SIZE;
}

//=========================================================
//������RunACommand
//˵������ȡ��ִ��һ��ָ��
//���룺
//�����
//���أ�
//���ڣ�2013-11-8
//��ע��
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
//������CmdNOOP
//˵����
//���룺
//�����
//���أ�
//���ڣ�2013-11-9
//��ע��
//=========================================================
void UnLikeVM::CmdNOOP()
{
}

//=========================================================
//������CmdMOV
//˵����
//���룺
//�����
//���أ�
//���ڣ�2013-11-9
//��ע��
//=========================================================
void UnLikeVM::CmdMOV()
{
	int nOpt1,nOpt2;
	char chOpt=MEMORY(RP);

	nOpt1=*(int *)&MEMORY(RP+1);
	nOpt2=*(int *)&MEMORY(RP+5);

	switch(chOpt&0x0F)
	{
	case 0x00://������-������
		*(int *)&MEMORY(nOpt1)=nOpt2;
		break;
	case 0x01://������-���������Ѱַ
		*(int *)&MEMORY(nOpt1)=*(int *)&MEMORY(nOpt2);
		break;
	case 0x02://������-�Ĵ���
		*(int *)&MEMORY(nOpt1)=REGISTER(nOpt2);
		break;
	case 0x03://������-�Ĵ������Ѱַ
		*(int *)&MEMORY(nOpt1)=*(int *)&MEMORY(REGISTER(nOpt2));
		break;
	case 0x04://���������Ѱַ-������
		*(int *)&MEMORY(nOpt1)=nOpt2;
		break;
	case 0x05://���������Ѱַ-���������Ѱַ
		*(int *)&MEMORY(nOpt1)=*(int *)&MEMORY(nOpt2);
		break;
	case 0x06://���������Ѱַ-�Ĵ���
		*(int *)&MEMORY(nOpt1)=REGISTER(nOpt2);
		break;
	case 0x07://���������Ѱַ-�Ĵ������Ѱַ
		*(int *)&MEMORY(nOpt1)=*(int *)&MEMORY(REGISTER(nOpt2));
		break;
	case 0x08://�Ĵ���-������
		REGISTER(nOpt1)=nOpt2;
		break;
	case 0x09://�Ĵ���-���������Ѱַ
		REGISTER(nOpt1)=*(int *)&MEMORY(nOpt2);
		break;
	case 0x0A://�Ĵ���-�Ĵ���
		REGISTER(nOpt1)=REGISTER(nOpt2);
		break;
	case 0x0B://�Ĵ���-�Ĵ������Ѱַ
		REGISTER(nOpt1)=*(int *)&MEMORY(REGISTER(nOpt2));
		break;
	case 0x0C://�Ĵ������Ѱַ-������
		*(int *)&MEMORY(REGISTER(nOpt1))=nOpt2;
		break;
	case 0x0D://�Ĵ������Ѱַ-���������Ѱַ
		*(int *)&MEMORY(REGISTER(nOpt1))=*(int *)&MEMORY(nOpt2);
		break;
	case 0x0E://�Ĵ������Ѱַ-�Ĵ���
		*(int *)&MEMORY(REGISTER(nOpt1))=REGISTER(nOpt2);
		break;
	case 0x0F://�Ĵ������Ѱַ-�Ĵ������Ѱַ
		*(int *)&MEMORY(REGISTER(nOpt1))=*(int *)&MEMORY(REGISTER(nOpt2));
		break;
	}
}

//=========================================================
//������CmdCMP
//˵����
//���룺
//�����
//���أ�
//���ڣ�2013-11-9
//��ע��
//=========================================================
void UnLikeVM::CmdCMP()
{
	int nOpt1,nOpt2;
	char chOpt=MEMORY(RP);

	nOpt1=*(int *)&MEMORY(RP+1);
	nOpt2=*(int *)&MEMORY(RP+5);

	switch(chOpt&0x0F)
	{
	case 0x00://������-������
		break;
	case 0x01://������-���������Ѱַ
		nOpt2=*(int *)&MEMORY(nOpt2);
		break;
	case 0x02://������-�Ĵ���
		nOpt2=REGISTER(nOpt2);
		break;
	case 0x03://������-�Ĵ������Ѱַ
		nOpt2=*(int *)&MEMORY(REGISTER(nOpt2));
		break;
	case 0x04://���������Ѱַ-������
		nOpt1=*(int *)&MEMORY(nOpt1);
		break;
	case 0x05://���������Ѱַ-���������Ѱַ
		nOpt1=*(int *)&MEMORY(nOpt1);
		nOpt2=*(int *)&MEMORY(nOpt2);
		break;
	case 0x06://���������Ѱַ-�Ĵ���
		nOpt1=*(int *)&MEMORY(nOpt1);
		nOpt2=REGISTER(nOpt2);
		break;
	case 0x07://���������Ѱַ-�Ĵ������Ѱַ
		nOpt1=*(int *)&MEMORY(nOpt1);
		nOpt2=*(int *)&MEMORY(REGISTER(nOpt2));
		break;
	case 0x08://�Ĵ���-������
		nOpt1=REGISTER(nOpt1);
		break;
	case 0x09://�Ĵ���-���������Ѱַ
		nOpt1=REGISTER(nOpt1);
		nOpt2=*(int *)&MEMORY(nOpt2);
		break;
	case 0x0A://�Ĵ���-�Ĵ���
		nOpt1=REGISTER(nOpt1);
		nOpt2=REGISTER(nOpt2);
		break;
	case 0x0B://�Ĵ���-�Ĵ������Ѱַ
		nOpt1=REGISTER(nOpt1);
		nOpt2=*(int *)&MEMORY(REGISTER(nOpt2));
		break;
	case 0x0C://�Ĵ������Ѱַ-������
		nOpt1=*(int *)&MEMORY(REGISTER(nOpt1));
		break;
	case 0x0D://�Ĵ������Ѱַ-���������Ѱַ
		nOpt1=*(int *)&MEMORY(REGISTER(nOpt1));
		nOpt2=*(int *)&MEMORY(nOpt2);
		break;
	case 0x0E://�Ĵ������Ѱַ-�Ĵ���
		nOpt1=*(int *)&MEMORY(REGISTER(nOpt1));
		nOpt2=REGISTER(nOpt2);
		break;
	case 0x0F://�Ĵ������Ѱַ-�Ĵ������Ѱַ
		nOpt1=*(int *)&MEMORY(REGISTER(nOpt1));
		nOpt2=*(int *)&MEMORY(REGISTER(nOpt2));
		break;
	}
	RF=0;
	if(nOpt1==0)//����1Ϊ0
		RF|=1;
	if(nOpt1>nOpt2)//����1���ڲ���2
		RF|=2;
	if(nOpt1==nOpt2)//����1���ڲ���2
		RF|=4;
}

//=========================================================
//������CmdJPC
//˵����
//���룺
//�����
//���أ�
//���ڣ�2013-11-9
//��ע��
//=========================================================
void UnLikeVM::CmdJPC()
{
	char chFindOpt=MEMORY(RP+5);
	char chOpt=MEMORY(RP);
	int nOpt=*(int *)&MEMORY(RP+1);

	switch(chFindOpt)
	{
	case 0://������
		break;
	case 1://���������Ѱַ
		nOpt=*(int *)&MEMORY(nOpt);
		break;
	case 2://�Ĵ���
		nOpt=REGISTER(nOpt);
		break;
	case 3://�Ĵ������Ѱַ
		nOpt=*(int *)&MEMORY(REGISTER(nOpt));
		break;
	}

	switch(chOpt&0x0F)
	{
	case 0x00://Z
		if(RF&1)//����1Ϊ0
		{
			RP=nOpt;
			RP-=6;
		}
		break;
	case 0x01://B
		if((RF&2)==0 && (RF&4)==0)//����1С�ڲ���2
		{
			RP=nOpt;
			RP-=6;
		}
		break;
	case 0x02://BE
		if((RF&2)==0 || (RF&4))//����1С�ڵ��ڲ���2
		{
			RP=nOpt;
			RP-=6;
		}
		break;
	case 0x03://A
		if(RF&2 && (RF&4)==0)//����1���ڲ���2
		{
			RP=nOpt;
			RP-=6;
		}
		break;
	case 0x04://AE
		if((RF&2) || (RF&4))// ����1���ڵ��ڲ���2
		{
			RP=nOpt;
			RP-=6;
		}
		break;
	case 0x05://NZ
		if((RF&1)==0)//����1��0
		{
			RP=nOpt;
			RP-=6;
		}
		break;
	case 0x06://E
		if(RF&4)//����1���ڲ���2
		{
			RP=nOpt;
			RP-=6;
		}
		break;
	case 0x07://NE
		if((RF&4)==0)//����1�����ڲ���2
		{
			RP=nOpt;
			RP-=6;
		}
		break;
	}
}

//=========================================================
//������CmdJMP
//˵����
//���룺
//�����
//���أ�
//���ڣ�2013-11-9
//��ע��
//=========================================================
void UnLikeVM::CmdJMP()
{
	char chOpt=MEMORY(RP);
	int nOpt=*(int *)&MEMORY(RP+1);

	switch(chOpt&0x0F)
	{
	case 0://������
		break;
	case 1://���������Ѱַ
		nOpt=*(int *)&MEMORY(nOpt);
		break;
	case 2://�Ĵ���
		nOpt=REGISTER(nOpt);
		break;
	case 3://�Ĵ������Ѱַ
		nOpt=*(int *)&MEMORY(REGISTER(nOpt));
		break;
	}

	RP=nOpt;
	RP-=5;
}

//=========================================================
//������CmdPUSH
//˵����
//���룺
//�����
//���أ�
//���ڣ�2013-11-9
//��ע��
//=========================================================
void UnLikeVM::CmdPUSH()
{
	char chOpt=MEMORY(RP);
	int nOpt=*(int *)&MEMORY(RP+1);

	switch(chOpt&0x0F)
	{
	case 0://������
		break;
	case 1://���������Ѱַ
		nOpt=*(int *)&MEMORY(nOpt);
		break;
	case 2://�Ĵ���
		nOpt=REGISTER(nOpt);
		break;
	case 3://�Ĵ������Ѱַ
		nOpt=*(int *)&MEMORY(REGISTER(nOpt));
		break;
	}

	*(int *)&MEMORY(RB)=nOpt;
	RB+=4;
}

//=========================================================
//������CmdPOP
//˵����
//���룺
//�����
//���أ�
//���ڣ�2013-11-9
//��ע��
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
	case 0://������
		break;
	case 1://���������Ѱַ
		*(int *)&MEMORY(nOpt)=nPop;
		break;
	case 2:;//�Ĵ���
		REGISTER(nOpt)=nPop;
		break;
	case 3://�Ĵ������Ѱַ
		*(int *)&MEMORY(REGISTER(nOpt))=nPop;
		break;
	}
}

//=========================================================
//������CmdCALL
//˵����
//���룺
//�����
//���أ�
//���ڣ�2013-11-9
//��ע��
//=========================================================
void UnLikeVM::CmdCALL()
{
	char chOpt=MEMORY(RP);
	int nOpt=*(int *)&MEMORY(RP+1);

	switch(chOpt&0x0F)
	{
	case 0://������
		break;
	case 1://���������Ѱַ
		nOpt=*(int *)&MEMORY(nOpt);
		break;
	case 2://�Ĵ���
		nOpt=REGISTER(nOpt);
		break;
	case 3://�Ĵ������Ѱַ
		nOpt=*(int *)&MEMORY(REGISTER(nOpt));
		break;
	}

	RT-=4;
	*(int *)&MEMORY(RT)=RP+5;
	RP=nOpt;
	RP-=5;
}

//=========================================================
//������CmdRET
//˵����
//���룺
//�����
//���أ�
//���ڣ�2013-11-9
//��ע��
//=========================================================
void UnLikeVM::CmdRET()
{
	RP=*(int *)&MEMORY(RT);
	RT+=4;
	RP-=1;
}

//=========================================================
//������CmdIN
//˵����
//���룺
//�����
//���أ�
//���ڣ�2013-11-9
//��ע��
//=========================================================
void UnLikeVM::CmdIN()
{
	int nOpt1,nOpt2;
	char chOpt=MEMORY(RP);

	nOpt1=*(int *)&MEMORY(RP+1);
	nOpt2=*(int *)&MEMORY(RP+5);

	switch(chOpt&0x0F)
	{
	case 0x00://������-������
		break;
	case 0x01://������-���������Ѱַ
		nOpt2=*(int *)&MEMORY(nOpt2);
		break;
	case 0x02://������-�Ĵ���
		nOpt2=REGISTER(nOpt2);
		break;
	case 0x03://������-�Ĵ������Ѱַ
		nOpt2=*(int *)&MEMORY(REGISTER(nOpt2));
		break;
	case 0x04://���������Ѱַ-������
		nOpt1=*(int *)&MEMORY(nOpt1);
		break;
	case 0x05://���������Ѱַ-���������Ѱַ
		nOpt1=*(int *)&MEMORY(nOpt1);
		nOpt2=*(int *)&MEMORY(nOpt2);
		break;
	case 0x06://���������Ѱַ-�Ĵ���
		nOpt1=*(int *)&MEMORY(nOpt1);
		nOpt2=REGISTER(nOpt2);
		break;
	case 0x07://���������Ѱַ-�Ĵ������Ѱַ
		nOpt1=*(int *)&MEMORY(nOpt1);
		nOpt2=*(int *)&MEMORY(REGISTER(nOpt2));
		break;
	case 0x08://�Ĵ���-������
		nOpt1=REGISTER(nOpt1);
		break;
	case 0x09://�Ĵ���-���������Ѱַ
		nOpt1=REGISTER(nOpt1);
		nOpt2=*(int *)&MEMORY(nOpt2);
		break;
	case 0x0A://�Ĵ���-�Ĵ���
		nOpt1=REGISTER(nOpt1);
		nOpt2=REGISTER(nOpt2);
		break;
	case 0x0B://�Ĵ���-�Ĵ������Ѱַ
		nOpt1=REGISTER(nOpt1);
		nOpt2=*(int *)&MEMORY(REGISTER(nOpt2));
		break;
	case 0x0C://�Ĵ������Ѱַ-������
		nOpt1=*(int *)&MEMORY(REGISTER(nOpt1));
		break;
	case 0x0D://�Ĵ������Ѱַ-���������Ѱַ
		nOpt1=*(int *)&MEMORY(REGISTER(nOpt1));
		nOpt2=*(int *)&MEMORY(nOpt2);
		break;
	case 0x0E://�Ĵ������Ѱַ-�Ĵ���
		nOpt1=*(int *)&MEMORY(REGISTER(nOpt1));
		nOpt2=REGISTER(nOpt2);
		break;
	case 0x0F://�Ĵ������Ѱַ-�Ĵ������Ѱַ
		nOpt1=*(int *)&MEMORY(REGISTER(nOpt1));
		nOpt2=*(int *)&MEMORY(REGISTER(nOpt2));
		break;
	}

	switch(nOpt2)
	{
	case 0x00://�����ַ������
		R3=-NewString();
		break;
	case 0x01://ɾ��������ַ������
		DeleteString(nOpt1);
		break;
	case 0x02://�ַ�����ֵ
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
//������CmdOUT
//˵����
//���룺
//�����
//���أ�
//���ڣ�2013-11-9
//��ע��
//=========================================================
void UnLikeVM::CmdOUT()
{
	int nOpt1,nOpt2;
	char chOpt=MEMORY(RP);

	nOpt1=*(int *)&MEMORY(RP+1);
	nOpt2=*(int *)&MEMORY(RP+5);

	switch(chOpt&0x0F)
	{
	case 0x00://������-������
		break;
	case 0x01://������-���������Ѱַ
		nOpt2=*(int *)&MEMORY(nOpt2);
		break;
	case 0x02://������-�Ĵ���
		nOpt2=REGISTER(nOpt2);
		break;
	case 0x03://������-�Ĵ������Ѱַ
		nOpt2=*(int *)&MEMORY(REGISTER(nOpt2));
		break;
	case 0x04://���������Ѱַ-������
		nOpt1=*(int *)&MEMORY(nOpt1);
		break;
	case 0x05://���������Ѱַ-���������Ѱַ
		nOpt1=*(int *)&MEMORY(nOpt1);
		nOpt2=*(int *)&MEMORY(nOpt2);
		break;
	case 0x06://���������Ѱַ-�Ĵ���
		nOpt1=*(int *)&MEMORY(nOpt1);
		nOpt2=REGISTER(nOpt2);
		break;
	case 0x07://���������Ѱַ-�Ĵ������Ѱַ
		nOpt1=*(int *)&MEMORY(nOpt1);
		nOpt2=*(int *)&MEMORY(REGISTER(nOpt2));
		break;
	case 0x08://�Ĵ���-������
		nOpt1=REGISTER(nOpt1);
		break;
	case 0x09://�Ĵ���-���������Ѱַ
		nOpt1=REGISTER(nOpt1);
		nOpt2=*(int *)&MEMORY(nOpt2);
		break;
	case 0x0A://�Ĵ���-�Ĵ���
		nOpt1=REGISTER(nOpt1);
		nOpt2=REGISTER(nOpt2);
		break;
	case 0x0B://�Ĵ���-�Ĵ������Ѱַ
		nOpt1=REGISTER(nOpt1);
		nOpt2=*(int *)&MEMORY(REGISTER(nOpt2));
		break;
	case 0x0C://�Ĵ������Ѱַ-������
		nOpt1=*(int *)&MEMORY(REGISTER(nOpt1));
		break;
	case 0x0D://�Ĵ������Ѱַ-���������Ѱַ
		nOpt1=*(int *)&MEMORY(REGISTER(nOpt1));
		nOpt2=*(int *)&MEMORY(nOpt2);
		break;
	case 0x0E://�Ĵ������Ѱַ-�Ĵ���
		nOpt1=*(int *)&MEMORY(REGISTER(nOpt1));
		nOpt2=REGISTER(nOpt2);
		break;
	case 0x0F://�Ĵ������Ѱַ-�Ĵ������Ѱַ
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
//������CmdCAL
//˵����
//���룺
//�����
//���أ�
//���ڣ�2013-11-9
//��ע��
//=========================================================
void UnLikeVM::CmdCAL()
{
	char chSym;//��ʶ�Ӽ��˳�ȡ��
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
	case 0x00://������-������
		break;
	case 0x01://������-���������Ѱַ
		nOpt2=*(int *)&MEMORY(nOpt2);
		break;
	case 0x02://������-�Ĵ���
		nOpt2=REGISTER(nOpt2);
		break;
	case 0x03://������-�Ĵ������Ѱַ
		nOpt2=*(int *)&MEMORY(REGISTER(nOpt2));
		break;
	case 0x04://���������Ѱַ-������
		nOpt1=*(int *)&MEMORY(nOpt1);
		break;
	case 0x05://���������Ѱַ-���������Ѱַ
		nOpt1=*(int *)&MEMORY(nOpt1);
		nOpt2=*(int *)&MEMORY(nOpt2);
		break;
	case 0x06://���������Ѱַ-�Ĵ���
		nOpt1=*(int *)&MEMORY(nOpt1);
		nOpt2=REGISTER(nOpt2);
		break;
	case 0x07://���������Ѱַ-�Ĵ������Ѱַ
		nOpt1=*(int *)&MEMORY(nOpt1);
		nOpt2=*(int *)&MEMORY(REGISTER(nOpt2));
		break;
	case 0x08://�Ĵ���-������
		nOpt1=REGISTER(nOpt1);
		break;
	case 0x09://�Ĵ���-���������Ѱַ
		nOpt1=REGISTER(nOpt1);
		nOpt2=*(int *)&MEMORY(nOpt2);
		break;
	case 0x0A://�Ĵ���-�Ĵ���
		nOpt1=REGISTER(nOpt1);
		nOpt2=REGISTER(nOpt2);
		break;
	case 0x0B://�Ĵ���-�Ĵ������Ѱַ
		nOpt1=REGISTER(nOpt1);
		nOpt2=*(int *)&MEMORY(REGISTER(nOpt2));
		break;
	case 0x0C://�Ĵ������Ѱַ-������
		nOpt1=*(int *)&MEMORY(REGISTER(nOpt1));
		break;
	case 0x0D://�Ĵ������Ѱַ-���������Ѱַ
		nOpt1=*(int *)&MEMORY(REGISTER(nOpt1));
		nOpt2=*(int *)&MEMORY(nOpt2);
		break;
	case 0x0E://�Ĵ������Ѱַ-�Ĵ���
		nOpt1=*(int *)&MEMORY(REGISTER(nOpt1));
		nOpt2=REGISTER(nOpt2);
		break;
	case 0x0F://�Ĵ������Ѱַ-�Ĵ������Ѱַ
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
	case 0x00://������-������
		break;
	case 0x01://������-���������Ѱַ
		break;
	case 0x02://������-�Ĵ���
		break;
	case 0x03://������-�Ĵ������Ѱַ
		break;
	case 0x04://���������Ѱַ-������
		*(int *)&MEMORY(nOpt1)=nResult;
		break;
	case 0x05://���������Ѱַ-���������Ѱַ
		*(int *)&MEMORY(nOpt1)=nResult;
		break;
	case 0x06://���������Ѱַ-�Ĵ���
		*(int *)&MEMORY(nOpt1)=nResult;
		break;
	case 0x07://���������Ѱַ-�Ĵ������Ѱַ
		*(int *)&MEMORY(nOpt1)=nResult;
		break;
	case 0x08://�Ĵ���-������
		REGISTER(nOpt1)=nResult;
		break;
	case 0x09://�Ĵ���-���������Ѱַ
		REGISTER(nOpt1)=nResult;
		break;
	case 0x0A://�Ĵ���-�Ĵ���
		REGISTER(nOpt1)=nResult;
		break;
	case 0x0B://�Ĵ���-�Ĵ������Ѱַ
		REGISTER(nOpt1)=nResult;
		break;
	case 0x0C://�Ĵ������Ѱַ-������
		*(int *)&MEMORY(REGISTER(nOpt1))=nResult;
		break;
	case 0x0D://�Ĵ������Ѱַ-���������Ѱַ
		*(int *)&MEMORY(REGISTER(nOpt1))=nResult;
		break;
	case 0x0E://�Ĵ������Ѱַ-�Ĵ���
		*(int *)&MEMORY(REGISTER(nOpt1))=nResult;
		break;
	case 0x0F://�Ĵ������Ѱַ-�Ĵ������Ѱַ
		*(int *)&MEMORY(REGISTER(nOpt1))=nResult;
		break;
	}
}

//=========================================================
//������CmdCALF
//˵����
//���룺
//�����
//���أ�
//���ڣ�2013-11-9
//��ע��
//=========================================================
void UnLikeVM::CmdCALF()
{
	char chSym;//��ʶ�Ӽ��˳�ȡ��
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
	case 0x00://������-������
		break;
	case 0x01://������-���������Ѱַ
		fOpt2=*(float *)&MEMORY(nOpt2);
		break;
	case 0x02://������-�Ĵ���
		fOpt2=*(float *)&REGISTER(nOpt2);
		break;
	case 0x03://������-�Ĵ������Ѱַ
		fOpt2=*(float *)&MEMORY(REGISTER(nOpt2));
		break;
	case 0x04://���������Ѱַ-������
		fOpt1=*(float *)&MEMORY(nOpt1);
		break;
	case 0x05://���������Ѱַ-���������Ѱַ
		fOpt1=*(float *)&MEMORY(nOpt1);
		fOpt2=*(float *)&MEMORY(nOpt2);
		break;
	case 0x06://���������Ѱַ-�Ĵ���
		fOpt1=*(float *)&MEMORY(nOpt1);
		fOpt2=*(float *)&REGISTER(nOpt2);
		break;
	case 0x07://���������Ѱַ-�Ĵ������Ѱַ
		fOpt1=*(float *)&MEMORY(nOpt1);
		fOpt2=*(float *)&MEMORY(REGISTER(nOpt2));
		break;
	case 0x08://�Ĵ���-������
		fOpt1=*(float *)&REGISTER(nOpt1);
		break;
	case 0x09://�Ĵ���-���������Ѱַ
		fOpt1=*(float *)&REGISTER(nOpt1);
		fOpt2=*(float *)&MEMORY(nOpt2);
		break;
	case 0x0A://�Ĵ���-�Ĵ���
		fOpt1=*(float *)&REGISTER(nOpt1);
		fOpt2=*(float *)REGISTER(nOpt2);
		break;
	case 0x0B://�Ĵ���-�Ĵ������Ѱַ
		fOpt1=*(float *)&REGISTER(nOpt1);
		fOpt2=*(float *)&MEMORY(REGISTER(nOpt2));
		break;
	case 0x0C://�Ĵ������Ѱַ-������
		fOpt1=*(float *)&MEMORY(REGISTER(nOpt1));
		fOpt2=*(float *)&(nOpt2);
		break;
	case 0x0D://�Ĵ������Ѱַ-���������Ѱַ
		fOpt1=*(float *)&MEMORY(REGISTER(nOpt1));
		fOpt2=*(float *)&MEMORY(nOpt2);
		break;
	case 0x0E://�Ĵ������Ѱַ-�Ĵ���
		fOpt1=*(float *)&MEMORY(REGISTER(nOpt1));
		fOpt2=*(float *)&REGISTER(nOpt2);
		break;
	case 0x0F://�Ĵ������Ѱַ-�Ĵ������Ѱַ
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
	case 0x00://������-������
		break;
	case 0x01://������-���������Ѱַ
		break;
	case 0x02://������-�Ĵ���
		break;
	case 0x03://������-�Ĵ������Ѱַ
		break;
	case 0x04://���������Ѱַ-������
		*(float *)&MEMORY(nOpt1)=fResult;
		break;
	case 0x05://���������Ѱַ-���������Ѱַ
		*(float *)&MEMORY(nOpt1)=fResult;
		break;
	case 0x06://���������Ѱַ-�Ĵ���
		*(float *)&MEMORY(nOpt1)=fResult;
		break;
	case 0x07://���������Ѱַ-�Ĵ������Ѱַ
		*(float *)&MEMORY(nOpt1)=fResult;
		break;
	case 0x08://�Ĵ���-������
		REGISTER(nOpt1)=*(int *)&fResult;
		break;
	case 0x09://�Ĵ���-���������Ѱַ
		REGISTER(nOpt1)=*(int *)&fResult;
		break;
	case 0x0A://�Ĵ���-�Ĵ���
		REGISTER(nOpt1)=*(int *)&fResult;
		break;
	case 0x0B://�Ĵ���-�Ĵ������Ѱַ
		REGISTER(nOpt1)=*(int *)&fResult;
		break;
	case 0x0C://�Ĵ������Ѱַ-������
		*(float *)&MEMORY(REGISTER(nOpt1))=fResult;
		break;
	case 0x0D://�Ĵ������Ѱַ-���������Ѱַ
		*(float *)&MEMORY(REGISTER(nOpt1))=fResult;
		break;
	case 0x0E://�Ĵ������Ѱַ-�Ĵ���
		*(float *)&MEMORY(REGISTER(nOpt1))=fResult;
		break;
	case 0x0F://�Ĵ������Ѱַ-�Ĵ������Ѱַ
		*(float *)&MEMORY(REGISTER(nOpt1))=fResult;
		break;
	}
}

//=========================================================
//������CmdEXIT
//˵����
//���룺
//�����
//���أ�
//���ڣ�2013-11-9
//��ע��
//=========================================================
void UnLikeVM::CmdEXIT()
{
	RP=m_nExeCodeLength;
}

//=========================================================
//������Start
//˵������������¿�ʼ����
//���룺
//�����
//���أ�
//���ڣ�2013-11-15
//��ע��
//=========================================================
void UnLikeVM::Start()
{
	Reset();
	Run();
}

//=========================================================
//������Run
//˵�������������
//���룺
//�����
//���أ�
//���ڣ�2013-11-8
//��ע��
//=========================================================
void UnLikeVM::Run()
{
	while(GetRP()<GetExeCodeLength())
		RunACommand();
}

//=========================================================
//������Stop
//˵�����������ͣ
//���룺
//�����
//���أ�
//���ڣ�2013-11-8
//��ע��
//=========================================================
void UnLikeVM::Stop()
{
}

//=========================================================
//������GetRP
//˵������ȡ��ǰ����ָ��λ��
//���룺
//�����
//���أ�
//���ڣ�2013-11-9
//��ע��
//=========================================================
int UnLikeVM::GetRP()
{
	return RP;
}

//=========================================================
//������GetExeCodeLength
//˵������ȡִ�д��볤��
//���룺
//�����
//���أ�
//���ڣ�2013-11-9
//��ע��
//=========================================================
int UnLikeVM::GetExeCodeLength()
{
	return m_nExeCodeLength;
}

