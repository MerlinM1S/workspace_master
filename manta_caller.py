import os
from cStringIO import StringIO
import subprocess

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
    def __init__(self, width, height, depth, seed):
        self.sb = StringBuilder()
        
        self.sb.AppendLine("from manta import *")
        self.sb.AppendLine("import random")
        self.sb.Line()
        self.sb.AppendLine("random.seed(%d)" % seed)
        self.sb.Line()
        self.sb.AppendLine("width = %d"% width)
        self.sb.AppendLine("height = %d"% height)
        self.sb.AppendLine("depth = %d"% depth)
        self.sb.Line()
        self.sb.AppendLine("gs  = vec3(width, height, depth)")
        self.sb.AppendLine("s   = FluidSolver(name='main', gridSize = gs)")
        self.sb.Line()
        self.sb.AppendLine("flags    = s.create(FlagGrid)")
        self.sb.AppendLine("vel      = s.create(MACGrid)")
        self.sb.AppendLine("density  = s.create(RealGrid)")
        self.sb.AppendLine("pressure = s.create(RealGrid)")
        self.sb.Line()
        self.sb.AppendLine("flags.initDomain()")
        self.sb.AppendLine("flags.fillGrid()")
        self.sb.Line()
        self.sb.AppendLine("def getRandomFloat():")
        self.sb.AppendLine("	return round(random.random(), 3)")
        
        
    def addSetConstDensity(self, value):
        self.sb.Line()
        self.sb.AppendLine("density.setConst(%f)" % value)
        
        
    def addPseudoRandomDensity(self):
        self.sb.Line()
        self.sb.AppendLine("for x in range(width):")
        self.sb.AppendLine("	for y in range(height):")
        self.sb.AppendLine("		for z in range(depth):")
        self.sb.AppendLine("			density.setValue(x, y, z, getRandomFloat())")
        
    def addPseudoRandomVel(self):
        self.sb.Line()
        self.sb.AppendLine("for x in range(width):")
        self.sb.AppendLine("	for y in range(height):")
        self.sb.AppendLine("		for z in range(depth):")
        self.sb.AppendLine("			vel.setValue(x, y, z, vec3(getRandomFloat(), getRandomFloat(), getRandomFloat()))")
        
    def addPseudoRandomFlags(self):
        self.sb.Line()
        self.sb.AppendLine("for x in range(width):")
        self.sb.AppendLine("	for y in range(height):")
        self.sb.AppendLine("		for z in range(depth):")
        self.sb.AppendLine("			flags.setValue(x, y, z, random.randint(1, 2))")
        
        
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
        
    def addFlagsToFile(self):
        self.sb.Line()
        self.sb.AppendLine("flags.save('./compare_data/flags_data.raw')")
        
        
    def addVelToFile(self):
        self.sb.Line()
        self.sb.AppendLine("vel.save('./compare_data/vel_data.raw')")
        
        
    def execute(self):
        file = open("./compare_data/manta_test.py", "w") 
        file.write(self.sb._file_str.getvalue())
        file.close() 
        
        output_str = subprocess.check_output(["./manta/build/manta", "compare_data/manta_test.py"])
        #print (output_str)