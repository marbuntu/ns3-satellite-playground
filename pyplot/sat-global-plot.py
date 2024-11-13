
from mpl_toolkits.basemap import Basemap
import matplotlib.pyplot as plt
import matplotlib.patches as pat
import numpy as np

# set up orthographic map projection with
# perspective of satellite looking down at 45N, 100W.
# use low resolution coastlines.
# ortho

def get_earth_basemap() -> Basemap:
    map = Basemap(projection='ortho',lat_0=35,lon_0=40,resolution='l')
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
# make up some data on a regular lat/lon grid.
# nlats = 73; nlons = 145; delta = 2.*np.pi/(nlons-1)
# lats = (0.5*np.pi-delta*np.indices((nlats,nlons))[0,:,:])
# lons = (delta*np.indices((nlats,nlons))[1,:,:])
# wave = 0.75*(np.sin(2.*lats)**8*np.cos(4.*lons))
# mean = 0.5*np.cos(2.*lats)*((np.sin(2.*lats))**2 + 2.)
# # compute native map projection coordinates of lat/lon grid.
# x, y = map(lons*180./np.pi, lats*180./np.pi)
# # contour data over the map.
# cs = map.contour(x,y,wave+mean,15,linewidths=1.5)

C_names = [
    "delta",
    "star"
]

N_orbs = 5

for cst in C_names:

    map = get_earth_basemap()

    for n in range(1,1+N_orbs):
        orb = np.loadtxt(f"./data/{cst}-orb{n}.txt")
        lats = orb[:,1]
        lons = orb[:,0]

        x, y = map(lats, lons)
        map.scatter(x, y, marker='h', color='purple', alpha=.6, label = f"Orb {n}")
    
        dif_x = x[1:] - x[:-1]
        dif_y = y[1:] - y[:-1]

        map.quiver(x[:-1], y[:-1], dif_x, dif_y, alpha=.7, color="blue")

    #map.quiver(x[0], y[0],(x[1]-x[0]), (y[1]-y[0]))
        #plt.arrow(x[0], y[0], (x[1]-x[0]), (y[1]-y[0]),  head_width=15, head_length=10, overhang=.2, color="blue", alpha=.9, length_includes_head=True)
        #map.annotate((x[0], y[0]), ((x[1]-x[0]), (y[1]-y[0])), arrowprops={'arrowstyle': '-|>', 'lw': 8, 'ec': 'r', 'shrinkA': 10})


    #plt.title('contour lines over filled continent background')
    plt.savefig(f"./img/walker-{cst}-demo.jpg")
    plt.close()