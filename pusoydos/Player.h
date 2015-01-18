#ifndef _PUSOYDOS_PLAYER_H_
#define _PUSOYDOS_PLAYER_H_

// game
#include "Card.h"
#include "Hand.h"

// pusoydos
#include "Combo.h"
#include "GameState.h"

using namespace game;

namespace pusoydos {

class Player
{
  public:
    Player(void);
    Player(const std::string name);

    virtual ~Player(void);

    void setName(const std::string name);
    const std::string& getName(void) const;

    void dealCard(CardPtr card);

    bool hasCard(const uint16_t value, const char suit);
    bool hasCard(const char face, const char suit);

    virtual const Combo& playLeadCombo(const GameState* state) = 0;
    virtual bool playFollowCombo(const GameState* state, Combo& combo) = 0;

    uint16_t cardsLeft(void) const;

    void reset(void);

    void printHand(std::ostream& os);

  protected:
    std::string  _name;
    Hand<Combo::CompareCards> _hand;
    Combo _combo;

};

class CpuPlayer : public Player
{
  public:
    CpuPlayer(void);
    CpuPlayer(const std::string name);

    ~CpuPlayer(void);

    virtual const Combo& playLeadCombo(const GameState* state);
    virtual bool playFollowCombo(const GameState* state, Combo& combo);


  private:
    // [card value, card count]
    typedef std::map<uint16_t, std::vector<uint16_t> > CardCountMap;
    CardCountMap _cardCounts;

    typedef enum {
        kSingle = 0, // valSets[0] = singles
        kPair   = 1, // valSets[1] = pairs
        kThree  = 2, // valSets[2] = three of kind
        kFour   = 3  // valSets[3] = four of kind
    } CountT;

    // valSets[0] = [(val0) (val1) ... ]
    std::vector<std::vector<uint16_t> > _valSets;

    // [(startVal0, endVal0) (startVal1, endval1) ... ]
    std::vector<std::pair<uint16_t, uint16_t> > _straights;

    void _updateCardCounts(void);
    void _updateStraights(void);

    void _findCombo(const Combo& curCombo, bool leader);

    bool _tryFourOfKind(const Combo& curCombo, bool leader);
    bool _tryFullHouse(const Combo& curCombo, bool leader);
    bool _tryStraight(const Combo& curCombo, bool leader);
    bool _tryThreeOfKind(const Combo& curCombo, bool leader);
    bool _tryPair(const Combo& curCombo, bool leader);
    bool _trySingle(const Combo& curCombo, bool leader);

};

class HumanPlayer : public Player
{
  public:
    HumanPlayer(void);
    HumanPlayer(const std::string name);

    ~HumanPlayer(void);

    virtual const Combo& playLeadCombo(const GameState* state);
    virtual bool playFollowCombo(const GameState* state, Combo& combo);

    void printComboTypes(std::ostream& os) const;
};

} /* namespace pusoydos */

#endif
