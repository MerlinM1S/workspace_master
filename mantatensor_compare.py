import manta_caller as mc

import tensorflow as tf
import mantatensor as mt

import numpy as np
from numpy import linalg as LA

import random
import os



file_path = "./compare_data/"
directory = os.path.dirname(file_path)

if not os.path.exists(directory):
    os.makedirs(directory)
    
    
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


seed = 1
res = 4
width = res
height = int(res*1.5)
depth = res
random.seed(seed)


mantaCaller = mc.MantaCaller(width, height, depth, seed)
mantaCaller.addSetConstDensity(1)
mantaCaller.addPseudoRandomDensity()
mantaCaller.addPseudoRandomVel()

mantaCaller.addAddBuoyancy()
mantaCaller.addAddBuoyancy()
mantaCaller.addAddBuoyancy()
mantaCaller.addAddBuoyancy()
#mantaCaller.addStep()

mantaCaller.addDensityToFile()
mantaCaller.addVelToFile()
mantaCaller.addFlagsToFile()

mantaCaller.execute()




mantaFileVel = open("./compare_data/vel_data.raw", "r")
mantaFlatVelArray = np.fromfile(mantaFileVel, dtype='f')
mantaVelArray = np.swapaxes(mantaFlatVelArray.reshape(width, height, depth, 3), 0, 2)

mantaFileDensity = open("./compare_data/density_data.raw", "r")
mantaFlatDensityArray = np.fromfile(mantaFileDensity, dtype='f')
mantaDensityArray = np.swapaxes(mantaFlatDensityArray.reshape(width, height, depth), 0, 2)

mantaFileFlags = open("./compare_data/flags_data.raw", "r")
mantaFlatFlagsArray = np.fromfile(mantaFileFlags, dtype='i')
mantaFlagsArray = np.swapaxes(mantaFlatFlagsArray.reshape(width, height, depth), 0, 2)


print "manta:\r\n"
printVecArray(mantaFlagsArray)



with tf.Session(''):
    solver = mt.MantaSolverTest(width, height, depth)
    solver.densityGrid = np.full((width, height, depth), 1, dtype=np.float)
    
    
    
    solver.randomizeDensity()
    solver.randomizeVel()
    
    solver.addBuoyancy([0,-6e-4,0])
    solver.addBuoyancy([0,-6e-4,0])
    solver.addBuoyancy([0,-6e-4,0])
    solver.addBuoyancy([0,-6e-4,0])


    print "tensormanta:\r\n"
    printVecArray(solver.flagsGrid)
    
    getL2Error(mantaVelArray, solver.velGrid)
    
    printVecArray(solver.velGrid)
    

    

'''

for z in range(0, gs[2]):
    grid = ''
    for y in range(0, gs[1]):
        for x in range(0, gs[0]):
            grid += '(%.3f, %.3f, %.3f), ' % (mantaVelArray[x, y, z, 0], mantaVelArray[x, y, z, 1], mantaVelArray[x, y, z, 2])
        grid += '\r\n'
    print(grid)

'''

'''

print(mantaDensityArray)


for z in range(0, gs[2]):
    grid = ''
    for y in range(0, gs[1]):
        for x in range(0, gs[0]):
            grid += '%.2f, ' % mantaDensityArray[x, y, z]
        grid += '\r\n'
    print(grid)
    

print('\r\n\r\n')


rArray = np.empty([gs[0], gs[1], gs[2]])
for x in range(0, gs[0]):
    for y in range(0, gs[1]):
        for z in range(0, gs[2]):
            rArray[x, y, z] =  random.random()

print(rArray)
'''
