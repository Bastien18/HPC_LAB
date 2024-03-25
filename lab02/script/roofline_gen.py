import matplotlib.pyplot as plt

def f(x, maxperf, maxband):
    if x * maxband < maxperf:
        return x * maxband
    else:
        return maxperf

with open('input.txt', 'r') as file:
    lines = file.readlines()
    maxperf = float(lines[0])
    maxband = float(lines[1])
    points = []
    if len(lines) > 2:
        for line in lines[2:]:
            line_values = line.strip().split()
            points.append((float(line_values[0]), float(line_values[1]), line_values[2]))

x_values = [x/10 for x in range(0, 100)]
y_values = [f(x, maxperf, maxband) for x in x_values]

if points != []:
    colors = ['red', 'green', 'blue', 'cyan', 'magenta', 'yellow', 'black']
    for i, point in enumerate(points):
        label = f'{point[2]}'
        plt.plot(point[0], point[1], color=colors[i % len(colors)], marker='o')
        plt.annotate(label, (point[0], point[1]), (point[0] + 0.1, point[1] + 0.1))

plt.plot(x_values, y_values)
plt.xlabel('Operational Intensity')
plt.ylabel('Performance [MFLOPS/s]')
plt.title(f'Roofline model')
plt.show()

