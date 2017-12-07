 
#
# Simple example scene (hello world)
# Simulation of a buoyant smoke density plume (with noise texture as smoke source)
#

#import pdb; pdb.set_trace()

from manta import *
import random;
import sys
mantaMsg("%d.%d" % (sys.version_info[0], sys.version_info[1]))

import numpy as np
from numpy import linalg as LA

import tensorflow as tf

import mantatensor as mt

def correctAxis(array):
    arrayShape =array.shape
    if(len(arrayShape) == 3):
        resultArray = np.empty([arrayShape[2], arrayShape[1], arrayShape[0]], np.result_type(array))
        for x in range(arrayShape[0]):
            for y in range(arrayShape[1]):
                for z in range(arrayShape[2]):
                    resultArray[z, y, x] = array[x, y, z]
        return resultArray
    if(len(arrayShape) == 4):
        resultArray = np.empty([arrayShape[1], arrayShape[0], arrayShape[2], arrayShape[3]], np.result_type(array))
        for x in range(arrayShape[0]):
            for y in range(arrayShape[1]):
                for z in range(arrayShape[2]):
                    for i in range(arrayShape[3]):
                        resultArray[y, x, z, i] = array[x, y, z, i]
        return resultArray

def getL2Error(array1, array2):
    assert array1.shape == array2.shape, "Could not evalute L2 error between arrays because of different shapes %s vs %s" % (str(array1.shape), str(array2.shape))
    arraySub = np.subtract(array1, array2)
    arrayNorm = LA.norm(arraySub, axis=3)
    printVecArray(arrayNorm)
    print np.sum(arrayNorm)
    
def printVecArray(array):
    if(len(array.shape) == 3):
        for z in range(array.shape[2]):
            grid = ''
            for y in range(array.shape[1]): 
                for x in range(array.shape[0]):
                    grid += "%0.3f, " % array[x, y, z]
                grid += '\r\n'
            print grid 
    if(len(array.shape) == 4):
        for z in range(array.shape[2]):
            grid = ''
            for y in range(array.shape[1]): 
                for x in range(array.shape[0]):
                    grid += "(%0.3f, %0.3f, %0.3f), " % (array[x, y, z, 0], array[x, y, z, 1], array[x, y, z, 2])
                grid += '\r\n'
            print grid 
            
            
            
def printVecGrid(grid, width, height, depth):
    if(isinstance(grid, RealGrid)):            
        for z in range(depth):
            text = ''
            for y in range(height): 
                for x in range(width):
                    text += "%0.3f, " % grid.getValue(x, y, z)
                text += '\r\n'
            print text 
    if(isinstance(grid, MACGrid)):           
        for z in range(depth):
            text = ''
            for y in range(height): 
                for x in range(width):
                    text += "(%0.3f, %0.3f, %0.3f), " % (grid.getValue(x, y, z).x, grid.getValue(x, y, z).y, grid.getValue(x, y, z).z)
                text += '\r\n'
            print text 
            


# solver params
seed = 1
res = 4
width = res
height = int(res*1.5)
depth = res
random.seed(seed)







with tf.Session(''):
    solver = mt.MantaSolverTest(width, height, depth)
    solver.densityGrid = np.full((width, height, depth), 1, dtype=np.float)
    
    solver.randomizeDensity()
    solver.randomizeVel()
    
    
    #printVecArray(solver.densityGrid)
    
    s   = FluidSolver(name='main', gridSize = vec3(width, height, depth))
    
    # prepare grids
    mantaFlags    = s.create(FlagGrid)
    mantaVel      = s.create(MACGrid)
    mantaDensity  = s.create(RealGrid)
    mantaPressure = s.create(RealGrid)

    mantaFlags.initDomain()
    mantaFlags.fillGrid()

    copyArrayToGridVec3( source=correctAxis(solver.velGrid), target=mantaVel )
    copyArrayToGridReal( source=correctAxis(solver.densityGrid), target=mantaDensity )
    
    
    
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
    printVecArray(mantaVelArray)
    
    print "\r\n"


    print "tensormanta:\r\n"
    getL2Error(mantaVelArray, solver.velGrid)
    
    printVecArray(solver.velGrid)
