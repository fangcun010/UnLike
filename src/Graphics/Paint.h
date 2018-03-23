#ifndef _PAINT_H_
#define _PAINT_H_

struct Pen
{
	int nStyle;
	int nWidth;
    int Color;
    int x,y;
};

class Paint
{
	Pen m_Pen;
};

#endif
