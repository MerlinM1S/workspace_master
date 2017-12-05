import tensorflow as tf

manta_add_buoyancy_module = tf.load_op_library('/home/ansorge/tensorflow/bazel-bin/tensorflow/core/mantatensor/add_buoyancy.so')
manta_add_buoyancy = manta_add_buoyancy_module.add_buoyancy