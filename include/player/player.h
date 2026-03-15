#pragma once

class Player{
    public:
        Player(int starting_health, int starting_money);

        int health() const;
        int money() const;

        void take_damage(int amount);
        void add_money(int amount);
        bool can_afford(int amount) const;
        bool spend_money(int amount);
        bool is_dead() const;
    private:
        int health_;
        int money_;
};