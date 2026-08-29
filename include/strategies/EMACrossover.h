#pragma once
#include "Strategy.h"

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
