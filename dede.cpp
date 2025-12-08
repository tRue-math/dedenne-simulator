#include <iostream>
#include <vector>
#include <random>
#include <iomanip>
#include <cmath>
#include <string>

const int SIMULATION_WEEKS = 100000;
const double SKILL_BOOST_PERCENT = 0.10;
const double MAX_CRIT_CHANCE = 0.70;
const int MEALS_PER_DAY = 3;
const int ITER_BETWEEN_MEAL = 100;

struct DayConfig {
    double base_crit_chance;
    double crit_multiplier;
};

const DayConfig WEEKDAY_CONFIG = {0.10, 2.0};
const DayConfig SUNDAY_CONFIG  = {0.30, 3.0};

std::random_device rd;
std::mt19937 gen(rd());
std::uniform_real_distribution<> dist_prob(0.0, 1.0);

std::pair<double,double> simulate_week(double triggers_per_day, double dedenne_cap=MAX_CRIT_CHANCE) {
    double total_week_multiplier = 0.0;
    double current_boost = 0.0;
    int dedenne_rest_counts = 0;
    std::poisson_distribution<> dist_poisson(triggers_per_day / (MEALS_PER_DAY * ITER_BETWEEN_MEAL));

    for (int day = 0; day < 7; ++day) {
        bool is_sunday = (day == 6);
        const DayConfig& config = is_sunday ? SUNDAY_CONFIG : WEEKDAY_CONFIG;

        bool hungry = false;
        for (int iter = 0; iter < MEALS_PER_DAY * ITER_BETWEEN_MEAL; ++iter) {
            if (iter % ITER_BETWEEN_MEAL == 0) hungry = true;
            
            if (current_boost >= dedenne_cap) dedenne_rest_counts++;
            else {
                int triggers = dist_poisson(gen);
                current_boost += triggers * SKILL_BOOST_PERCENT;
                if (current_boost > dedenne_cap) current_boost = dedenne_cap;
            }
            
            if (hungry && (current_boost >= dedenne_cap || iter % ITER_BETWEEN_MEAL == ITER_BETWEEN_MEAL - 1)) {
                hungry = false;
                double final_chance = config.base_crit_chance + current_boost;
                if (dist_prob(gen) < final_chance) {
                    total_week_multiplier += config.crit_multiplier;
                    current_boost = 0.0;
                } else {
                    total_week_multiplier += 1.0;
                }
            }
        }
    }
    return {total_week_multiplier, dedenne_rest_counts / (double)(7 * (MEALS_PER_DAY * ITER_BETWEEN_MEAL))};
}

double calculate_baseline() {
    double weekday_ev = (0.10 * 2.0 + 0.90 * 1.0) * 3 * 6;
    double sunday_ev  = (0.30 * 3.0 + 0.70 * 1.0) * 3 * 1;
    return weekday_ev + sunday_ev;
}

int main() {
    double baseline = calculate_baseline();

    std::cout << "Trigger/Day, " 
            << "Net_Gain_30%(Week), Rest_Rate_30%, "
            << "Net_Gain_40%(Week), Rest_Rate_40%, "
            << "Net_Gain_50%(Week), Rest_Rate_50%, "
            << "Net_Gain_60%(Week), Rest_Rate_60%, "
            << "Net_Gain_70%(Week), Rest_Rate_70%"
            << std::endl;

    for (int i = 20; i <= 60; ++i) {
        std::cout << std::fixed << std::setprecision(1) << i * 0.1 << ", ";
        for (int j = 3; j <= 7; ++j) {
            double rate = i * 0.1;
            double cap = j * 0.1;
            double total_multipler_sum = 0.0;
            double total_rest_rate_sum = 0.0;

            for (int w = 0; w < SIMULATION_WEEKS; ++w) {
                auto [multipler, rest_rate] = simulate_week(rate, cap);
                total_multipler_sum += multipler;
                total_rest_rate_sum += rest_rate;
            }

            double avg_multipler_total = total_multipler_sum / SIMULATION_WEEKS;
            double net_gain_week = avg_multipler_total - baseline;

            double avg_rest_rate = total_rest_rate_sum / SIMULATION_WEEKS;
            
            double total_triggers_week = rate * 7.0;
            
            double value_per_trigger = net_gain_week / total_triggers_week;

            std::cout << std::setprecision(4) << net_gain_week << ", "
                    << std::setprecision(4) << avg_rest_rate << (j == 7 ? "" : ", ");
        }
        std::cout << std::endl;
    }

    return 0;
}