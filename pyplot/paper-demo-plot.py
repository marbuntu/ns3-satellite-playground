
import numpy as np
import matplotlib.pyplot as plt



def plot_smpl():
    
    raw_iso = np.loadtxt("./data/paper-demo-gain-constant.txt")
    raw_dyn = np.loadtxt("./data/paper-demo-gain-dynamic.txt")

    # idx  = raw[:,0]
    # fspl = raw[:,1]

    # G_t2 = raw[:,3]
    loss = (np.max(raw_dyn[:,1:-1], axis=1))

    pwr_iso = 10 * np.log10(0 - (raw_iso[:,-1]))
    pwr_dyn = 10 * np.log10(0 - raw_dyn[:,-1]) + loss



    #plt.plot(raw[:,1:-1], alpha=.6, label=["T1", "T2", "T3", "T4"])
    plt.plot(pwr_iso, alpha=.6, color='magenta', label="Pwr Rx Iso")
    plt.plot(pwr_dyn, alpha=.6, color='teal', label='Pwr Tx Dyn')




    plt.legend()
    plt.tight_layout()
    plt.savefig("./img/paper-demo-test.jpg")





if __name__ == "__main__":
    plot_smpl()