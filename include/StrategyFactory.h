#pragma once
#include <memory>
#include "Strategy.h"
#include "json.hpp"
#include "strategies/SMACrossover.h"
#include "strategies/EMACrossover.h"
#include "strategies/BollingerBands.h"
#include "strategies/RSIMomentum.h"
#include "strategies/ZScoreArbitrage.h"
using namespace std;

class StrategyCreator {
public:
    virtual unique_ptr<Strategy> createStrategy(const nlohmann::json& params) = 0;
    virtual ~StrategyCreator() = default;
};

class SMACreator : public StrategyCreator {
public:
    unique_ptr<Strategy> createStrategy(const nlohmann::json& p) override {
        return make_unique<SMACrossover>(p.value("fast_sma", 10), p.value("slow_sma", 50));
    }
};

class EMACreator : public StrategyCreator {
public:
    unique_ptr<Strategy> createStrategy(const nlohmann::json& p) override {
        return make_unique<EMACrossover>(p.value("fast_sma", 10), p.value("slow_sma", 50));
    }
};

class BollingerBandsCreator : public StrategyCreator {
public:
    unique_ptr<Strategy> createStrategy(const nlohmann::json& p) override {
        return make_unique<BollingerBands>(p.value("fast_sma", 20), 2.0);
    }
};

class RSICreator : public StrategyCreator {
public:
    unique_ptr<Strategy> createStrategy(const nlohmann::json& p) override {
        return make_unique<RSIMomentum>(p.value("fast_sma", 14));
    }
};

class ZScoreCreator : public StrategyCreator {
public:
    unique_ptr<Strategy> createStrategy(const nlohmann::json& p) override {
        return make_unique<ZScoreArbitrage>(p.value("fast_sma", 20), 2.0);
    }
};