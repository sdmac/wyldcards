#include <iostream>
#include <sstream>
#include <stdexcept>
#include <iomanip>

#include "Combo.h"

namespace pusoydos {

const uint16_t Combo::sMaxComboSize = 5;

Combo::Combo(const ComboT type)
    : _name(""), _type(type)
{
}

Combo::Combo(const std::string name, const ComboT type)
    : _name(name), _type(type)
{
}

Combo::Combo(const Combo& other)
    : _name(other._name), _type(other._type)
{
    _cardCombo.assign(other._cardCombo.begin(), other._cardCombo.end());
}

Combo&
Combo::operator=(const Combo& other)
{
    if (this != &other) {
        _cardCombo.assign(other._cardCombo.begin(), other._cardCombo.end());
        _name = other._name;
        _type = other._type;
    }
    return *this;
}

void
Combo::setType(const ComboT type)
{
    _type = type;
}

Combo::ComboT
Combo::getType(void) const
{
    return _type;
}

bool
Combo::addCard(CardPtr card)
{
    bool valid = true;
    uint16_t size = _cardCombo.size();
    switch (_type) {
      case kSingle:
        if (size > 0) {
            std::cerr << "cannot add more cards to single combo\n";
            return false;
        }
        break;
      case kPair:
        if (size > 1) {
            std::cerr << "cannot add more cards to pair combo\n";
            return false;
        }
        else if (size == 1) {
            valid = (card->getNumber() == _cardCombo[0]->getNumber());
            if (!valid) {
                std::cerr << "cards in pair must be of same rank\n";
            }
        }
        break;
      case kThreeKind:
        if (size > 2) {
            std::cerr << "cannot add more cards to three-of-a-kind combo\n";
            return false;
        }
        else if (size == 1 || size == 2) {
            valid = (card->getNumber() == _cardCombo[0]->getNumber());
            if (!valid) {
                std::cerr << "cards in three-of-a-kind  must be of same rank\n";
            }
        }
        break;
      case kStraight:
      case kFlush:
      case kFullHouse:
      case kFourKind:
      case kStraightFlush:
        if (size > 4) {
            std::cerr << "cannot add more cards to five-card combo\n";
            return false;
        }
        else if (size == 4) {
            _cardCombo.push_back(card);
            return _validateFiveCardCombo();
        }
        break;
      default:
        throw std::invalid_argument("cannot add card if combo type not defined");
        break;
    }
    if (_cardCombo.size() >= sMaxComboSize) {
        std::cerr << "exceeding maximum size of combo\n";
        valid = false;
    }
    if (valid) {
        _cardCombo.push_back(card);
    }
    return valid;
}

uint16_t
Combo::getNumCardsInCombo(ComboT type)
{
    uint16_t numCards = 0;
    switch (type) {
      case kSingle:
        numCards = 1;
        break;
      case kPair:
        numCards = 2;
        break;
      case kThreeKind:
        numCards = 3;
        break;
      case kStraight:
      case kFlush:
      case kFullHouse:
      case kFourKind:
      case kStraightFlush:
        numCards = 5;
        break;
      default:
        numCards = 0;
        break;
    }
    return numCards;
}

bool
Combo::_validateFiveCardCombo(void)
{
    // precondition: all 5 cards must have already been added
    sort();
    uint16_t val;
    char suit;
    std::map<uint16_t, uint16_t> counts;
    switch (_type) {
      case kStraight:
        val = _cardCombo[0]->getValue();
        for (uint16_t i = 1; i < _cardCombo.size(); ++i) {
            if (_cardCombo[i]->getValue() == val+1 ||
                (_cardCombo[0]->getValue() == 3 &&
                    (i == 3 && _cardCombo[i]->getValue() == 14) ||
                    (i == 4 && _cardCombo[i]->getValue() == 15))) {
                ++val;
            }
            else {
                std::cerr << "Straight must be a set of cards with monotonically increasing value\n";
                return false;
            }
        }
        break;
      case kFlush:
        suit = _cardCombo[0]->getSuit();
        for (uint16_t i = 1; i < _cardCombo.size(); ++i) {
            if (_cardCombo[i]->getSuit() != suit) {
                std::cerr << "All cards in Flush must be of same suit\n";
                return false;
            }
        }
        break;
      case kFullHouse:
        for (uint16_t i = 0; i < _cardCombo.size(); ++i) {
            val = _cardCombo[i]->getValue();
            (counts.find(val) == counts.end()) ? counts[val] = 1
                                               : counts[val]++;
        }
        if (counts.size() == 2 &&
            (counts.begin()->second == 3 || counts.rbegin()->second == 3)) {
            return true;
        }
        else {
            std::cerr << "Full House must have one pair and one three-of-a-kind\n";
            return false;
        }
        break;
      case kFourKind:
        for (uint16_t i = 0; i < _cardCombo.size(); ++i) {
            val = _cardCombo[i]->getValue();
            (counts.find(val) == counts.end()) ? counts[val] = 1
                                               : counts[val]++;
        }
        if (counts.size() == 2 &&
            (counts.begin()->second == 4 || counts.rbegin()->second == 4)) {
            return true;
        }
        else {
            std::cerr << "Four-of-a-kind must have four cards of the same value\n";
            return false;
        }
        break;
      case kStraightFlush:
        suit = _cardCombo[0]->getSuit();
        for (uint16_t i = 1; i < _cardCombo.size(); ++i) {
            if (_cardCombo[i]->getSuit() != suit) {
                std::cerr << "All cards in Straight Flush must be of same suit\n";
                return false;
            }
        }
        val = _cardCombo[0]->getValue();
        for (uint16_t i = 1; i < _cardCombo.size(); ++i) {
            if (_cardCombo[i]->getValue() == val+1 ||
                (_cardCombo[0]->getValue() == 3 &&
                    (i == 3 && _cardCombo[i]->getValue() == 14) ||
                    (i == 4 && _cardCombo[i]->getValue() == 15))) {
                ++val;
            }
            else {
                std::cerr << "Straight Flush must be a set of cards with monotonically increasing value\n";
                return false;
            }
        }
        break;
      default:
        return false;
        break;
    }
    return true;
}

const CardPtr&
Combo::getCard(const uint16_t index) const
{
    return _cardCombo.at(index);
}

void
Combo::setCardCombo(const std::vector<CardPtr>& cardCombo)
{
    if (cardCombo.size() > sMaxComboSize) {
        throw std::invalid_argument("exceeding maximum size of combo");
    }
    _cardCombo.assign(cardCombo.begin(), cardCombo.end());
}

const std::vector<CardPtr>&
Combo::getCardCombo(void) const
{
    return _cardCombo;
}

uint16_t
Combo::getSize(void) const
{
    return _cardCombo.size();
}

const CardPtr&
Combo::highCard(void) const
{
    SuitList suits = Card::getSuitList();
    return (*std::max_element(_cardCombo.begin(),
                              _cardCombo.end(),
                              CompareCards(suits)));
}

const CardPtr&
Combo::lowCard(void) const
{
    SuitList suits = Card::getSuitList();
    return (*std::min_element(_cardCombo.begin(),
                              _cardCombo.end(),
                              CompareCards(suits)));
}

void
Combo::setOwner(const std::string name)
{
    _name = name;
}

const std::string&
Combo::getOwner(void) const
{
    return _name;
}

std::string
Combo::getComboTypeString(ComboT type)
{
    std::string typeString = "";
    switch (type) {
      case kSingle:
        typeString = "High Card";
        break;
      case kPair:
        typeString = "Pair";
        break;
      case kThreeKind:
        typeString = "Three-of-a-Kind";
        break;
      case kStraight:
        typeString = "Straight";
        break;
      case kFlush:
        typeString = "Flush";
        break;
      case kFullHouse:
        typeString = "Full House";
        break;
      case kFourKind:
        typeString = "Four-of-a-Kind";
        break;
      case kStraightFlush:
        typeString = "Straight Flush";
        break;
      default:
        break;
    }
    return typeString;
}

std::ostream&
Combo::printToStream(std::ostream& os)
{
    sort();
    os << std::setw(20) << std::right << _name << " | "
       << std::setw(16) << std::left << getComboTypeString(_type) << "  ";
    for (uint16_t i = 0; i < _cardCombo.size(); ++i) {
        _cardCombo[i]->printToStream(os);
        os << " ";
    }
    os << "\n";
    return os;
}

void
Combo::sort(void)
{
    SuitList suits = Card::getSuitList();
    std::sort(_cardCombo.begin(), _cardCombo.end(), CompareCards(suits));
}

bool
Combo::operator<(const Combo& rhs) const
{
    if (_cardCombo.size() == 0) {
        return false;
    }
    if (_type != rhs._type) {
        return (_type < rhs._type);
    }
    SuitList suits = Card::getSuitList();
    CardPtr lHigh = highCard();
    CardPtr rHigh = rhs.highCard();
    if (lHigh.get() == NULL ||
        rHigh.get() == NULL) {
        throw std::invalid_argument("cannot compare combos");
    }
    switch (_type) {
      case kThreeKind:
        if (_cardCombo.size() < 3) {
            throw std::invalid_argument("cannot compare combos");
        }
      case kPair:
        if (_cardCombo.size() < 2) {
            throw std::invalid_argument("cannot compare combos");
        }
      case kSingle:
        if (_cardCombo.size() < 1) {
            throw std::invalid_argument("cannot compare combos");
        }
        // 1, 2, 3 or straight combo
        return (lHigh->getValue() < rHigh->getValue() ||
                    (lHigh->getValue() == rHigh->getValue() &&
                     suits[lHigh->getSuit()] < suits[rHigh->getSuit()]));
                        
        break;
      case kStraight:
      case kStraightFlush:
        if (_cardCombo.size() < 5) {
            throw std::invalid_argument("cannot compare combos");
        }
        // ace is low card in straight
        if (hasCard(Card::Ace) && lowCard()->getValue() == 3) {
            // since 2 has internally highest value, 3 is lowest value card
            lHigh = findHighCardInAceLowStraight(_cardCombo);
        }
        if (rhs.hasCard(Card::Ace) && rhs.lowCard()->getValue() == 3) {
            // since 2 has internally highest value, 3 is lowest value card
            rHigh = findHighCardInAceLowStraight(rhs.getCardCombo());
        }
        return (lHigh->getValue() < rHigh->getValue() ||
                    (lHigh->getValue() == rHigh->getValue() &&
                     suits[lHigh->getSuit()] < suits[rHigh->getSuit()]));
        break;
      case kFullHouse:
      case kFourKind:
        if (_cardCombo.size() < 5) {
            throw std::invalid_argument("cannot compare combos");
        }
        if (_type == rhs._type) {
            // only consider card that appears more than 2 times
            uint16_t threshold = 2;
            uint16_t lHighVal = findHighCountCard(_cardCombo, threshold);
            uint16_t rHighVal = findHighCountCard(rhs.getCardCombo(), threshold);
            // card value guaranteed to be higher or lower since only
            // one deck is used
            return (lHighVal < rHighVal);
        }
        break;
      case kFlush:
        if (_cardCombo.size() < 5) {
            throw std::invalid_argument("cannot compare combos");
        }
        if (_type == rhs._type) {
            ComboListT lCombo(_cardCombo);
            ComboListT rCombo(rhs.getCardCombo());
            while (findHighValCard(lCombo) == findHighValCard(rCombo) &&
                   lCombo.size() > 1) {
                lCombo.pop_back();
                rCombo.pop_back();
            }
            uint16_t lHighVal = findHighValCard(lCombo);
            uint16_t rHighVal = findHighValCard(rCombo);
            if (lCombo.size() > 1) {
                // at least two cards differed in value
                return (lHighVal < rHighVal);
            }
            else {
                // one or zero cards differed in value
                if (lHighVal == rHighVal) {
                    // all cards have same value so compare suit of last remaining card
                    return (suits[lCombo[0]->getSuit()] < suits[rCombo[0]->getSuit()]);
                }
                else {
                    // last card differed
                    return (lHighVal < rHighVal);
                }
            }
        }
        break;
      default:
        throw std::invalid_argument("cannot compare combos");
        break;
    }
    return false;
}

CardPtr
Combo::findHighCardInAceLowStraight(const ComboListT &combo)
{
    SuitList suits = Card::getSuitList();
    ComboListT comb(combo);
    for (ComboListT::iterator it = comb.begin();
         it != comb.end(); ++it) {
        if (((*it)->isFaceCard() && (*it)->getFace() == Card::Ace) ||
            (*it)->getNumber() == 2) {
            // remove ace and 2 (highest value card)
            comb.erase(it--);
        }
    }
    // find new high card (with ace and 2 now removed)
    return (*std::max_element(comb.begin(),
                              comb.end(),
                              CompareCards(suits)));
}
uint16_t
Combo::findHighValCard(ComboListT& combo)
{
    // sort by value and suit
    std::sort(combo.begin(), combo.end(),
              CompareCards(Card::getSuitList()));
    // highest value returned (suit ignored)
    return combo.back()->getValue();
}

uint16_t
Combo::findHighCountCard(const ComboListT& combo, const uint16_t threshold)
{
    std::map<uint16_t, uint16_t> cardCount;
    for (uint16_t i = 0; i < combo.size(); ++i) {
        uint16_t cardVal = combo.at(i)->getValue();
        (cardCount.find(cardVal) == cardCount.end()) ? cardCount[cardVal] = 1
                                                     : cardCount[cardVal] += 1;
    }
    for (std::map<uint16_t, uint16_t>::iterator it = cardCount.begin();
         it != cardCount.end(); ++it) {
        // return first card above threshold
        if (it->second > threshold) {
            return it->first;
        }
    }
    return 0;
}

bool
Combo::hasCard(const uint16_t number) const
{
    for (uint16_t i = 0; i < _cardCombo.size(); ++i) {
        if (_cardCombo[i]->getNumber() == number) {
            return true;
        }
    }
    return false;
}

bool
Combo::hasCard(const uint16_t number, const char suit) const
{
    for (uint16_t i = 0; i < _cardCombo.size(); ++i) {
        if (_cardCombo[i]->getNumber() == number &&
            _cardCombo[i]->getSuit() == suit) {
            return true;
        }
    }
    return false;
}

bool
Combo::hasCard(const char face) const
{
    for (uint16_t i = 0; i < _cardCombo.size(); ++i) {
        if (_cardCombo[i]->isFaceCard() &&
            _cardCombo[i]->getFace() == face) {
            return true;
        }
    }
    return false;
}

bool
Combo::hasCard(const char face, const char suit) const
{
    for (uint16_t i = 0; i < _cardCombo.size(); ++i) {
        if (_cardCombo[i]->isFaceCard() &&
            _cardCombo[i]->getFace() == face &&
            _cardCombo[i]->getSuit() == suit) {
            return true;
        }
    }
    return false;
}

uint16_t
Combo::hasNumOfCard(const uint16_t number) const
{
    uint16_t numOfCard = 0;
    for (uint16_t i = 0; i < _cardCombo.size(); ++i) {
        if (_cardCombo[i]->getNumber() == number) {
            ++numOfCard;
        }
    }
    return numOfCard;
}

uint16_t
Combo::hasNumOfCard(const uint16_t number, const char suit) const
{
    uint16_t numOfCard = 0;
    for (uint16_t i = 0; i < _cardCombo.size(); ++i) {
        if (_cardCombo[i]->getNumber() == number &&
            _cardCombo[i]->getSuit() == suit) {
            ++numOfCard;
        }
    }
    return numOfCard;
}

uint16_t
Combo::hasNumOfCard(const char face) const
{
    uint16_t numOfCard = 0;
    for (uint16_t i = 0; i < _cardCombo.size(); ++i) {
        if (_cardCombo[i]->isFaceCard() &&
            _cardCombo[i]->getFace() == face) {
            ++numOfCard;
        }
    }
    return numOfCard;
}

uint16_t
Combo::hasNumOfCard(const char face, const char suit) const
{
    uint16_t numOfCard = 0;
    for (uint16_t i = 0; i < _cardCombo.size(); ++i) {
        if (_cardCombo[i]->isFaceCard() &&
            _cardCombo[i]->getFace() == face &&
            _cardCombo[i]->getSuit() == suit) {
            ++numOfCard;
        }
    }
    return numOfCard;
}

void
Combo::resetCards(void)
{
    _cardCombo.clear();
}

void
Combo::resetAll(void)
{
    _type = kUndef;
    _name.clear();
    _cardCombo.clear();
}

} /* namespace pusoydos */
