#include "BacktestEngine.h"
#include "StatsUtils.h"
#include "Tick.h"
#include <stdexcept>
#include <vector>

BacktestEngine::BacktestEngine(std::unique_ptr<Strategy> strategy, double startingCash, double riskPct)
    : strategy_(std::move(strategy)), portfolio_(startingCash), riskPct_(riskPct) {}

nlohmann::json BacktestEngine::run(const nlohmann::json& marketData) {
    nlohmann::json result;
    result["equity_curve"] = nlohmann::json::array();
    result["trades"] = nlohmann::json::array();

    std::vector<double> historicalPrices;
    int totalExecutedTrades = 0;

    for (auto& dataPoint : marketData) {
        double closePrice = dataPoint.value("close", dataPoint.value("price", 0.0));
        double openPrice = dataPoint.value("open", closePrice);
        double highPrice = dataPoint.value("high", closePrice);
        double lowPrice = dataPoint.value("low", closePrice);

        Tick currentTick{dataPoint.at("date"), openPrice, highPrice, lowPrice, closePrice};
        historicalPrices.push_back(currentTick.close);

        std::string tradeAction = strategy_->evaluateTrade(currentTick, portfolio_, riskPct_);

        if (tradeAction != "HOLD") {
            double executedPrice = (tradeAction.find("STOP LOSS") != std::string::npos)
                ? (portfolio_.getAverageEntryPrice() * 0.95)
                : currentTick.close;

            result["trades"].push_back({
                {"date", currentTick.timestamp},
                {"action", tradeAction},
                {"price", executedPrice}
            });
            totalExecutedTrades++;
        }

        result["equity_curve"].push_back({
            {"date", currentTick.timestamp},
            {"val", portfolio_.getTotalPortfolioValue(currentTick.close)},
            {"price", currentTick.close}
        });
    }

    if (historicalPrices.empty()) {
        throw std::runtime_error("Market data array is empty.");
    }

    double hurstExponent = StatsUtils::calculateHurstExponent(historicalPrices);
    StatsUtils::HurstResult hurst = StatsUtils::interpretHurst(hurstExponent);

    double kellyFraction = StatsUtils::calculateKellyFraction(portfolio_);
    StatsUtils::KellyResult kelly = StatsUtils::interpretKelly(kellyFraction);

    result["final_value"] = portfolio_.getTotalPortfolioValue(historicalPrices.back());
    result["total_trades"] = totalExecutedTrades;
    result["hurst_exponent"] = hurst.exponent;
    result["kelly_fraction"] = kelly.fraction;
    result["hurst_msg"] = hurst.message;
    result["kelly_msg"] = kelly.message;

    return result;
}
