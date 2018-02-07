import mantatensor as mt

import tensorflow as tf

import platform
print("Python version: " + platform.python_version())

mantatensor_module = tf.load_op_library('/home/ansorge/tensorflow/bazel-bin/tensorflow/core/mantatensor/mantatensor.so')
manta_add_buoyancy = mantatensor_module.add_buoyancy
manta_advect_semi_lagrange_mac_grid = mantatensor_module.advect_semi_lagrange_mac_grid
manta_advect_semi_lagrange_grid_vec3 = mantatensor_module.advect_semi_lagrange_grid_vec3
manta_advect_semi_lagrange_gridfloat = mantatensor_module.advect_semi_lagrange_gridfloat


import numpy as np

with tf.Session(''):
	velocityArray = np.ones((1, 4, 6, 4, 3), dtype=np.float32)

	

	velocityTensor = tf.Variable(velocityArray)
	mt.manta_add_buoyancy(np.ones((1, 4, 6, 4), dtype=np.int32), np.ones((1, 4, 6, 4), dtype=np.float32), velocityArray, np.array([0., -0.2, 0.0]), np.array([1])).eval()
