#pragma once
#include <vector>
#include "Strategy.h"


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
