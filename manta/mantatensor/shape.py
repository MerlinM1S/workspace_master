import numpy as np

class Shape:
    def __init__(self, width, height, depth):        
        self.width = width
        self.height = height
        self.depth = depth
        
        self.mask = np.empty((width, height, depth, 1), dtype=np.bool_)
        
    def getMask(self):
        return self.mask
        


class Cylinder(Shape):
    def __init__(self, width, height, depth, center, radius, z):
        Shape.__init__(self, width, height, depth) 
        
        
        self.center = center
        self.radiusSq = radius*radius
        self.z = norm(z)
        self.zDir = np.multiply(z, self.z)
        
        
        self.calculate()
        
    def setZ(self, z):
        self.z = z.normalize()

    def calculate(self):
        for x in range(self.width):
            for y in range(self.height):
                for z in range(self.depth):
                    self.mask[x, y, z] = self.isInside([x, y, z])
    
    def isInside(self, pos):
        distVec = np.subtract(pos, self.center)
        z = np.dot(distVec, self.zDir)
	if (abs(z) > self.z):
            return False
	r2 = normSquare(distVec) - z*z;
	return r2 < self.radiusSq

def normSquare(vec):
    return np.sum(np.square(vec))
    
def norm(vec):
    return 1 / normSquare(vec)