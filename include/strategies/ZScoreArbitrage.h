#pragma once
#include <vector>
#include "Strategy.h"

// Mean-reversion: BUY when the price's rolling Z-score drops below
// -threshold, SELL when it rises above +threshold. See project report
// Section 4.5.
class ZScoreArbitrage : public Strategy {
public:
    ZScoreArbitrage(int lookbackWindow, double zScoreThreshold);

protected:
    std::string generateSignal(const Tick& currentTick, Portfolio& userPortfolio, double riskPct) override;

private:
    int lookbackWindow_;
    double zScoreThreshold_;
    std::vector<double> priceHistory_;
};
