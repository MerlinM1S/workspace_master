 
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

import mantatensor_debug as mt


# solver params
seed = 1
res = 4
width = res
height = int(res*1.5)
depth = res
random.seed(seed)


with tf.Session(''):
    solver = mt.MantaSolverTest(width, height, depth, batches = 1)
    solver.setDensityValue(1)
    
    solver.randomizeDensity()
    solver.randomizeVelocity()
    
    
    #printVecArray(solver.densityGrid)
    
    s   = FluidSolver(name='main', gridSize = vec3(width, height, depth))
    
    # prepare grids
    mantaFlags    = s.create(FlagGrid)
    mantaVel      = s.create(MACGrid)
    mantaDensity  = s.create(RealGrid)
    mantaPressure = s.create(RealGrid)

    mantaFlags.initDomain()
    mantaFlags.fillGrid()

    copyArrayToGridVec3( source=prepareMantaArray(solver.velocityGrid), target=mantaVel )
    copyArrayToGridReal( source=prepareMantaArray(solver.densityGrid), target=mantaDensity )
    
    
    
    addBuoyancy(density=mantaDensity, vel=mantaVel, gravity=vec3(0,-6e-4,0), flags=mantaFlags)
    solver.addBuoyancy([0,-6e-4,0])
    
    
    
    tMantaVelArray = np.empty((height, width, depth, 3), dtype=np.float)
    copyGridToArrayVec3( source=mantaVel, target=tMantaVelArray )
    mantaVelArray = np.swapaxes(tMantaVelArray, 0, 1)
    
    tMantaDensityArray = np.empty((depth, height, width), dtype=np.float)
    copyGridToArrayReal( source=mantaDensity, target=tMantaDensityArray )
    mantaDensityArray = np.swapaxes(tMantaDensityArray, 0, 2)


    
    
    print "manta:\r\n"

    #printVecGrid(mantaVel, width, height, depth)
    print "\r\n"
    printVecArray(mantaVelArray, batches = false)
    
    print "\r\n"


    print "tensormanta:\r\n"
    getL2Error(solver.velocityGrid[0, :, :, :, :] , mantaVelArray)
    
    printVecArray(solver.velocityGrid, batch = 0)
