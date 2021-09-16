#include <iostream>
#include <vector>
#include <string>
#include <algorithm>
#include <ctime>
#include <cstdlib>
#include "card.hpp"
#include "game.hpp"

#define DEFAULT_MIN_BANK 200.00

/** 
 * \class card
 * \brief Класс с атрибутами игральной карты.
 * Содержит атрибут номера и масти.
 */
class card 
{
private: 
    int8_t number;
    suit_t suit;
public:
    card() {}
    card(uint8_t n, suit_t s): number(n), suit(s) {}
    ~card() { std::cout << "card was destroyed" << std::endl; }
    void set(uint8_t n, suit_t s) { number = n; suit = s; }
    uint8_t get_number() { return number; }
    suit_t get_suit() { return suit; }
    void display();
};

void card::display()
{
    if(number >= 2 && number <= 10)
        std::cout << number;
    else 
    {
        switch(number)
        {
            case jack: std::cout << 'J'; break;
            case queen: std::cout << 'Q'; break;
            case king: std::cout << 'K'; break;
            case ace: std::cout << 'A'; break;
        }
    }
    switch(suit)
    {
        case clubs: std::cout << static_cast<char>(5); break;
        case diamonds: std::cout << static_cast<char>(4); break;
        case hearts: std::cout << static_cast<char>(3); break;
        case spades: std::cout << static_cast<char>(6); break;
    }
}

/////////////////////////////////////////////////

/** 
 * \class deck
 * \brief Класс с атрибутами колоды карт.
 */
class deck
{
protected:
    std::vector<card> cards;
public:
    deck() { init(); }
    virtual void init() = 0;
    int8_t shuffle();
    card* get_card();
};

int8_t deck::shuffle()
{
    if(!cards.empty())
    {
        std::vector<card>::const_iterator iter;
        srand(static_cast<unsigned int>(time(0)));
        std::random_shuffle(cards.begin(), cards.end());
        return 0;
    }
    else
    {
        std::cout << "deck is empty!" << std::endl;
        return 1;
    }
}

card* deck::get_card()
{
    if(!cards.empty())
    {
        card temp = cards.back();
        cards.pop_back();
        return &temp;
    }
    else 
    {
        std::cout << "deck is empty!" << std::endl;
        return nullptr;
    }
}

/** 
 * \class blackjack_deck
 * \brief Класс с атрибутами колоды карт для игры BlackJack.
 */
class blackjack_deck: public deck { void init() override; };

void blackjack_deck::init()
{
    for(int i = 0; i < 4; i++)  
        for(int j = 2; j < 15; j++)      
            cards.push_back(card(j, static_cast<suit_t>(i)));
}

////////////////////////////////////////////////////

/** 
 * \class player
 * \brief Класс с атрибутами карточного игрока.
 */
class player
{
public:
    static uint8_t counter;
    std::string name;
    std::vector<card> hand;
    unsigned int score;
    void show_hand();
    virtual unsigned int get_score() = 0;
    player(std::string n = "Player" + counter): name(n) { counter++; }
    ~player() { std::cout << name << " has left!" << std::endl; }
};

void player::show_hand()
{
    std::vector<card>::iterator iter;
    for(iter = hand.begin(); iter != hand.end(); ++iter)
        (*iter).display();
    std::cout << std::endl;
}
///////////////////////////////////////////////////

/** 
 * \class blackjack_player
 * \brief Дочерний класс с атрибутами игрока в BlackJack.
 */
class blackjack_player: private player
{
public:
    double bank;
    void hit(blackjack_dealer &d) { d.deal; };
    void stand(blackjack_dealer &d);
    //void split();
    //void double_down();
    void set_bank(double b);
    unsigned int get_score() override;
    blackjack_player(std::string n): player(n) {}
};

void blackjack_player::set_bank(double b) 
{ 
    if(b < DEFAULT_MIN_BANK) 
        std::cout << "min bank is " << DEFAULT_MIN_BANK << std::endl;
    else
        bank = b;
}

unsigned int blackjack_player::get_score()
{
    if(!hand.empty())
    {
        uint8_t sum = 0;
        std::vector<card>::iterator iter;
        for(iter = hand.begin(); iter != hand.end(); ++iter)
        {
            if(iter->get_number() < jack)
                sum += iter->get_number();
            else
            {
                if(iter->get_number() < ace)
                    sum += 10;
                else
                    (sum <= 10) ? sum += 11 : sum += 1;
            }
        }
    }
    else 
    {
        std::cout << "empty hand!" << std::endl;
    }   
}

///////////////////////////////////////////////////


////////////////////////////////////////////////////
/** 
 * \class game
 * \brief Класс с атрибутами игры.
 */
template<typename T>
class game 
{
public:
    std::vector<T> lobby; // добавить конструктор с крупье
    void add_player(T& p) { lobby.push_back(p); }
    void remove_player(T& p);
};



///////////////////////////////////////////////////

/** 
 * \class blackjack
 * \brief Класс с атрибутами игры BlackJack.
 */
class blackjack: public game<blackjack_player>
{  
public:
    blackjack_deck deck_; // to dealer
    blackjack_dealer dealer_;
    blackjack() { std::cout << "welcome to blackjack game!" << std::endl; }
    void polling()
    {
        // give cards  
        dealer_.deal();
        std::vector<blackjack_player>::iterator iter;
        for(iter = lobby.begin(); iter != lobby.end(); ++iter)
            ask((*iter));
        
    }
    void start()
    {
        deck_.shuffle();
    }
    void ask(const blackjack_player &p) // для оболочки cmd
    {
        char ans;
        std::cout << "(h)it or (s)tand?: ";
        std::cin >> ans;
        (ans == 'h') ? p.hit(dealer_) : p.stand(dealer_);
    }
    void tracking();
    //void cut();
    bool burst(player p)
    {
        if(p.score() > 21)
        {
            std::cout << p.name << " was burst!" << std::endl;
            return 1;
        }
        else 
            return 0;
    }

    
};


////////////////////////////////////////////////////

/** 
 * \class ai_player
 * \brief Дочерний класс с атрибутами игрока-ИИ.
 */
class ai_player: public player
{

};


/////////////////////////////////////////////////////

/** 
 * \class dealer
 * \brief Класс с атрибутами крупье.
 */
class blackjack_dealer
{
public:
    const std::string name = "dealer"; 
    std::vector<card> hand;
    void deal(std::vector<blackjack_player> lobby, blackjack_deck deck);
};

void blackjack_dealer::deal(std::vector<blackjack_player> lobby, blackjack_deck deck)
{
    static uint8_t counter = 1;
    std::vector<blackjack_player>::iterator iter;
    for(iter = lobby.begin(); iter != lobby.end(); ++iter)
    {
        if(counter++ == 1) 
        {
            iter->hand.push_back(*deck.get_card());
            iter->hand.push_back(*deck.get_card());
        }
        else
            iter->hand.push_back(*deck.get_card());
    }
}



/////////////////////////////////////////////////////

int main()
{
    blackjack room;
    room.add_player(blackjack_player("test_player"));
    room.add_player(blackjack_player("test_player1")); 
    room.start();
    room.polling();
    return 0;
}