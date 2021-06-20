#include <iostream>
#include <random>
#include <time.h>
#include <chrono>
#include <vector>
#include <assert.h>

#include "headers/ErlangDistribution.hpp"
#include "headers/TeamMatch.hpp"
#include "headers/Team.hpp"
#include "headers/Player.hpp"

#define TOTAL_MATCHES 1000

int main() {
    int aw, bw, dw;
    aw = bw = dw = 0;

    srand(time(NULL));
    unsigned seed = std::chrono::system_clock::now().time_since_epoch().count();
    std::chi_squared_distribution<float> uniDub(1.0f);
    std::default_random_engine gen(seed);

    auto start = std::chrono::high_resolution_clock::now();

    for (size_t i = 0; i < TOTAL_MATCHES; i++)
    {
        std::vector<FieldUnit::Player> APlayers{
            FieldUnit::Player(7.5f,uniDub(gen),"Kanker", FieldUnit::Player::MIDFIELDER),
            FieldUnit::Player(8.0f,uniDub(gen),"asda", FieldUnit::Player::MIDFIELDER),
            FieldUnit::Player(9.0f,uniDub(gen),"Epfdgfdgfgd", FieldUnit::Player::MIDFIELDER),
            FieldUnit::Player(7.5f,uniDub(gen),"Epxcxxcxcic", FieldUnit::Player::MIDFIELDER),
            FieldUnit::Player(9.0f,uniDub(gen),"Eqwqwwqwqwpic", FieldUnit::Player::MIDFIELDER),
            FieldUnit::Player(7.5f,uniDub(gen),"Epicgfdgfdgfdg", FieldUnit::Player::STRIKERS),
            FieldUnit::Player(8.5f,uniDub(gen),"Eczxxczxczxczpic", FieldUnit::Player::STRIKERS),
            FieldUnit::Player(9.0f,uniDub(gen),"Epqqqqqqqqqic", FieldUnit::Player::DEFENDER),
            FieldUnit::Player(8.0f,uniDub(gen),"Epaaaaaaaaic", FieldUnit::Player::DEFENDER),
            FieldUnit::Player(9.0f,uniDub(gen),"Epzzzzzzzzzzic", FieldUnit::Player::DEFENDER)
        };

        std::vector<FieldUnit::Player> BPlayers{
            FieldUnit::Player(9.0f,uniDub(gen),"Kanker", FieldUnit::Player::MIDFIELDER),
            FieldUnit::Player(9.0f,uniDub(gen),"asda", FieldUnit::Player::MIDFIELDER),
            FieldUnit::Player(9.5f,uniDub(gen),"Epfdgfdgfgd", FieldUnit::Player::MIDFIELDER),
            FieldUnit::Player(10.0f,uniDub(gen),"Epxcxxcxcic", FieldUnit::Player::MIDFIELDER),
            FieldUnit::Player(8.0f,uniDub(gen),"Eqwqwwqwqwpic", FieldUnit::Player::STRIKERS),
            FieldUnit::Player(7.0f,uniDub(gen),"Epicgfdgfdgfdg", FieldUnit::Player::STRIKERS),
            FieldUnit::Player(8.5f,uniDub(gen),"Eczxxczxczxczpic", FieldUnit::Player::DEFENDER),
            FieldUnit::Player(7.0f,uniDub(gen),"Epqqqqqqqqqic", FieldUnit::Player::DEFENDER),
            FieldUnit::Player(7.0f,uniDub(gen),"Epaaaaaaaaic", FieldUnit::Player::DEFENDER),
            FieldUnit::Player(7.5f,uniDub(gen),"Epzzzzzzzzzzic", FieldUnit::Player::DEFENDER)
        };

        Match::Team a("Cheese Cakes", 0.35f, 3.0f, 2.0f, 0.0f , APlayers, FieldUnit::Player(8.0f,uniDub(gen), "Rofl", FieldUnit::Player::GOALKEEPER), Match::Team::Formations::_4_4_2);
        Match::Team b("Goed God", 0.20f, 2.0f , 1.0f , 0.0f , BPlayers, FieldUnit::Player(9.0f,uniDub(gen), "Lmao", FieldUnit::Player::GOALKEEPER), Match::Team::Formations::_4_4_2);
        Match::FaceOff PlayOff(a, b);

        PlayOff.Simulate();

        if( PlayOff.ScoreA > PlayOff.ScoreB) aw++;
        else if( PlayOff.ScoreA < PlayOff.ScoreB) bw++;
        else dw++;

    }

    auto stop = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(stop - start);
    std::cout << "Total matches : " << TOTAL_MATCHES << " Runtime: " << duration.count() << " miliseconds" << std::endl;

    std::cout << "******************score*********************" << std::endl;
    std::cout << "A wins : " << aw << std::endl;
    std::cout << "B wins : " << bw << std::endl;
    std::cout << "Draws : " << dw << std::endl;
    std::cout << "******************score*********************" << std::endl;
    return 0;
}
