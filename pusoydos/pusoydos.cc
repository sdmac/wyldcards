#include <iostream>
#include <stdexcept>

#include "Deck.h"
#include "Hand.h"

#include "Game.h"

using namespace pusoydos;

int main(int argc, const char* argv[])
{
    // create pusoydos game instance
    Game pusoydos;
    pusoydos.playGame(std::cerr);

    /*
    CardPtr card1(new Card((uint16_t)2, Card::Diamonds, 15));
    CardPtr card2(new Card((uint16_t)3, Card::Diamonds, 3));
    CardPtr card3(new Card(Card::Ace, Card::Clubs, 14));
    CardPtr card4(new Card((uint16_t)10, Card::Hearts, 10));
    CardPtr card5(new Card(Card::Ace, Card::Diamonds, 14));

    Combo combo;
    combo.setType(Combo::kFourKind);
    combo.addCard(card1);
    combo.addCard(card2);
    combo.addCard(card3);
    combo.addCard(card4);
    combo.addCard(card5);

    combo.printToStream(std::cerr);

    //pusoydos.deal();

    //pusoydos.findStartingCard();
    //while (pusoydos.playRound(std::cerr));

    CardPtr pairTens1(new Card((uint16_t)10, Card::Diamonds, 10));
    CardPtr pairTens2(new Card((uint16_t)10, Card::Clubs, 10));

    CardPtr pairTens3(new Card((uint16_t)10, Card::Hearts, 10));
    CardPtr pairTens4(new Card((uint16_t)10, Card::Spades, 10));

    Combo pairCombo1(Combo::kPair);
    pairCombo1.addCard(pairTens1);
    pairCombo1.addCard(pairTens2);

    Combo pairCombo2(Combo::kPair);
    pairCombo2.addCard(pairTens3);
    pairCombo2.addCard(pairTens4);

    pairCombo1.printToStream(std::cerr);
    std::cout << "beats\n";
    pairCombo2.printToStream(std::cerr);
    std::cout << "\n";
    (pairCombo2 < pairCombo1) ? std::cout << "YES\n\n" 
                              : std::cout << "NO\n\n";

    CardPtr threeKings1(new Card(Card::King, Card::Hearts, 13));
    CardPtr threeKings2(new Card(Card::King, Card::Spades, 13));
    CardPtr threeKings3(new Card(Card::King, Card::Clubs, 13));

    CardPtr threeQueens1(new Card(Card::Queen, Card::Diamonds, 12));
    CardPtr threeQueens2(new Card(Card::Queen, Card::Spades, 12));
    CardPtr threeQueens3(new Card(Card::Queen, Card::Clubs, 12));

    Combo threeCombo1(Combo::kThreeKind);
    threeCombo1.addCard(threeKings1);
    threeCombo1.addCard(threeKings2);
    threeCombo1.addCard(threeKings3);

    Combo threeCombo2(Combo::kThreeKind);
    threeCombo2.addCard(threeQueens1);
    threeCombo2.addCard(threeQueens2);
    threeCombo2.addCard(threeQueens3);

    threeCombo1.printToStream(std::cerr);
    std::cout << "beats\n";
    threeCombo2.printToStream(std::cerr);
    std::cout << "\n";
    (threeCombo2 < threeCombo1) ? std::cout << "YES\n\n"
                                : std::cout << "NO\n\n";

    Combo fullHouse1(Combo::kFullHouse);
    fullHouse1.addCard(threeKings1);
    fullHouse1.addCard(threeKings2);
    fullHouse1.addCard(threeKings3);
    fullHouse1.addCard(threeQueens1);
    fullHouse1.addCard(threeQueens2);

    Combo fourTens1(Combo::kFourKind);
    fourTens1.addCard(pairTens1);
    fourTens1.addCard(pairTens2);
    fourTens1.addCard(pairTens3);
    fourTens1.addCard(pairTens4);
    fourTens1.addCard(card1);

    fullHouse1.printToStream(std::cerr);
    std::cout << "beats\n";
    fourTens1.printToStream(std::cerr);
    std::cout << "\n";
    (fourTens1 < fullHouse1) ? std::cout << "YES\n\n"
                             : std::cout << "NO\n\n";

    Combo fullHouse2(Combo::kFullHouse);
    fullHouse2.addCard(threeQueens1);
    fullHouse2.addCard(threeQueens2);
    fullHouse2.addCard(threeQueens3);
    fullHouse2.addCard(threeKings1);
    fullHouse2.addCard(threeKings2);

    fullHouse1.printToStream(std::cerr);
    std::cout << "beats\n";
    fullHouse2.printToStream(std::cerr);
    std::cout << "\n";
    (fullHouse2 < fullHouse1) ? std::cout << "YES\n\n"
                              : std::cout << "NO\n\n";

    CardPtr heart1(new Card(Card::Ace, Card::Hearts, 14));
    CardPtr heart2(new Card(Card::King, Card::Hearts, 13));
    CardPtr heart3(new Card(Card::Queen, Card::Hearts, 12));
    CardPtr heart4(new Card(Card::Jack, Card::Hearts, 11));
    CardPtr heart5(new Card((uint16_t)10, Card::Hearts, 10));
    Combo flushHearts(Combo::kFlush);
    flushHearts.addCard(heart1);
    flushHearts.addCard(heart2);
    flushHearts.addCard(heart3);
    flushHearts.addCard(heart4);
    flushHearts.addCard(heart5);

    CardPtr club1(new Card(Card::Ace, Card::Clubs, 14));
    CardPtr club2(new Card(Card::King, Card::Clubs, 13));
    CardPtr club3(new Card(Card::Queen, Card::Clubs, 12));
    CardPtr club4(new Card(Card::Jack, Card::Clubs, 11));
    CardPtr club5(new Card((uint16_t)10, Card::Clubs, 10));
    Combo flushClubs(Combo::kFlush);
    flushClubs.addCard(club1);
    flushClubs.addCard(club2);
    flushClubs.addCard(club3);
    flushClubs.addCard(club4);
    flushClubs.addCard(club5);

    flushClubs.printToStream(std::cerr);
    std::cout << "beats\n";
    flushHearts.printToStream(std::cerr);
    std::cout << "\n";
    (flushHearts < flushClubs) ? std::cout << "YES\n\n"
                               : std::cout << "NO\n\n";


    CardPtr straightClub1(new Card(Card::Ace, Card::Clubs, 14));
    CardPtr straightClub2(new Card((uint16_t)2, Card::Clubs, 15));
    CardPtr straightClub3(new Card((uint16_t)3, Card::Clubs, 3));
    CardPtr straightClub4(new Card((uint16_t)4, Card::Clubs, 4));
    CardPtr straightClub5(new Card((uint16_t)5, Card::Clubs, 5));
    Combo straightClubs(Combo::kStraightFlush);
    straightClubs.addCard(straightClub1);
    straightClubs.addCard(straightClub2);
    straightClubs.addCard(straightClub3);
    straightClubs.addCard(straightClub4);
    straightClubs.addCard(straightClub5);

    Combo straightHearts(Combo::kStraightFlush);
    straightHearts.addCard(heart1);
    straightHearts.addCard(heart2);
    straightHearts.addCard(heart3);
    straightHearts.addCard(heart4);
    straightHearts.addCard(heart5);

    straightClubs.printToStream(std::cerr);
    std::cout << "beats\n";
    straightHearts.printToStream(std::cerr);
    std::cout << "\n";
    (straightHearts < straightClubs) ? std::cout << "YES\n\n"
                                     : std::cout << "NO\n\n";
    */

    return 0;
}
