#pragma once
#include <vector>
#include "Strategy.h"


class RSIMomentum : public Strategy {
public:
    explicit RSIMomentum(int lookbackWindow);

protected:
    std::string generateSignal(const Tick& currentTick, Portfolio& userPortfolio, double riskPct) override;

private:
    int lookbackWindow_;
    std::vector<double> priceHistory_;
};
