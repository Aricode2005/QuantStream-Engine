#include "Portfolio.h"
#include <cmath>

Portfolio::Portfolio(double initialCash)
    : availableCash_(initialCash), averageEntryPrice_(0.0), assetQuantity_(0) {}

bool Portfolio::buyAsset(double currentPrice, int orderQuantity) {
    if (availableCash_ >= currentPrice * orderQuantity) {
        averageEntryPrice_ = currentPrice;
        availableCash_ -= currentPrice * orderQuantity;
        assetQuantity_ += orderQuantity;
        return true;
    }
    return false;
}

bool Portfolio::sellAsset(double currentPrice, int orderQuantity) {
    if (assetQuantity_ >= orderQuantity) {
        double realizedPnL = (currentPrice - averageEntryPrice_) * orderQuantity;

        if (realizedPnL > 0) {
            winningTrades_++;
            sumWinningPnL_ += realizedPnL;
        } else {
            losingTrades_++;
            sumLosingPnL_ += std::abs(realizedPnL);
        }

        availableCash_ += currentPrice * orderQuantity;
        assetQuantity_ -= orderQuantity;
        return true;
    }
    return false;
}

double Portfolio::getTotalPortfolioValue(double currentPrice) const {
    return availableCash_ + (assetQuantity_ * currentPrice);
}

double Portfolio::getAvailableCash() const { return availableCash_; }
int Portfolio::getAssetQuantity() const { return assetQuantity_; }
double Portfolio::getAverageEntryPrice() const { return averageEntryPrice_; }
