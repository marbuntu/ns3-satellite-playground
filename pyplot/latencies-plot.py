
import numpy as np
import matplotlib.pyplot as plt


lat_star = np.loadtxt("./data/star/latencies.txt")
dis_star = np.loadtxt("./data/star/distances.txt")

lat_delt = np.loadtxt("./data/delta/latencies.txt") 
dis_delt = np.loadtxt("./data/delta/distances.txt")

# dis_delt = np.sort(dis_delt)
# dis_star = np.sort(dis_star)

fig, axs = plt.subplots(1, figsize=(12, 6))

axs.plot(np.sort(lat_star), '.', alpha=.7)
axs.plot(np.sort(lat_delt), '.', alpha=.7)

plt.savefig("./img/test-latencies.jpg")
plt.close

fig, axs = plt.subplots(1, figsize=(12, 6))

# axs.plot(dis_star[:,0], dis_star[:,1], '.', alpha=.7, color="blue")
axs.plot(np.sort(dis_star[:,0]), '.', alpha=.7, color="blue")
# axs.plot(dis_delt[:,0], dis_delt[:,1], '.', alpha=.7, color="magenta")
axs.plot(np.sort(dis_delt[:,0]), '.', alpha=.7, color="magenta")

print(np.mean(dis_star[:,0]), np.mean(dis_delt[:,0]))

# axs.set_ylim(0, 5)

plt.savefig("./img/test-distances.jpg")