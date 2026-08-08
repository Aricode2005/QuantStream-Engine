#include "strategies/EMACrossover.h"
#include "StrategyFactory.h"

EMACrossover::EMACrossover(int fastWindow, int slowWindow)
    : fastWindow_(fastWindow), slowWindow_(slowWindow) {}

std::string EMACrossover::generateSignal(const Tick& currentTick, Portfolio& userPortfolio, double riskPct) {
    ticks_++;
    double fastAlpha = 2.0 / (fastWindow_ + 1);
    double slowAlpha = 2.0 / (slowWindow_ + 1);

    if (ticks_ == 1) {
        fastEMA_ = currentTick.close;
        slowEMA_ = currentTick.close;
        return "HOLD";
    }

    fastEMA_ = (currentTick.close - fastEMA_) * fastAlpha + fastEMA_;
    slowEMA_ = (currentTick.close - slowEMA_) * slowAlpha + slowEMA_;

    if (ticks_ < slowWindow_) return "HOLD";

    if (fastEMA_ > slowEMA_ && userPortfolio.getAvailableCash() >= currentTick.close) {
        int qty = computeOrderQuantity(userPortfolio, currentTick.close, riskPct);
        if (qty > 0 && userPortfolio.buyAsset(currentTick.close, qty)) {
            return "BUY " + std::to_string(qty);
        }
    } else if (fastEMA_ < slowEMA_ && userPortfolio.getAssetQuantity() > 0) {
        int qty = userPortfolio.getAssetQuantity();
        if (qty > 0 && userPortfolio.sellAsset(currentTick.close, qty)) {
            return "SELL " + std::to_string(qty);
        }
    }
    return "HOLD";
}

REGISTER_STRATEGY("EMA", [](const nlohmann::json& p) -> std::unique_ptr<Strategy> {
    return std::make_unique<EMACrossover>(p.value("fast_sma", 10), p.value("slow_sma", 50));
});
