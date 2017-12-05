 
#
# Simple example scene (hello world)
# Simulation of a buoyant smoke density plume (with noise texture as smoke source)
#

#import pdb; pdb.set_trace()

from manta import *
import random;
#import numpy as np


# solver params
res = 4
gs  = vec3(res, int(1.5*res), res)
s   = FluidSolver(name='main', gridSize = gs)

# prepare grids
flags    = s.create(FlagGrid)
vel      = s.create(MACGrid)
density  = s.create(RealGrid)
pressure = s.create(RealGrid)

flags.initDomain()
flags.fillGrid()

density.setConst(1);


'''
for x in range(0, int(gs.x)):
    for y in range(0, int(gs.y)):
        for z in range(0, int(gs.z)):
            density.setValue(x, y, z, x+ y*10+z*100); #random.random()
#flags.fillGrid(1)
'''



                

for z in range(0, int(gs.z)):
    grid = ''
    for y in range(0, int(gs.y)):
        for x in range(0, int(gs.x)):
            grid += '%.2f, ' % density.getValue(x, y, z)
        grid += '\r\n'
    mantaMsg(grid);


def gridToFile(fName, grid):
    array = np.empty([grid.width, grid.height, grid. depth])
    for x in range(0, int(grid.width)):
        for y in range(0, int(grid.height)):
            for z in range(0, int(grid.depth)):
                array[x, y, z] = density.getValue(x, y, z)
    np.save(fName, array)
    
#gridToFile('ttt.npy', density)
   


density.save('den_data.raw')



if (GUI):
	gui = Gui()
	gui.show()
	gui.pause()
	
#main loop
for t in range(250):
	mantaMsg('\nFrame %i' % (s.frame))
	addBuoyancy(density=density, vel=vel, gravity=vec3(0,-6e-4,0), flags=flags)
	s.step()


for z in range(0, int(gs.z)):
    grid = ''
    for y in range(0, int(gs.y)):
        for x in range(0, int(gs.x)):
            grid += '%.2f, ' % vel.getValue(x, y, z).y
        grid += '\r\n'
    mantaMsg(grid);
