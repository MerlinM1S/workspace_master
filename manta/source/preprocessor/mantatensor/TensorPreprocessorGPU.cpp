#include "TensorPreprocessorGPU.h"

using namespace std;


void TensorProcessorGPU::addIncludesEtc(CodeGenerator &codeGenerator) const {
    codeGenerator.addLine("#include \"mt_dim_size.h\"");
    codeGenerator.addLine("#include \"mt_util.h\"");
    codeGenerator.newLine();
    if(addTimer) {
        codeGenerator.addLine("#include <time.h>");
        codeGenerator.newLine();
    }
    codeGenerator.addLine("#include \"tensorflow/core/util/cuda_kernel_helper.h\"");
    codeGenerator.newLine();
    addUsingNamespaces(codeGenerator);
}


string TensorProcessorGPU::getDeviceName() const {
    return "GPUDevice";
}

void TensorProcessorGPU::addFuncImplementation(CodeGenerator& codeGenerator) const {
    addFuncHeader(codeGenerator);

    if(addTimer) {
        codeGenerator.addLine("struct timespec timeStartFunctor, timeFinishFunctor;");
        codeGenerator.addLine("struct timespec timeStartFuncCall, timeFinishFuncCall;");
        codeGenerator.addLine("clock_gettime(CLOCK_MONOTONIC, &timeStartFunctor);");
        codeGenerator.addLine("double elapsedFuncCall = 0;");
    }


    // Copy In to Out
    for(size_t i = 0; i < tArguments.size(); i++) {
        tArguments[i]->addCopyInToOutLines(codeGenerator);
    }

    codeGenerator.addLine("FluidSolver fluidSolver = FluidSolver(Vec3i(dimSize.width, dimSize.height, dimSize.depth));");
    codeGenerator.newLine();

    codeGenerator.addLine("for(int i_b = 0; i_b < dimSize.batches; i_b++) {", 1);

    // Transform In-types to Tensorflow-types
    for(size_t i = 0; i < tArguments.size(); i++) {
        tArguments[i]->addMantaVariableCreation(codeGenerator, "i_b");
    }

    codeGenerator.newLine();

    if(addTimer) {
        codeGenerator.addLine("clock_gettime(CLOCK_MONOTONIC, &timeStartFuncCall);");
    }


    // Func Call
    {
        string opLine = "";
        StringList arguments;

        opLine += mantaFuncName + "(";

        for(size_t i = 0; i < tArguments.size(); i++) {
            arguments.add(tArguments[i]->generateMantaParam());
        }
        opLine += arguments.toString();

        opLine += ");";

        codeGenerator.addLine(opLine);
    }

    if(addTimer) {
        codeGenerator.addLine("clock_gettime(CLOCK_MONOTONIC, &timeFinishFuncCall);");
        codeGenerator.addLine("elapsedFuncCall += (timeFinishFuncCall.tv_sec - timeStartFuncCall.tv_sec);");
        codeGenerator.addLine("elapsedFuncCall += (timeFinishFuncCall.tv_nsec - timeStartFuncCall.tv_nsec) / 1000000000.0;");
    }


    codeGenerator.newLine();

    // Clean up
    for(size_t i = 0; i < tArguments.size(); i++) {
        tArguments[i]->addCleanUp(codeGenerator);
    }

    codeGenerator.addLine("}", -1);

    if(addTimer) {
        codeGenerator.addLine("clock_gettime(CLOCK_MONOTONIC, &timeFinishFunctor);");
        codeGenerator.addLine("double elapsedFunctor = (timeFinishFunctor.tv_sec - timeStartFunctor.tv_sec);");
        codeGenerator.addLine("elapsedFunctor += (timeFinishFunctor.tv_nsec - timeStartFunctor.tv_nsec) / 1000000000.0;");
        codeGenerator.addLine("double diff = elapsedFunctor - elapsedFuncCall;");
        codeGenerator.addLine("cout << \"Time (" + getTensorFuncName(NS_NameStyle) + "): \" << SSTR(elapsedFunctor) << \"  Overhead: \" << SSTR(diff) << endl;");
    }

    codeGenerator.addLine("}", -1);
}

//    OP_REQUIRES(context, vel_shape.dims() == 5,
//                 errors::InvalidArgument("AddBuoyancy expects as first parameter a 5-D float velocity array: batches, width, height, depth, dimension"));

string TensorProcessorGPU::generateString() const {
    CodeGenerator codeGenerator;

    codeGenerator.newLine();
    codeGenerator.newLine();

    codeGenerator.addLine("} // namespace");

    codeGenerator.newLine();
    codeGenerator.newLine();

    codeGenerator.addLine("// -------------------------- TENSORFLOW OP ------------------------- ");

    codeGenerator.newLine();


    codeGenerator.addLine("#ifdef GOOGLE_CUDA");
    codeGenerator.addLine("#define EIGEN_USE_GPU");

    codeGenerator.newLine();

    addIncludesEtc(codeGenerator);

    codeGenerator.newLine();

    addHeader(codeGenerator);

    codeGenerator.newLine();

    codeGenerator.addLine("using GPUDevice = Eigen::GpuDevice;");

    codeGenerator.newLine();
    codeGenerator.newLine();

    addFuncImplementation(codeGenerator);

    codeGenerator.newLine();

    codeGenerator.addLine("#endif  // GOOGLE_CUDA");

    codeGenerator.newLine();


    codeGenerator.addLine("// ------------------------------------------------------------------ ");

    codeGenerator.newLine();
    codeGenerator.newLine();


    codeGenerator.addLine("namespace Manta {");


    return codeGenerator.toString();
}
