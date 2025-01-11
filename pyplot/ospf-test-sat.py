import heapq
import numpy as np
import networkx as nx
import random
import matplotlib.pyplot as plt

class Satellite:
    def __init__(self, sat_id, area, is_abr=False):
        self.sat_id = sat_id
        self.area = area
        self.is_abr = is_abr
        self.link_state_db = {}

    def generate_lsa(self, graph):
        """Generate intra-area Link-State Advertisement (LSA)"""
        self.link_state_db[self.sat_id] = graph.edges(self.sat_id)


class Constellation:
    def __init__(self, interconnect_matrix, areas, abr_ids):
        self.graph = nx.Graph()
        self.satellites = {}
        self.routers = {}
        self.areas = areas
        self.abr_ids = abr_ids
        
        # Initialize satellites
        for sat_id in range(len(interconnect_matrix)):
            area = areas[sat_id]
            is_abr = sat_id in abr_ids
            self.satellites[sat_id] = Satellite(sat_id, area, is_abr)
            self.graph.add_node(sat_id)

        # Add links based on the interconnection matrix
        for i, row in enumerate(interconnect_matrix):
            for j, weight in enumerate(row):
                if weight > 0:
                    self.graph.add_edge(i, j, weight=weight)

    def propagate_lsas(self):
        """Simulate LSA propagation within areas and by ABRs between areas."""
        for satellite in self.satellites.values():
            if satellite.is_abr:
                # ABRs send LSAs for inter-area routes
                for area in set(self.areas):
                    if area != satellite.area:
                        satellite.link_state_db.update(
                            {k: v for k, v in self.graph.edges.items() if self.satellites[k].area == area}
                        )
            else:
                # Regular satellites send LSAs within their own area
                satellite.generate_lsa(self.graph)

    def calculate_shortest_paths(self):
        """Calculate shortest paths for each satellite, considering area boundaries."""
        paths = {}
        for sat_id, satellite in self.satellites.items():
            # Intra-area routing
            if not satellite.is_abr:
                intra_area_nodes = [k for k, v in self.satellites.items() if v.area == satellite.area]
                subgraph = self.graph.subgraph(intra_area_nodes)
                paths[sat_id] = nx.single_source_dijkstra_path_length(subgraph, sat_id)
            # Inter-area routing via ABRs
            else:
                paths[sat_id] = nx.single_source_dijkstra_path_length(self.graph, sat_id)
        return paths


    def simulate_traffic(self, num_pairs=5):
        """Simulate traffic between random satellite pairs and compute delays."""
        delays = []
        for _ in range(num_pairs):
            src, dst = random.sample(list(self.satellites.keys()), 2)
            try:
                delay = nx.shortest_path_length(self.graph, src, dst, weight='weight')
                delays.append((src, dst, delay))
            except nx.NetworkXNoPath:
                delays.append((src, dst, float('inf')))
        return delays


    def plot_network(self, No : int = 0, Ns : int = 0):
        """Visualize the network graph using networkx and matplotlib"""
        #G = nx.Graph() #self.graph
        G = nx.Graph() #self.graph

        # # Add nodes and edges to the NetworkX graph
        # for node in self.graph.nodes:
        #     G.add_node(node)
        # for node, neighbors in self.graph.edges.items():
        #     print(neighbors)
        #     for neighbor, weight in neighbors:
        #         G.add_edge(node, neighbor, weight=weight)
        
        # Draw the graph
          # Positioning of nodes


        node_list = list(self.graph.nodes)
        if No > 0 and Ns > 0:
            offs_step = (np.pi / 4.5)
            for o in range(min(No, 5)):
                for s in range(min(Ns, 7)):
                    idx = (o * Ns) + s
                    G.add_node(node_list[idx])

            for node in list(G.nodes):
                for node, neighbor in self.graph.edges(node):
                    #print(self.graph.get_edge_data(node, neighbor)["weight"])
                    if G.has_node(neighbor):
                        G.add_edge(node, neighbor, weight=self.graph.get_edge_data(node, neighbor)["weight"])
                    #for neighbor, weight in graph.get_edge_data(node, neighbor):


        pos = nx.kamada_kawai_layout(G)
        print(G)

        if No > 0 and Ns > 0:
            offs_step = (np.pi / 8)
            for o in range(min(No, 5)):
                for s in range(min(Ns, 7)):
                    idx = (o * Ns) + s

                    off_x = np.cos(offs_step * s)
                    off_y = np.sin(offs_step * s)

                    pos[idx] = np.array([(2*o) - off_x,s + off_y])


        options = {
            "node_color": "#A0CBE2",
            "edge_cmap": plt.cm.inferno,
            "edge_color": list(nx.get_edge_attributes(G, 'weight').values()),
            "alpha": .7,
           # "edge_vmax": 20,
            "width": 1.85,
            "with_labels": True
        }

        print(self.graph.edges(0))

        fig = plt.figure(figsize=(15,10))
        nx.draw(G, pos, node_size=700, font_size=15, font_weight="bold", **options)
        edge_labels = nx.get_edge_attributes(G, 'weight')
        #nx.draw_networkx_edge_labels(G, pos, edge_labels=edge_labels, font_size=12)
        
        plt.tight_layout()
        plt.title("Network Topology with OSPF Weights")
        plt.savefig("./img/isl-interconnect-graph.jpg")


class ConstellationGenerator():
    N_spo : int = 0
    N_orb : int = 0

    W_lb : float = 1.0

    def __init__(self, No : int, Ns : int):
        '''
        ### Parameters:

            No : Int - Number of Orbits
            Ns : Int - Number of Satellites per Orbit
        '''
        self.N_spo = int(Ns)
        self.N_orb = int(No)


    def _patternGridPlus(self, sat_idx : int) -> None:
        sat_idx[0] = 1


    def _renderSymmetry(self, con_matrix : np.array) -> None:

        N = con_matrix.shape[0]

        for n in range(N):
            for m in range(n, N):
                con_matrix[m, n] = con_matrix[n, m]


    def genGridPlus(self):
        N_sats = self.N_orb * self.N_spo
        Mcon = np.zeros((N_sats, N_sats))

        for r in range(N_sats):
            sat = Mcon[r,:]

            # Loopback
            sat[r] = self.W_lb

            # Leader
            if r < (N_sats - 1) and ((r+1) % self.N_spo) != 0:
                sat[r+1] = np.random.randint(2, 4) #2

            # Follower for 1. Sat of each Orbit
            if (r % self.N_spo) == 0:
                sat[r+self.N_spo-1] = np.random.randint(2, 4)
            
            # Right Neighbour
            if r < (N_sats - self.N_spo):
                sat[r+self.N_spo] = np.random.randint(10, 20)


        self._renderSymmetry(Mcon)

        return Mcon

# Example Interconnection Matrix (20x20) and Area Config

lat_matrix = np.loadtxt("./data/sat-datarate-matrix.txt")

No = 5
Ns = 40
Nt = No * Ns

gen = ConstellationGenerator(No, Ns)
gen.W_lb = 1
interconnect_matrix = lat_matrix #gen.genGridPlus()

# # Total Number of Satellites
# N = 200

# # Number of Sats per Orbit
# O = 20

# w_io = 1

# interconnect_matrix = np.zeros((N,N))

# for n in range(0, N, O):
    
#     # Generate O Edge Weights
#     wn = np.ones(O) #np.random.randint(2, 10, O)
    
#     for so in range(O):
#         wl = wn[so]
#         wf = wn[so-1]

#         r = n + so
#         pf = r - 1
#         pl = r + 1

#         if so == 0:
#             pf = n + O - 1

#         if so == (O - 1):
#             pl = n

#         interconnect_matrix[r, pf] = wf
#         interconnect_matrix[r, n + so] = 0  # Loopback
#         interconnect_matrix[r, pl] = wl

#     if n < (N-O):
#         interconnect_matrix[n, n+O] = w_io

#     if n > 0:
#         interconnect_matrix[n, n-O] = w_io

    #nterconnect_matrix[n,n-1] = np.random.randint(1, 5)
    #interconnect_matrix[n,n] = np.random.randint(1, 5)
    #
    # if n < (N-1):
    #     interconnect_matrix[n,n+1] = np.random.randint(1, 5)
    # else:
    #     interconnect_matrix[n,0] = np.random.randint(1, 5)

#print(interconnect_matrix)



areas = np.zeros(Nt) #[0, 0, 0, 0, 1, 1, 1, 1, 2, 2, 2, 2, 3, 3, 3, 3, 0, 1, 2, 3]  # 4 areas
abr_ids = [3, 7, 11, 15]  # One ABR per area

# # Initialize the constellation
constellation = Constellation(interconnect_matrix, areas, abr_ids)
constellation.plot_network(No, Ns)

# Nr = 5
# ri = 0
# red_inter = np.zeros((25, 25))
# for r in range(0, interconnect_matrix.shape[0], Ns):
#     print(r)
#     red_inter[ri:ri+Nr,ri:ri+Nr] = interconnect_matrix[r:r+Nr,r:r+Nr]
#     ri += Nr
#     #r = np.append(red_inter, slic, axis=1)

# print(red_inter)
# cs = Constellation(red_inter, areas, abr_ids)
# cs.plot_network(5, 6)

# Propagate LSAs and calculate shortest paths
constellation.propagate_lsas()
shortest_paths = constellation.calculate_shortest_paths()

# Simulate traffic between random satellite pairs and measure delays
delays = np.array(constellation.simulate_traffic(num_pairs=1000))

# Display the results
hops = []
print("Shortest Paths (from each satellite):")
for sat_id, paths in shortest_paths.items():
    # print(f"Satellite {sat_id}: {paths}")
    hops.append(list(paths.values()))

# print("\nSimulated Traffic Delays (between satellite pairs):")
# for src, dst, delay in delays:
#     print(f"From Satellite {src} to Satellite {dst}: Delay = {delay}")

delay = delays[:,2]

print("\n")
print(f"Hops:\t {np.mean(hops)} avg \t {np.min(hops)} min \t {np.max(hops)} max")
print(f"Delay:\t {np.mean(delay)} avg \t {np.min(delay)} min \t {np.max(delay)} max")

print("\n")
print(f"Transitivity: {nx.transitivity(constellation.graph)}")