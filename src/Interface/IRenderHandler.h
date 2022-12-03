#ifndef __IRENDERHANDLER_H__
#define __IRENDERHANDLER_H__

class IRenderHandler
{
public:
	virtual bool Render(float alpha) = 0;
};

#endif