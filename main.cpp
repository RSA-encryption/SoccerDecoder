#include <iostream>
#include <random>
#include <time.h>
#include <chrono>
#include <vector>
#include <assert.h>
#include <thread>
#include <atomic>

#include "headers/ErlangDistribution.hpp"
#include "headers/TeamMatch.hpp"
#include "headers/Team.hpp"
#include "headers/Player.hpp"

#define TOTAL_MATCHES 50000

unsigned seed = std::chrono::system_clock::now().time_since_epoch().count();
auto TCount = std::thread::hardware_concurrency();
std::atomic<int> aw {0}, dw{0}, bw{0};
std::chi_squared_distribution<float> uniDub(1.0f);
std::default_random_engine gen(seed);

void func(std::vector<FieldUnit::Player> APlayers, std::vector<FieldUnit::Player> BPlayers, int chunk)
{
    srand(time(NULL));
    for (size_t i = 0; i < chunk; i++)
    {
        Match::Team a("Cheese Cakes", 0.35f, 3.0f, 2.0f, 0.0f, APlayers, FieldUnit::Player(8.0f, uniDub(gen), "Rofl", FieldUnit::Player::GOALKEEPER), Match::Team::Formations::_4_4_2);
        Match::Team b("Goed God", 0.2f, 2.0f, 1.0f, 0.0f, BPlayers, FieldUnit::Player(9.0f, uniDub(gen), "Lmao", FieldUnit::Player::GOALKEEPER), Match::Team::Formations::_4_4_2);
        Match::FaceOff PlayOff(a, b);

        PlayOff.Simulate();

        if (PlayOff.ScoreA > PlayOff.ScoreB)
            aw++;
        else if (PlayOff.ScoreA < PlayOff.ScoreB)
            bw++;
        else
            dw++;
    }
}

std::vector<int> chunks()
{
    std::vector<int> chunk;
    int total = 0;
    for (size_t i = 0; i < TCount; i++)
    {
        chunk.push_back((int)std::floor(TOTAL_MATCHES) / (TCount + 1));
        total += (int)std::floor(TOTAL_MATCHES) / (TCount + 1);
    }
    if ((int)std::floor(TOTAL_MATCHES) - total != 0)
        chunk.push_back((int)std::floor(TOTAL_MATCHES) - total);
    return chunk;
}

int main()
{
    std::vector<int> chunk = chunks();
    std::vector<std::thread> Threads;
    std::vector<FieldUnit::Player> APlayers{
        FieldUnit::Player(7.5f, uniDub(gen), "Kanker", FieldUnit::Player::MIDFIELDER),
        FieldUnit::Player(8.0f, uniDub(gen), "asda", FieldUnit::Player::MIDFIELDER),
        FieldUnit::Player(9.0f, uniDub(gen), "Epfdgfdgfgd", FieldUnit::Player::MIDFIELDER),
        FieldUnit::Player(7.5f, uniDub(gen), "Epxcxxcxcic", FieldUnit::Player::MIDFIELDER),
        FieldUnit::Player(9.0f, uniDub(gen), "Eqwqwwqwqwpic", FieldUnit::Player::MIDFIELDER),
        FieldUnit::Player(7.5f, uniDub(gen), "Epicgfdgfdgfdg", FieldUnit::Player::STRIKERS),
        FieldUnit::Player(8.5f, uniDub(gen), "Eczxxczxczxczpic", FieldUnit::Player::STRIKERS),
        FieldUnit::Player(9.0f, uniDub(gen), "Epqqqqqqqqqic", FieldUnit::Player::DEFENDER),
        FieldUnit::Player(8.0f, uniDub(gen), "Epaaaaaaaaic", FieldUnit::Player::DEFENDER),
        FieldUnit::Player(9.0f, uniDub(gen), "Epzzzzzzzzzzic", FieldUnit::Player::DEFENDER)};

    std::vector<FieldUnit::Player> BPlayers{
        FieldUnit::Player(9.0f, uniDub(gen), "Kanker", FieldUnit::Player::MIDFIELDER),
        FieldUnit::Player(9.0f, uniDub(gen), "asda", FieldUnit::Player::MIDFIELDER),
        FieldUnit::Player(9.5f, uniDub(gen), "Epfdgfdgfgd", FieldUnit::Player::MIDFIELDER),
        FieldUnit::Player(10.0f, uniDub(gen), "Epxcxxcxcic", FieldUnit::Player::MIDFIELDER),
        FieldUnit::Player(8.0f, uniDub(gen), "Eqwqwwqwqwpic", FieldUnit::Player::STRIKERS),
        FieldUnit::Player(7.0f, uniDub(gen), "Epicgfdgfdgfdg", FieldUnit::Player::STRIKERS),
        FieldUnit::Player(8.5f, uniDub(gen), "Eczxxczxczxczpic", FieldUnit::Player::DEFENDER),
        FieldUnit::Player(7.0f, uniDub(gen), "Epqqqqqqqqqic", FieldUnit::Player::DEFENDER),
        FieldUnit::Player(7.0f, uniDub(gen), "Epaaaaaaaaic", FieldUnit::Player::DEFENDER),
        FieldUnit::Player(7.5f, uniDub(gen), "Epzzzzzzzzzzic", FieldUnit::Player::DEFENDER)};
    auto start = std::chrono::high_resolution_clock::now();
    for (auto &var : chunks())
    {
        Threads.push_back(std::thread([&]
                                      { func(APlayers, BPlayers, var); }));
    }
    for (auto &t : Threads)
    {
        if (t.joinable()) t.join();
    }
    auto stop = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(stop - start);
    std::cout << "******************score*********************" << std::endl;
    std::cout << "A wins : " << aw << std::endl;
    std::cout << "B wins : " << bw << std::endl;
    std::cout << "Draws : " << dw << std::endl;
    std::cout << "Total matches : " << TOTAL_MATCHES << " Runtime: " << duration.count() << " miliseconds" << std::endl;
    std::cout << "******************score*********************" << std::endl;
    return 0;
}