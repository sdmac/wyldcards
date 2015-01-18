#include <sstream>
#include <time.h>
#include <iomanip>
#include <math.h>

#include <iostream>

// game
#include "Util.h"

// hearts
#include "Game.h"

namespace pusoydos {

const uint16_t Game::sDefaultNumPlayers = 4;
const uint16_t Game::sDefaultScreenHeight = 64;

Game::Game(const uint16_t screenHeight)
    : _deck(52),
      _gameState(new GameState()),
      _numPlayers(sDefaultNumPlayers),
      _players(sDefaultNumPlayers),
      _screenHeight(screenHeight),
      _numSets(0)
{
    _setupSuits();
    _setupPlayers();
}

Game::Game(const uint16_t numPlayers, const uint16_t screenHeight)
    : _deck(52),
      _gameState(new GameState()),
      _numPlayers(numPlayers),
      _players(numPlayers),
      _screenHeight(screenHeight),
      _numSets(0)
{
    _setupSuits();
    _setupPlayers();
}

void
Game::_setupSuits(void)
{
    Card::setSuitRank(Card::Clubs, kClubs);
    Card::setSuitRank(Card::Spades, kSpades);
    Card::setSuitRank(Card::Hearts, kHearts);
    Card::setSuitRank(Card::Diamonds, kDiamonds);
}

void
Game::_setupPlayers(void)
{
    for (uint16_t i = 0; i < _numPlayers; ++i) {
        std::ostringstream oss;
        oss << i+1;
        if (i == 0) {
            _players[i] = new HumanPlayer("player" + oss.str());
            _humanPlayer = i;
        }
        else {
            _players[i] = new CpuPlayer("player" + oss.str());
        }
        _scores.insert(ScoreMapT::value_type(_players[i]->getName(), 0));
    }
}

Game::~Game(void)
{
    for (uint16_t i = 0; i < _players.size(); ++i) {
        if (_players[i]) {
            delete _players[i];
        }
    }
    _players.clear();
    if (_gameState) {
        delete _gameState;
    }
}

void
Game::_createDeck(void)
{
    const SuitList& suits = Card::getSuitList();
    const FaceList& faces = Card::getFaceList();
    for (SuitList::const_iterator suitIt = suits.begin();
         suitIt != suits.end(); ++suitIt) {

        // number cards
        for (uint16_t i = 2; i < 11; ++i) {
            // 2s are highest-value cards
            uint16_t value = (i == 2) ? 15 : i;
            CardPtr numberCard(new Card(i, suitIt->first, value));
            _deck.insertCard(numberCard);
        }

        // face cards
        for (FaceList::const_iterator faceIt = faces.begin();
             faceIt != faces.end(); ++faceIt) {
            CardPtr faceCard(new Card(faceIt->first, suitIt->first, faceIt->second));
            _deck.insertCard(faceCard);
        }
    }
}

void
Game::deal(void)
{
    _deck.reset();
    _createDeck();
    _deck.shuffle(10000 * (1+_numSets));
    uint16_t deckSize = _deck.getDeckSize();
    uint16_t numCardsPerPlayer = deckSize / _numPlayers;
    for (uint16_t nCard = 0; nCard < numCardsPerPlayer; ++nCard) {
        for (uint16_t i = 0; i < _numPlayers && _deck.getDeckSize() > 0; ++i) {
            _players[i]->dealCard(_deck.pullFromTop());
        }
    }
}

void
Game::findStartingCard(void)
{
    for (uint16_t i = 0; i < _numPlayers; ++i) {
        if (_players[i]->hasCard((uint16_t)3, Card::Clubs)) {
            _gameState->leadPlayer = i;
            _gameState->firstCombo = true;
            break;
        }
    }
}

bool
Game::playRound(std::ostream& os)
{
    _gameState->combo.resetAll();

    for (uint16_t i = 0; i < _numPlayers; ++i) {
        _players[i]->printHand(os);
        os << "\n";
    }

    uint16_t playerIdx = _gameState->leadPlayer;
    os << "Starting round, leader: " << _players[playerIdx]->getName() << "\n\n";
    std::string cardPileString = "";
    do {
        std::ostringstream oss;
        if (playerIdx == _humanPlayer) {
            os << cardPileString;
        }
        if (playerIdx == _gameState->leadPlayer) {
            // lead combo
            _gameState->combo = _players[playerIdx]->playLeadCombo(_gameState);
            if (_gameState->firstCombo) {
                _gameState->firstCombo = false;
            }
            _gameState->combo.printToStream(oss);
            cardPileString += std::string(oss.str());
        }
        else {
            // non-lead players can pass or beat the current combo
            Combo followCombo(_players[playerIdx]->getName());
            if (_players[playerIdx]->playFollowCombo(_gameState, followCombo)) {
                // player beat current combo, so lead changes
                _gameState->combo = followCombo;
                _gameState->leadPlayer = playerIdx;
                _gameState->combo.printToStream(oss);
                cardPileString += std::string(oss.str());
            }
        }
        if (playerIdx == _humanPlayer) {
            os << cardPileString;
            _promptForEnter();
            Util::clearScreen(os, _screenHeight);
        }
        if (_players[playerIdx]->cardsLeft() == 0) {
            // player that ran out of cards wins
            os << _players[playerIdx]->getName() << " WINS!\n";
            scoreGame(_players[playerIdx]->getName(), _gameState->combo);
            return false;
        }
        playerIdx = (playerIdx == _players.size()-1) ? 0 : playerIdx+1;
    }
    while (playerIdx != _gameState->leadPlayer);

    os << "\nAll other players have passed. "
       << _players[_gameState->leadPlayer]->getName()
       << " wins the round.\n\n";
    _promptForEnter();
    return true;
}

void
Game::_promptForEnter(void)
{
    std::string enter;
    printf("\n\n<Press Enter>");
    std::cin.ignore(10000,'\n');
    std::getline( std::cin, enter );
}

void
Game::scoreGame(const std::string& name, const Combo& finalCombo)
{
    // if last combo included a deuce, points (2^[#deuces])
    // otherwise just 1 point
    uint16_t numTwos = finalCombo.hasNumOfCard((uint16_t)2);
    if (numTwos > 0) {
        _scores[name] += pow(2, numTwos);
    }
    else {
        _scores[name] += 1;
    }
    return;
}

bool
Game::maxScoreReached(const uint16_t maxScore)
{
    for (ScoreMapT::const_iterator cit = _scores.begin();
         cit != _scores.end(); ++cit) {
        if (cit->second >= maxScore) {
            return true;
        }
    }
    return false;
}

Player *
Game::determineWinner(const uint16_t maxScore)
{
    Player * winner = _players[0];
    for (uint16_t i = 0; i < _numPlayers; ++i) {
        if (_scores[_players[i]->getName()] >= maxScore) {
            winner = _players[i];
        }
    }
    return winner;
}

void
Game::playSet(std::ostream& os)
{
    deal();
    findStartingCard();
    Util::clearScreen(os, _screenHeight);
    do {
        Util::clearScreen(os, _screenHeight);
        os << "================================== NEW ROUND ===================================\n\n";
    }
    while (playRound(os));
    ++_numSets;

    for (uint16_t i = 0; i < _numPlayers; ++i) {
        _players[i]->reset();
    }
    printScoreTable(os, _scores);
    _promptForEnter();
}

void
Game::playGame(std::ostream& os)
{
    Util::clearScreen(os, _screenHeight);
    while (!maxScoreReached(3)) {
        os << "================================================================================\n"
           << "================================== NEW SET =====================================\n"
           << "================================================================================\n\n";
        playSet(os);
        os << "GAME SCORE:\n";
    }
    os << "=========== GAME OVER ============\n";
    os << "WINNER: " << std::right << determineWinner(3)->getName() << " !!!\n\n";
    os << "==================================\n";
}

void
Game::printScoreTable(std::ostream& os, ScoreMapT& scores)
{
    os << "\n\n";
    os << "================================\n";
    os << "|          ScoreBoard          |\n";
    os << "================================\n";
    for (uint16_t i = 0; i < _players.size(); ++i) {
        os << "| " << std::left << std::setw(22) << _players[i]->getName() << std::right << std::setw(6) << scores[_players[i]->getName()] << " |\n";
    }
    os << "================================\n";
    os << "\n\n";
}

} /* namespace pusoydos */
