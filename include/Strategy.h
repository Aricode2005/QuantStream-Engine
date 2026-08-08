#pragma once
#include <string>
#include "Tick.h"
#include "Portfolio.h"

// Abstract base for every trading strategy.
//
// TEMPLATE METHOD PATTERN: evaluateTrade() is the fixed algorithm
// skeleton shared by all six strategies — check the shared stop-loss
// policy first, then hand off to the strategy-specific signal logic.
// It is deliberately NOT virtual, so subclasses cannot skip or
// reorder the stop-loss step. This is what removes the six
// byte-for-byte duplicated stop-loss blocks that existed in the
// original monolithic file: every concrete strategy now implements
// only generateSignal().
class Strategy {
public:
    virtual ~Strategy() = default;

    // Fixed algorithm skeleton. Do not override.
    std::string evaluateTrade(const Tick& currentTick, Portfolio& userPortfolio, double riskPct);

protected:
    // Shared risk-control policy, applied identically ahead of every
    // strategy's own signal. Declared virtual (not pure) so a future
    // strategy could opt into a different stop-loss policy without
    // touching this base class — while every current strategy gets
    // the same 5% intraday stop for free.
    virtual std::string checkStopLoss(const Tick& currentTick, Portfolio& userPortfolio) const;

    // Shared position-sizing helper available to every subclass:
    // quantity = floor((availableCash * riskPct) / price).
    int computeOrderQuantity(const Portfolio& userPortfolio, double price, double riskPct) const;

    // Strategy-specific indicator logic — the only method each
    // concrete strategy needs to implement.
    virtual std::string generateSignal(const Tick& currentTick, Portfolio& userPortfolio, double riskPct) = 0;

    static constexpr double kStopLossPct = 0.05;
};
