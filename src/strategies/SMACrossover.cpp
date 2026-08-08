#include "strategies/SMACrossover.h"
#include "StrategyFactory.h"

SMACrossover::SMACrossover(int fastWindow, int slowWindow)
    : fastWindow_(fastWindow), slowWindow_(slowWindow) {}

std::string SMACrossover::generateSignal(const Tick& currentTick, Portfolio& userPortfolio, double riskPct) {
    priceHistory_.push_back(currentTick.close);
    int n = static_cast<int>(priceHistory_.size());
    if (n < slowWindow_) return "HOLD";

    double fastMA = 0.0, slowMA = 0.0;

    for (int i = n - fastWindow_; i < n; i++) fastMA += priceHistory_[i];
    fastMA /= fastWindow_;

    for (int i = n - slowWindow_; i < n; i++) slowMA += priceHistory_[i];
    slowMA /= slowWindow_;

    if (fastMA > slowMA && userPortfolio.getAvailableCash() >= currentTick.close) {
        int qty = computeOrderQuantity(userPortfolio, currentTick.close, riskPct);
        if (qty > 0 && userPortfolio.buyAsset(currentTick.close, qty)) {
            return "BUY " + std::to_string(qty);
        }
    } else if (fastMA < slowMA && userPortfolio.getAssetQuantity() > 0) {
        int qty = userPortfolio.getAssetQuantity();
        if (qty > 0 && userPortfolio.sellAsset(currentTick.close, qty)) {
            return "SELL " + std::to_string(qty);
        }
    }
    return "HOLD";
}

// This is also the fallback strategy used by StrategyFactory::create()
// whenever an unrecognized strat_type is supplied, matching the
// original server's default behavior.
REGISTER_STRATEGY("SMA", [](const nlohmann::json& p) -> std::unique_ptr<Strategy> {
    return std::make_unique<SMACrossover>(p.value("fast_sma", 10), p.value("slow_sma", 50));
});
