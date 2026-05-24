#include "MouseCommand.h"

#include <SDL3/SDL.h>

void ge::MouseCommand::Execute(float deltaTime)
{
	float x{ 0.f };
	float y{ 0.f };

	SDL_GetMouseState(&x, &y);
	OnClick(x, y, deltaTime);
}
