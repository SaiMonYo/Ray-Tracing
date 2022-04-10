import matplotlib.pyplot as plt

'''function to plot 3d points from obj file to give idea how big model is'''

fig = plt.figure()
ax = fig.add_subplot(111, projection='3d')

X = []
Y = []
Z = []

with open("Objects/cow.obj", "r") as f:
    for line in f:
        if line.startswith("v "):
            x, y, z = line.split()[1:]
            X.append(float(x))
            Y.append(float(y))
            Z.append(float(z))

ax.scatter(X, Y, Z, c='r', marker='o')

ax.set_xlabel('X axis')
ax.set_ylabel('Y axis')
ax.set_zlabel('Z axis')

plt.show()