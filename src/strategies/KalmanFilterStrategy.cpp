#include "strategies/KalmanFilterStrategy.h"
#include "StrategyFactory.h"

KalmanFilterStrategy::KalmanFilterStrategy(double processNoise, double measurementNoise)
    : processNoise_(processNoise), measurementNoise_(measurementNoise) {}

std::string KalmanFilterStrategy::generateSignal(const Tick& currentTick, Portfolio& userPortfolio, double riskPct) {
    if (!isInitialized_) {
        estimatedPrice_ = currentTick.close;
        isInitialized_ = true;
        return "HOLD";
    }

    double predictedCovariance = errorCovariance_ + processNoise_;
    double kalmanGain = predictedCovariance / (predictedCovariance + measurementNoise_);
    estimatedPrice_ = estimatedPrice_ + kalmanGain * (currentTick.close - estimatedPrice_);
    errorCovariance_ = (1 - kalmanGain) * predictedCovariance;

    if (currentTick.close > estimatedPrice_ * 1.002 && userPortfolio.getAvailableCash() >= currentTick.close) {
        int qty = computeOrderQuantity(userPortfolio, currentTick.close, riskPct);
        if (qty > 0 && userPortfolio.buyAsset(currentTick.close, qty)) {
            return "BUY " + std::to_string(qty);
        }
    } else if (currentTick.close < estimatedPrice_ * 0.998 && userPortfolio.getAssetQuantity() > 0) {
        int qty = userPortfolio.getAssetQuantity();
        if (qty > 0 && userPortfolio.sellAsset(currentTick.close, qty)) {
            return "SELL " + std::to_string(qty);
        }
    }
    return "HOLD";
}

REGISTER_STRATEGY("KALMAN", [](const nlohmann::json&) -> std::unique_ptr<Strategy> {
    return std::make_unique<KalmanFilterStrategy>();
});
