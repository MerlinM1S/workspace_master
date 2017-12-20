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
        
        self.dt = dt
        
        self.applyVelocityArray     (np.zeros((batches, width, height, depth, 3), dtype=np.float32))
        self.applyFlagsArray        (np.empty((batches, width, height, depth, 1), dtype=np.int32))
        self.applyDensityArray      (np.zeros((batches, width, height, depth, 1), dtype=np.float32))
        
        self.initFlagsTensor()
        
        self.sess = tf.Session(config = tf.ConfigProto(device_count = {'GPU': 0}))
        
        
    def initFlagsTensor(self):
        array = np.full((self.batches, self.width, self.height, self.depth, 1), 2, dtype=np.int32)
        for b in range(self.batches):
            for x in range(1, self.width-1):
                for y in range(1, self.height - 1):
                    for z in range(1, self.depth - 1):
                        array[b, x, y, z] = 1
        self.applyFlagsArray(array)
        
    def init(self):
        self.sess.run(tf.global_variables_initializer()) 
        
        
    def addBuoyancy(self, force, coefficient = 1):
        tForce = np.multiply (np.array(force), -1 * self.getDt() / self.getDx() * coefficient).tolist();
        self.sess.run(manta_add_buoyancy(self.velocityTensor, self.flagsTensor, self.densityTensor, tForce))
        
    def advectSemiLagrangeDensity(self, orderSpace = 1):
        self.densityGrid = manta_advection(self.densityTensor, self.flagsTensor, self.velocityTensor, self.dt, orderSpace).eval()
        
    def advectSemiLagrangeVelocity(self, orderSpace = 1):
        self.velocityGrid = manta_advection(self.velocityTensor, self.flagsTensor, self.velocityTensor, self.dt, orderSpace).eval()
        
        
        
    ## Setter
    def applyVelocityArray(self, velocityArray):
         self.velocityTensor = tf.Variable(velocityArray)
         
         
    def applyFlagsArray(self, flagsArray):
         self.flagsTensor = tf.Variable(flagsArray)
         
         
    def applyDensityArray(self, densityArray):
         self.densityTensor = tf.Variable(densityArray)
    
    
    ## Getter
    def getVelocityArray(self):
        return self.sess.run(self.velocityTensor)
    
    
    def getFlagsArray(self):
        return self.sess.run(self.flagsTensor)
    
    
    def getDensityArray(self):
        return self.sess.run(self.densityTensor)
    
    
    def getDt(self):
        return self.dt
        
        
    def getDx(self):
        return 1.0 / (max(self.width, self.height, self.depth))
    
    
    
    
    