import tensorflow as tf
import numpy as np

import random
import mantatensor as mt

from numpy import linalg as LA

from PIL import Image


def randomizeFloatArray(array):
    for i in range(array.size):
        array.flat[i] = round(random.random(), 3)

class MantaSolverTest(mt.MantaSolver):
    def __init__(self, width, height, depth, batches = 1, dt = 1):
        mt.MantaSolver.__init__(self, width, height, depth, batches, dt) 
        
        
    def createEmptyDensityArray(self):
        return np.empty((self.batches, self.width, self.height, self.depth, 1), dtype=np.float32)
    
    def createEmptyVelocityArray(self):
        return np.empty((self.batches, self.width, self.height, self.depth, 3), dtype=np.float32)
    
    def createEmptyFlagsArray(self):
        return np.empty((self.batches, self.width, self.height, self.depth, 1), dtype=np.int32)
        
        
    def setDensityValue(self, value):
        array = self.createEmptyDensityArray()
        for i in range(array.size):
            array.flat[i] = value
        self.applyDensityArray(array)
        
    def randomizeDensity(self):
        array = self.createEmptyDensityArray()
        randomizeFloatArray(array)
        self.applyDensityArray(array)
        
    def randomizeVelocity(self):
        array = self.createEmptyVelocityArray()
        randomizeFloatArray(array)
        self.applyVelocityArray(array)
    
    def pseudoRandomizeDensity(self):
        array = self.createEmptyDensityArray()
        for b in range(self.batches):
            for x in range(self.width):
                for y in range(self.height):
                    for z in range(self.depth):
                            array[b, x, y, z] = x + y*10 + z*100
        self.applyDensityArray(array)
        
    def pseudoRandomizeVelocity(self):
        array = self.createEmptyVelocityArray()
        for b in range(self.batches):
            for x in range(self.width):
                for y in range(self.height):
                    for z in range(self.depth):
                            array[b, x, y, z, 0] = float(x) / (self.width - 1)
                            array[b, x, y, z, 1] = float(y) / (self.height - 1)
                            array[b, x, y, z, 2] = float(z) / (self.depth - 1)
        self.applyVelocityArray(array)
                            
                            
    def pseudoRandomizeVelocityX(self):
        array = self.createEmptyVelocityArray()
        for b in range(self.batches):
            for x in range(self.width):
                for y in range(self.height):
                    for z in range(self.depth):
                            array[b, x, y, z, 0] = 1
                            array[b, x, y, z, 1] = 0
                            array[b, x, y, z, 2] = 0
        self.applyVelocityArray(array)
        
    def randomizeFlags(self):
        array = self.createEmptyFlagsArray()
        for i in range(array.size):
            array.flat[i] = random.randint(1, 2)
        self.applyFlagArray(array)


def createImage(name, array, slice, batch = 0, scale = 1):
    arrayShape = array.shape
    
    width = arrayShape[1]
    height = arrayShape[2]
        
    im = Image.new("RGB", (width, height))
    pix = im.load()
    
    if(arrayShape[4] == 1):
        for x in range(width):
            for y in range(height):
                val = array[batch, x, y, slice]
                pix[x, height - y - 1] = (int(val*255), int(val*255), int(val*255))    
    if(arrayShape[4] == 3):
        for x in range(width):
            for y in range(height):
                pix[x, height - y - 1] = (int(array[batch, x, y, slice, 0]*255), int(array[batch, x, y, slice, 1]*255), int(array[batch, x, y, slice, 2]*255))

    im.save("tensor_" +name + ".png", "PNG")
    
    
def createImageGrid(name, grid, width, height, slice, batch = 0, scale = 1):        
    im = Image.new("RGB", (width, height))
    pix = im.load()
    
    if(isinstance(grid, RealGrid)):      
        for x in range(width):
            for y in range(height):
                val = grid.getValue(x, y, slice)
                pix[x, height - y - 1] =  (int(val*255), int(val*255), int(val*255))
    if(isinstance(grid, MACGrid)):     
        for x in range(width):
            for y in range(height):
                vec = grid.getValue(x, y, slice)
                pix[x, height - y - 1] =  (int(vec.x*255), int(vec.y*255), int(vec.z*255))

    im.save("flow_" + name + ".png", "PNG")
    
    
def createImageBool(name, array, slice, scale = 1):
    arrayShape = array.shape
    
    width = arrayShape[0]
    height = arrayShape[1]
        
    im = Image.new("RGB", (width, height))
    pix = im.load()
    for x in range(width):
        for y in range(height):
            if(array[x, y, slice]):
                pix[x,y] = (255, 255, 255)
            else:
                pix[x,y] = (0, 0, 0)
            

    im.save( name + ".png", "PNG")


def prepareMantaArray(array, batch = 0):
    arrayShape = array.shape
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
            
