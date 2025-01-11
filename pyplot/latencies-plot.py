
import numpy as np
import matplotlib.pyplot as plt


lat_star = np.loadtxt("./data/star/latencies.txt")
dis_star = np.loadtxt("./data/star/distances.txt")

lat_delt = np.loadtxt("./data/delta/latencies.txt") 
dis_delt = np.loadtxt("./data/delta/distances.txt")

print("Star: ", lat_star.max(), lat_star[np.nonzero(lat_star)].min(), lat_star.mean())
print("Delta: ", lat_delt.max(), lat_delt.mean())

# dis_delt = np.sort(dis_delt)
# dis_star = np.sort(dis_star)

fig, axs = plt.subplots(1, figsize=(9, 5))

axs.plot(np.sort(lat_star), '.', alpha=.7, color="teal", label="W-Star")
axs.plot(np.sort(lat_delt), '.', alpha=.7, color="purple", label="W-Delta")

axs.set_xlabel("Number of Routes")
axs.set_ylabel("Total Latency in ms")
axs.legend(loc='upper left')
fig.tight_layout()
plt.savefig("./img/test-latencies.jpg")
plt.close

fig, axs = plt.subplots(1, figsize=(9, 5))

# axs.plot(dis_star[:,0], dis_star[:,1], '.', alpha=.7, color="blue")
axs.plot(np.sort(dis_star[:,0]), '.', alpha=.7, color="teal", label="W-Star")
# axs.plot(dis_delt[:,0], dis_delt[:,1], '.', alpha=.7, color="magenta")
axs.plot(np.sort(dis_delt[:,0]), '.', alpha=.7, color="purple", label="W-Delta")

print(np.mean(dis_star[:,0]), np.mean(dis_delt[:,0]))

axs.set_xlabel("Number of Routes")
axs.set_ylabel("Arc Distance to PS in km")

axs.legend(loc='upper left')
fig.tight_layout()
plt.savefig("./img/test-distances.jpg")