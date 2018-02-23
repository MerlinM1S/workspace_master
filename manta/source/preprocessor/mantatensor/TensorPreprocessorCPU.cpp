#include "TensorPreprocessorCPU.h"

using namespace std;

string TensorProcessor::getTensorFuncName(NameStyle nameStyle) {
    switch(nameStyle) {
    case NS_name_style:
        return convertToSnake_case(tensorFuncName);
    case NS_NameStyle: {
        string result = tensorFuncName;
        result[0] = toupper(result[0]);
        return result;
    }
    case NS_Functor:
        return tensorFuncName + "_Functor";
    case NS_OP:
        return tensorFuncName + "_OP";
    default:
        return tensorFuncName;
    }
}


void TensorProcessor::addIncludesEtc(CodeGenerator &codeGenerator) {
    codeGenerator.addLine("#define EIGEN_USE_THREADS");
    codeGenerator.newLine();
    codeGenerator.addLine("#if GOOGLE_CUDA");
    codeGenerator.addLine("#define EIGEN_USE_GPU");
    codeGenerator.addLine("#endif");
    codeGenerator.newLine();
    codeGenerator.newLine();
    codeGenerator.addLine("#include \"mt_dim_size.h\"");
    codeGenerator.addLine("#include \"mt_util.h\"");
    codeGenerator.newLine();
    if(addTimer) {
        codeGenerator.addLine("#include <time.h>");
        codeGenerator.newLine();
    }
    codeGenerator.addLine("#include \"tensorflow/core/framework/op.h\"");
    codeGenerator.addLine( "#include \"tensorflow/core/framework/shape_inference.h\"");
    codeGenerator.addLine("#include \"tensorflow/core/framework/op_kernel.h\"");
    codeGenerator.newLine();
    codeGenerator.addLine("using namespace tensorflow;");
    codeGenerator.addLine("using namespace Manta;");
    codeGenerator.addLine("using namespace mantatensor;");
}

void TensorProcessor::addHeader(CodeGenerator &codeGenerator) {
    StringList arguments;
    string opLine = "";

    codeGenerator.addLine("template <typename Device>");
    codeGenerator.addLine("struct " + getTensorFuncName(NS_Functor) + " {", 1);
    opLine += "void operator()(";
    arguments.add("const Device& d");

    arguments.add("const DimSize dimSize");
    for(size_t i = 0; i < tArguments.size(); i++) {
        arguments.add(tArguments[i]->generateInHeaderParam());
        arguments.add(tArguments[i]->generateOutHeaderParam());
    }
    opLine += arguments.toString();
    opLine += ");";

    codeGenerator.addLine(opLine);
    codeGenerator.addLine("};", -1);
}

void TensorProcessor::addRegisterOP(CodeGenerator& codeGenerator) {
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

void TensorProcessor::addUsingDefs(CodeGenerator& codeGenerator) {
    codeGenerator.addLine("using CPUDevice = Eigen::ThreadPoolDevice;");
    codeGenerator.addLine("using GPUDevice = Eigen::GpuDevice;");
}


void TensorProcessor::addFuncHeader(CodeGenerator& codeGenerator) {
    StringList arguments;
    string opLine = "";

    codeGenerator.addLine("template <>");
    opLine += "void " + getTensorFuncName(NS_Functor) + "<CPUDevice>::operator()(";
    arguments.add("const CPUDevice& d");

    arguments.add("const DimSize dimSize");
    for(size_t i = 0; i < tArguments.size(); i++) {
        arguments.add(tArguments[i]->generateInHeaderParam());
        arguments.add(tArguments[i]->generateOutHeaderParam());
    }
    opLine += arguments.toString();
    opLine += ") {";

    codeGenerator.addLine(opLine, 1);
}


void TensorProcessor::addFuncImplementationCPU(CodeGenerator& codeGenerator) {
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


void TensorProcessor::addFuncOP(CodeGenerator& codeGenerator) {
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
    if(argumentWithHighestDims) {
        argumentWithHighestDims->addDimSizeLines(codeGenerator);
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

void TensorProcessor::addRegisterKernelCPU(CodeGenerator& codeGenerator) {
    codeGenerator.addLine("REGISTER_KERNEL_BUILDER(Name(\"" + getTensorFuncName(NS_NameStyle) + "\").Device(DEVICE_CPU), " + getTensorFuncName(NS_OP) + "<CPUDevice>);");
}

void TensorProcessor::addRegisterKernelGPU(CodeGenerator& codeGenerator) {
    codeGenerator.addLine("#if GOOGLE_CUDA");
    codeGenerator.newLine();
    codeGenerator.addLine("REGISTER_KERNEL_BUILDER(Name(\"" + getTensorFuncName(NS_NameStyle) + "\").Device(DEVICE_GPU), " + getTensorFuncName(NS_OP) + "<GPUDevice>);");
    codeGenerator.newLine();
    codeGenerator.addLine("#endif");
}

TensorProcessor::TensorProcessor(const SimpleBlock& sBlock, const std::string& code, Sink& sink, bool _addTimer) : addTimer(_addTimer) {
    const Block* block = &sBlock.block;
    tensorFuncName = sBlock.tensorFuncName;
    mantaFuncName = sBlock.mantaFuncName;

    argumentWithHighestDims = 0;
    int highestDims = -1;

    errorMsg = "";

    if(block->func.returnType.name.length() == 0) {
        ignoredMsg = "Constructors can not be converted.";
        return;
    }

    if(block->parent) {
        ignoredMsg = "Methods can not be converted.";
        return;
    }

    if(block->func.returnType.name.compare("void") != 0) {
        errorMsg = "Functions may not have a return value.";
        return;
    }

    {
        int inIndex = 0;
        int outIndex = 0;
        for(unsigned int i = 0; i < block->func.arguments.size(); i++) {
            TArgument* argument = TArgument::create(&(block->func.arguments[i]));

            if(!argument) {
                errorMsg = block->func.arguments[i].type.toString() + " is not a valid parameter type.";
                return;
            }

            argument->applyIndex(inIndex, outIndex);

            tArguments.push_back(argument);

            if(argument->tType.promisedDims > highestDims) {
                highestDims = argument->tType.promisedDims ;
                argumentWithHighestDims = argument;
            }
        }

        if(outIndex > 1) {
            errorMsg = "Too many non-const parameters.";
            return;
        }

        if(!argumentWithHighestDims || argumentWithHighestDims->tType.promisedDims < 3) {
            errorMsg = "Functions require at least one parameter of type Grid.";
            return;
        }

    }

    filename = sink.getFilename() + "wat";
}

bool TensorProcessor::canConvert() {
    return errorMsg.length() == 0 && ignoredMsg.length() == 0;
}

bool TensorProcessor::threwError() {
    return errorMsg.length() != 0;
}

string TensorProcessor::getErrorMsg() {
    return errorMsg;
}


//    OP_REQUIRES(context, vel_shape.dims() == 5,
//                 errors::InvalidArgument("AddBuoyancy expects as first parameter a 5-D float velocity array: batches, width, height, depth, dimension"));

string TensorProcessor::generateString() {
    CodeGenerator codeGenerator;

    codeGenerator.newLine();
    codeGenerator.newLine();

    codeGenerator.addLine("} // namespace");

    codeGenerator.newLine();
    codeGenerator.newLine();

    codeGenerator.addLine("// -------------------------- TENSORFLOW OP ------------------------- ");

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

    addFuncImplementationCPU(codeGenerator);

    codeGenerator.newLine();
    codeGenerator.newLine();

    addFuncOP(codeGenerator);

    codeGenerator.newLine();
    codeGenerator.newLine();

    addRegisterKernelCPU(codeGenerator);

//        codeGenerator.newLine();
//        codeGenerator.newLine();

//        addRegisterKernelGPU(codeGenerator);

    codeGenerator.newLine();


    codeGenerator.addLine("// ------------------------------------------------------------------ ");

    codeGenerator.newLine();
    codeGenerator.newLine();


    codeGenerator.addLine("namespace Manta {");


    return codeGenerator.toString();
}

string TensorProcessor::getOpName() {
    return getTensorFuncName(NS_name_style);
}

TensorProcessor::~TensorProcessor() {
    for(size_t i = 0; i < tArguments.size(); i++) {
        delete (tArguments[i]);
    }
}
