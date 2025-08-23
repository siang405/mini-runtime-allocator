import pandas as pd
import matplotlib.pyplot as plt

strategies = ["first", "best", "worst", "buddy"]
colors = {"first": "blue", "best": "green", "worst": "orange", "buddy": "red"}

plt.figure(figsize=(10, 6))

for strat in strategies:
    filename = f"benchmark_{strat}.csv"
    try:
        df = pd.read_csv(filename)
        plt.plot(df["step"], df["fragmentation_ratio"], label=strat.capitalize(), color=colors[strat])

        avg_frag = df["fragmentation_ratio"].mean()
        print(f"{strat.capitalize():6s} average fragmentation ratio: {avg_frag:.3f}")

    except FileNotFoundError:
        print(f"[Warning] {filename} not found, skipping.")

plt.xlabel("Operations")
plt.ylabel("Fragmentation Ratio")
plt.title("Fragmentation Ratio Comparison of Allocation Strategies")
plt.legend()
plt.grid(True)
plt.tight_layout()

plt.savefig("benchmark_comparison.png")
print("[Saved] benchmark_comparison.png")
