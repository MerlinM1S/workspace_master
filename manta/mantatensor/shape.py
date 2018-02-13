import numpy as np
import math

class Shape:
    def __init__(self, width, height, depth):        
        self.width = width
        self.height = height
        self.depth = depth
        
        self.mask = np.empty((width, height, depth, 1), dtype=np.int32)	#np.bool_
        
    def getMask(self):
        return self.mask
        


class Cylinder(Shape):
    def __init__(self, width, height, depth, bot, radius, dir):
        Shape.__init__(self, width, height, depth) 
        
        self.bot = bot
        self.radiusSq = radius*radius
        self.length = math.sqrt(dir[0]*dir[0] + dir[1]*dir[1] + dir[2]*dir[2])
        self.dir = [dir[0]/self.length, dir[1]/self.length, dir[2]/self.length]
        
        
        #self.z = norm(z)
        #self.zDir = np.multiply(z, self.z)
        
        
        self.calculate()
        
    def setZ(self, z):
        self.z = z.normalize()

    def calculate(self):
        for x in range(self.width):
            for y in range(self.height):
                for z in range(self.depth):
                    self.mask[x, y, z] = self.isInside([x, y, z])
    '''
    def isInside(self, pos):
        distVec = np.subtract(pos, self.center)
        z = np.dot(distVec, self.zDir)
	if (abs(z) > self.z):
            return False
	r2 = normSquare(distVec) - z*z;
	return r2 < self.radiusSq
    '''
    def isInside(self, pos):
	pdx = pos[0] - self.bot[0]	
	pdy = pos[1] - self.bot[1]
	pdz = pos[2] - self.bot[2]

	dot = pdx * self.dir[0] + pdy * self.dir[1] + pdz * self.dir[2];

	if(abs(dot) > self.length):
            return False
	else:
            dsq = (pdx*pdx + pdy*pdy + pdz*pdz) - dot*dot;
            return dsq < self.radiusSq
            
	
    
    
def normSquare(vec):
    return np.sum(np.square(vec))
    
def norm(vec):
    return 1 / normSquare(vec)

