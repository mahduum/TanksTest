#pragma once
#include <Engine.h>
#include <memory>

class InputAction;

struct ActionKeyMapping
{
	ActionKeyMapping(const std::shared_ptr<const InputAction> InAction = nullptr, const SDL_Scancode InKey = SDL_SCANCODE_UNKNOWN);

	//keys are mapped to action name

	//what pointer, how is it made?
	const std::shared_ptr<const InputAction> Action;
	SDL_Scancode Key;
};

