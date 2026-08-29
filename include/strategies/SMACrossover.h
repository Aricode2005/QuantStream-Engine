#pragma once
#include <vector>
#include "Strategy.h"
using namespace std;

class SMACrossover : public Strategy {
public:
    SMACrossover(int fastWindow, int slowWindow);

protected:
    string generateSignal(const Tick& currentTick, Portfolio& userPortfolio, double riskPct) override;

private:
    int fastWindow_;
    int slowWindow_;
    vector<double> priceHistory_;
};
