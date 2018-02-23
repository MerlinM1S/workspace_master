 
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
res = 64
width = res
height = int(res*1.5)
depth = res
random.seed(seed)

loops = 50

imgScale = 6

slice = int(depth/2.0 + 0.5)


solver = mtd.MantaSolverTest(width, height, depth, batches = 1)
#solver.setDensityValue(1)
#solver.setDensityValueS(1, 3)
#solver.setDensityValueS(1, 2)
#solver.setDensityValue(1, 1)
#solver.setDensityValue(1, 0)
#solver.pseudoRandomizeDensity()
#solver.randomizeDensity()
#solver.pseudoRandomizeVelocity()
#solver.randomizeVelocity()

solver.init()

testBatch = 0


#source = shape.Cylinder(width, height, depth, center=[0.5*width, 0.1*height, 0.5*depth], radius=res*0.14, z=[0, 0, 10])
source = shape.Cylinder(width, height, depth, bot=[0.5*width, 0.1*height, 0.5*depth], radius=res*0.14, dir=[0, 0.02*height, 0])




mantaFluidSolver   = FluidSolver(name='main', gridSize = vec3(width, height, depth))

# prepare grids
mantaFlags    = mantaFluidSolver.create(FlagGrid)
mantaVel      = mantaFluidSolver.create(MACGrid)
mantaDensity  = mantaFluidSolver.create(RealGrid)
mantaPressure = mantaFluidSolver.create(RealGrid)

mantaFlags.initDomain()
mantaFlags.fillGrid()

copyArrayToGridVec3( source=mtd.prepareMantaArray(solver.getVelocityArray(), batch = testBatch), target=mantaVel )
copyArrayToGridReal( source=mtd.prepareMantaArray(solver.getDensityArray(), batch = testBatch), target=mantaDensity )

#mtd.printArray(solver.getDensityArray())
#mtd.printGrid(mantaDensity, width, height, depth)




mantaSource = mantaFluidSolver.create(Cylinder, center=vec3(0.5*width, 0.1*height, 0.5*depth), radius=res*0.14, z=vec3(0, 0.02*height, 0))
mantaSource.applyToGrid(grid=mantaDensity, value=1)


for t in range(loops):
	mantaSource.applyToGrid(grid=mantaDensity, value=1)
		
	advectSemiLagrange(flags=mantaFlags, vel=mantaVel, grid=mantaDensity, order=1) 
	advectSemiLagrange(flags=mantaFlags, vel=mantaVel, grid=mantaVel,     order=1)
	#resetOutflow(flags=mantaFlags,real=mantaDensity) 

	setWallBcs(flags=mantaFlags, vel=mantaVel)    
	addBuoyancy(density=mantaDensity, vel=mantaVel, gravity=vec3(0,-6e-4,0), flags=mantaFlags)

	#Real cgAccuracy = 1e-3, const Grid<Real>* phi = 0, const Grid<Real>* perCellCorr = 0, const MACGrid* fractions = 0, Real gfClamp = 1e-04, Real cgMaxIterFac = 1.5, bool precondition = true, int preconditioner = PcMIC, bool enforceCompatibility = false, bool useL2Norm = false, bool zeroPressureFixing = false, const Grid<Real> *curv = NULL, const Real surfTens = 0., Grid<Real>* retRhs = NULL
	solvePressure(flags=mantaFlags, vel=mantaVel, pressure=mantaPressure)
	
	#timings.display()    
	#mantaFluidSolver.step()
	print ("step: %i" %t)



#addBuoyancy(density=mantaDensity, vel=mantaVel, gravity=vec3(0,-0.1,0), flags=mantaFlags)
	
mtd.createImageGrid("manta_den", mantaDensity, width, height, depth, slice, scale = imgScale)
mtd.createImageGrid("manta_vel", mantaVel, width, height, depth, slice, scale = imgScale)

#mtd.printGrid(mantaVel, width, height, depth)



solver.applyInflow(source.getMask(), 1.0)


for t in range(loops):
    solver.applyInflow(source.getMask(), 1.0)

    solver.advectSemiLagrangeDensity()
    solver.advectSemiLagrangeVelocity()

    solver.setWallBcs()
    solver.addBuoyancy([0,-6e-4,0])

    solver.solvePressure()

    print ("step: %i" %t)



#solver.addBuoyancy([0, -0.1,0])

mtd.createImage("tensor_den", solver.getDensityArray(), slice, scale = imgScale) 
mtd.createImage("tensor_vel", solver.getVelocityArray(), slice, scale = imgScale) 


'''

start = time.time()
addBuoyancy(density=mantaDensity, vel=mantaVel, gravity=vec3(0,-6e-4,0), flags=mantaFlags)
addBuoyancy(density=mantaDensity, vel=mantaVel, gravity=vec3(0,-6e-4,0), flags=mantaFlags)
addBuoyancy(density=mantaDensity, vel=mantaVel, gravity=vec3(0,-6e-4,0), flags=mantaFlags)
addBuoyancy(density=mantaDensity, vel=mantaVel, gravity=vec3(0,-6e-4,0), flags=mantaFlags)
addBuoyancy(density=mantaDensity, vel=mantaVel, gravity=vec3(0,-6e-4,0), flags=mantaFlags)
end = time.time()
print ('MANTA BUO TIME: %f' %(end - start))



start = time.time()
solver.addBuoyancy([0,-6e-4,0])
solver.addBuoyancy([0,-6e-4,0])
solver.addBuoyancy([0,-6e-4,0])
solver.addBuoyancy([0,-6e-4,0])
solver.addBuoyancy([0,-6e-4,0])
end = time.time()
print ('TENSOR BUO TIME: %f' %(end - start))

'''





