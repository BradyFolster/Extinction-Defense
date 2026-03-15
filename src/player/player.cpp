#include "player/player.h"

Player::Player(int starting_health, int starting_money) : health_(starting_health), money_(starting_money) {}

int Player::health() const{
    return health_;
}
int Player::money() const{
    return money_;
}

void Player::add_money(int amount){
    if (amount < 0){
        return;
    }
    money_ += amount;
}
bool Player::can_afford(int amount) const{
    if (amount < 0){
        return true;
    }
    return money_ >= amount;
}
bool Player::spend_money(int amount){
    if (amount < 0){
        return false;
    }
    if (!can_afford(amount)){
        return false;
    }
    money_ -= amount;
    return true;
}
void Player::take_damage(int amount){
    if (amount < 0){
        return;
    }

    health_ -= amount;

    if (health_ < 0){
        health_ = 0;
    }
}
bool Player::is_dead() const{
    return health_ <= 0;
}