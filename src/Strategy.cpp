#include "Strategy.h"

std::string Strategy::evaluateTrade(const Tick& currentTick, Portfolio& userPortfolio, double riskPct) {
    std::string stopLossAction = checkStopLoss(currentTick, userPortfolio);
    if (!stopLossAction.empty()) return stopLossAction;
    return generateSignal(currentTick, userPortfolio, riskPct);
}

std::string Strategy::checkStopLoss(const Tick& currentTick, Portfolio& userPortfolio) const {
    if (userPortfolio.getAssetQuantity() > 0) {
        double stopLossPrice = userPortfolio.getAverageEntryPrice() * (1.0 - kStopLossPct);
        if (currentTick.low <= stopLossPrice) {
            int orderQty = userPortfolio.getAssetQuantity();
            if (userPortfolio.sellAsset(stopLossPrice, orderQty)) {
                return "SELL " + std::to_string(orderQty) + " (STOP LOSS)";
            }
        }
    }
    return "";
}

int Strategy::computeOrderQuantity(const Portfolio& userPortfolio, double price, double riskPct) const {
    return static_cast<int>((userPortfolio.getAvailableCash() * riskPct) / price);
}
