import numpy as np
import matplotlib.pyplot as plt

#plt.rcParams['text.latex.preamble']=[r"\usepackage{wasysym}"]
#plt.rcParams['text.usetex'] = True

N_start = 50
N_end = -1 * N_start

data = np.loadtxt("./data/sat-isl-channel-gain.txt", skiprows=1)
idx = data[N_start:N_end,0]
dist = data[N_start:N_end,1] * 1e-3
gain_db = data[N_start:N_end,2::4]

gain_comp = data[N_start:N_end,4::4]

gain_diff = data[N_start:N_end,5::4] / gain_db

# rot = data[:,1]
# vel = data[:,2:4]

# azi = np.abs(data[:,1::3])
# inc = data[:,2::3]
# gain = data[:,3::3]
# gain_max = np.max(np.nan_to_num(gain, nan=-100), axis=1)
# azi_min = np.min(azi, axis=1)
# print(gain)


fig = plt.figure(figsize=(9,5))

plt.step(dist, gain_db[:,0], color='darkviolet', alpha=.5, label="Channel Gain @30 GHz")
plt.step(dist, gain_db[:,1], color='red', alpha=.5, label="Channel Gain @250 GHz")
plt.step(dist, gain_db[:,2], color='blue', alpha=.5, label="Channel Gain @200 THz")

#plt.yscale('log')
plt.ylabel("Channel Gain in dB")
plt.xlabel("Distance in km")

plt.legend()
plt.tight_layout()
plt.savefig("./img/isl-channel-gain.jpg")



### FIGURE 2 ###
fig = plt.figure(figsize=(9,5))

plt.step(idx, gain_diff[:,0], color='darkviolet', alpha=.5, label="Channel Gain Err @30 GHz")
plt.step(idx, gain_diff[:,1], color='red', alpha=.5, label="Channel Gain Err @250 GHz")
plt.step(idx, gain_diff[:,2], color='blue', alpha=.5, label="Channel Gain Err @200 THz")

#plt.yscale('log')
plt.ylabel("Channel Gain in % dB")
plt.xlabel("Simulation Time in seconds")

plt.legend()
plt.tight_layout()
plt.savefig("./img/isl-channel-gain-doppler.jpg")