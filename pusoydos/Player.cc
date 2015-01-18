#include <algorithm>
#include <stdexcept>
#include <iostream>

#include "Player.h"

using namespace game;

namespace pusoydos {

Player::Player(void)
    : _name(""), _hand(), _combo()
{
}

Player::Player(const std::string name)
    : _name(name), _hand(name), _combo(name)
{
}

Player::~Player(void)
{
}

void
Player::setName(const std::string name)
{
    _name = name;
}

const std::string&
Player::getName(void) const
{
    return _name;
}

void
Player::dealCard(CardPtr card)
{
    _hand.addCard(card);
}

bool
Player::hasCard(const uint16_t value, const char suit)
{
    uint16_t index;
    _hand.findCard(value, suit, index);
    return (index != _hand.getHandSize());
}

bool
Player::hasCard(const char face, const char suit)
{
    uint16_t index;
    _hand.findCard(face, suit, index);
    return (index != _hand.getHandSize());
}

uint16_t
Player::cardsLeft(void) const
{
    return _hand.getHandSize();
}

void
Player::reset(void)
{
    _hand.reset();
    _combo.resetCards();
}

void
Player::printHand(std::ostream& os)
{
    os << _name << ": ";
    _hand.sort();
    _hand.printToStream(os);
}


/****************************************************
 ******************* CpuPlayer **********************
 ****************************************************/

CpuPlayer::CpuPlayer(void)
    : Player()
{
}

CpuPlayer::CpuPlayer(const std::string name)
    : Player(name)
{
}

CpuPlayer::~CpuPlayer(void)
{
}

const Combo&
CpuPlayer::playLeadCombo(const GameState* state)
{
    if (_hand.getHandSize() == 0) {
        throw std::runtime_error("player has no more cards");
    }
    _combo.resetCards();
    uint16_t index = 0;

    if (state->firstCombo) {
        // must lead with 3c
        _hand.findCard((uint16_t)3, Card::Clubs, index);
        if (index != _hand.getHandSize()) {
            _combo.setType(Combo::kSingle);
            _combo.addCard(_hand.playCard(index));
        }
        else {
            throw std::runtime_error("player does not have 3 of clubs");
        }
    }
    else {
        // find best combo to lead with
        _findCombo(state->combo, true);
    }
    return _combo;
}

void
CpuPlayer::_findCombo(const Combo& curCombo, bool leader)
{
    _updateCardCounts();
    _updateStraights();
    // ordered by descending combo rank
    if (_tryFourOfKind(curCombo, leader)) return;
    if (_tryFullHouse(curCombo, leader)) return;
    if (_tryStraight(curCombo, leader)) return;
    if (_tryThreeOfKind(curCombo, leader)) return;
    if (_tryPair(curCombo, leader)) return;
    if (_trySingle(curCombo, leader) == false) {
        throw std::runtime_error("player has no more cards");
    }
}

void
CpuPlayer::_updateCardCounts(void)
{
    _cardCounts.clear();
    for (uint16_t idx = 0; idx < _hand.getHandSize(); ++idx) {
        const CardPtr& card = _hand.checkCard(idx);
        _cardCounts[card->getValue()].push_back(idx);
    }

    _valSets.clear();
    _valSets.resize(4);
    for (CardCountMap::iterator it = _cardCounts.begin();
         it != _cardCounts.end(); ++it) {
        // index into array (map value) is card count (index vector size) minus 1
        // vector element is card value
        _valSets[it->second.size()-1].push_back(it->first);
    }
/*
for (uint16_t i = 0; i < _valSets.size(); ++i) {
 for (uint16_t j = 0; j < _valSets[i].size(); ++j) {
  std::cerr << "valSets[" << i << "][" << j << "] = " << _valSets[i][j] << std::endl;
 }
}
*/
}

void
CpuPlayer::_updateStraights(void)
{
    _hand.sort();
    _straights.clear();
    uint16_t idx = 0;
    uint16_t straightStart = _hand.checkCard(idx++)->getValue();
    uint16_t straightCur = straightStart;
    for (; idx < _hand.getHandSize(); ++idx) {
        const CardPtr& card = _hand.checkCard(idx);
        uint16_t value = card->getValue();
        // check for continuous values (ascending order)
        if (value == (straightCur+1)) {
            straightCur++;
        }
        else if (value != straightCur) {
            straightStart = value;
        }
        if ((straightCur - straightStart + 1) == 5) {
            // store start and end values
            std::pair<uint16_t, uint16_t> interval(straightStart, straightCur);
            _straights.push_back(interval);
            // slide interval in case other straights contain the last 4 cards
            straightStart++;
        }
    }
/*
for (uint16_t i = 0; i < _straights.size(); ++i) {
 std::cerr << "straights[" << i << "]=(" << _straights[i].first << ", " << _straights[i].second << ")\n";
}
*/
}

bool
CpuPlayer::_tryStraight(const Combo& curCombo, bool leader)
{
    uint16_t index = 0;
    uint16_t straightIdx = 0;
    std::vector<uint16_t> indices;
    if (_straights.size() > 0) {
        _combo.setType(Combo::kStraight);
        do {
            _combo.resetCards();
            indices.clear();
            for (uint16_t val = _straights[straightIdx].first;
                 val < (_straights[straightIdx].first+5); ++val) {
                // TODO: use lowest suit of card if more than one
                if ((index = _hand.findCardIndexByValue(val)) == _hand.getHandSize()) {
                    throw std::runtime_error("expected to find a straight but failed");
                }
                _combo.addCard(_hand.checkCard(index));
                indices.push_back(index);
            }
        }
        while (!leader && (_combo < curCombo) && (++straightIdx < _straights.size()));

        if (straightIdx < _straights.size()) {
            _hand.playCards(indices);
            return true;
        }
    }
    return false;
}

bool
CpuPlayer::_tryFourOfKind(const Combo& curCombo, bool leader)
{
    uint16_t index = 0;
    uint16_t fourIdx = 0;
    std::vector<uint16_t> indices;
    // check for four-of-a-kind
    if (!_valSets[kFour].empty()) {
        _combo.setType(Combo::kFourKind);
        // sort pairs in ascending order by value
        std::sort(_valSets[kFour].begin(), _valSets[kFour].end());
        do {
            _combo.resetCards();
            indices.clear();
            uint16_t fourValue = _valSets[kFour][fourIdx];
            for (uint16_t n = 0; n < 4; ++n) {
                if ((index = _cardCounts[fourValue][n]) >= _hand.getHandSize()) {
                    throw std::runtime_error("expected to find four of a kind but failed");
                }
                _combo.addCard(_hand.checkCard(index));
                indices.push_back(index);
            }
        }
        while (!leader && (_combo < curCombo) && (++fourIdx < _valSets[kFour].size()));

        if (fourIdx < _valSets[kFour].size()) {
            // combo already created, just play out (remove) cards from hand
            _hand.playCards(indices);
            // use lowest single  // TODO: make sure non-pair card
            _hand.getLowestCard(index);
            _combo.addCard(_hand.playCard(index));
            return true;
        }
    }
    return false;
}

bool
CpuPlayer::_tryFullHouse(const Combo& curCombo, bool leader)
{
    uint16_t index = 0;
    uint16_t threeIdx = 0;
    std::vector<uint16_t> indices;
    // check for three of a kind
    if (!_valSets[kThree].empty() &&
        !_valSets[kPair].empty()) {
        _combo.setType(Combo::kFullHouse);
        // sort three of kinds in ascending order by value
        std::sort(_valSets[kThree].begin(), _valSets[kThree].end());
        do {
            _combo.resetCards();
            indices.clear();
            uint16_t threeValue = _valSets[kThree][threeIdx];
            for (uint16_t n = 0; n < 3; ++n) {
                if ((index = _cardCounts[threeValue][n]) >= _hand.getHandSize()) {
                    throw std::runtime_error("expected to find three of a kind but failed");
                }
                _combo.addCard(_hand.checkCard(index));
                indices.push_back(index);
            }
        }
        while (!leader && (_combo < curCombo) && (++threeIdx < _valSets[kThree].size()));
        // warning: assumes operator< only compares triple (and not whole hand)

        if (threeIdx < _valSets[kThree].size()) {
            // combo already has three-of-kind, just play out (remove) cards from hand
            _hand.playCards(indices);

            // re-calculate indices and sort pairs in ascending order
            _updateCardCounts();
            std::sort(_valSets[kPair].begin(), _valSets[kPair].end());
            // use lowest pair
            uint16_t pairIdx = 0;
            for (uint16_t n = 0; n < 2; ++n) {
                if ((index = _hand.findCardIndexByValue(_valSets[kPair][pairIdx])) == _hand.getHandSize()) {
                    throw std::runtime_error("expected to find pair but failed");
                }
                _combo.addCard(_hand.playCard(index));
            }
            return true;
        }
    }
    return false;
}

bool
CpuPlayer::_tryThreeOfKind(const Combo& curCombo, bool leader)
{
    uint16_t index = 0;
    uint16_t threeIdx = 0;
    std::vector<uint16_t> indices;
    // check for three of kinds
    if (!_valSets[kThree].empty()) {
        _combo.setType(Combo::kThreeKind);
        // sort pairs in ascending order by value
        std::sort(_valSets[kThree].begin(), _valSets[kThree].end());
        do {
            _combo.resetCards();
            indices.clear();
            uint16_t threeValue = _valSets[kThree][threeIdx];
            for (uint16_t n = 0; n < 3; ++n) {
                if ((index = _cardCounts[threeValue][n]) >= _hand.getHandSize()) {
                    throw std::runtime_error("expected to find three of a kind but failed");
                }
                _combo.addCard(_hand.checkCard(index));
                indices.push_back(index);
            }
        }
        while (!leader && (_combo < curCombo) && (++threeIdx < _valSets[kThree].size()));

        if (threeIdx < _valSets[kThree].size()) {
            // combo already created, just play out (remove) cards from hand
            _hand.playCards(indices);
            return true;
        }
    }
    return false;
}

bool
CpuPlayer::_tryPair(const Combo& curCombo, bool leader)
{
    uint16_t index = 0;
    uint16_t pairIdx = 0;
    std::vector<uint16_t> indices;
    // check for pairs
    if (!_valSets[kPair].empty()) {
        _combo.setType(Combo::kPair);
        // sort pairs in ascending order by value
        std::sort(_valSets[kPair].begin(), _valSets[kPair].end());
        do {
            _combo.resetCards();
            indices.clear();
            uint16_t pairValue = _valSets[kPair][pairIdx];
            for (uint16_t n = 0; n < 2; ++n) {
                if ((index = _cardCounts[pairValue][n]) >= _hand.getHandSize()) {
                    throw std::runtime_error("expected to find pair but failed");
                }
                _combo.addCard(_hand.checkCard(index));
                indices.push_back(index);
            }
        }
        while (!leader && (_combo < curCombo) && (++pairIdx < _valSets[kPair].size()));

        if (pairIdx < _valSets[kPair].size()) {
            // combo already created, just play out (remove) cards from hand
            _hand.playCards(indices);
            return true;
        }
    }
    return false;
}

bool
CpuPlayer::_trySingle(const Combo& curCombo, bool leader)
{
    uint16_t index = 0;
    uint16_t singleIdx = 0;
    if (!_valSets[kSingle].empty()) {
        _combo.setType(Combo::kSingle);
        // use lowest possible card
        std::sort(_valSets[kSingle].begin(), _valSets[kSingle].end());
        do {
            _combo.resetCards();
            if ((index = _hand.findCardIndexByValue(_valSets[kSingle][singleIdx])) == _hand.getHandSize()) {
                throw std::runtime_error("expected to find single but failed");
            }
            _combo.addCard(_hand.checkCard(index));
        }
        while (!leader && (_combo < curCombo) && (++singleIdx < _valSets[kSingle].size()));

        if (singleIdx < _valSets[kSingle].size()) {
            // combo already created, just play out (remove) card from hand
            _hand.playCard(index);
            return true;
        }
    }
    return false;
}

bool
CpuPlayer::playFollowCombo(const GameState* state, Combo& combo)
{
    if (_hand.getHandSize() == 0) {
        return false;
    }
    combo.resetAll();
    combo.setOwner(_name);
    if (state->combo.getType() != Combo::kStraight) {
        _updateCardCounts();
    }
    switch (state->combo.getType()) {
      case Combo::kSingle:
        if (_trySingle(state->combo, false)) {
            combo = _combo;
            return true;
        }
        break;
      case Combo::kPair:
        if (_tryPair(state->combo, false)) {
            combo = _combo;
            return true;
        }
        break;
      case Combo::kThreeKind:
        if (_tryThreeOfKind(state->combo, false)) {
            combo = _combo;
            return true;
        }
        break;
      case Combo::kStraight:
      case Combo::kFullHouse:
      case Combo::kFourKind:
        _updateStraights();
        if (_tryStraight(state->combo, false)) {
            combo = _combo;
            return true;
        }
        if (_tryFullHouse(state->combo, false)) {
            combo = _combo;
            return true;
        }
        if (_tryFourOfKind(state->combo, false)) {
            combo = _combo;
            return true;
        }
        break;
      default:
        return false;
        break;
    }

    // pass
    return false;
}


/****************************************************
 ***************** HumanPlayer **********************
 ****************************************************/

HumanPlayer::HumanPlayer(void)
    : Player()
{
}

HumanPlayer::HumanPlayer(const std::string name)
    : Player(name)
{
}

HumanPlayer::~HumanPlayer(void)
{
}

const Combo&
HumanPlayer::playLeadCombo(const GameState* state)
{
    printComboTypes(std::cerr);
    int type;
    uint16_t numCards = 0;
    while (true) {
        std::cerr << "\nindices:    0    1    2    3    4    5    6    7    8    9   10   11   12\n";
        printHand(std::cerr);
        printf("\nEnter combo type to lead with: ");
        scanf("%d", &type);
        if (type < 0 || type > Combo::NUMTYPES) {
            printf("Invalid combo type.\n");
            continue;
        }
        _combo.resetAll();
        _combo.setOwner(_name);
        _combo.setType((Combo::ComboT)type);
        if ((numCards = Combo::getNumCardsInCombo((Combo::ComboT)type)) == 0) {
            printf("Invalid combo type.\n");
            continue;
        }
        std::vector<CardPtr> cardsToPlay;
        while (cardsToPlay.size() < numCards) {
            int index;
            while (true) {
                printf("\nEnter card index to play [%lu]: ", cardsToPlay.size()+1);
                scanf("%d", &index);
                if (index < 0 || index > _hand.getHandSize()-1) {
                    printf("Invalid index.\n");
                    continue;
                }
                break;
            }
            if (_combo.addCard(_hand.checkCard(index))) {
                cardsToPlay.push_back(CardPtr(new Card(*(_hand.checkCard(index)))));
            }
        }
        for (uint16_t i = 0; i < cardsToPlay.size(); ++i) {
            uint16_t index;
            if (cardsToPlay[i]->isFaceCard()) {
                _hand.findCard(cardsToPlay[i]->getFace(), cardsToPlay[i]->getSuit(), index);
            }
            else {
                _hand.findCard(cardsToPlay[i]->getValue(), cardsToPlay[i]->getSuit(), index);
            }
            _hand.playCard(index);
        }
        break;
    } /* end while */
    
    return _combo;
}

bool
HumanPlayer::playFollowCombo(const GameState* state, Combo& combo)
{
    printComboTypes(std::cerr);
    int pass = -1;
    int type;
    uint16_t numCards = 0;
    while (true) {
        std::cerr << "\nindices:    0    1    2    3    4    5    6    7    8    9   10   11   12\n";
        printHand(std::cerr);
        printf("\nPass (0) or play (1)? ");
        scanf("%d", &pass);
        if (pass != 0 && pass != 1) {
            continue;
        }
        if (pass == 0) {
            return false;
        }
        printf("\nEnter combo type to follow: ");
        scanf("%d", &type);
        if (type < 0 || type > Combo::NUMTYPES) {
            printf("Invalid combo type.\n");
            continue;
        }
        if (state->combo.getType() < Combo::kStraight && type != state->combo.getType()) {
            printf("Must follow combo type for non-five-card combos.\n");
            continue;
        }
        if (type < state->combo.getType()) {
            printf("Must be able to meet or beat current combo type.\n");
            continue;
        }
        _combo.resetAll();
        _combo.setOwner(_name);
        _combo.setType((Combo::ComboT)type);
        if ((numCards = Combo::getNumCardsInCombo((Combo::ComboT)type)) == 0) {
            printf("Invalid combo type.\n");
            continue;
        }
        std::vector<uint16_t> indices;
        while (indices.size() < numCards) {
            int index;
            while (true) {
                printf("\nEnter card index to play [%lu]: ", indices.size()+1);//cardsToPlay.size()+1);
                scanf("%d", &index);
                if (index < 0 || index > _hand.getHandSize()-1) {
                    printf("Invalid index.\n");
                    continue;
                }
                break;
            }
            if (_combo.addCard(_hand.checkCard(index))) {
                indices.push_back(index);
            }
        }
        if (type == state->combo.getType() && _combo < state->combo) {
            printf("Combo does not beat current combo.\n");
            _combo.resetAll();
            continue;
        }
        _hand.playCards(indices);
        combo = _combo;
        break;
    } /* end while */
    return true;
}

void
HumanPlayer::printComboTypes(std::ostream& os) const
{
    os << "\n"
       << "===== Combo Types =====\n"
       << "High card          : " << Combo::kSingle << "\n"
       << "One Pair           : " << Combo::kPair << "\n"
       << "Three-of-a-kind    : " << Combo::kThreeKind << "\n"
       << "Straight           : " << Combo::kStraight << "\n"
       << "Flush              : " << Combo::kFlush << "\n"
       << "Full House         : " << Combo::kFullHouse << "\n"
       << "Four-of-a-kind     : " << Combo::kFourKind << "\n"
       << "Straight Flush     : " << Combo::kStraightFlush << "\n"
       << "=======================\n\n";
}

} /* namespace pusoydos */
