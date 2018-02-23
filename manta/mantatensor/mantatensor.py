import tensorflow as tf
import numpy as np


mantatensor_module = tf.load_op_library('/home/ansorge/tensorflow/bazel-bin/tensorflow/core/mantatensor/mantatensor.so')
manta_apply_mask_to_grid_int = mantatensor_module.apply_mask_to_grid_int
manta_apply_mask_to_grid_float = mantatensor_module.apply_mask_to_grid_float
manta_apply_mask_to_grid_vec3 = mantatensor_module.apply_mask_to_grid_vec3
manta_add_buoyancy = mantatensor_module.add_buoyancy
manta_kn_set_wall_bcs = mantatensor_module.kn_set_wall_bcs
manta_advect_semi_lagrange_mac_grid = mantatensor_module.advect_semi_lagrange_mac_grid
manta_advect_semi_lagrange_grid_vec3 = mantatensor_module.advect_semi_lagrange_grid_vec3
manta_advect_semi_lagrange_gridfloat = mantatensor_module.advect_semi_lagrange_gridfloat
manta_solve_pressure_part1 = mantatensor_module.solve_pressure_part1
manta_solve_pressure_part2 = mantatensor_module.solve_pressure_part2

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
        self.applyPressureArray     (np.zeros((batches, width, height, depth, 1), dtype=np.float32))
        
        self.initFlagsTensor()
        
        self.sess = tf.Session(config = tf.ConfigProto(device_count = {'GPU': 0}))
        
        
    def initFlagsTensor(self):
        array = np.full((self.batches, self.width, self.height, self.depth, 1), 2, dtype=np.int32)
        for b in range(self.batches):
            for x in range(1, self.width - 1):
                for y in range(1, self.height - 1):
            	    for z in range(1, self.depth - 1):
                        array[b, x, y, z] = 1
        self.applyFlagsArray(array)
        
    def init(self):
        self.sess.run(tf.global_variables_initializer()) 
        
        
    def addBuoyancy(self, force, coefficient = 1.0):
	'''
        tForce = np.multiply (np.array(force), -1 * self.getDt() / self.getDx() * coefficient).tolist();
        #self.sess.run(manta_add_buoyancy(self.velocityTensor, self.flagsTensor, self.densityTensor, tForce))
        step = tf.group(self.velocityTensor.assign(manta_add_buoyancy(self.velocityTensor, self.flagsTensor, self.densityTensor, tForce)))
	'''
        step = tf.group(self.velocityTensor.assign(manta_add_buoyancy(self.flagsTensor, self.densityTensor, self.velocityTensor, force, coefficient)))
        self.sess.run(step)
        
    def advectSemiLagrangeDensity(self, order = 1, strength = 1.0, orderSpace = 1, openBounds = False, boundaryWidth = 1, clampMode = 1):
        step = tf.group(self.densityTensor.assign(manta_advect_semi_lagrange_gridfloat(self.flagsTensor, self.velocityTensor, self.densityTensor, order, strength, orderSpace, openBounds, boundaryWidth, clampMode)))
        self.sess.run(step)

    def advectSemiLagrangeVelocity(self, order = 1, strength = 1.0, orderSpace = 1, openBounds = False, boundaryWidth = 1, clampMode = 1):
        step = tf.group(self.velocityTensor.assign(manta_advect_semi_lagrange_mac_grid(self.flagsTensor, self.velocityTensor, self.velocityTensor, order, strength, orderSpace, openBounds, boundaryWidth, clampMode)))
        self.sess.run(step)
    
    def setWallBcs(self):
        step = tf.group(self.velocityTensor.assign(manta_kn_set_wall_bcs(self.flagsTensor, self.velocityTensor)))
        self.sess.run(step)
	
    def applyInflow(self, mask, value):
        step = tf.group(self.densityTensor.assign(manta_apply_mask_to_grid_float(self.densityTensor, mask, value, self.flagsTensor)))
        self.sess.run(step)

    def solvePressure(self, cgAccuracy = 1e-3, phi = 0, perCellCorr = 0, fractions = 0, gfClamp = 1e-04, cgMaxIterFac = 1.5, precondition = True, preconditioner = 1, enforceCompatibility = False, useL2Norm = False, zeroPressureFixing = False, curv = 0, surfTens = 0.0):
        step1 = tf.group(self.pressureTensor.assign(manta_solve_pressure_part1(self.velocityTensor, self.pressureTensor, self.flagsTensor, cgAccuracy, phi, perCellCorr, fractions, gfClamp, cgMaxIterFac, precondition, preconditioner, enforceCompatibility, useL2Norm, zeroPressureFixing, curv, surfTens)))
        step2 = tf.group(self.velocityTensor.assign(manta_solve_pressure_part2(self.velocityTensor, self.pressureTensor, self.flagsTensor, cgAccuracy, phi, gfClamp, curv, surfTens)))
        self.sess.run(step1)
        self.sess.run(step2)
    
	
        
    ## Setter
    def applyVelocityArray(self, velocityArray):
         self.velocityTensor = tf.Variable(velocityArray)
         
         
    def applyFlagsArray(self, flagsArray):
         self.flagsTensor = tf.Variable(flagsArray)
         
         
    def applyDensityArray(self, densityArray):
         self.densityTensor = tf.Variable(densityArray)


    def applyPressureArray(self, pressureArray):
         self.pressureTensor = tf.Variable(pressureArray)
    
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
    
    
    
    
    
