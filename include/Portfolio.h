#pragma once

// Encapsulates all trading-account state and enforces its invariants.
//
// Every field is private. External code — including every Strategy —
// may only observe or mutate this state through the methods below.
// buyAsset()/sellAsset() are not simple setters: they validate
// affordability/holdings and atomically update derived bookkeeping
// (win/loss counters, realized P&L) alongside the state change, so
// those invariants can never be bypassed by a caller writing to a
// field directly.
class Portfolio {
public:
    explicit Portfolio(double initialCash);

    // Returns false (no state change) if cash is insufficient.
    bool buyAsset(double currentPrice, int orderQuantity);

    // Returns false (no state change) if held quantity is insufficient.
    bool sellAsset(double currentPrice, int orderQuantity);

    double getTotalPortfolioValue(double currentPrice) const;
    double getAvailableCash() const;
    int getAssetQuantity() const;
    double getAverageEntryPrice() const;

    // Trade-outcome statistics, exposed read-only for diagnostics
    // (StatsUtils Hurst/Kelly calculations). Writes only ever happen
    // inside sellAsset(), never from outside the class.
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
