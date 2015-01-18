#ifndef _PUSOYDOS_COMBO_H_
#define _PUSOYDOS_COMBO_H_

#include <vector>
#include <stdexcept>

// game
#include "Card.h"

using namespace game;

namespace pusoydos {

// TODO: maybe derive from std::vector to simplify interface
// and eliminate need for ComboListT
class Combo
{
  public:
    // TODO: rename to CardListT
    typedef std::vector<CardPtr> ComboListT;
    typedef enum {
        kSingle        = 0,
        kPair          = 1,
        kThreeKind     = 2,
        kStraight      = 3,
        kFlush         = 4,
        kFullHouse     = 5,
        kFourKind      = 6,
        kStraightFlush = 7,
        NUMTYPES       = 8,
        kUndef         = 9
    } ComboT;

  public:
    Combo(const ComboT type = kUndef);
    Combo(const std::string name, const ComboT type = kUndef);

    Combo(const Combo& other);
    Combo& operator=(const Combo& other);

    void setType(const ComboT type);
    ComboT getType(void) const;

    bool addCard(CardPtr card);
    const CardPtr& getCard(const uint16_t index) const;

    void setCardCombo(const ComboListT& cardCombo);
    const ComboListT& getCardCombo(void) const;

    uint16_t getSize(void) const;

    const CardPtr& highCard(void) const;
    const CardPtr& lowCard(void) const;

    bool hasCard(const uint16_t number) const;
    bool hasCard(const uint16_t number, const char suit) const;
    bool hasCard(const char face) const;
    bool hasCard(const char face, const char suit) const;

    uint16_t hasNumOfCard(const uint16_t number) const;
    uint16_t hasNumOfCard(const uint16_t number, const char suit) const;
    uint16_t hasNumOfCard(const char face) const;
    uint16_t hasNumOfCard(const char face, const char suit) const;

    void setOwner(const std::string name);
    const std::string& getOwner(void) const;

    std::ostream& printToStream(std::ostream& os);

    void resetCards(void);
    void resetAll(void);

    void sort(void);

    bool operator<(const Combo &other) const;

    // sorts and returns high card
    static uint16_t findHighValCard(ComboListT& combo);
    static uint16_t findHighCountCard(const ComboListT& combo, const uint16_t threshold);

    static CardPtr findHighCardInAceLowStraight(const ComboListT &combo);

    static uint16_t getNumCardsInCombo(ComboT type);

    static std::string getComboTypeString(ComboT type);

    static const uint16_t sMaxComboSize;

    class CompareCards : public std::binary_function<CardPtr&, CardPtr&, bool>
    {
      public:
        SuitList _suitList;

        CompareCards(const SuitList& suitList) : _suitList(suitList) { }

        bool operator()(const CardPtr &a, const CardPtr &b)
        {
            if (a.get() == NULL || b.get() == NULL) {
                throw std::runtime_error("invalid operands for card comparison");
            }
            return (a->getValue() < b->getValue() ||
                        (a->getValue() == b->getValue() &&
                         _suitList[a->getSuit()] < _suitList[b->getSuit()]));
        }
    };

  private:
    ComboListT   _cardCombo;
    std::string  _name;
    ComboT       _type;

    bool _validateFiveCardCombo(void);
    CardPtr _emptyCard;
};

} /* namespace pusoydos */

#endif
