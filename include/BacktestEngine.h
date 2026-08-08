#pragma once
#include <memory>
#include "Strategy.h"
#include "Portfolio.h"
#include "json.hpp"

// Owns one backtest run end-to-end: replays market data through a
// Strategy against a fresh Portfolio and assembles the JSON result,
// including the post-run Hurst/Kelly diagnostics.
//
// Deliberately has zero knowledge of HTTP or httplib — main.cpp is the
// only file in the project that talks to the transport layer, so this
// class can be constructed and run directly in a unit test without
// starting a server.
class BacktestEngine {
public:
    BacktestEngine(std::unique_ptr<Strategy> strategy, double startingCash, double riskPct);

    // Throws std::runtime_error if marketData is empty.
    nlohmann::json run(const nlohmann::json& marketData);

private:
    std::unique_ptr<Strategy> strategy_;
    Portfolio portfolio_;
    double riskPct_;
};
