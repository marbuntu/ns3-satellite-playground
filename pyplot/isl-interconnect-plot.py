

import numpy as np
import matplotlib.pyplot as plt
import matplotlib.colors as mpc

prefs = ["delta", "star"]

fig, axs = plt.subplots(1, len(prefs), figsize=(10,7), sharey=True)
norm = mpc.TwoSlopeNorm(vmin=0, vmax=1200, vcenter=50)
#norm = mpc.TwoSlopeNorm(vmin=0, vmax=36, vcenter=18)

for pref, ax in zip(prefs, axs):
    intercon = np.loadtxt(f"./data/{pref}/sat-datarate-matrix.txt")


    # #intercon = intercon[hist.argsort()]

    # plt.plot(hist)
    # plt.savefig("./test.jpg")


    hist = np.count_nonzero(intercon > 0, axis=1)
    print(pref, hist.mean(), hist.max(), hist.min())
    print(pref, intercon[np.nonzero(intercon)].mean(), intercon[np.nonzero(intercon)].max(), intercon[np.nonzero(intercon)].min())

    dat = np.where((intercon == 0), -20, intercon)


    pcm = ax.imshow((dat), cmap=plt.cm.gnuplot2_r, norm=norm)

    for c in range(40, 201, 40):
        ax.plot([c, c], [0, 200], '--', color="black", alpha=.8)
        ax.plot([0, 200], [c, c], '--', color="black", alpha=.8)

    ax.set_xlabel("Satellite ID")
    #ax.set_ylabel("Satellite ID")

    ax.set_xlim(0, 200)
    ax.set_ylim(0, 200)
    ax.set_title(f"Walker-{pref}")

axs[0].set_ylabel("Satellite ID")

fig.tight_layout() # Call before Colorbar!!!!
fig.colorbar(pcm, extend='max', ax=axs.ravel().tolist(), orientation="horizontal", label="DR in Mbps")
plt.savefig(f"./img/isl-intercon-plot.jpg")