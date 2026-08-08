# QuantStream: Algorithmic Trading Backtester

A C++ REST server for backtesting algorithmic trading strategies against historical OHLC (open/high/low/close) market data. Send it a price series and a strategy selection; it replays the data tick-by-tick against a simulated portfolio and returns an equity curve, a full trade log, and post-run diagnostics on market regime and position sizing.

## Features

- **Six built-in trading strategies**, spanning both trend-following and mean-reversion styles:
  - SMA Crossover
  - EMA Crossover
  - Bollinger Bands
  - RSI Momentum
  - Z-Score Statistical Arbitrage
  - Kalman Filter
- **Shared risk controls** applied to every strategy: a 5% intraday stop-loss and configurable risk-percentage position sizing.
- **Post-run diagnostics**:
  - **Hurst exponent** — estimates whether the market was trending, mean-reverting, or random, and suggests a matching strategy family.
  - **Kelly criterion** — computes the statistically edge-maximizing risk-per-trade from the realized win rate and payoff ratio.
- **Pluggable architecture** — new strategies can be added without modifying any existing file (see [Adding a New Strategy](#adding-a-new-strategy)).
- Single REST endpoint, JSON in / JSON out.

## Requirements

- A C++17 compiler (g++ 9+ or clang++ 10+)
- CMake 3.15+
- [nlohmann/json](https://github.com/nlohmann/json) — single-header, included in `third_party/`
- [cpp-httplib](https://github.com/yhirose/cpp-httplib) — single-header, included in `third_party/`
- POSIX threads (`pthread`)

## Build

```bash
mkdir build && cd build
cmake ..
cmake --build .
```

This produces a `backtester` executable in the `build/` directory.

## Run

```bash
./backtester
```

The server listens on `0.0.0.0:$PORT`, defaulting to port `10000` if the `PORT` environment variable is unset:

```bash
PORT=8080 ./backtester
```

## API

### `POST /api/run_backtest`

Runs a full backtest and returns the resulting equity curve, trade log, and diagnostics.

**Request body**

| Field | Type | Required | Default | Description |
|---|---|---|---|---|
| `start_cash` | number | yes | — | Starting portfolio cash |
| `strat_type` | string | no | `"SMA"` | One of `SMA`, `EMA`, `BBAND`, `RSI`, `ZSCORE`, `KALMAN`. Unrecognized values fall back to `SMA`. |
| `risk_pct` | number | no | `0.10` | Fraction of available cash risked per buy order |
| `fast_sma` | number | no | strategy-dependent | Fast window / lookback window; meaning depends on `strat_type` |
| `slow_sma` | number | no | strategy-dependent | Slow window (used by `SMA` and `EMA` only) |
| `market_data` | array | yes | — | Array of OHLC bars, oldest first (see below) |

Each entry in `market_data`:

```json
{
  "date": "2024-01-01",
  "open": 100.0,
  "high": 102.0,
  "low": 99.0,
  "close": 101.0
}
```

`open`, `high`, and `low` are optional and default to `close` if omitted (a `price` field is also accepted as an alias for `close`).

**Example request**

```bash
curl -X POST http://localhost:10000/api/run_backtest \
  -H "Content-Type: application/json" \
  -d '{
        "start_cash": 10000,
        "strat_type": "SMA",
        "risk_pct": 0.2,
        "fast_sma": 10,
        "slow_sma": 50,
        "market_data": [
          {"date": "2024-01-01", "open": 100, "high": 101, "low": 99, "close": 100},
          {"date": "2024-01-02", "open": 100, "high": 102, "low": 99, "close": 101}
        ]
      }'
```

**Response body**

| Field | Type | Description |
|---|---|---|
| `equity_curve` | array | `{date, val, price}` per tick — mark-to-market portfolio value over time |
| `trades` | array | `{date, action, price}` per executed trade — `action` is e.g. `"BUY 12"`, `"SELL 12"`, or `"SELL 12 (STOP LOSS)"` |
| `total_trades` | number | Count of non-HOLD ticks |
| `final_value` | number | Portfolio value at the last tick |
| `hurst_exponent` | number | Estimated Hurst exponent for the price series |
| `hurst_msg` | string | Human-readable regime interpretation |
| `kelly_fraction` | number | Kelly-optimal fraction of capital to risk per trade |
| `kelly_msg` | string | Human-readable sizing recommendation |

Errors are returned as plain text with a `400` (malformed JSON) or `500` (server error) status.

### `GET /`

Serves `index.html` from the working directory, if present.

## Project Structure

```
trading_backtester/
├── CMakeLists.txt
├── include/
│   ├── Tick.h                   # OHLC bar data
│   ├── Portfolio.h              # account state (cash, position, PnL)
│   ├── Strategy.h                # abstract strategy interface
│   ├── StrategyFactory.h        # strategy construction by name
│   ├── StatsUtils.h             # Hurst exponent / Kelly criterion
│   ├── BacktestEngine.h         # runs one backtest end-to-end
│   └── strategies/
│       ├── SMACrossover.h
│       ├── EMACrossover.h
│       ├── BollingerBands.h
│       ├── RSIMomentum.h
│       ├── ZScoreArbitrage.h
│       └── KalmanFilterStrategy.h
├── src/
│   ├── Portfolio.cpp
│   ├── Strategy.cpp
│   ├── StrategyFactory.cpp
│   ├── StatsUtils.cpp
│   ├── BacktestEngine.cpp
│   ├── main.cpp                 # HTTP server and request handling
│   └── strategies/               # one .cpp per strategy
└── third_party/
    ├── httplib.h
    └── json.hpp
```

## Adding a New Strategy

1. Create `include/strategies/MyStrategy.h` and `src/strategies/MyStrategy.cpp`. Subclass `Strategy` and implement `generateSignal(const Tick&, Portfolio&, double riskPct)` — this is the only method a new strategy needs to write; the stop-loss check and position-sizing helper are inherited.
2. At the bottom of `MyStrategy.cpp`, register the strategy under a name clients can pass as `strat_type`:
   ```cpp
   REGISTER_STRATEGY("MYSTRAT", [](const nlohmann::json& p) -> std::unique_ptr<Strategy> {
       return std::make_unique<MyStrategy>(p.value("some_param", 42));
   });
   ```
3. Rebuild. CMake picks up the new `.cpp` file automatically and the strategy becomes available immediately as `"strat_type": "MYSTRAT"` — no other file needs to change.

## Strategy Reference

| `strat_type` | Style | Signal |
|---|---|---|
| `SMA` (default) | Trend-following | BUY when fast SMA > slow SMA, SELL when it crosses below |
| `EMA` | Trend-following | Same as SMA, using exponential moving averages |
| `BBAND` | Mean-reversion | BUY below the lower Bollinger Band, SELL above the upper band |
| `RSI` | Mean-reversion | BUY when RSI < 30, SELL when RSI > 70 |
| `ZSCORE` | Mean-reversion | BUY when rolling Z-score < −threshold, SELL when > +threshold |
| `KALMAN` | Estimate-divergence | BUY/SELL when price diverges 0.2% from a Kalman-filtered price estimate |

Full mathematical formulations for each strategy and diagnostic are covered in the accompanying architecture report.

