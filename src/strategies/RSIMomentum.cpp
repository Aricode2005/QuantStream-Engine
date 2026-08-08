#include "strategies/RSIMomentum.h"
#include "StrategyFactory.h"

RSIMomentum::RSIMomentum(int lookbackWindow) : lookbackWindow_(lookbackWindow) {}

std::string RSIMomentum::generateSignal(const Tick& currentTick, Portfolio& userPortfolio, double riskPct) {
    priceHistory_.push_back(currentTick.close);
    int n = static_cast<int>(priceHistory_.size());
    if (n <= lookbackWindow_) return "HOLD";

    double averageGain = 0.0, averageLoss = 0.0;

    for (int i = n - lookbackWindow_; i < n; i++) {
        double diff = priceHistory_[i] - priceHistory_[i - 1];
        if (diff > 0) {
            averageGain += diff;
        } else {
            averageLoss -= diff;
        }
    }

    averageGain /= lookbackWindow_;
    averageLoss /= lookbackWindow_;

    double rsi = 100.0;
    if (averageLoss != 0) {
        double relativeStrength = averageGain / averageLoss;
        rsi = 100.0 - (100.0 / (1.0 + relativeStrength));
    }

    if (rsi < 30.0 && userPortfolio.getAvailableCash() >= currentTick.close) {
        int qty = computeOrderQuantity(userPortfolio, currentTick.close, riskPct);
        if (qty > 0 && userPortfolio.buyAsset(currentTick.close, qty)) {
            return "BUY " + std::to_string(qty);
        }
    } else if (rsi > 70.0 && userPortfolio.getAssetQuantity() > 0) {
        int qty = userPortfolio.getAssetQuantity();
        if (qty > 0 && userPortfolio.sellAsset(currentTick.close, qty)) {
            return "SELL " + std::to_string(qty);
        }
    }
    return "HOLD";
}

REGISTER_STRATEGY("RSI", [](const nlohmann::json& p) -> std::unique_ptr<Strategy> {
    return std::make_unique<RSIMomentum>(p.value("fast_sma", 14));
});
