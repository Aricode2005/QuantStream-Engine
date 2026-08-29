#pragma once
#include <vector>
#include "Strategy.h"
using namespace std;

class BollingerBands : public Strategy {
public:
    BollingerBands(int windowSize, double stdDevMultiplier);

protected:
    string generateSignal(const Tick& currentTick, Portfolio& userPortfolio, double riskPct) override;

private:
    int windowSize_;
    double stdDevMultiplier_;
    vector<double> priceHistory_;
};
