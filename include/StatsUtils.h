#pragma once
#include <string>
#include <vector>
#include "Portfolio.h"

namespace StatsUtils {

struct HurstResult {
    double exponent;
    std::string message;
};

struct KellyResult {
    double fraction;
    std::string message;
};


double calculateHurstExponent(const std::vector<double>& priceData);
HurstResult interpretHurst(double hurstExponent);
}  
