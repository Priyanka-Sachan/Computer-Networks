import pandas as pd
import matplotlib.pyplot as plt

#Graph-1
df = pd.read_csv("graph1.txt", sep=' ')
df.columns=['x','y']
x = df['x'].tolist()
y = df['y'].tolist()
plt.plot(x,y)
plt.xlabel("Lambda")
plt.ylabel("Average delay")
plt.show()

#Graph-2
df = pd.read_csv("graph2.txt", sep=' ')
df.columns=['x','y']
x = df['x'].tolist()
y = df['y'].tolist()
plt.plot(x,y)
plt.xlabel("Lambda")
plt.ylabel("Average Queue Size")
plt.show()

# Graph-3
df = pd.read_csv("graph3.txt", sep=' ')

data = []
for column in df:
    data.append(df[column].tolist())

plt.boxplot(data)
plt.xlabel("Source")
plt.ylabel("Average delay")
plt.show()

# Graph-4
df = pd.read_csv("graph4.txt", sep=' ')
data = []
for column in df:
    data.append(df[column].tolist())

plt.boxplot(data)
plt.xlabel("Source")
plt.ylabel("Average packet drop")
plt.show()