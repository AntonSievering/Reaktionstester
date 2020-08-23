#define OLC_PGE_APPLICATION
#include "Engine.h"

int main()
{
	Engine engine;
	if (engine.Construct(1920, 1080, 1, 1, true, false))
		engine.Start();

	return 0;
}