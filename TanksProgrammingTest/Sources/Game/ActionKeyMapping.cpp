#include "ActionKeyMapping.h"

ActionKeyMapping::ActionKeyMapping(const std::shared_ptr<const InputAction> InAction, const SDL_Scancode InKey) : Action(InAction), Key(InKey)
{
}
