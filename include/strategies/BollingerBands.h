#pragma once
#include <vector>
#include "Strategy.h"

// Mean-reversion: BUY when price closes below the lower band, SELL
// when it closes above the upper band. See project report Section 4.3.
class BollingerBands : public Strategy {
public:
    BollingerBands(int windowSize, double stdDevMultiplier);

protected:
    std::string generateSignal(const Tick& currentTick, Portfolio& userPortfolio, double riskPct) override;

private:
    int windowSize_;
    double stdDevMultiplier_;
    std::vector<double> priceHistory_;
};
