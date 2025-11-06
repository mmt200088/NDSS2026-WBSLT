import csv
import matplotlib.pyplot as plt

# Define category mapping
CATEGORY_MAP = {
    1: "Ours (optimized)",
    2: "Ours (general)",
    3: "Chow et al.'s",
    4: "Xiao et al.'s",
    5: "Lee et al.'s (Table)",
    6: "Lee et al.'s (Mask)"
}

def process_data(file_path):
    """Reads 2n lines from data.txt and pairs generation time with encryption speed."""
    with open(file_path, 'r') as f:
        lines = [line.strip() for line in f.readlines() if line.strip()]
    
    data = []
    for i in range(0, len(lines), 2):
        gen_time = float(lines[i])
        enc_speed = float(lines[i + 1])
        category_num = (i // 2) + 1
        category_name = CATEGORY_MAP.get(category_num, f"Category {category_num}")
        data.append([category_num, category_name, gen_time, enc_speed])
    
    return data

def write_csv(data, output_file):
    """Writes the processed data to a CSV file with headers."""
    with open(output_file, 'w', newline='') as f:
        writer = csv.writer(f)
        writer.writerow(['Index', 'Category', 'Generation Time', 'Encryption Speed'])
        for row in data:
            writer.writerow(row)
    # print(f"✅ CSV file written to {output_file}")

def generate_generation_time_histogram(data, output_image_file):
    """Generates and saves histogram for Table Generation Time."""
    categories = [row[1] for row in data]
    generation_times = [row[2] for row in data]
    
    plt.figure(figsize=(10, 6))
    plt.bar(categories, generation_times, color='royalblue', alpha=0.8)
    
    plt.xlabel('Category')
    plt.ylabel('Generation Time (s)')
    plt.title('Table Generation Time Comparison')
    plt.xticks(rotation=25, ha='right')
    plt.tight_layout()
    plt.savefig(output_image_file)
    # print(f"📊 Generation Time Histogram saved to {output_image_file}")
    plt.close()

def generate_encryption_speed_histogram(data, output_image_file):
    """Generates and saves histogram for Encryption Speed."""
    categories = [row[1] for row in data]
    encryption_speeds = [row[3] for row in data]
    
    plt.figure(figsize=(10, 6))
    plt.bar(categories, encryption_speeds, color='seagreen', alpha=0.8)
    
    plt.xlabel('Category')
    plt.ylabel('Encryption Speed (MB/s)')
    plt.title('Encryption Speed Comparison')
    plt.xticks(rotation=25, ha='right')
    plt.tight_layout()
    plt.savefig(output_image_file)
    # print(f"📊 Encryption Speed Histogram saved to {output_image_file}")
    plt.close()

def generate_plots(data, output_image_file):
    # Prepare data for both plots
    categories = [row[1] for row in data]
    generation_times = [row[2] for row in data]
    encryption_speeds = [row[3] for row in data]
    
    # Create a figure with two subplots (1 row, 2 columns)
    fig, ax = plt.subplots(1, 2, figsize=(15, 6))
    
    # Generation Time Histogram (First Subplot)
    ax[0].bar(categories, generation_times, color='royalblue', alpha=0.8)
    ax[0].set_xlabel('Category')
    ax[0].set_ylabel('Generation Time (seconds)')
    ax[0].set_title('Table Generation Time Comparison')
    ax[0].tick_params(axis='x', rotation=25)
    
    # Encryption Speed Histogram (Second Subplot)
    ax[1].bar(categories, encryption_speeds, color='seagreen', alpha=0.8)
    ax[1].set_xlabel('Category')
    ax[1].set_ylabel('Encryption Speed (units)')
    ax[1].set_title('Encryption Speed Comparison')
    ax[1].tick_params(axis='x', rotation=25)
    
    # Adjust layout for better spacing
    plt.tight_layout()
    
    # Save the combined figure
    plt.savefig(output_image_file)
    # print(f"📊 Combined Histograms saved to {output_image_file}")
    plt.close()


def main():
    input_file = './results/data.txt'
    output_csv = './results/data.csv'
    gen_fig = './results/figure.png'
    # enc_speed_fig = './encryption_speed_histogram.png'

    data = process_data(input_file)
    write_csv(data, output_csv)

    # generate_generation_time_histogram(data, gen_time_fig)
    # generate_encryption_speed_histogram(data, enc_speed_fig)
    generate_plots(data, gen_fig)

if __name__ == "__main__":
    main()
