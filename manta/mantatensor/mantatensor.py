import tensorflow as tf
import numpy as np

import random

manta_add_buoyancy_module = tf.load_op_library('/home/ansorge/tensorflow/bazel-bin/tensorflow/core/mantatensor/add_buoyancy.so')
manta_add_buoyancy = manta_add_buoyancy_module.add_buoyancy




class MantaSolver:
    def __init__(self, width, height, depth, dt = 1):
        #self.velGrid = np.random.rand(width, height, depth, 3)
        self.width = width
        self.height = height
        self.depth = depth
        
        self.velGrid = np.zeros((width, height, depth, 3), dtype=np.float)
        self.flagsGrid = np.empty((width, height, depth), dtype=np.int)
        self.densityGrid = np.zeros((width, height, depth), dtype=np.float)
        
        self.dt = dt
        
        self.initFlagsGrid()
        
    def getDt(self):
        return self.dt
        
    def getDx(self):
        return 1.0 / (max(self.width, self.height, self.depth))
        
    def addBuoyancy(self, force, coefficient = 1):
        tForce = np.multiply (np.array(force), -1 * self.getDt() / self.getDx() * coefficient).tolist();
        self.velGrid = manta_add_buoyancy(self.flagsGrid, self.densityGrid, self.velGrid, tForce).eval()
        
    def initFlagsGrid(self):
        self.flagsGrid = np.full((self.width, self.height, self.depth), 2, dtype=np.int)
        for x in range(1, self.width-1):
            for y in range(1, self.height - 1):
                for z in range(1, self.depth - 1):
                    self.flagsGrid[x, y, z] = 1
        
    
    

def randomizeFloatArray(array):
    for i in range(array.size):
        array.flat[i] = round(random.random(), 3)

class MantaSolverTest(MantaSolver):
    def __init__(self, width, height, depth, dt = 1):
        MantaSolver.__init__(self, width, height, depth, dt) 
        
        
    def randomizeDensity(self):
        randomizeFloatArray(self.densityGrid)
        
    def randomizeVel(self):
        randomizeFloatArray(self.velGrid)
        
    def randomizeFlags(self):
        for i in range(array.size):
            array.flat[i] = random.randint(1, 2)
