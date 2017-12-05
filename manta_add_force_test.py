import tensorflow as tf
import mantatensor as mt
import math
import numpy as np

class MantaSolver:
    def __init__(self, width, height, depth):
        self.velGrid = np.random.rand(width, height, depth, 3)
        self.flagsGrid = np.zeros((width, height, depth), dtype=np.int)
        self.densityGrid = np.zeros((width, height, depth), dtype=np.float)
        
    def addBuoyancy(self, force):
        self.velGrid = mt.manta_add_buoyancy(self.flagsGrid, self.densityGrid, self.velGrid, force).eval()


def testBuoyancy(width, height, depth):
    solver = MantaSolver(width, height, depth)
    solver.flagsGrid = np.ones((width, height, depth), dtype=np.int)
    solver.densityGrid = np.ones((width, height, depth), dtype=np.float)
    
    solver.flagsGrid[1, 1, 1] = 1
    solver.flagsGrid[2, 1, 1] = 1
    
    solver.addBuoyancy([1, 2, 3])
    
    print solver.flagsGrid
    print solver.velGrid
    
    

with tf.Session(''):
    with tf.device("/device:GPU:0"):
    #with tf.device("/cpu:0"):
    
        testBuoyancy(4, 4, 4)
    
    #solver = MantaSolver(4, 4, 4)
    
    #solver.addBuoyancy([1, 2, 3])
    #solver.addBuoyancy([1, 2, 3])
    
    #print solver.velGrid
    
