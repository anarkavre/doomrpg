#ifndef __IEVENTHANDLER_H__
#define __IEVENTHANDLER_H__

#include "SDL.h"

class IEventHandler
{
public:
	virtual bool ProcessEvent(const SDL_Event &event) = 0;
};

#endif