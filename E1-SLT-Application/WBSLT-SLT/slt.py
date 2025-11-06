import csv
import matplotlib.pyplot as plt

# Read and process the data from data.txt
def process_data(file_path):
    with open(file_path, 'r') as f:
        lines = [line.strip() for line in f.readlines() if line.strip()]

    data = {'64': {'generation_time': [], 'encryption_speed': []}, 
            '128': {'generation_time': [], 'encryption_speed': []}}
    round_numbers = list(range(6, 33))

    for i in range(0, len(lines), 2):
        generation_time = float(lines[i])
        encryption_speed = float(lines[i + 1])
        
        # Determine whether the entry is for n=64 or n=128
        if (i // 2) % 2 == 0:  # Even index corresponds to n=64
            data['64']['generation_time'].append(generation_time)
            data['64']['encryption_speed'].append(encryption_speed)
        else:  # Odd index corresponds to n=128
            data['128']['generation_time'].append(generation_time)
            data['128']['encryption_speed'].append(encryption_speed)

    return round_numbers, data

# Function to write processed data to CSV
def write_csv(data, output_file):
    with open(output_file, 'w', newline='') as f:
        writer = csv.writer(f)
        writer.writerow(['Round', 'Category', 'Generation Time', 'Encryption Speed'])
        
        for r in range(6, 33):
            writer.writerow([r, '64', data['64']['generation_time'][r-6], data['64']['encryption_speed'][r-6]])
            writer.writerow([r, '128', data['128']['generation_time'][r-6], data['128']['encryption_speed'][r-6]])

    # print(f"✅ CSV file written to {output_file}")

# Function to generate the 4 line diagrams
def generate_plots(round_numbers, data, shark_data_file, present_data_file):
     # Read the shark and present data from the files
    def read_data(file_path):
        with open(file_path, 'r') as f:
            lines = f.readlines()
            table_gen_time = list(map(float, lines[0].split()))  # Convert first line to float for table gen time
            encryption_speed = list(map(float, lines[1].split()))  # Convert second line to float for encryption speed
        return table_gen_time, encryption_speed
    
    # Read shark data (round 6)
    shark_table_gen_time, shark_encryption_speed = read_data(shark_data_file)
    # Read present data (round 30)
    present_table_gen_time, present_encryption_speed = read_data(present_data_file)

    # Define the figure size
    plt.figure(figsize=(12, 10))

    # Subplot for 64-bit Table Generation Time
    plt.subplot(2, 2, 1)
    plt.plot(round_numbers, data['64']['generation_time'], marker='o', label='Generation Time (64)', color='b')
    
    # Adding a single dot for shark data at round 6
    plt.scatter(6, shark_table_gen_time[0], color='black', zorder=5, label='SHARK')
    # Adding a single dot for present data at round 30
    plt.scatter(30, present_table_gen_time[0], color='purple', zorder=5, label='PRESENT')
    
    plt.xlabel('Round Number')
    plt.ylabel('Table Generation Time (s)')
    plt.title('64-bit Table Generation Time')
    plt.grid(True)
    plt.legend()

    # Subplot for 64-bit Encryption Speed
    plt.subplot(2, 2, 2)
    plt.plot(round_numbers, data['64']['encryption_speed'], marker='o', label='Encryption Speed (64)', color='g')
    
    # Adding a single dot for shark data at round 6
    plt.scatter(6, shark_encryption_speed[0], color='black', zorder=5, label='SHARK')
    # Adding a single dot for present data at round 30
    plt.scatter(30, present_encryption_speed[0], color='purple', zorder=5, label='PRESENT')

    plt.xlabel('Round Number')
    plt.ylabel('Encryption Speed (MB/s)')
    plt.title('64-bit Encryption Speed')
    plt.grid(True)
    plt.legend()

    # Subplot for 128-bit Table Generation Time
    plt.subplot(2, 2, 3)
    plt.plot(round_numbers, data['128']['generation_time'], marker='o', label='Generation Time (128)', color='r')
    
    plt.xlabel('Round Number')
    plt.ylabel('Table Generation Time (seconds)')
    plt.title('128-bit Table Generation Time')
    plt.grid(True)
    plt.legend()

    # Subplot for 128-bit Encryption Speed
    plt.subplot(2, 2, 4)
    plt.plot(round_numbers, data['128']['encryption_speed'], marker='o', label='Encryption Speed (128)', color='orange')
    plt.xlabel('Round Number')
    plt.ylabel('Encryption Speed (units)')
    plt.title('128-bit Encryption Speed')
    plt.grid(True)
    plt.legend()

    # Save the figure to a file
    plt.tight_layout()
    plt.savefig('../results/figure.png')
    # print("📊 Four-line diagram saved to comparison_plots.png")
    plt.close()

def main():
    input_file = '../results/data.txt'
    output_csv = '../results/data.csv'
    shark_data_file = '../results/shark-data.txt'
    present_data_file = '../results/present-data.txt'

    # Process the data
    round_numbers, data = process_data(input_file)

    # Write data to CSV
    write_csv(data, output_csv)

    # Generate plots
    generate_plots(round_numbers, data, shark_data_file, present_data_file)

if __name__ == "__main__":
    main()
