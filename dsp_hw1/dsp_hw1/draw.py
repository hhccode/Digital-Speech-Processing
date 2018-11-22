import matplotlib.pyplot as plt

file = open("acc_all.txt", "r")

accuracy = []
count = 0

for i in file:
    accuracy.append(float(i.split()[3]))
    count += 1

x = [i for i in range(1, 1001)]

maximum = max(accuracy)
maximum_index = accuracy.index(maximum)


plt.plot(x, accuracy)
plt.title('Maximum accuracy: ' + str(maximum) + ' when iteration = ' + str(maximum_index+1))
plt.xlabel('Iteration')
plt.ylabel('Accuracy')
plt.show()

