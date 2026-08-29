#pragma once
class Portfolio {
public:
    explicit Portfolio(double initialCash);

    bool buyAsset(double currentPrice, int orderQuantity);

    bool sellAsset(double currentPrice, int orderQuantity);

    double getTotalPortfolioValue(double currentPrice) const;
    double getAvailableCash() const;
    int getAssetQuantity() const;
    double getAverageEntryPrice() const;
    int getWinningTrades() const { return winningTrades_; }
    int getLosingTrades() const { return losingTrades_; }
    double getSumWinningPnL() const { return sumWinningPnL_; }
    double getSumLosingPnL() const { return sumLosingPnL_; }

private:
    double availableCash_;
    double averageEntryPrice_;
    int assetQuantity_;

    int winningTrades_ = 0;
    int losingTrades_ = 0;
    double sumWinningPnL_ = 0.0;
    double sumLosingPnL_ = 0.0;
};
