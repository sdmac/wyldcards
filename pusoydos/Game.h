#ifndef _PUSOYDOS_GAME_H_
#define _PUSOYDOS_GAME_H_

#include <vector>

// game
#include "Deck.h"

// pusoydos
#include "Combo.h"
#include "Player.h"
#include "GameState.h"

using namespace game;

namespace pusoydos {

// TODO: maybe abstract Game into base class (playHand, playGame, deal, etc.)
// should work well for games with well-defined "hands""
class Game
{
    typedef std::map<std::string, uint16_t> ScoreMapT;

  public:
    Game(const uint16_t screenHeight = sDefaultScreenHeight);
    Game(const uint16_t numPlayers, const uint16_t screenHeight = sDefaultScreenHeight);

    ~Game(void);

    void deal(void);

    bool playRound(std::ostream& os);
    void playSet(std::ostream& os);
    void playGame(std::ostream& os);

    Player * determineWinner(const uint16_t maxScore);
    bool maxScoreReached(const uint16_t maxScore);

    void printScoreTable(std::ostream& os, ScoreMapT& scores);

    // suit order/rank
    typedef enum {
        kUndef    = 0,
        kClubs    = 1,
        kSpades   = 2,
        kHearts   = 3,
        kDiamonds = 4
    } Suits;

    static const uint16_t sDefaultNumPlayers;
    static const uint16_t sDefaultScreenHeight;

    // TODO: make private
    void findStartingCard(void);
    void scoreGame(const std::string& name, const Combo& finalCombo);

  private:
    Deck      _deck;

    ScoreMapT   _scores;

    // state
    Combo _currentCombo;
    GameState *_gameState;

    uint16_t  _numPlayers;
    uint16_t  _humanPlayer;
    std::vector<Player *> _players;
    uint16_t  _screenHeight;
    uint16_t  _numSets;

    void _setupSuits(void);
    void _setupPlayers(void);

    void _createDeck(void);

    void _promptForEnter(void);

};

} /* namespace pusoydos */

#endif
