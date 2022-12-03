#ifndef __IUPDATEHANDLER_H__
#define __IUPDATEHANDLER_H__

class IUpdateHandler
{
public:
	virtual bool Update(float elapsedTime) = 0;
};

#endif