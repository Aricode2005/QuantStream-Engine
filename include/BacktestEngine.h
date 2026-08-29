#pragma once
#include <memory>
#include "Strategy.h"
#include "Portfolio.h"
#include "json.hpp"


class BacktestEngine {
public:
    BacktestEngine(std::unique_ptr<Strategy> strategy, double startingCash, double riskPct);

    nlohmann::json run(const nlohmann::json& marketData);

private:
    std::unique_ptr<Strategy> strategy_;
    Portfolio portfolio_;
    double riskPct_;
};
