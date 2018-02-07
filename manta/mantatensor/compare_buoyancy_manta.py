 
#
# Simple example scene (hello world)
# Simulation of a buoyant smoke density plume (with noise texture as smoke source)
#

#import pdb; pdb.set_trace()

from manta import *
import random
import sys

import numpy as np
from numpy import linalg as LA

import tensorflow as tf

import mantatensor_debug as mtd
import mantatensor as mt

import shape


import time


# solver params
seed = 1
res = 4
width = res
height = int(res*1.5)
depth = res
random.seed(seed)


solver = mtd.MantaSolverTest(width, height, depth, batches = 1)
solver.setDensityValue(1)

    
#solver.randomizeDensity()
#solver.randomizeVelocity()

solver.init()


mtd.printVecArray(solver.getVelocityArray(), batch = 0)
#printVecArray(solver.densityGrid)

mantaFluidSolver   = FluidSolver(name='main', gridSize = vec3(width, height, depth))

# prepare grids
mantaFlags    = mantaFluidSolver.create(FlagGrid)
mantaVel      = mantaFluidSolver.create(MACGrid)
mantaDensity  = mantaFluidSolver.create(RealGrid)
mantaPressure = mantaFluidSolver.create(RealGrid)

mantaFlags.initDomain(boundaryWidth=0)
mantaFlags.fillGrid()

#copyArrayToGridVec3( source=solver.getVelocityArray(), target=mantaVel )
copyArrayToGridReal( source=solver.getDensityArray(), target=mantaDensity )



addBuoyancy(density=mantaDensity, vel=mantaVel, gravity=vec3(0,-6e-4,0), flags=mantaFlags)



mantaVelArray = np.empty((width, height, depth, 3), dtype=np.float)
copyGridToArrayVec3( source=mantaVel, target=mantaVelArray )
#mantaVelArray = np.swapaxes(tMantaVelArray, 0, 1)

mantaDensityArray = np.empty((width, height, depth), dtype=np.float)
copyGridToArrayReal( source=mantaDensity, target=mantaDensityArray )
#mantaDensityArray = np.swapaxes(tMantaDensityArray, 0, 2)



print "manta:\r\n"
mtd.printVecArray(mantaVelArray, batches = false)

