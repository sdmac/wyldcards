#ifndef _PUSOYDOS_GAMESTATE_H_
#define _PUSOYDOS_GAMESTATE_H_

#include "Combo.h"

namespace pusoydos {

class GameState
{
  public:
    GameState(void);

    void reset(void);

    Combo    combo;
    uint16_t leadPlayer;
    bool     firstCombo;
};

} /* namespace pusoydos */

#endif
