import tensorflow as tf
import math
import numpy as np
import timeit

zero_out_module = tf.load_op_library('/home/ansorge/tensorflow/bazel-bin/tensorflow/core/user_ops/simple_kernel_example.so')
zero_out = zero_out_module.simple_kernel_example

with tf.Session(''):
    #with tf.device("/device:GPU:0"):
    #with tf.device("/cpu:0"):
        rnd_array = np.random.random_integers(0, 100, (4000, 4000))
        res = 64
        gs  = tf.constant([res, int(1.5*res), res])
        array = [[1, 2], [3, 4]]
        
        x = tf.constant(np.random.rand(4000, 4000).astype(np.float32))
        
        start_time = timeit.default_timer()
        zero_out(x).eval()
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
