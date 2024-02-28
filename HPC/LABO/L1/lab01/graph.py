import matplotlib.pyplot as plt

# Data
images = ['medallion', 'half-life', 'nyc', 'small img']
methods = ['1d array', 'Linked list']
execution_times = {
    'medallion': {'1d array': 0.131, 'Linked list': 19.137},
    'half-life': {'1d array': 0.288, 'Linked list': 109.654},
    'nyc': {'1d array': 0.106, 'Linked list': 11.807},
    'small img': {'1d array': 0.035, 'Linked list': 0.573}
}
image_sizes = {
    'medallion': '1267x919',
    'half-life': '2000x2090',
    'nyc': '1150x710',
    'small img': '348x348'
}

# Sorting images based on sizes
sorted_images = sorted(images, key=lambda x: int(image_sizes[x].split('x')[0]))

# Plotting
x = range(len(sorted_images))

for method in methods:
    execution_data = [execution_times[img][method] for img in sorted_images]
    image_sizes_legend = [f'{img} ({image_sizes[img]})' for img in sorted_images]
    plt.plot(x, execution_data, label=method)

plt.xlabel('Images')
plt.ylabel('Execution Time (seconds)')
plt.title('Execution Time Comparison')
plt.xticks(x, image_sizes_legend)  # Set image names with sizes as x-axis ticks
plt.yscale('log')  # Set y-axis to log scale
plt.legend()
plt.grid(True)  # Add gridlines for better readability

plt.tight_layout()
plt.show()
