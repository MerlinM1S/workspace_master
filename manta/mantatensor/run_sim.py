 
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

from PIL import Image

import time





# solver params
seed = 1
res = 64
width = res
height = int(res*1.5)
depth = res
random.seed(seed)


vVel = tf.Variable(tf.zeros([1, width, height, depth, 3]))
y = mtd.test_empty(vVel)

sess = tf.InteractiveSession()
tf.global_variables_initializer().run()


    
start = time.time()
sess.run(y)
end = time.time()
print ('TENSOR EMPTY_TIME: %f' %(end - start))


solver = mtd.MantaSolverTest(width, height, depth, batches = 1)
solver.setDensityValue(1)

solver.randomizeDensity()
#solver.pseudoRandomizeVelocity()
solver.randomizeVelocity()

testBatch = 0



mantatensor = mt.Mantatensor(solver)


start = time.time()
mantatensor.addBuoyancy([0,-6e-4,0])
end = time.time()
print ('TENSOR BUO TIME: %f' %(end - start))








with tf.Session(''):
    s   = FluidSolver(name='main', gridSize = vec3(width, height, depth))
    
    # prepare grids
    mantaFlags    = s.create(FlagGrid)
    mantaVel      = s.create(MACGrid)
    mantaDensity  = s.create(RealGrid)
    mantaPressure = s.create(RealGrid)

    mantaFlags.initDomain()
    mantaFlags.fillGrid()

    copyArrayToGridVec3( source=mtd.prepareMantaArray(solver.velocityGrid, batch = testBatch), target=mantaVel )
    copyArrayToGridReal( source=mtd.prepareMantaArray(solver.densityGrid, batch = testBatch), target=mantaDensity )
    
    
    addBuoyancy(density=mantaDensity, vel=mantaVel, gravity=vec3(0,-6e-4,0), flags=mantaFlags)
    solver.addBuoyancy([0,-6e-4,0])
    


    
    
    
    start = time.time()
    mtd.test_empty(solver.velocityGrid).eval()
    end = time.time()
    print ('ARRAY EMPTY_TIME: %f' %(end - start))
    
    start = time.time()
    addBuoyancy(density=mantaDensity, vel=mantaVel, gravity=vec3(0,-6e-4,0), flags=mantaFlags)
    end = time.time()
    print ('MANTA TIME: %f' %(end - start))
    
    start = time.time()
    solver.addBuoyancy([0,-6e-4,0])
    end = time.time()
    print ('ARRAY BUO TIME: %f' %(end - start))

