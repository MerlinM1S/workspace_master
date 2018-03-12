#define EIGEN_USE_THREADS

#if GOOGLE_CUDA
#define EIGEN_USE_GPU
#endif

#include "dim_size.h"
#include "grid.h"

#include "tensorflow/core/framework/op.h"
#include "tensorflow/core/framework/shape_inference.h"
#include "tensorflow/core/framework/op_kernel.h"

using namespace tensorflow;
using namespace Manta;

REGISTER_OP("ApplyToArray")
    .Input("in_array: float")
    .Input("flags_grid: int32")
    .Input("mask: bool")
    .Input("value: float")
    .Output("out_array: float")
    .SetShapeFn([](::tensorflow::shape_inference::InferenceContext* c) {
      c->set_output(0, c->input(0));
      return Status::OK();
    });


using CPUDevice = Eigen::ThreadPoolDevice;
using GPUDevice = Eigen::GpuDevice;


template <typename Device>
struct ApplyToArray {
  void operator()(const CPUDevice& d, const DimSize dimSize, const int* in_flags, const float* in_array, const bool* mask, const float value, float* out_array, int size);
};

// CPU specialization of actual computation.
template <>
void ApplyToArray<CPUDevice>::operator()(const CPUDevice& d, const DimSize dimSize, const int* in_flags, const float* in_array, const bool* mask, const float value, float* out_array, int size) {
    FluidSolver fluidSolver = FluidSolver(Vec3i(dimSize.width, dimSize.height, dimSize.depth));

    FlagGrid flags = FlagGrid(&fluidSolver, const_cast<int*>(in_flags + dimSize.batchToIndex(4, i_b)), 3, true);

    for (int i = 0; i < size; i++) {
        if(mask[i] && !flags.isObstacle(i))
            out_array[i] = value;
        else
            out_array[i] = in_array[i];
    }
}



template <typename Device>
class ApplyToArrayOp : public OpKernel {
    public:
        explicit ApplyToArrayOp(OpKernelConstruction* context) : OpKernel(context) {}

        void Compute(OpKernelContext* context) override {
            // Grab the input tensor
            const Tensor& in_tensor = context->input(0);
            TensorShape in_shape = in_tensor.shape();

            const Tensor& flag_grid_tensor = context->input(1);
            TensorShape flag_grid_shape = flag_grid_tensor.shape();
            const int* in_flags = flag_grid_tensor.flat<int>().data();

            const Tensor& mask_tensor = context->input(2);
            TensorShape mask_shape = mask_tensor.shape();
            auto mask_flat = mask_tensor.flat<bool>();

            const Tensor& value_tensor = context->input(3);
            TensorShape value_tensor_shape = value_tensor.shape();


            OP_REQUIRES(context, in_tensor.NumElements() <= tensorflow::kint32max,
                        errors::InvalidArgument("Too many elements in tensor"));

            // Create an output tensor
            Tensor* out_tensor = NULL;
            OP_REQUIRES_OK(context, context->allocate_output(0, in_tensor.shape(), &out_tensor));


            long batches = in_shape.dim_size(0);
            long depth = in_shape.dim_size(1);
            long height = in_shape.dim_size(2);
            long width = in_shape.dim_size(3);
            DimSize dimSize = DimSize(batches, depth, height, width);

            const float value = value_tensor.scalar<float>().data()[0];


            // Do the computation.
            ApplyToArray<Device>()(
                context->eigen_device<Device>(),
                dimSize,
                in_flags,
                in_tensor.flat<float>().data(),
                mask_flat.data(),
                value,
                out_tensor->flat<float>().data(),
                static_cast<int>(in_tensor.NumElements()));

        }


};


// Register the CPU kernels.
REGISTER_KERNEL_BUILDER(Name("ApplyToArray").Device(DEVICE_CPU), ApplyToArrayOp<CPUDevice>);







