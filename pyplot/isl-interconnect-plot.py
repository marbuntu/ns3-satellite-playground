

import numpy as np
import matplotlib.pyplot as plt



intercon = np.loadtxt("./data/sat-datarate-matrix.txt")


hist = np.count_nonzero(intercon > 0, axis=1)
#intercon = intercon[hist.argsort()]

plt.plot(hist)
plt.savefig("./test.jpg")


print(hist.mean(), hist.max(), hist.min())


fig, ax = plt.subplots(figsize=(6,6))

ax.imshow(intercon, cmap=plt.cm.PuBuGn)

for c in range(40, 201, 40):
    ax.plot([c, c], [0, 200], '--', color="black", alpha=.8)
    ax.plot([0, 200], [c, c], '--', color="black", alpha=.8)

ax.set_xlabel("Satellite ID")
ax.set_ylabel("Satellite ID")

ax.set_xlim(0, 200)
ax.set_ylim(0, 200)
fig.tight_layout()
plt.savefig("./img/isl-intercon-plot.jpg")