#pragma once
#include <string>
#include <vector>
#include "Portfolio.h"

// Free-standing statistical diagnostics computed once per completed
// backtest: market-regime detection (Hurst exponent) and edge-based
// position sizing (Kelly criterion). See project report Sections
// 4.9–4.10. Grouped as a namespace rather than a class because they
// are stateless, pure functions of their inputs.
namespace StatsUtils {

struct HurstResult {
    double exponent;
    std::string message;
};

struct KellyResult {
    double fraction;
    std::string message;
};

// Variance-ratio proxy for the Hurst exponent, comparing the variance
// of lag-2 price differences to lag-1 price differences.
double calculateHurstExponent(const std::vector<double>& priceData);
HurstResult interpretHurst(double hurstExponent);

// Kelly fraction f* = W - (1-W)/R computed from a Portfolio's realized
// win/loss statistics.
double calculateKellyFraction(const Portfolio& portfolio);
KellyResult interpretKelly(double kellyFraction);

}  // namespace StatsUtils
