import os
from cStringIO import StringIO
import subprocess
import numpy as np
import struct
import random

class StringBuilder:
     _file_str = None

     def __init__(self):
         self._file_str = StringIO()

     def Append(self, str):
         self._file_str.write(str)

     def AppendLine(self, str):
         self._file_str.write(str + "\r\n")
         
     def Line(self):
         self._file_str.write("\r\n")

     def __str__(self):
         return self._file_str.getvalue()


class MantaCaller:
    def __init__(self, res, seed):
        self.sb = StringBuilder()
        
        self.sb.AppendLine("from manta import *")
        self.sb.AppendLine("import random")
        self.sb.Line()
        self.sb.AppendLine("random.seed(%d)" % seed)
        self.sb.Line()
        self.sb.AppendLine("res = %d"% res)
        self.sb.AppendLine("gs  = vec3(res, int(1.5*res), res)")
        self.sb.AppendLine("s   = FluidSolver(name='main', gridSize = gs)")
        self.sb.Line()
        self.sb.AppendLine("flags    = s.create(FlagGrid)")
        self.sb.AppendLine("vel      = s.create(MACGrid)")
        self.sb.AppendLine("density  = s.create(RealGrid)")
        self.sb.AppendLine("pressure = s.create(RealGrid)")
        self.sb.Line()
        self.sb.AppendLine("flags.initDomain()")
        self.sb.AppendLine("flags.fillGrid()")
        
        
    def addSetConstDensity(self, value):
        self.sb.Line()
        self.sb.AppendLine("density.setConst(%f)" % value)

        
    def addPseudoRandomDensity(self):
        self.sb.Line()
        self.sb.AppendLine("for x in range(0, int(gs.x)):")
        self.sb.AppendLine("	for y in range(0, int(gs.y)):")
        self.sb.AppendLine("		for z in range(0, int(gs.z)):")
        self.sb.AppendLine("			density.setValue(x, y, z, random.random())")
        
    def addPseudoRandomVel(self):
        self.sb.Line()
        self.sb.AppendLine("for x in range(0, int(gs.x)):")
        self.sb.AppendLine("	for y in range(0, int(gs.y)):")
        self.sb.AppendLine("		for z in range(0, int(gs.z)):")
        self.sb.AppendLine("			vel.setValue(x, y, z, vec3(0, 0, 0))")
        
        
    def addLoop(self):
        self.sb.Line()
        self.sb.AppendLine("for t in range(250):")
        self.sb.AppendLine("	mantaMsg('\\nFrame %i' % (s.frame))")
        self.sb.AppendLine("	addBuoyancy(density=density, vel=vel, gravity=vec3(0,-6e-4,0), flags=flags)")
        self.sb.AppendLine("	s.step()")
        
        
        
    def addStep(self):
        self.sb.Line()
        self.sb.AppendLine("s.step()")
        
    def addAddBuoyancy(self):
        self.sb.Line()
        self.sb.AppendLine("addBuoyancy(density=density, vel=vel, gravity=vec3(0,-6e-4,0), flags=flags)")
	

    def addDensityToFile(self):
        self.sb.Line()
        self.sb.AppendLine("density.save('./compare_data/density_data.raw')")
        
        
    def addVelToFile(self):
        self.sb.Line()
        self.sb.AppendLine("vel.save('./compare_data/vel_data.raw')")
        
        
    def execute(self):
        file = open("./compare_data/manta_test.py", "w") 
        file.write(self.sb._file_str.getvalue())
        file.close() 
        
        output_str = subprocess.check_output(["./manta/build/manta", "compare_data/manta_test.py"])
        #print (output_str)


file_path = "./compare_data/"
directory = os.path.dirname(file_path)

if not os.path.exists(directory):
    os.makedirs(directory)
    

seed = 1
res = 4
gs = [res, int(1.5*res), res]

random.seed(seed)


mantaCaller = MantaCaller(res, seed)
mantaCaller.addSetConstDensity(1)
#mantaCaller.addPseudoRandomDensity()
mantaCaller.addPseudoRandomVel()

mantaCaller.addAddBuoyancy()
mantaCaller.addStep()

mantaCaller.addDensityToFile()
mantaCaller.addVelToFile()

mantaCaller.execute()


mantaFileVel = open("./compare_data/vel_data.raw", "r")
mantaFlatVelArray = np.fromfile(mantaFileVel, dtype='f')
mantaVelArray = mantaFlatVelArray.reshape(gs[0], gs[1], gs[2], 3)

print(mantaVelArray)


for z in range(0, gs[2]):
    grid = ''
    for y in range(0, gs[1]):
        for x in range(0, gs[0]):
            grid += '(%.3f, %.3f, %.3f), ' % (mantaVelArray[x, y, z, 0], mantaVelArray[x, y, z, 1], mantaVelArray[x, y, z, 2])
        grid += '\r\n'
    print(grid)


'''
mantaFileDensity = open("./compare_data/density_data.raw", "r")
mantaFlatDensityArray = np.fromfile(mantaFileDensity, dtype='f')
mantaDensityArray = np.swapaxes(mantaFlatDensityArray.reshape(gs[0], gs[1], gs[2]), 0, 2)

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
