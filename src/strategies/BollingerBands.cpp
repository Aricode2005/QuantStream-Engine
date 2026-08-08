#include "strategies/BollingerBands.h"
#include "StrategyFactory.h"
#include <cmath>

BollingerBands::BollingerBands(int windowSize, double stdDevMultiplier)
    : windowSize_(windowSize), stdDevMultiplier_(stdDevMultiplier) {}

std::string BollingerBands::generateSignal(const Tick& currentTick, Portfolio& userPortfolio, double riskPct) {
    priceHistory_.push_back(currentTick.close);
    int n = static_cast<int>(priceHistory_.size());
    if (n < windowSize_) return "HOLD";

    double movingAverage = 0.0;
    for (int i = n - windowSize_; i < n; i++) movingAverage += priceHistory_[i];
    movingAverage /= windowSize_;

    double variance = 0.0;
    for (int i = n - windowSize_; i < n; i++) {
        double diff = priceHistory_[i] - movingAverage;
        variance += diff * diff;
    }
    variance /= windowSize_;
    double stdDev = std::sqrt(variance);

    double upperBand = movingAverage + (stdDevMultiplier_ * stdDev);
    double lowerBand = movingAverage - (stdDevMultiplier_ * stdDev);

    if (currentTick.close < lowerBand && userPortfolio.getAvailableCash() >= currentTick.close) {
        int qty = computeOrderQuantity(userPortfolio, currentTick.close, riskPct);
        if (qty > 0 && userPortfolio.buyAsset(currentTick.close, qty)) {
            return "BUY " + std::to_string(qty);
        }
    } else if (currentTick.close > upperBand && userPortfolio.getAssetQuantity() > 0) {
        int qty = userPortfolio.getAssetQuantity();
        if (qty > 0 && userPortfolio.sellAsset(currentTick.close, qty)) {
            return "SELL " + std::to_string(qty);
        }
    }
    return "HOLD";
}

REGISTER_STRATEGY("BBAND", [](const nlohmann::json& p) -> std::unique_ptr<Strategy> {
    return std::make_unique<BollingerBands>(p.value("fast_sma", 20), 2.0);
});
