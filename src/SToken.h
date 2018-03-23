//=========================================================
//SToken
//��־��
//
//��д������
//
//
//���ڣ�2013-8-14
//=========================================================
#ifndef _S_TOKEN_
#define _S_TOKEN_

#include "SString.h"

//�����ַ�����
#define						CH_ENDLINE			10
#define						CH_ENDTEXT			0
#define						CH_TAB				9
#define						CH_SPACE			32
#define						CH_STRINGMARK		'\"'

class SToken
{
	//Data
private:
	char *m_pText;//�ı�
	int m_nCur;//��ǰλ��ָ��
	int m_nTokenType;//��־����
	int m_nLineCount;
	int m_nTextLength;
	bool m_bNegative;//����
	SString m_strToken;
public:
	enum{TYPE_NONE,TYPE_LABEL,TYPE_SYMBOL,TYPE_NUMBER,TYPE_FLOAT,TYPE_STRING,TYPE_ENDLINE,TYPE_ENDTEXT,TYPE_ERROR};
	//Function
public:
	//=========================================================
	//������SToken
	//˵�������캯��
	//���룺
	//�����
	//���أ�
	//���ڣ�2013-8-14
	//��ע��
	//=========================================================
	SToken()
	{
		m_pText=NULL;
		m_nTokenType=TYPE_NONE;
		m_nCur=0;
		m_nTextLength=0;
	}

	//=========================================================
	//������~SToken
	//˵������������
	//���룺
	//�����
	//���أ�
	//���ڣ�2013-8-14
	//��ע��
	//=========================================================
	~SToken()
	{
		if(m_pText)
			delete []m_pText;
	}
	//=========================================================
	//������Token
	//˵�������ı���ǰλ�÷�����һ����־
	//���룺
	//�����
	//���أ�
	//���ڣ�2013-8-14
	//��ע��
	//=========================================================
	SString &Token()
	{
		char *pch;
		char ReadChar;
		SString str;

		if(m_nCur>m_nTextLength)
		{
			m_strToken="";
			m_nTokenType=TYPE_ENDTEXT;
			return m_strToken;
		}

		SkipSpace();//�����հ��ַ�

		str="";

		pch=&m_pText[m_nCur];

		ReadChar=*pch;

		if(isdigit(ReadChar))//����
		{
			do
			{
				str+=ReadChar;
				ReadChar=*(++pch);
				m_nCur++;
			}while(isdigit(ReadChar));

			if(isalpha(ReadChar))//���ֺ������ĸ
			{
				m_nTokenType=TYPE_ERROR;
				m_strToken="";
				return m_strToken;
			}

			if(ReadChar=='.')//���ֺ����С����
			{
				do
				{
					str+=ReadChar;
					ReadChar=*(++pch);
					m_nCur++;
				}while(isdigit(ReadChar));

				m_nTokenType=TYPE_FLOAT;
				m_strToken=str;

				return m_strToken;
			}

			m_nTokenType=TYPE_NUMBER;
			m_strToken=str;
			return m_strToken;
		}
		else if(m_bNegative && ReadChar=='-')
		{
			ReadChar=*(++pch);
			m_nCur++;

			if(!isdigit(ReadChar))
			{
				m_nTokenType=TYPE_ERROR;
				m_strToken="";
				return m_strToken;
			}

			m_strToken="-";

			while(isdigit(ReadChar))
			{
				m_strToken+=ReadChar;
				ReadChar=*(++pch);
				m_nCur++;
			}

			if(ReadChar=='.')
			{
				m_nTokenType=TYPE_FLOAT;

				m_strToken+='.';
				ReadChar=*(++pch);
				m_nCur++;

				if(!isdigit(ReadChar))
				{
					m_strToken="";
					m_nTokenType=TYPE_ERROR;
					return m_strToken;
				}

				while(isdigit(ReadChar))
				{
					m_strToken+=ReadChar;
					ReadChar=*(++pch);
					m_nCur++;
				}
			}
			else
			{
				m_nTokenType=TYPE_NUMBER;
			}

			if(isalpha(ReadChar))
			{
				m_nTokenType=TYPE_ERROR;
				m_strToken="";
				return m_strToken;
			}

			return m_strToken;
		}
		else if(isalpha(ReadChar))//��ĸ
		{
			do
			{
				str+=ReadChar;
				ReadChar=*(++pch);
				m_nCur++;
			}while(isalpha(ReadChar) || isdigit(ReadChar) || ReadChar=='_');
			m_nTokenType=TYPE_LABEL;
			m_strToken=str;
			return m_strToken;
		}
		else if(ReadChar==CH_STRINGMARK)//˫����
		{
			ReadChar=*(++pch);
			do
			{
				str+=ReadChar;
				ReadChar=*(++pch);
				m_nCur++;
				if(ReadChar==CH_ENDLINE || ReadChar==CH_ENDTEXT || ReadChar==13)//�Ƿ��ַ����ַ�
				{
					m_nTokenType=TYPE_ERROR;
					m_strToken="";
					return m_strToken;
				}
			}while(ReadChar!=CH_STRINGMARK);
			m_nTokenType=TYPE_STRING;
			m_nCur+=2;
			m_strToken=str;
			return m_strToken;
		}
		else if(ispunct(ReadChar))
		{
			m_nCur++;
			m_strToken="";
			m_strToken+=ReadChar;
			m_nTokenType=TYPE_SYMBOL;
			return m_strToken;
		}
		else if(ReadChar==CH_ENDLINE)//��ĩ
		{
			m_nLineCount++;
			m_nCur++;
			m_nTokenType=TYPE_ENDLINE;
			m_strToken="";
			return m_strToken;
		}
		else if(ReadChar==13)//��ĩ
		{
			pch++;
			m_nCur++;
			m_strToken="";
			if(*pch!=CH_ENDLINE)
			{
				m_nTokenType=TYPE_ERROR;
				return m_strToken;
			}
			m_nCur++;
			m_nTokenType=TYPE_ENDLINE;
			m_nLineCount++;
			return m_strToken;
		}
		else if(ReadChar==CH_ENDTEXT)
		{
			m_nCur++;
			m_nTokenType=TYPE_ENDTEXT;
			m_strToken="";
			return m_strToken;
		}
		else
		{
			m_nTokenType=TYPE_ERROR;
			m_strToken="";
			return m_strToken;
		}
	}
	//=========================================================
	//������GetCurrentLineCount
	//˵������ǰָ����������
	//���룺
	//�����
	//���أ�
	//���ڣ�2013-8-15
	//��ע��
	//=========================================================
	int GetCurrentLineCount()
	{
		return m_nLineCount;
	}
	//=========================================================
	//������SetCur
	//˵��������ָ�룬��������ĵ�ǰ����
	//���룺nPos:λ��
	//�����
	//���أ�
	//���ڣ�2013-8-15
	//��ע��
	//=========================================================
	void SetCur(int nPos)
	{
		m_nCur=nPos;
	}
	//=========================================================
	//������GetCur
	//˵������ȡָ�룬��������ĵ�ǰ����
	//���룺
	//�����
	//���أ�
	//���ڣ�2013-8-15
	//��ע��
	//=========================================================
	long GetCur()
	{
		return m_nCur;
	}

	//=========================================================
	//������SetNegative
	//˵���������Ƿ񽫸�����Ϊһ����λ
	//���룺
	//�����
	//���أ�
	//���ڣ�2013-8-15
	//��ע��
	//=========================================================
	void SetNegative(bool bNegative)
	{
		m_bNegative=bNegative;
	}

	//=========================================================
	//������GoBack
	//˵����ָ�����
	//���룺n�����˵ĳ���
	//�����
	//���أ�
	//���ڣ�2013-8-15
	//��ע��
	//=========================================================
	void GoBack(int n)
	{
		char *pch;
		int i;

		pch=&m_pText[m_nCur];

		if(m_nCur==0) return;
		for(i=0;i<n;i++)
		{
			pch--;
			if(*pch=='\n')
				m_nLineCount--;
		}

		m_nCur-=n;
	}
	//=========================================================
	//������GoBack
	//˵����ָ�����һ����־
	//���룺
	//�����
	//���أ�
	//���ڣ�2013-8-15
	//��ע��
	//=========================================================
	void GoBack()
	{
		if(m_nTokenType==TYPE_STRING)
			GoBack(m_strToken.GetLength()+2);
		else if(m_nTokenType==TYPE_NUMBER)
			GoBack(m_strToken.GetLength());
		else if(m_nTokenType==TYPE_SYMBOL)
			GoBack(1);
		else if(m_nTokenType==TYPE_FLOAT)
			GoBack(m_strToken.GetLength());
		else if(m_nTokenType==TYPE_LABEL)
			if(m_strToken.GetLength())
				GoBack(m_strToken.GetLength());
			else
				GoBack(1);
		else if(m_nTokenType==TYPE_ENDLINE || m_nTokenType==TYPE_ENDTEXT)
			GoBack(1);
		m_strToken="";
	}

	//=========================================================
	//������GetTokenType
	//˵������ȡ��־����
	//���룺
	//�����
	//���أ�
	//���ڣ�2013-8-14
	//��ע��
	//=========================================================
	int GetTokenType()
	{
		return m_nTokenType;
	}

	//=========================================================
	//������GetToken
	//˵������ȡ��־�ַ���
	//���룺
	//�����
	//���أ�
	//���ڣ�2013-8-14
	//��ע��
	//=========================================================
	SString &GetToken()
	{
		return m_strToken;
	}
	//=========================================================
	//������SkipToLineEnd
	//˵����������ĩ
	//���룺
	//�����
	//���أ�
	//���ڣ�2013-8-14
	//��ע��
	//=========================================================
	void SkipToLineEnd()
	{
		char *pch;

		pch=&m_pText[m_nCur];

		while(*pch!=CH_ENDLINE && *pch!=CH_ENDTEXT)
		{
			pch++;
			m_nCur++;
		}

		if(*pch==CH_ENDLINE)
			m_nTokenType=TYPE_ENDLINE;
		else
			m_nTokenType=TYPE_ENDTEXT;
	}
	//=========================================================
	//������SetText
	//˵���������ı�
	//���룺
	//�����
	//���أ�
	//���ڣ�2013-8-14
	//��ע��
	//=========================================================
	void SetText(const char *str)
	{
		if(m_pText)
			delete []m_pText;
		m_nTextLength=strlen(str);
		m_pText=new char[m_nTextLength+1];
		strcpy(m_pText,str);
		m_nCur=0;//��ʼ��λ��ָ��
		m_nLineCount=1;
	}
	//=========================================================
	//������SetTokenType
	//˵��������Token������
	//���룺
	//�����
	//���أ�
	//���ڣ�2013-8-14
	//��ע��
	//=========================================================
	void SetTokenType(int nType)
	{
		m_nTokenType=nType;
	}
	//=========================================================
	//������Clear
	//˵�������
	//���룺
	//�����
	//���أ�
	//���ڣ�2015-2-22
	//��ע��
	//=========================================================
	void Clear()
	{
		if(m_pText)
			delete []m_pText;

		m_nTextLength=0;
		m_nTokenType=TYPE_NONE;
		m_pText=NULL;
	}
private:
	//=========================================================
	//������SkipSpace
	//˵���������հ��ַ�
	//���룺
	//�����
	//���أ�
	//���ڣ�2013-8-14
	//��ע��
	//=========================================================
	void SkipSpace()
	{
		char *pch;

		pch=&m_pText[m_nCur];

		while(*pch==CH_SPACE || *pch==CH_TAB)
		{
			pch++;
			m_nCur++;
		}
	}
};

#endif
