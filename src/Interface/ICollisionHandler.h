#ifndef __ICOLLISIONHANDLER_H__
#define __ICOLLISIONHANDLER_H__

class CObject;

class ICollisionHandler
{
public:
	virtual void HandleCollision(CObject *object) = 0;
};

#endif