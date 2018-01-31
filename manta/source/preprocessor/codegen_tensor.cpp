#include "prep.h"
// using ofstream constructors.
#include <iostream>
#include <fstream>
#include <map>
#include <sstream>
#include <algorithm>


#define SSTR( x ) static_cast< std::ostringstream & >( \
        ( std::ostringstream() << std::dec << x ) ).str()

using namespace std;


enum TType {
    TTypeMACGrid, TTypeFlagGrid, TTypeRealGrid, TTypeVec3, TTypeReal, TTypeFloat, TTypeInt, TTypeBool
};

class TTypeOp {
public:
    string cName;
    string name;            // type name
    string hName;           // header type name
    bool isConst;
    int promisedDims;


    string pName;           // paramter type name

    TType tType;


    bool isScalar() {
        return promisedDims == 0;
    }

       TTypeOp() {}

    TTypeOp(TType _tType) : tType(_tType) {
        switch(tType) {
        case TTypeMACGrid:
            cName = "MACGrid";
            name = "float";
            hName = "float";
            isConst = false;
            promisedDims = 5;
            break;
        case TTypeFlagGrid:
            cName = "FlagGrid";
            name = "int";
            hName = "int32";
            isConst = false;
            promisedDims = 4;
            break;
        case TTypeRealGrid:
            cName = "Grid<float>";
            name = "float";
            hName = "float";
            isConst = false;
            promisedDims = 4;
            break;
        case TTypeVec3:
            cName = "Vec3";
            name = "float";
            hName = "float";
            isConst = true;
            promisedDims = 2;
            break;
        case TTypeReal:
        case TTypeFloat:
            cName = "float";
            name = "float";
            hName = "float";
            isConst = true;
            promisedDims = 1;
            break;
        case TTypeInt:
            cName = "int";
            name = "int";
            hName = "int32";
            isConst = true;
            promisedDims = 1;
            break;
        case TTypeBool:
            cName = "bool";
            name = "bool";
            hName = "bool";
            isConst = true;
            promisedDims = 1;
            break;
        }

        pName = name;
        if(!isScalar())
            pName += "*";
    }

    TTypeOp(string _cName, string _name, string _hName, bool _isConst, int _promisedDims) : cName(_cName), name(_name), hName(_hName), isConst(_isConst), promisedDims(_promisedDims) {
        pName = name;
        if(!isScalar())
            pName += "*";
    }

    virtual string makeVariable(string batch, string baseVariableName, bool constCast) {
        switch(tType) {
        case TTypeFlagGrid:
            return "FlagGrid(&fluidSolver, " + AddConstCast(baseVariableName + " + dimSize.batchToIndex(4, " + batch + ")", constCast) + ", true);\r\n";
        case TTypeRealGrid:
            return "Grid<float>(&fluidSolver, " + AddConstCast(baseVariableName + " + dimSize.batchToIndex(4, " + batch + ")", constCast) + ", true);\r\n";
        case TTypeMACGrid:
            return "MACGrid(&fluidSolver, (Vec3*) (" + AddConstCast(baseVariableName + " + dimSize.batchToIndex(4, " + batch + ")", constCast) +  "), true);\r\n";
        case TTypeVec3:
            return "Vec3(" +  baseVariableName + " + (3 * " + batch + "));\r\n";
        default:
            return baseVariableName + "[" + batch + "];\r\n";
        }
    }

private:
    string AddConstCast(string arrayPointer, bool constCast) {
        if(constCast) {
            return "const_cast<" + pName + ">(" +  arrayPointer + ")";
        } else {
            return arrayPointer;
        }
    }
};

class TArgument {
public:
    const Argument* argument;
    TTypeOp tType;

    int inIndex;
    int outIndex;


    TArgument(const Argument* _argument) : argument(_argument){
        inIndex = -1;
        outIndex = -1;

        std::map<std::string, TTypeOp> typeConverter;
        typeConverter["MACGrid"]    = TTypeOp(TTypeMACGrid);
        typeConverter["FlagGrid"]   = TTypeOp(TTypeFlagGrid);
        typeConverter["Grid"]       = TTypeOp(TTypeRealGrid);
        typeConverter["Vec3"]       = TTypeOp(TTypeVec3);
        typeConverter["Real"]       = TTypeOp(TTypeReal);
        typeConverter["float"]      = TTypeOp(TTypeFloat);
        typeConverter["int"]        = TTypeOp(TTypeInt);
        typeConverter["bool"]        = TTypeOp(TTypeBool);


        if (typeConverter.find(argument->type.name) != typeConverter.end()) {
            tType = typeConverter[argument->type.name];
        }
    }

    bool isTypeConst() {
        return argument->type.isConst || tType.isConst;
    }

    string getInputName() {
        return "in_" + argument->name;
    }

    string getOutputName() {
        return "out_" + argument->name;
    }

    string getInTensorName() {
        return getInputName() + "_tensor";
    }

    string getOutTensorName() {
        return getOutputName() + "_tensor";
    }

    string generateDimSizeLines() {
        string text;

        string dimNames[4] = {"batches", "depth", "height", "width"};
        for(int i = 0; i < 4; i++) {
            text += "\t\tlong " + dimNames[i] + " = ";

            if(tType.promisedDims > i)
               text += getInTensorName() + ".shape().dim_size(" + SSTR(i) + ");\r\n";
            else
               text += "-1;\r\n";
        }

        return text;
    }

    string generateInputLine() {
        if(inIndex < 0)
            return "";
        else
            return "\t.Input(\"" + getInputName() + ": " + tType.hName + "\")" + "\r\n";
    }

    string generateOutputLine() {
        if(outIndex < 0)
            return "";
        else
            return "\t.Output(\"" + getOutputName() + ": " + tType.hName + "\")" + "\r\n";
    }

    string generateShapeInferenceLine() {
        if(outIndex < 0 || inIndex < 0)
            return "";
        else
            return "\t\tc->set_output(" + SSTR(outIndex) + ", c->input(" + SSTR(inIndex) + "));" + "\r\n";
    }

    string generateInHeaderParam() {
        if(inIndex < 0)
            return "";
        else
            return "const " + tType.pName + " " + getInputName() + ", ";
    }

    string generateOutHeaderParam() {
        if(outIndex < 0)
            return "";
        else
            return tType.pName + " " + getOutputName() + ", ";
    }

    string generateHeaderParam() {
        return generateInHeaderParam() + generateOutHeaderParam();
    }

    string generateInTensorLines() {
        string text = "";
        if(inIndex >= 0) {
            text += "\t\tconst Tensor& " + getInTensorName() + " = context->input(" + SSTR(inIndex) + ");\r\n";

            if(tType.isScalar()) {
                text += "\t\tconst " + tType.pName + " " + getInputName() + " = " + getInTensorName() + ".scalar<" + tType.name + ">().data()[0];\r\n";
            } else {
                text += "\t\tconst " + tType.pName + " " + getInputName() + " = " + getInTensorName() + ".flat<" + tType.name + ">().data();\r\n";
            }

            text += "\r\n";
        }
        return text;
    }

    string generateOutTensorLines() {
        string text = "";
        if(outIndex >= 0) {
            text += "\t\tTensor* " + getOutTensorName() + " = NULL;\r\n";
            text += "\t\tOP_REQUIRES_OK(context, context->allocate_output(" + SSTR(outIndex) + ", " + getInTensorName() + ".shape(), &" + getOutTensorName() + "));\r\n";
            text += "\t\t" + tType.pName + " " + getOutputName() + " = " + getOutTensorName() + "->flat<" + tType.name + ">().data();\r\n";
            text += "\r\n";
        }
        return text;
    }


    string generateInParam() {
        if(inIndex < 0)
            return "";
        else
            return getInputName() + ", ";
    }

    string generateOutParam() {
        if(outIndex < 0)
            return "";
        else
            return getOutputName() + ", ";
    }

    string generateParam() {
        return generateInParam() + generateOutParam();
    }


    string generateCopyInToOutLines() {
        string text = "";

        if(outIndex >= 0 && inIndex >= 0) {
            text += "\tfor(int i = 0; i < dimSize.lengthOf(" + SSTR(tType.promisedDims) + "); i++) {\r\n";
            text += "\t\t" + getOutputName() + "[i] = " + getInputName() + "[i];\r\n";
            text += "\t}\r\n";
            text += "\r\n";
        }

        return text;
    }

    string generateVariableLine(string batch) {
        string text;

        text +=  tType.cName + " " +  argument->name + " = ";

        if(outIndex >= 0) {
            text += tType.makeVariable(batch, getOutputName(), false);
        } else if (inIndex >= 0){
            text += tType.makeVariable(batch, getInputName(), true);
        }


        return text;
    }

};

string convertToSnake_case(string camelCase) {
    std::string str(1, tolower(camelCase[0]));

    // First place underscores between contiguous lower and upper case letters.
    // For example, `_LowerCamelCase` becomes `_Lower_Camel_Case`.
    for (string::iterator it = camelCase.begin() + 1; it != camelCase.end(); ++it) {
      if (isupper(*it) && *(it-1) != '_' && islower(*(it-1))) {
        str += "_";
      }
      str += *it;
    }

    // Then convert it to lower case.
    std::transform(str.begin(), str.end(), str.begin(), ::tolower);

    return str;
}
class TensorProcessor {
private:
    string funcName;
    string filename;

    List<TArgument*> tArguments;

    TArgument* argumentWithHighestDims;

    string func_name() {
        return convertToSnake_case(funcName);
    }

    string FuncName() {
        string result = funcName;
        result[0] = toupper(result[0]);
        return result;
    }

    string funcName_Functor() {
        return funcName + "_Functor";
    }

    string funcName_OP() {
        return funcName + "_OP";
    }

    enum DeviceType {
        DeviceTypeGeneral, DeviceTypeCPU, DeviceTypeGPU
    };

    string generateIncludesEtc() {
        string result = "";

        result += "#define EIGEN_USE_THREADS\r\n";
        result += "\r\n";
        result += "#if GOOGLE_CUDA\r\n";
        result += "#define EIGEN_USE_GPU\r\n";
        result += "#endif\r\n";
        result += "\r\n";
        result += "#include \"dim_size.h\"\r\n";
        result += "\r\n";
        result += "#include \"tensorflow/core/framework/op.h\"\r\n";
        result += "#include \"tensorflow/core/framework/shape_inference.h\"\r\n";
        result += "#include \"tensorflow/core/framework/op_kernel.h\"\r\n";
        result += "\r\n";
        result += "using namespace tensorflow;\r\n";
        result += "using namespace Manta;\r\n";

        return result;
    }

    string generateFuncHeader(DeviceType deviceType) {
        string text;


        string deviceName;
        switch(deviceType) {
        case DeviceTypeGeneral:
            deviceName = "Device";
            break;
        case DeviceTypeCPU:
            deviceName = "CPUDevice";
            break;
        case DeviceTypeGPU:
            deviceName = "GPUDevice";
            break;
        }


        switch(deviceType) {
        case DeviceTypeGeneral:
            text += "template <typename Device>\r\n";
            text += "struct " + funcName_Functor() + " {\r\n";
            text += "\tvoid operator()(const Device& d, ";
            break;
        case DeviceTypeCPU:
        case DeviceTypeGPU:
            text += "template <>\r\n";
            text += "void " + funcName_Functor() + "<" + deviceName + ">::operator()(const " + deviceName + "& d, ";
            break;
        }

        text += "const DimSize dimSize, ";
        for(unsigned int i = 0; i < tArguments.size(); i++) {
            text += tArguments[i]->generateHeaderParam();
        }
        text = text.substr(0, text.size() - 2);            // Remove last ", "
        text += ")";

        return text;
    }

    string generateHeader() {
        string text;

        text += generateFuncHeader(DeviceTypeGeneral) + ";\r\n";
        text += "};\r\n";

        return text;
    }

    string generateRegisterOP() {
        string text;

        text += "REGISTER_OP(\"" + FuncName() + "\")\r\n";

        for(unsigned int i = 0; i < tArguments.size(); i++) {
            text += tArguments[i]->generateInputLine();
        }

        for(unsigned int i = 0; i < tArguments.size(); i++) {
            text += tArguments[i]->generateOutputLine();
        }

        {
            text += "\t.SetShapeFn([](::tensorflow::shape_inference::InferenceContext* c) {\r\n";
            for(unsigned int i = 0; i < tArguments.size(); i++) {
                text += tArguments[i]->generateShapeInferenceLine();
            }
            text += "\t\treturn Status::OK();\r\n";
            text += "\t});\r\n";
        }

        return text;
    }

    string generateUsingDefs() {
        string result = "";

        result += "using CPUDevice = Eigen::ThreadPoolDevice;\r\n";
        result += "using GPUDevice = Eigen::GpuDevice;\r\n";

        return result;
    }

    string generateFuncImplementationCPU() {
        string text;

        text += generateFuncHeader(DeviceTypeCPU) + " {\r\n";

        // Copy In to Out
        for(unsigned int i = 0; i < tArguments.size(); i++) {
            text += tArguments[i]->generateCopyInToOutLines();
        }

        text += "\tFluidSolver fluidSolver = FluidSolver(Vec3i(dimSize.width, dimSize.height, dimSize.depth));\r\n\r\n";


        text += "\tfor(int i_b = 0; i_b < dimSize.batches; i_b++) {\r\n";

        // Transform In-types to Tensorflow-types
        for(unsigned int i = 0; i < tArguments.size(); i++) {
            text += "\t\t" + tArguments[i]->generateVariableLine("i_b");
        }

        text += "\r\n";

        // Func Call
        {
            text += "\t\t" + funcName + "(";

            for(unsigned int i = 0; i < tArguments.size(); i++) {
                text += tArguments[i]->argument->name + ", ";
            }
            text = text.substr(0, text.size() - 2);            // Remove last ", "

            text += ");\r\n";
        }


        text += "\t}\r\n\r\n";




        text += "}\r\n";

        return text;
    }


    string generateFuncOP() {
        string text;

        text += "template <typename Device>\r\n";
        text += "class " + funcName_OP() + " : public OpKernel {\r\n";
        text += "public:\r\n";
        text += "\texplicit " + funcName_OP() +"(OpKernelConstruction* context) : OpKernel(context) {}\r\n";
        text += "\r\n";
        text += "\tvoid Compute(OpKernelContext* context) override {\r\n";

        // InTensor
        for(unsigned int i = 0; i < tArguments.size(); i++) {
            text += tArguments[i]->generateInTensorLines();
        }

        // OutTensor
        for(unsigned int i = 0; i < tArguments.size(); i++) {
            text += tArguments[i]->generateOutTensorLines();
        }


        // DimSize
        text += argumentWithHighestDims->generateDimSizeLines();
        text += "\t\tDimSize dimSize = DimSize(batches, depth, height, width);\r\n";    // TODO might be: batches, width, depth, height
        text += "\r\n";

        // Function call
        {
            string funcCall;
            funcCall += "\t\t" + funcName_Functor() + "<Device>()(\r\n";
            funcCall += "\t\t\tcontext->eigen_device<Device>(), \r\n";
            funcCall += "\t\t\tdimSize, \r\n";

            funcCall += "\t\t\t";
            for(unsigned int i = 0; i < tArguments.size(); i++) {
                funcCall += tArguments[i]->generateParam();
            }
            funcCall = funcCall.substr(0, funcCall.size() - 2);            // Remove last ", "

            text += funcCall + ");\r\n";
        }


        text += "\t}\r\n";
        text += "};\r\n";

        return text;
    }

    string generateRegisterKernelCPU() {
        return "REGISTER_KERNEL_BUILDER(Name(\"" + FuncName() + "\").Device(DEVICE_CPU), " + funcName_OP() + "<CPUDevice>);\r\n";
    }

    string generateRegisterKernelGPU() {
        string text;

        text += "#if GOOGLE_CUDA\r\n";
        text += "\r\n";
        text += "REGISTER_KERNEL_BUILDER(Name(\"" + FuncName() + "\").Device(DEVICE_GPU), " + funcName_OP() + "<GPUDevice>);\r\n";
        text += "\r\n";
        text += "#endif\r\n";

        return text;
    }

public:
    TensorProcessor(const Block& block, const std::string& code, Sink& sink) {
        funcName = block.func.name;

        argumentWithHighestDims = 0;
        int highestDims = -1;

        for(int i = 0; i < block.func.templateTypes.size(); i++) {
            cout << block.func.templateTypes[i].name << endl;
        }

        {
            int inIndex = 0;
            int outIndex = 0;
            for(unsigned int i = 0; i < block.func.arguments.size(); i++) {
                TArgument* argument = new TArgument(&(block.func.arguments[i]));



                if(argument->argument->type.isTemplated()) {
                    cout << argument->argument->name << "Type: " << argument->argument->type.templateTypes[0].name << endl;
                }

                argument->inIndex = inIndex;
                inIndex++;
                if(!argument->isTypeConst()) {
                    argument->outIndex = outIndex;
                    outIndex++;
                }

                tArguments.push_back(argument);

                if(argument->tType.promisedDims > highestDims) {
                    highestDims = argument->tType.promisedDims ;
                    argumentWithHighestDims = argument;
                }
            }
        }

        filename = sink.getFilename() + "wat";
    }

    string generateString() {
        std::stringstream ss;

        ss << endl;
        ss << endl;

        ss << "} // namespace" << endl;

        ss << endl;
        ss << endl;

        ss << "// -------------------------- TENSORFLOW OP ------------------------- " << endl;

        ss << endl;

        ss << generateIncludesEtc();

        ss << endl;

        ss << generateHeader();

        ss << std::endl;

        ss << generateRegisterOP();

        ss << endl;
        ss << endl;

        ss << generateUsingDefs();

        ss << std::endl;
        ss << std::endl;

        ss << generateFuncImplementationCPU();

        ss << std::endl;
        ss << std::endl;

        ss << generateFuncOP();

        ss << std::endl;
        ss << std::endl;

        ss << generateRegisterKernelCPU();

//        ss << std::endl;
//        ss << std::endl;

//        ss << generateRegisterKernelGPU();

        ss << endl;


        ss << "// ------------------------------------------------------------------ " << endl;

        ss << endl;
        ss << endl;


        ss << "namespace manta {" << endl;


        return ss.str();
    }

    void write() {
        std::ofstream outfile (filename.c_str());

        outfile << generateString();

        outfile.close();
    }

    string getOpName() {
        return func_name();
    }

    ~TensorProcessor() {
        for(unsigned int i = 0; i < tArguments.size(); i++) {
            delete (tArguments[i]);
        }
    }
};

void processTensorFunction(const Block& block, const string& code, Sink& sink) {
    TensorProcessor tensorProcessor = TensorProcessor(block, code, sink);
    sink.buildInfo << tensorProcessor.getOpName() << endl;
    sink.inplace << tensorProcessor.generateString();

    //tensorProcessor.write();
}
