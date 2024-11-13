import networkx as nx
import numpy as np
import matplotlib.pyplot as plt
from mpl_toolkits.basemap import Basemap
import matplotlib.patches as pat


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
    def __init__(self, interconnect_matrix, areas = 0, abr_ids = False):
        self.graph = nx.Graph()
        self.satellites = {}
        self.routers = {}
        self.areas = areas
        self.abr_ids = abr_ids
        
        # Initialize satellites
        for sat_id in range(len(interconnect_matrix)):
            area = 0 # areas[sat_id]
            is_abr = False #sat_id in abr_ids
            self.satellites[sat_id] = Satellite(sat_id, area, is_abr)
            self.graph.add_node(sat_id)

        # Add links based on the interconnection matrix
        for i, row in enumerate(interconnect_matrix):
            for j, weight in enumerate(row):
                if weight > 0:
                    self.graph.add_edge(i, j, weight=weight)



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
        plt.savefig("./img/isl-graph-output.jpg")



    def plot_heatmap(self, data_matrix, filename):

        fig, ax = plt.subplots(figsize=(6,6))
        ax.imshow(data_matrix, cmap=plt.cm.PuBuGn)

        for c in range(40, 201, 40):
            ax.plot([c, c], [0, 200], '--', color="black", alpha=.8)
            ax.plot([0, 200], [c, c], '--', color="black", alpha=.8)

        ax.set_xlabel("Satellite ID")
        ax.set_ylabel("Satellite ID")

        ax.set_xlim(0, 200)
        ax.set_ylim(0, 200)
        fig.tight_layout()
        plt.savefig(f"./img/isl-{filename}-plot.jpg")


class ConstellationPlot():
    def __init__(self, No : int, Ns : int, cst_prefix : str = 'delta'):
        self.No = No
        self.Ns = Ns
        self._pref = cst_prefix
        self.coords = np.zeros((No * Ns, 2))
        self.load_const()


    def load_const(self):
        Ni = 0

        for n in range(1,1+self.No):
            orb = np.loadtxt(f"./data/{self._pref}-orb{n}.txt")
            self.coords[Ni:Ni+self.Ns,0] = orb[:,0]
            self.coords[Ni:Ni+self.Ns,1] = orb[:,1]

            Ni += self.Ns
            print(Ni)
            # lats = orb[:,1]
            # lons = orb[:,0]

        print(self.coords)


    def get_earth_basemap(self, projection : str = 'ortho') -> Basemap:
        plt.figure(figsize=(12, 12))
        map = Basemap(projection=projection,lat_0=35,lon_0=40,resolution='l')
        # draw coastlines, country boundaries, fill continents.
        map.drawcoastlines(linewidth=0.25)
        map.drawcountries(linewidth=0.25)
        map.fillcontinents(color='ivory',lake_color='azure')
        # draw the edge of the map projection region (the projection limb)
        map.drawmapboundary(fill_color='azure')
        # draw lat/lon grid lines every 30 degrees.
        map.drawmeridians(np.arange(0,360,30))
        map.drawparallels(np.arange(-90,90,30))

        return map


    def plot_global_traffic(self, traffic, filename, projection : str = 'ortho'):

        map = self.get_earth_basemap(projection=projection)
        lats = self.coords[:,1]
        lons = self.coords[:,0]

        x, y = map(lats, lons)
        map.scatter(x, y, marker='h', color='purple', alpha=.6)
    
        dif_x = x[1:] - x[:-1]
        dif_y = y[1:] - y[:-1]

        print(traffic.shape)
        for r in range(traffic.shape[1]):
            for n in np.where(traffic[r,:] > 0)[0]:
                lt = [x[r], x[n]]
                ln = [y[r], y[n]]
                map.plot(lt, ln, 'b')

        #map.quiver(x[:-1], y[:-1], dif_x, dif_y, alpha=.7, color="blue")

        #map.quiver(x[0], y[0],(x[1]-x[0]), (y[1]-y[0]))
            #plt.arrow(x[0], y[0], (x[1]-x[0]), (y[1]-y[0]),  head_width=15, head_length=10, overhang=.2, color="blue", alpha=.9, length_includes_head=True)
            #map.annotate((x[0], y[0]), ((x[1]-x[0]), (y[1]-y[0])), arrowprops={'arrowstyle': '-|>', 'lw': 8, 'ec': 'r', 'shrinkA': 10})


        #plt.title('contour lines over filled continent background')
        plt.savefig(f"./img/global-{filename}-plot.jpg")
        plt.close()

        return map