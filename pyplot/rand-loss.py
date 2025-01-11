import numpy as np
import numpy.random as rnd


pck_loss = np.zeros(200)


while np.count_nonzero(pck_loss) < 10:

    id = rnd.randint(0, 199)
    pck_loss[id] = 1
