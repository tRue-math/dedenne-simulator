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

struct DayConfig {
    double base_crit_chance;
    double crit_multiplier;
};

const DayConfig WEEKDAY_CONFIG = {0.10, 2.0};
const DayConfig SUNDAY_CONFIG  = {0.30, 3.0};

std::random_device rd;
std::mt19937 gen(rd());
std::uniform_real_distribution<> dist_prob(0.0, 1.0);

double simulate_week(double triggers_per_day) {
    double total_week_multiplier = 0.0;
    double current_boost = 0.0;
    std::poisson_distribution<> dist_poisson(triggers_per_day / 3.0);

    for (int day = 0; day < 7; ++day) {
        bool is_sunday = (day == 6);
        const DayConfig& config = is_sunday ? SUNDAY_CONFIG : WEEKDAY_CONFIG;

        for (int meal = 0; meal < 3; ++meal) {
            int triggers = dist_poisson(gen);
            current_boost += triggers * SKILL_BOOST_PERCENT;

            if (current_boost > MAX_CRIT_CHANCE) current_boost = MAX_CRIT_CHANCE;
            double final_chance = config.base_crit_chance + current_boost;

            if (dist_prob(gen) < final_chance) {
                total_week_multiplier += config.crit_multiplier;
                current_boost = 0.0;
            } else {
                total_week_multiplier += 1.0;
            }
        }
    }
    return total_week_multiplier;
}

double calculate_baseline() {
    double weekday_ev = (0.10 * 2.0 + 0.90 * 1.0) * 3 * 6;
    double sunday_ev  = (0.30 * 3.0 + 0.70 * 1.0) * 3 * 1;
    return weekday_ev + sunday_ev;
}

int main() {
    double baseline = calculate_baseline();

    std::cout << "Trigger/Day, Net_Gain(Week), Value_Per_Trigger(Coeff)" << std::endl;

    for (int i = 1; i <= 100; ++i) {
        double rate = i * 0.1;
        double total_sum = 0.0;

        for (int w = 0; w < SIMULATION_WEEKS; ++w) {
            total_sum += simulate_week(rate);
        }

        double avg_total = total_sum / SIMULATION_WEEKS;
        double net_gain_week = avg_total - baseline;
        
        double total_triggers_week = rate * 7.0;
        
        double value_per_trigger = net_gain_week / total_triggers_week;

        std::cout << std::fixed << std::setprecision(1) << rate << ", "
                  << std::setprecision(4) << net_gain_week << ", "
                  << std::setprecision(5) << value_per_trigger << std::endl;
    }

    return 0;
}