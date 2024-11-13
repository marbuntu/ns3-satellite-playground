
import networkx as nx
import matplotlib.pyplot as plt
import numpy as np
import random

from lib.constellation import Constellation, ConstellationPlot


########### SETUP ##############

intercon = np.loadtxt("./data/sat-datarate-matrix.txt")
Cst = Constellation(intercon)

Nn = Cst.graph.number_of_nodes()      # Number of Nodes

Cr = intercon
Dt = np.zeros(Cr.shape)

N_load = 9000 * 112 * 8

G = Cst.graph


cplot = ConstellationPlot(5, 40, cst_prefix="delta")


def degrade_link(n1 : int, n2 : int, load_bits : float) -> None:
    
    beta = 2

    ns = min(n1, n2)
    nr = max(n1, n2)

    C_max = Cr[ns, nr]

    Dt[ns, nr] += load_bits
    D_tot = Dt[ns, nr]

    C_max = 500 * 1e6
    C_new = C_max * np.exp(-beta * (D_tot / C_max))

    # print(C_new * 1e-6)
    # print(Dt)


def run_process(G : nx.Graph, PS : int = 1) -> None:
    '''
        G  : Graph  - Network Graph
        PS : int    - Id of the Parameter Server
    '''
    used = [PS]

    #paths = dict(nx.all_pairs_all_shortest_paths(G))

    while (len(used) < Nn):
        src = random.sample(list(G.nodes()), 1)[0]

        if src in used:
            continue

        path = nx.shortest_path(G, src, PS)
        for n in range(1, len(path)):
            n1 = path[n-1]
            n2 = path[n]
            degrade_link(n1, n2, N_load)


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

run_process(G, PS=1)

# print(Dt * 1e-6)


# print(f"Transitivity: {nx.local_efficiency(G)}")
# print(f"Transitivity: {nx.global_efficiency(G)}")

#degrade_link(2, 3, (9000 * 8 * 112))

plt.savefig("./img/isl-shortest-test.jpg")


Cst.plot_network(No = 5, Ns = 40)
Cst.plot_heatmap(Dt, filename="traffic")

cplot.plot_global_traffic(Dt, filename="traffic", projection="mill")
