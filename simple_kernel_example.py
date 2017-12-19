import tensorflow as tf
import math
import numpy as np
import timeit

zero_out_module = tf.load_op_library('/home/ansorge/tensorflow/bazel-bin/tensorflow/core/user_ops/simple_kernel_example.so')
zero_out = zero_out_module.simple_kernel_example

with tf.Session(''):
    #with tf.device("/device:GPU:0"):
    #with tf.device("/cpu:0"):
        batches =2
        width = 4
        height = 6
        depth = 4
        
        velGrid = np.zeros((batches, width, height, depth, 3), dtype=np.float)
        flagsGrid = np.empty((batches, width, height, depth), dtype=np.int)
        densityGrid = np.zeros((batches, width, height, depth), dtype=np.float)
        
        start_time = timeit.default_timer()
        zero_out(velGrid, flagsGrid, densityGrid, np.array([0,-6e-4,0])).eval()
        elapsed = timeit.default_timer() - start_time
        
        print("time: %f\r\n" % elapsed)



#class ZeroOutTest(tf.test.TestCase):
#  def testZeroOut(self):
#    with self.test_session():
#      result = zero_out([5, 4, 3, 2, 1])
#      result.eval()
#      self.assertAllEqual(result.eval(), [5, 0, 0, 0, 0])

#if __name__ == "__main__":
#  tf.test.main()
