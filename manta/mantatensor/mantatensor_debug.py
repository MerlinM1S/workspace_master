import tensorflow as tf
import numpy as np

import random
import mantatensor as mt


test_empty_module = tf.load_op_library('/home/ansorge/tensorflow/bazel-bin/tensorflow/core/mantatensor/test_empty.so')
test_empty = test_empty_module.test_empty
    

def randomizeFloatArray(array):
    for i in range(array.size):
        array.flat[i] = round(random.random(), 3)

class MantaSolverTest(mt.MantaSolver):
    def __init__(self, width, height, depth, batches = 1, dt = 1):
        mt.MantaSolver.__init__(self, width, height, depth, batches, dt) 
        
    def setDensityValue(self, value):
        for i in range(self.densityGrid.size):
            self.densityGrid.flat[i] = value
        
    def randomizeDensity(self):
        randomizeFloatArray(self.densityGrid)
        
    def randomizeVelocity(self):
        randomizeFloatArray(self.velocityGrid)
    
    def pseudoRandomizeDensity(self):
        for b in range(self.batches):
            for x in range(self.width):
                for y in range(self.height):
                    for z in range(self.depth):
                            self.densityGrid[b, x, y, z] = x + y*10 + z*100
        
    def pseudoRandomizeVelocity(self):
        for b in range(self.batches):
            for x in range(self.width):
                for y in range(self.height):
                    for z in range(self.depth):
                            self.velocityGrid[b, x, y, z, 0] = float(x) / (self.width - 1)
                            self.velocityGrid[b, x, y, z, 1] = float(y) / (self.height - 1)
                            self.velocityGrid[b, x, y, z, 2] = float(z) / (self.depth - 1)
                            
                            
    def pseudoRandomizeVelocityX(self):
        for b in range(self.batches):
            for x in range(self.width):
                for y in range(self.height):
                    for z in range(self.depth):
                            self.velocityGrid[b, x, y, z, 0] = 1
                            self.velocityGrid[b, x, y, z, 1] = 0
                            self.velocityGrid[b, x, y, z, 2] = 0
        
    def randomizeFlags(self):
        for i in range(array.size):
            array.flat[i] = random.randint(1, 2)




def prepareMantaArray(array, batch = 0):
    arrayShape =array.shape
    if(len(arrayShape) == 4):                           # scalar array
        resultArray = np.empty([arrayShape[3], arrayShape[2], arrayShape[1]], np.result_type(array))
        for x in range(arrayShape[1]):
            for y in range(arrayShape[2]):
                for z in range(arrayShape[3]):
                    resultArray[z, y, x] = array[batch, x, y, z]
        return resultArray
    if(len(arrayShape) == 5):                           # vec array
        resultArray = np.empty([arrayShape[2], arrayShape[1], arrayShape[3], arrayShape[4]], np.result_type(array))
        for x in range(arrayShape[1]):
            for y in range(arrayShape[2]):
                for z in range(arrayShape[3]):
                    for i in range(arrayShape[4]):
                        resultArray[y, x, z, i] = array[batch, x, y, z, i]
        return resultArray

def getL2Error(array1, array2):
    assert array1.shape == array2.shape, "Could not evalute L2 error between arrays because of different shapes %s vs %s" % (str(array1.shape), str(array2.shape))
    arraySub = np.subtract(array1, array2)
    arrayNorm = LA.norm(arraySub, axis=3)
    printVecArray(arrayNorm)
    print np.sum(arrayNorm)
    
def printVecArray(array, batch = 0, batches = True):
    if(batches and len(array.shape) == 4):
        array = array[batch, :, :, :]
    if(batches and len(array.shape) == 5):
        array = array[batch, :, :, :, :]
        
    if(len(array.shape) == 3):                          # scalar array
        for z in range(array.shape[2]):
            grid = ''
            for y in range(array.shape[1]): 
                for x in range(array.shape[0]):
                    grid += "%0.3f, " % array[x, y, z]
                grid += '\r\n'
            print grid 
    if(len(array.shape) == 4):                           # vec array
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
            