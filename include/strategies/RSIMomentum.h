#pragma once
#include <vector>
#include "Strategy.h"

// Mean-reversion: BUY when RSI < 30 (oversold), SELL when RSI > 70
// (overbought). See project report Section 4.4.
class RSIMomentum : public Strategy {
public:
    explicit RSIMomentum(int lookbackWindow);

protected:
    std::string generateSignal(const Tick& currentTick, Portfolio& userPortfolio, double riskPct) override;

private:
    int lookbackWindow_;
    std::vector<double> priceHistory_;
};
