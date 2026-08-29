#include "strategies/ZScoreArbitrage.h"
#include "StrategyFactory.h"
#include <cmath>

ZScoreArbitrage::ZScoreArbitrage(int lookbackWindow, double zScoreThreshold)
    : lookbackWindow_(lookbackWindow), zScoreThreshold_(zScoreThreshold) {}

std::string ZScoreArbitrage::generateSignal(const Tick& currentTick, Portfolio& userPortfolio, double riskPct) {
    priceHistory_.push_back(currentTick.close);
    int n = static_cast<int>(priceHistory_.size());
    if (n < lookbackWindow_) return "HOLD";

    double rollingMean = 0.0;
    for (int i = n - lookbackWindow_; i < n; i++) rollingMean += priceHistory_[i];
    rollingMean /= lookbackWindow_;

    double rollingVariance = 0.0;
    for (int i = n - lookbackWindow_; i < n; i++) {
        rollingVariance += std::pow(priceHistory_[i] - rollingMean, 2);
    }
    double stdDev = std::sqrt(rollingVariance / lookbackWindow_);

    if (stdDev == 0) return "HOLD";

    double zScore = (currentTick.close - rollingMean) / stdDev;

    if (zScore < -zScoreThreshold_ && userPortfolio.getAvailableCash() >= currentTick.close) {
        int qty = computeOrderQuantity(userPortfolio, currentTick.close, riskPct);
        if (qty > 0 && userPortfolio.buyAsset(currentTick.close, qty)) {
            return "BUY " + std::to_string(qty);
        }
    } else if (zScore > zScoreThreshold_ && userPortfolio.getAssetQuantity() > 0) {
        int qty = userPortfolio.getAssetQuantity();
        if (qty > 0 && userPortfolio.sellAsset(currentTick.close, qty)) {
            return "SELL " + std::to_string(qty);
        }
    }
    return "HOLD";
}

