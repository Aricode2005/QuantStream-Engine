#include "StatsUtils.h"
#include <cmath>

namespace StatsUtils {

double calculateHurstExponent(const std::vector<double>& priceData) {
    int n = static_cast<int>(priceData.size());
    if (n < 10) return 0.5;

    double meanLag1 = 0, varianceLag1 = 0;
    double meanLag2 = 0, varianceLag2 = 0;

    for (int i = 1; i < n; i++) meanLag1 += (priceData[i] - priceData[i - 1]);
    meanLag1 /= (n - 1);
    for (int i = 1; i < n; i++) varianceLag1 += std::pow((priceData[i] - priceData[i - 1]) - meanLag1, 2);
    varianceLag1 /= (n - 1);

    for (int i = 2; i < n; i++) meanLag2 += (priceData[i] - priceData[i - 2]);
    meanLag2 /= (n - 2);
    for (int i = 2; i < n; i++) varianceLag2 += std::pow((priceData[i] - priceData[i - 2]) - meanLag2, 2);
    varianceLag2 /= (n - 2);

    if (varianceLag1 == 0) return 0.5;
    return 0.5 * std::log2(varianceLag2 / varianceLag1);
}

HurstResult interpretHurst(double hurstExponent) {
    if (hurstExponent > 0.55) {
        return {hurstExponent, "Market is Trending. Trend-following strategies (SMA/EMA) are optimal."};
    } else if (hurstExponent < 0.45) {
        return {hurstExponent, "Market is Ranging. Mean-reversion strategies (RSI/Bollinger/Z-Score) are optimal."};
    }
    return {hurstExponent, "Market is Random. No statistical edge based on market regime. Trading is highly risky."};
}

double calculateKellyFraction(const Portfolio& portfolio) {
    int totalClosedTrades = portfolio.getWinningTrades() + portfolio.getLosingTrades();
    if (totalClosedTrades == 0) return 0.0;

    double winRateProbability = static_cast<double>(portfolio.getWinningTrades()) / totalClosedTrades;
    double averageWin = portfolio.getWinningTrades() > 0
        ? portfolio.getSumWinningPnL() / portfolio.getWinningTrades()
        : 0.0;
    double averageLoss = portfolio.getLosingTrades() > 0
        ? portfolio.getSumLosingPnL() / portfolio.getLosingTrades()
        : 1.0;
    double winLossRatio = (averageLoss > 0) ? (averageWin / averageLoss) : 1.0;

    return winRateProbability - ((1.0 - winRateProbability) / winLossRatio);
}

KellyResult interpretKelly(double kellyFraction) {
    if (kellyFraction > 0) {
        double halfKellyPct = std::round((kellyFraction / 2.0) * 10000.0) / 100.0;
        return {kellyFraction,
                "Positive Edge. Suggested max risk per trade (Half-Kelly): " + std::to_string(halfKellyPct) + "%"};
    }
    return {kellyFraction, "Negative Edge. The math strongly advises against deploying this strategy."};
}

}  // namespace StatsUtils
