import csv
import json
import random
import requests
from datetime import datetime, timedelta

def generate_mock_csv(filename, num_points=100):
    current_date = datetime(2023, 1, 1)
    current_price = 100.0

    with open(filename, mode='w', newline='') as file:
        writer = csv.writer(file)
        writer.writerow(["date", "open", "high", "low", "close"])

        for _ in range(num_points):
            change = random.uniform(-2.0, 2.0)
            open_price = current_price
            high_price = open_price + abs(random.uniform(0, 2.0))
            low_price = open_price - abs(random.uniform(0, 2.0))
            close_price = current_price + change
            
            high_price = max(high_price, open_price, close_price)
            low_price = min(low_price, open_price, close_price)

            writer.writerow([
                current_date.strftime("%Y-%m-%d"),
                round(open_price, 2),
                round(high_price, 2),
                round(low_price, 2),
                round(close_price, 2)
            ])

            current_price = close_price
            current_date += timedelta(days=1)
    print(f"Generated {filename}")

def load_csv_and_test_api(filename, strategy="SMA"):
    url = "http://localhost:10000/api/run_backtest"
    
    # Read CSV and convert to JSON format expected by API
    market_data = []
    with open(filename, mode='r') as file:
        reader = csv.DictReader(file)
        for row in reader:
            market_data.append({
                "date": row["date"],
                "open": float(row["open"]),
                "high": float(row["high"]),
                "low": float(row["low"]),
                "close": float(row["close"])
            })
    
    payload = {
        "start_cash": 10000.0,
        "risk_pct": 0.1,
        "strat_type": strategy,
        "market_data": market_data
    }

    print(f"--- Testing Strategy: {strategy} ---")
    try:
        response = requests.post(url, json=payload)
        if response.status_code == 200:
            result = response.json()
            print(f"Final Value: ${result.get('final_value', 0):.2f}")
            print(f"Total Trades: {result.get('total_trades', 0)}")
            print(f"Hurst Exponent: {result.get('hurst_exponent', 0):.4f}")
            print("")
        else:
            print(f"Error response ({response.status_code}): {response.text}\n")
    except Exception as e:
        print(f"Failed to connect to the API: {e}\n")

if __name__ == "__main__":
    generate_mock_csv("mock_ohlc.csv")
    strategies = ["SMA", "EMA", "BBAND", "RSI", "ZSCORE"]
    for strat in strategies:
        load_csv_and_test_api("mock_ohlc.csv", strategy=strat)
