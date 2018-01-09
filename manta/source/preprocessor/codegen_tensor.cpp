#include "prep.h"
// using ofstream constructors.
#include <iostream>
#include <fstream>
#include <map>
#include <sstream>
#include <algorithm>

#define SSTR( x ) static_cast< std::ostringstream & >( \
        ( std::ostringstream() << std::dec << x ) ).str()

class TType {
public:
    string cName;
    string name;            // type name
    string hName;           // header type name
    bool isConst;
    int promisedDims;


    string pName;           // paramter type name


    bool isScalar() {
        return promisedDims == 0;
    }

    TType() { }

    TType(string _cName, string _name, string _hName, bool _isConst, int _promisedDims) : cName(_cName), name(_name), hName(_hName), isConst(_isConst), promisedDims(_promisedDims) {
        pName = name;
        if(!isScalar())
            pName += "*";
    }
};

class TArgument {
public:
    const Argument* argument;
    TType tType;

    int inIndex;
    int outIndex;


    TArgument(const Argument* _argument) : argument(_argument){
        inIndex = -1;
        outIndex = -1;



        std::map<std::string, TType> typeConverter;
        typeConverter["MACGrid"]    = TType("MACGrid",  "float",  "float", false,   5);
        typeConverter["FlagGrid"]   = TType("FlagGrid", "int",    "int32", false,   4);
        typeConverter["Grid"]       = TType("Grid",     "float",  "float", false,   4);
        typeConverter["Vec3"]       = TType("Vec3",     "float",  "float", true,    2);
        typeConverter["Real"]       = TType("float",    "float",  "float", true,    1);
        typeConverter["int"]        = TType("int",      "int",    "int32", true,    1);
        typeConverter["float"]      = TType("float",    "float",  "float", true,    1);


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

        string dimNames[5] = {"batches", "width", "height", "depth", "dim"};
        for(int i = 0; i < 5; i++) {
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
            text += "\tfor(int i = 0; i < dimSize.LengthOf(" + SSTR(tType.promisedDims) + "); i++) {\r\n";
            text += "\t\t" + getOutputName() + "[i] = " + getInputName() + "[i];\r\n";
            text += "\t}\r\n";
            text += "\r\n";
        }

        return text;
    }

    string generateVariableLine(string batch) {
        string text;

        text +=  tType.cName + " " +  argument->name + " = ";

        if(tType.promisedDims == 1) {
            text += "*";
        } else {
            text += tType.cName;
        }

        text += "(" + getOutputName() + "[" + batch + "]);\r\n";

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

    List<TArgument*> tArguments;

    TArgument* argumentWithHighestDims;

    string func_name() {
        return convertToSnake_case(funcName);
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

        text += "REGISTER_OP(\"" + funcName + "\")\r\n";

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

    string generateFuncImplementationCPU() {
        string text;

        text += generateFuncHeader(DeviceTypeCPU) + " {\r\n";


        for(unsigned int i = 0; i < tArguments.size(); i++) {
            text += tArguments[i]->generateCopyInToOutLines();
        }


        text += "\tfor(int i_b = 0; i_b < dimSize.batches; i_b++) {\r\n";
        for(unsigned int i = 0; i < tArguments.size(); i++) {
            text += "\t\t" + tArguments[i]->generateVariableLine("i_b");
        }

        text += "\t}\r\n";



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
        text += "\t\tDimSize dimSize = DimSize(batches, width, depth, height, dim);\r\n";
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
        return "REGISTER_KERNEL_BUILDER(Name(\"" + funcName + "\").Device(DEVICE_CPU), " + funcName_OP() + "<CPUDevice>);\r\n";
    }

    string generateRegisterKernelGPU() {
        string text;

        text += "#if GOOGLE_CUDA\r\n";
        text += "\r\n";
        text += "REGISTER_KERNEL_BUILDER(Name(\"" + funcName + "\").Device(DEVICE_GPU), " + funcName_OP() + "<GPUDevice>);\r\n";
        text += "\r\n";
        text += "#endif\r\n";

        return text;
    }

public:
    TensorProcessor(const Block& block, const std::string& code, Sink& sink, std::vector<Instantiation>& inst) {
        funcName = block.func.name;

        argumentWithHighestDims = 0;
        int highestDims = -1;

        {
            int inIndex = 0;
            int outIndex = 0;
            for(unsigned int i = 0; i < block.func.arguments.size(); i++) {
                TArgument* argument = new TArgument(&(block.func.arguments[i]));

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
    }


    void write() {
        std::ofstream outfile ("test.txt");

        outfile << generateHeader();

        outfile << std::endl;

        outfile << generateRegisterOP();

        outfile << std::endl;
        outfile << std::endl;

        outfile << generateFuncImplementationCPU();

        outfile << std::endl;
        outfile << std::endl;

        outfile << generateFuncOP();

        outfile << std::endl;
        outfile << std::endl;

        outfile << generateRegisterKernelCPU();

//        outfile << std::endl;
//        outfile << std::endl;

//        outfile << generateRegisterKernelGPU();

        outfile.close();
    }

    ~TensorProcessor() {
        for(unsigned int i = 0; i < tArguments.size(); i++) {
            delete (tArguments[i]);
        }
    }
};

void processTensorFunction(const Block& block, const std::string& code, Sink& sink, std::vector<Instantiation>& inst) {
    TensorProcessor tensorProcessor = TensorProcessor(block, code, sink, inst);
    tensorProcessor.write();
}
