import tensorflow as tf
import mantatensor as mt
import math
import numpy as np


def testBuoyancy(width, height, depth):
    solver = MantaSolver(width, height, depth)
    solver.flagsGrid = np.ones((width, height, depth), dtype=np.int)
    solver.densityGrid = np.ones((width, height, depth), dtype=np.float)
    
    #solver.flagsGrid[1, 1, 1] = 1
    #solver.flagsGrid[2, 1, 1] = 1
    
    solver.addBuoyancy([0,-6e-4,0])
    
    print solver.flagsGrid
    print solver.velGrid
    
    

with tf.Session(''):
    #with tf.device("/device:GPU:0"):
    #with tf.device("/cpu:0"):
        testBuoyancy(4, 6, 4)
        solver = MantaSolver(4, 6, 4)
        print solver.getDx()
    
    #solver = MantaSolver(4, 4, 4)
    
    #solver.addBuoyancy([1, 2, 3])
    #solver.addBuoyancy([1, 2, 3])
    
    #print solver.velGrid
    
