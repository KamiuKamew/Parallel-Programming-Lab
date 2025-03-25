#!/usr/bin/env python3
import pandas as pd
import matplotlib.pyplot as plt
import seaborn as sns
import os
import sys
import numpy as np

# No need for Chinese font support anymore
# We'll use standard English labels and titles


def plot_raw_data(csv_file, output_dir):
    """Plot distribution graphs of raw data"""
    if not os.path.exists(csv_file):
        print(f"File not found: {csv_file}")
        return

    df = pd.read_csv(csv_file)

    # 1. Box plot
    plt.figure(figsize=(10, 6))
    sns.boxplot(x="Algorithm", y="Duration(us)", data=df)
    plt.title("Performance Distribution Box Plot")
    plt.xticks(rotation=45)
    plt.tight_layout()
    plt.savefig(os.path.join(output_dir, "boxplot.png"))

    # 2. Violin plot
    plt.figure(figsize=(10, 6))
    sns.violinplot(x="Algorithm", y="Duration(us)", data=df)
    plt.title("Performance Distribution Violin Plot")
    plt.xticks(rotation=45)
    plt.tight_layout()
    plt.savefig(os.path.join(output_dir, "violinplot.png"))

    # 3. Time series plot
    plt.figure(figsize=(12, 6))
    for algo in df["Algorithm"].unique():
        algo_data = df[df["Algorithm"] == algo]
        plt.plot(algo_data["TestNumber"], algo_data["Duration(us)"], label=algo)

    plt.title("Execution Time Series Plot")
    plt.xlabel("Test Number")
    plt.ylabel("Execution Time (us)")
    plt.legend()
    plt.grid(True)
    plt.tight_layout()
    plt.savefig(os.path.join(output_dir, "time_series.png"))

    # 4. Kernel density estimation plot
    plt.figure(figsize=(10, 6))
    for algo in df["Algorithm"].unique():
        algo_data = df[df["Algorithm"] == algo]
        sns.kdeplot(algo_data["Duration(us)"], label=algo)

    plt.title("Execution Time Density Estimation")
    plt.xlabel("Execution Time (us)")
    plt.ylabel("Density")
    plt.legend()
    plt.grid(True)
    plt.tight_layout()
    plt.savefig(os.path.join(output_dir, "density.png"))


def plot_statistics(csv_file, output_dir):
    """Plot bar charts of statistical summary data"""
    if not os.path.exists(csv_file):
        print(f"File not found: {csv_file}")
        return

    df = pd.read_csv(csv_file)

    # 1. Mean bar chart
    plt.figure(figsize=(10, 6))
    bars = plt.bar(df["Algorithm"], df["Mean(us)"])
    plt.errorbar(
        df["Algorithm"],
        df["Mean(us)"],
        yerr=df["StdDev(us)"],
        fmt="none",
        ecolor="black",
        capsize=5,
    )

    # Show values on bars
    for bar in bars:
        height = bar.get_height()
        plt.text(
            bar.get_x() + bar.get_width() / 2.0,
            height + 0.1,
            f"{height:.2f}",
            ha="center",
            va="bottom",
            rotation=0,
        )

    plt.title("Average Execution Time (with Standard Deviation)")
    plt.ylabel("Execution Time (us)")
    plt.xticks(rotation=45)
    plt.grid(axis="y")
    plt.tight_layout()
    plt.savefig(os.path.join(output_dir, "mean_bar.png"))

    # 2. Min, median, max bar chart
    plt.figure(figsize=(12, 6))

    x = np.arange(len(df["Algorithm"]))
    width = 0.25

    # Convert Series to numpy arrays
    min_values = df["Min(us)"].values.astype(float)
    median_values = df["Median(us)"].values.astype(float)
    max_values = df["Max(us)"].values.astype(float)

    plt.bar(x - width, min_values, width, label="Min")
    plt.bar(x, median_values, width, label="Median")
    plt.bar(x + width, max_values, width, label="Max")

    plt.title("Performance Statistics (Min/Median/Max)")
    plt.ylabel("Execution Time (us)")
    plt.xticks(x, df["Algorithm"].tolist(), rotation=45)
    plt.legend()
    plt.grid(axis="y")
    plt.tight_layout()
    plt.savefig(os.path.join(output_dir, "min_median_max.png"))

    # 3. Radar chart for performance comparison
    if len(df) > 2:  # Need at least 3 algorithms
        plt.figure(figsize=(8, 8))

        # Prepare radar chart data
        categories = ["Mean Time", "Std Dev", "Min Time", "Max Time"]

        # Normalize data (lower is better)
        norm_mean = df["Mean(us)"].values.astype(float) / df["Mean(us)"].max()
        norm_stddev = df["StdDev(us)"].values.astype(float) / df["StdDev(us)"].max()
        norm_min = df["Min(us)"].values.astype(float) / df["Min(us)"].max()
        norm_max = df["Max(us)"].values.astype(float) / df["Max(us)"].max()

        # Set angles
        N = len(categories)
        angles = [n / float(N) * 2 * np.pi for n in range(N)]
        angles += angles[:1]  # Close the loop

        # Initialize radar chart
        ax = plt.subplot(111, polar=True)

        # Plot each algorithm
        for i, algo in enumerate(df["Algorithm"]):
            values = [norm_mean[i], norm_stddev[i], norm_min[i], norm_max[i]]
            values += values[:1]  # Close the loop

            plt.plot(angles, values, linewidth=2, label=algo)
            plt.fill(angles, values, alpha=0.25)

        # Set radar chart properties
        plt.xticks(angles[:-1], categories)
        plt.yticks(
            [0.2, 0.4, 0.6, 0.8], ["0.2", "0.4", "0.6", "0.8"], color="grey", size=8
        )
        plt.title("Performance Comparison Radar Chart (Lower is Better)")
        plt.legend(loc="upper right")
        plt.tight_layout()
        plt.savefig(os.path.join(output_dir, "radar.png"))


def main():
    # Create visualization directories
    array_sum_dir = "results/array_sum/viz"
    inner_product_dir = "results/inner_product/viz"

    os.makedirs(array_sum_dir, exist_ok=True)
    os.makedirs(inner_product_dir, exist_ok=True)

    # Generate array sum visualizations
    plot_raw_data("results/array_sum/raw_data.csv", array_sum_dir)
    plot_statistics("results/array_sum/statistics.csv", array_sum_dir)

    # Generate matrix inner product visualizations
    plot_raw_data("results/inner_product/raw_data.csv", inner_product_dir)
    plot_statistics("results/inner_product/statistics.csv", inner_product_dir)

    print("Visualization results generated!")
    print(f"Array sum charts: {array_sum_dir}")
    print(f"Inner product charts: {inner_product_dir}")


if __name__ == "__main__":
    main()
