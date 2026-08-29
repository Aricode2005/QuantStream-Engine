#pragma once
#include <string>
#include "Tick.h"
#include "Portfolio.h"
using namespace std;
class Strategy {
public:
    virtual ~Strategy() = default;
    string evaluateTrade(const Tick& currentTick, Portfolio& userPortfolio, double riskPct);
protected:
    virtual string checkStopLoss(const Tick& currentTick, Portfolio& userPortfolio) const;
    int computeOrderQuantity(const Portfolio& userPortfolio, double price, double riskPct) const;
    virtual string generateSignal(const Tick& currentTick, Portfolio& userPortfolio, double riskPct) = 0;
    static constexpr double kStopLossPct = 0.05;
};
