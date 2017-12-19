 
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
    solver = mt.MantaSolverTest(width, height, depth, batches = 2)
    solver.setDensityValue(1)
    
    solver.randomizeDensity()
    #solver.pseudoRandomizeVelocity()
    solver.randomizeVelocity()
    
    testBatch = 0
    
    print "density:" 
    printVecArray(solver.densityGrid, batch = testBatch)
    
    
    print "velocity:" 
    printVecArray(solver.velocityGrid, batch = testBatch)
    
    
    
    
    s   = FluidSolver(name='main', gridSize = vec3(width, height, depth))
    
    # prepare grids
    mantaFlags    = s.create(FlagGrid)
    mantaVel      = s.create(MACGrid)
    mantaDensity  = s.create(RealGrid)
    mantaPressure = s.create(RealGrid)

    mantaFlags.initDomain()
    mantaFlags.fillGrid()

    copyArrayToGridVec3( source=prepareMantaArray(solver.velocityGrid, batch = testBatch), target=mantaVel )
    copyArrayToGridReal( source=prepareMantaArray(solver.densityGrid, batch = testBatch), target=mantaDensity )
    
    
    
    print "manta:"
    printVecGrid(mantaDensity, width, height, depth)
    
    
    advectSemiLagrange(flags=mantaFlags, vel=mantaVel, grid=mantaDensity, orderSpace = 1)    
    solver.advectSemiLagrangeDensity(orderSpace = 1)
    
    
    print "manta:"
    printVecGrid(mantaDensity, width, height, depth)
    
    
    print "mantatensor:"
    printVecArray(solver.densityGrid)
    
    
    advectSemiLagrange(flags=mantaFlags, vel=mantaVel, grid=mantaVel, orderSpace = 1)    
    solver.advectSemiLagrangeVelocity(orderSpace = 1)

    print "manta:"
    printVecGrid(mantaVel, width, height, depth)
    
    
    print "mantatensor:"
    printVecArray(solver.velocityGrid, batch = testBatch)