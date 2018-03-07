#include "tensor_preprocessor_cpu.h"
#include "tensor_op.h"
#include "string_util.h"

using namespace std;


void TensorPreprocessorCPU::addIncludesEtc(CodeGenerator &codeGenerator) const {
    codeGenerator.addLine("#define EIGEN_USE_THREADS");
    codeGenerator.newLine();
    codeGenerator.addLine("#if GOOGLE_CUDA");
    codeGenerator.addLine("#define EIGEN_USE_GPU");
    codeGenerator.addLine("#endif");
    codeGenerator.newLine();
    codeGenerator.newLine();
    codeGenerator.addLine("#include \"mt_dim_size.h\"");
    codeGenerator.addLine("#include \"mt_util.h\"");
    codeGenerator.addLine("#include \"levelset.h\"");
    codeGenerator.newLine();
    if(mAddTimer) {
        codeGenerator.addLine("#include <time.h>");
        codeGenerator.newLine();
    }
    codeGenerator.addLine("#include \"tensorflow/core/framework/op.h\"");
    codeGenerator.addLine( "#include \"tensorflow/core/framework/shape_inference.h\"");
    codeGenerator.addLine("#include \"tensorflow/core/framework/op_kernel.h\"");
    codeGenerator.newLine();

    addUsingNamespaces(codeGenerator);
}

void TensorPreprocessorCPU::addRegisterOP(CodeGenerator& codeGenerator) const {
    codeGenerator.addLine("REGISTER_OP(\"" + getTensorFuncName(NS_NameStyle) + "\")", 1);

    for(size_t i = 0; i < tArguments.size(); i++) {
        tArguments[i]->addInputLine(codeGenerator);
    }

    for(size_t i = 0; i < tArguments.size(); i++) {
        tArguments[i]->addOutputLine(codeGenerator);
    }

    {
        codeGenerator.addLine(".SetShapeFn([](::tensorflow::shape_inference::InferenceContext* c) {", 1);
        for(size_t i = 0; i < tArguments.size(); i++) {
            tArguments[i]->addShapeInferenceLine(codeGenerator);
        }
        codeGenerator.addLine("return Status::OK();");
        codeGenerator.addLine("});", -1);
        codeGenerator.addLine("", -1);
    }
}

void TensorPreprocessorCPU::addUsingDefs(CodeGenerator& codeGenerator) const {
    codeGenerator.addLine("using CPUDevice = Eigen::ThreadPoolDevice;");
    codeGenerator.addLine("using GPUDevice = Eigen::GpuDevice;");
}

string TensorPreprocessorCPU::getDeviceName() const {
    return "CPUDevice";
}


void TensorPreprocessorCPU::addFuncImplementation(CodeGenerator& codeGenerator) const {
    addFuncHeader(codeGenerator);

    if(mAddTimer) {
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

    if(mAddTimer) {
        codeGenerator.addLine("clock_gettime(CLOCK_MONOTONIC, &timeStartFuncCall);");
    }


    // Func Call
    {
        string opLine = "";
        StringList arguments;

        opLine += mMantaFuncName + "(";

        for(size_t i = 0; i < tArguments.size(); i++) {
            arguments.add(tArguments[i]->generateMantaParam());
        }
        opLine += arguments.toString();

        opLine += ");";

        codeGenerator.addLine(opLine);
    }

    if(mAddTimer) {
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

    if(mAddTimer) {
        codeGenerator.addLine("clock_gettime(CLOCK_MONOTONIC, &timeFinishFunctor);");
        codeGenerator.addLine("double elapsedFunctor = (timeFinishFunctor.tv_sec - timeStartFunctor.tv_sec);");
        codeGenerator.addLine("elapsedFunctor += (timeFinishFunctor.tv_nsec - timeStartFunctor.tv_nsec) / 1000000000.0;");
        codeGenerator.addLine("double diff = elapsedFunctor - elapsedFuncCall;");
        codeGenerator.addLine("cout << \"Time (" + getTensorFuncName(NS_NameStyle) + "): \" << SSTR(elapsedFunctor) << \"  Overhead: \" << SSTR(diff) << endl;");
    }

    codeGenerator.addLine("}", -1);
}


void TensorPreprocessorCPU::addFuncOP(CodeGenerator& codeGenerator) const {
    codeGenerator.addLine("template <typename Device>");
    codeGenerator.addLine("class " + getTensorFuncName(NS_OP) + " : public OpKernel {");
    codeGenerator.addLine("public:", 1);
    codeGenerator.addLine("explicit " + getTensorFuncName(NS_OP) +"(OpKernelConstruction* context) : OpKernel(context) {}");
    codeGenerator.newLine();
    codeGenerator.addLine("void Compute(OpKernelContext* context) override {", 1);

    // InTensor
    for(size_t i = 0; i < tArguments.size(); i++) {
        tArguments[i]->addInTensorLines(codeGenerator);
    }

    // OutTensor
    for(size_t i = 0; i < tArguments.size(); i++) {
        tArguments[i]->addOutTensorLines(codeGenerator);
    }

    // DimSize
    if(mArgumentWithHighestDims) {
        mArgumentWithHighestDims->addDimSizeLines(codeGenerator);
    }

    codeGenerator.newLine();

    // Function call
    {
        string funcCall = "";
        StringList arguments;

        funcCall += getTensorFuncName(NS_Functor) + "<Device>()(";

        arguments.add("context->eigen_device<Device>()");
        arguments.add("dimSize");

        for(size_t i = 0; i < tArguments.size(); i++) {
            arguments.add(tArguments[i]->generateInParam());
            arguments.add(tArguments[i]->generateOutParam());
        }
        funcCall += arguments.toString();
        funcCall += ");";

        codeGenerator.addLine(funcCall);
    }


    codeGenerator.addLine("}", -1);
    codeGenerator.addLine("};", -1);
}

void TensorPreprocessorCPU::addRegisterKernelCPU(CodeGenerator& codeGenerator) const {
    codeGenerator.addLine("REGISTER_KERNEL_BUILDER(Name(\"" + getTensorFuncName(NS_NameStyle) + "\").Device(DEVICE_CPU), " + getTensorFuncName(NS_OP) + "<CPUDevice>);");
}

void TensorPreprocessorCPU::addRegisterKernelGPU(CodeGenerator& codeGenerator) const {
    codeGenerator.addLine("#if GOOGLE_CUDA");
    codeGenerator.newLine();
    codeGenerator.addLine("REGISTER_KERNEL_BUILDER(Name(\"" + getTensorFuncName(NS_NameStyle) + "\").Device(DEVICE_GPU), " + getTensorFuncName(NS_OP) + "<GPUDevice>);");
    codeGenerator.newLine();
    codeGenerator.addLine("#endif");
}

//    OP_REQUIRES(context, vel_shape.dims() == 5,
//                 errors::InvalidArgument("AddBuoyancy expects as first parameter a 5-D float velocity array: batches, width, height, depth, dimension"));

string TensorPreprocessorCPU::generateOpString() const {
    CodeGenerator codeGenerator;

    codeGenerator.newLine();
    codeGenerator.newLine();

    codeGenerator.addLine("// -------------------------- " + getTensorFuncName(NS_NameStyle) + "-------------------------");

    codeGenerator.newLine();

    addIncludesEtc(codeGenerator);

    codeGenerator.newLine();

    addHeader(codeGenerator);

    codeGenerator.newLine();

    addRegisterOP(codeGenerator);

    codeGenerator.newLine();
    codeGenerator.newLine();

    addUsingDefs(codeGenerator);

    codeGenerator.newLine();
    codeGenerator.newLine();

    addFuncImplementation(codeGenerator);

    codeGenerator.newLine();
    codeGenerator.newLine();

    addFuncOP(codeGenerator);

    codeGenerator.newLine();
    codeGenerator.newLine();

    addRegisterKernelCPU(codeGenerator);

//        codeGenerator.newLine();
//        codeGenerator.newLine();

//        addRegisterKernelGPU(codeGenerator);



    return codeGenerator.toString();
}

string typeToString(Type type) {
    string result = "";

    result += type.name;

    if(type.isTemplated()) {
        result += "<";
        StringList templateTypes;
        for(size_t i = 0; i < type.templateTypes.size(); i++) {
             templateTypes.add(typeToString(type.templateTypes[i]));
        }
        result += templateTypes.toString();
        result += ">";
    }

    return result;
}


string TensorPreprocessorCPU::generateBuildString() const {
    TensorOp tensorOp(mMantaFuncName, getTensorFuncName(NS_name_style));

    for(size_t i = 0; i < tArguments.size(); i++) {
        if(!tArguments[i]->isTypeConst())
            tensorOp.addReturnType(tArguments[i]->getMantaName());
    }

    for(size_t i = 0; i < tArguments.size(); i++) {
        tensorOp.addParameter(!tArguments[i]->isTypeUnkown(), typeToString(tArguments[i]->mArgument->type), tArguments[i]->getMantaName(), tArguments[i]->getDefaultValue());
    }

    return tensorOp.toString() + "\n";
}
