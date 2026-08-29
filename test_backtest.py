import requests
import json
import random
from datetime import datetime, timedelta

def generate_mock_data(num_points=100):
    data = []
    current_date = datetime(2023, 1, 1)
    current_price = 100.0

    for _ in range(num_points):
        # Random walk for price
        change = random.uniform(-2.0, 2.0)
        open_price = current_price
        high_price = open_price + abs(random.uniform(0, 2.0))
        low_price = open_price - abs(random.uniform(0, 2.0))
        close_price = current_price + change
        
        # Ensure high >= low, open, close
        high_price = max(high_price, open_price, close_price)
        low_price = min(low_price, open_price, close_price)

        data.append({
            "date": current_date.strftime("%Y-%m-%d"),
            "open": round(open_price, 2),
            "high": round(high_price, 2),
            "low": round(low_price, 2),
            "close": round(close_price, 2)
        })

        current_price = close_price
        current_date += timedelta(days=1)

    return data

def test_api():
    url = "http://localhost:10000/api/run_backtest"
    
    mock_data = generate_mock_data(100)
    
    payload = {
        "start_cash": 10000.0,
        "risk_pct": 0.1,
        "strat_type": "SMA",
        "market_data": mock_data
    }

    print("Sending payload to API...")
    
    try:
        response = requests.post(url, json=payload)
        print(f"Status Code: {response.status_code}")
        
        if response.status_code == 200:
            result = response.json()
            print("Backtest successful!")
            print(f"Final Value: ${result.get('final_value', 0):.2f}")
            print(f"Total Trades: {result.get('total_trades', 0)}")
            print(f"Hurst Exponent: {result.get('hurst_exponent', 0):.4f} - {result.get('hurst_msg', '')}")
            print(f"First 5 trades: {json.dumps(result.get('trades', [])[:5], indent=2)}")
        else:
            print("Error response:")
            print(response.text)
    except Exception as e:
        print(f"Failed to connect to the API: {e}")

if __name__ == "__main__":
    test_api()
