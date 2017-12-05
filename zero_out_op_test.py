import tensorflow as tf
import math

zero_out_module = tf.load_op_library('/home/ansorge/tensorflow/bazel-bin/tensorflow/core/user_ops/zero_out.so')
zero_out = zero_out_module.zero_out

with tf.Session(''):
    res = 64
    gs  = tf.constant([res, int(1.5*res), res])
    array = [[1, 2], [3, 4]]
    print(zero_out(array).eval())
#    print((gs * gs).eval())
    print gs.eval()



#class ZeroOutTest(tf.test.TestCase):
#  def testZeroOut(self):
#    with self.test_session():
#      result = zero_out([5, 4, 3, 2, 1])
#      result.eval()
#      self.assertAllEqual(result.eval(), [5, 0, 0, 0, 0])

#if __name__ == "__main__":
#  tf.test.main()
