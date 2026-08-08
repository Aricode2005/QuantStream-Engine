#pragma once
#include "Strategy.h"

// Trend-following: BUY when the fast EMA crosses above the slow EMA,
// SELL when it crosses below. See project report Section 4.2.
class EMACrossover : public Strategy {
public:
    EMACrossover(int fastWindow, int slowWindow);

protected:
    std::string generateSignal(const Tick& currentTick, Portfolio& userPortfolio, double riskPct) override;

private:
    int fastWindow_;
    int slowWindow_;
    double fastEMA_ = 0.0;
    double slowEMA_ = 0.0;
    int ticks_ = 0;
};
