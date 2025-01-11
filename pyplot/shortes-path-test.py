
import networkx as nx
import matplotlib.pyplot as plt
import numpy as np
import numpy.random as rnd
import random
from geopy import distance

from lib.constellation import Constellation, ConstellationPlot


########### SETUP ##############

Pref = "delta"
latmat = np.loadtxt(f"./data/{Pref}/sat-latency-matrix.txt")
intercon = np.loadtxt(f"./data/{Pref}/sat-datarate-matrix.txt")
positions = np.loadtxt(f"./data/{Pref}/sat-pos.txt")
Cst = Constellation(intercon)

Nn = Cst.graph.number_of_nodes()      # Number of Nodes

Cr = intercon
Dt = np.zeros(Cr.shape)
Uses = np.zeros(Cr.shape)

latencies = np.zeros(Cr.shape[0])
distances = np.zeros((Cr.shape[0], 2))

N_load = 9000 * 112 * 8

G = Cst.graph

if Pref == "star":
    PS = 119# 119 #25
else:
    PS = 25

cplot = ConstellationPlot(5, 40, cst_prefix=Pref)

# Setup Pck Loss
use_pck_loss = True
pck_loss = np.zeros(200)
while np.count_nonzero(pck_loss) < 10:
    if id == PS:
        continue

    id = rnd.randint(0, 199)
    pck_loss[id] = 1


def degrade_link(n1 : int, n2 : int, load_bits : float) -> float:
    
    beta = 2

    laty = latmat[n1, n2]
    dist = laty * 299.792458

    ns = min(n1, n2)
    nr = max(n1, n2)

    C_max = Cr[ns, nr]

    Uses[ns, nr] += 1
    Dt[ns, nr] += load_bits
    D_tot = Dt[ns, nr]

    C_max = 500 * 1e6
    C_new = C_max * np.exp(-beta * (D_tot / C_max))

    laty += load_bits / C_new
    #print(adlat)

    return (laty, dist)

    # print(C_new * 1e-6)
    # print(Dt)


def run_process(G : nx.Graph, PS : int = 1) -> None:
    '''
        G  : Graph  - Network Graph
        PS : int    - Id of the Parameter Server
    '''
    used = [PS]

    pos_ps = positions[PS]
    ps_ln = distance.lonlat(pos_ps[1], pos_ps[0], 2000)

    #paths = dict(nx.all_pairs_all_shortest_paths(G))

    while (len(used) < Nn):
        src = random.sample(list(G.nodes()), 1)[0]

        if src in used:
            continue

        path = nx.shortest_path(G, src, PS)
        clat = 0.0
        dist = 0.0
        for n in range(1, len(path)):
            n1 = path[n-1]
            n2 = path[n]
            lat, dis = degrade_link(n1, n2, N_load)
            clat += lat
            dist += dis

            if n > 1:
                clat += 1

        if pck_loss[src]:
            clat += 500

        pos_src = positions[src]
        sr_ln = distance.lonlat(pos_src[1], pos_src[0], 2000)

        latencies[src] = clat
        distances[src] = [ distance.great_circle(sr_ln, ps_ln, radius=distance.EARTH_RADIUS + 2000).km, dist]
        used.append(src)


# G = nx.Graph()
# G.add_node(1)
# G.add_node(2)
# G.add_node(3)
# G.add_node(4)
# G.add_node(5)
# G.add_node(6)
# G.add_node(7)
# G.add_node(8)
# G.add_node(9)


# G.add_edge(1, 2, weight=2)
# G.add_edge(1, 4, weight=10)

# G.add_edge(2, 3, weight=2)
# G.add_edge(2, 5, weight=10)

# G.add_edge(3, 6, weight=10)

# G.add_edge(4, 5, weight=2)
# G.add_edge(4, 7, weight=10)

# G.add_edge(5, 6, weight=2)
# G.add_edge(5, 8, weight=10)

# G.add_edge(6, 9, weight=10)

# G.add_edge(7, 8, weight=2)

# G.add_edge(8, 9, weight=2)


fig = plt.figure(figsize=(15,10))
nx.draw(G, node_size=700, font_size=15, font_weight="bold")
# nx.draw_networkx_edge_labels(G,  font_size=12)

paths = dict(nx.all_pairs_all_shortest_paths(G))[1]

run_process(G, PS=PS)

# print(Dt * 1e-6)


# print(f"Transitivity: {nx.local_efficiency(G)}")
# print(f"Transitivity: {nx.global_efficiency(G)}")

#degrade_link(2, 3, (9000 * 8 * 112))

plt.savefig("./img/isl-shortest-test.jpg")


Cst.plot_network(No = 5, Ns = 40)
Cst.plot_heatmap(Dt, filename="traffic")

cplot.plot_global_traffic(Dt, PS=PS, filename="traffic", projection="ortho")
plt.close()

# fig, axs = plt.subplots(1)

# axs.plot(np.sort(latencies))

# plt.savefig("./img/test-latencies.jpg")


np.savetxt(f"./data/{Pref}/degraded-data-rate.txt", Cr)
np.savetxt(f"./data/{Pref}/traffic.txt", Dt)
np.savetxt(f"./data/{Pref}/latencies.txt", latencies)
np.savetxt(f"./data/{Pref}/distances.txt", distances)


# fig = plt.figure(figsize=(10,6))
# for s in range(26, 41):        
#     eta = np.log2(Uses[s-1,s])
#     plt.scatter(s, eta)

# plt.savefig("./img/test-traffic.jpg")
