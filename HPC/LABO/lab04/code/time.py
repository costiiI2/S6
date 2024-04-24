import subprocess
import statistics
import time


# Function to run the command and capture time statistics
def run_command(command, iterations):
    times = []
    for _ in range(iterations):
        start_time = time.time()
        result = subprocess.run(command, capture_output=True, text=True, check=True)
        end_time = time.time()
        elapsed_time = end_time - start_time
        times.append(elapsed_time)
    return times


# Parameters
img_array = ['../img/nyc.png', '../img/half-life.png', '../img/medalion.png', '../img/sample_640_2.png']
output = 'h.png'
mode = '1'


cmd_list =['./edge_detection', './non_optimized_code','./lab01']

for img in img_array:
    print(f"Image: {img}")
  
    iterations = 50  # Number of times to run the command

    for cmd in cmd_list:
        time_result = run_command([cmd, img, output, mode], iterations)
        print(f"Command: {cmd}")
        
        # Calculate statistics
        mean_time = statistics.mean(time_result)
        median_time = statistics.median(time_result)
        std_deviation = statistics.stdev(time_result)

        # Print statistics
        print(f"Time statistics for the command: {cmd}")
        print(f"Mean time: {mean_time:.5f}")
        print(f"Median time: {median_time:.5f}")
        print(f"Standard deviation: {std_deviation:.5f}")


