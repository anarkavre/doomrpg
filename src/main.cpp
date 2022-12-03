#include "CDoomRPG.h"

int main(int argc, char *argv[])
{
	try
	{
		CDoomRPG doomrpg(argc, argv);
		doomrpg.Run();
	}
	catch (const std::exception &e)
	{
		SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_ERROR, "Error", e.what(), nullptr);
	}

	return 0;
}