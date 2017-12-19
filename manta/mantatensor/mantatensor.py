import tensorflow as tf
import numpy as np


manta_add_buoyancy_module = tf.load_op_library('/home/ansorge/tensorflow/bazel-bin/tensorflow/core/mantatensor/add_buoyancy.so')
manta_add_buoyancy = manta_add_buoyancy_module.add_buoyancy

manta_advection_module = tf.load_op_library('/home/ansorge/tensorflow/bazel-bin/tensorflow/core/mantatensor/advection.so')
manta_advection = manta_advection_module.advection


class MantaSolver:
    def __init__(self, width, height, depth, batches, dt = 1):
        self.batches = batches
        self.width = width
        self.height = height
        self.depth = depth
        
        self.velocityGrid = np.zeros((batches, width, height, depth, 3), dtype=np.float32)
        self.flagsGrid = np.empty((batches, width, height, depth), dtype=np.int32)
        self.densityGrid = np.zeros((batches, width, height, depth), dtype=np.float32)
        
        self.dt = dt
        
        self.initFlagsGrid()
        
    def getDt(self):
        return self.dt
        
    def getDx(self):
        return 1.0 / (max(self.width, self.height, self.depth))
        
    def addBuoyancy(self, force, coefficient = 1):
        tForce = np.multiply (np.array(force), -1 * self.getDt() / self.getDx() * coefficient).tolist();
        self.velocityGrid = manta_add_buoyancy(self.velocityGrid, self.flagsGrid, self.densityGrid, tForce).eval()
        
    def advectSemiLagrangeDensity(self, orderSpace = 1):
        self.densityGrid = manta_advection(self.densityGrid, self.flagsGrid, self.velocityGrid, self.dt, orderSpace).eval()
        
    def advectSemiLagrangeVelocity(self, orderSpace = 1):
        self.velocityGrid = manta_advection(self.velocityGrid, self.flagsGrid, self.velocityGrid, self.dt, orderSpace).eval()
        
    def initFlagsGrid(self):
        self.flagsGrid = np.full((self.batches, self.width, self.height, self.depth), 2, dtype=np.int32)
        for b in range(self.batches):
            for x in range(1, self.width-1):
                for y in range(1, self.height - 1):
                    for z in range(1, self.depth - 1):
                        self.flagsGrid[b, x, y, z] = 1
        


class Mantatensor:
    def __init__(self, mantaSolver):
        self.batches    = mantaSolver.batches
        self.width      = mantaSolver.width
        self.height     = mantaSolver.height
        self.depth      = mantaSolver.depth
        self.dt         = mantaSolver.dt
        
        self.velocityGrid   = tf.Variable(mantaSolver.velocityGrid, )
        self.flagsGrid      = tf.Variable(mantaSolver.flagsGrid)
        self.densityGrid    = tf.Variable(mantaSolver.densityGrid)
        
        config = tf.ConfigProto(
                device_count = {'GPU': 0}
            )
        self.sess = tf.Session(config=config)
        self.sess.run(tf.global_variables_initializer()) 
        

    def getDt(self):
        return self.dt
        
    def getDx(self):
        return 1.0 / (max(self.width, self.height, self.depth))

    def addBuoyancy(self, force, coefficient = 1):
        tForce = np.multiply (np.array(force), -1 * self.getDt() / self.getDx() * coefficient).tolist();
        self.sess.run(manta_add_buoyancy(self.velocityGrid, self.flagsGrid, self.densityGrid, tForce))