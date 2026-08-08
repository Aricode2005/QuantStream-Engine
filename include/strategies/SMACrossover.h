#pragma once
#include <vector>
#include "Strategy.h"

// Trend-following: BUY when the fast SMA crosses above the slow SMA,
// SELL when it crosses below. See project report Section 4.1.
class SMACrossover : public Strategy {
public:
    SMACrossover(int fastWindow, int slowWindow);

protected:
    std::string generateSignal(const Tick& currentTick, Portfolio& userPortfolio, double riskPct) override;

private:
    int fastWindow_;
    int slowWindow_;
    std::vector<double> priceHistory_;
};
