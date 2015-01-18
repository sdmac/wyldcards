#include "GameState.h"

namespace pusoydos {

GameState::GameState(void)
    : leadPlayer(0),
      firstCombo(false)
{
}

void
GameState::reset(void)
{
    combo.resetAll();
    leadPlayer = 0;
    firstCombo = false;
}

} /* namespace pusoydos */
