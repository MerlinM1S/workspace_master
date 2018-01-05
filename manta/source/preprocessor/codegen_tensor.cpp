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
    string hName;              // header type name
    string pName;              // paramter type name
    bool isConst;

    TType() { }

    TType(string _hName, string _pName, bool _isConst) : hName(_hName), pName(_pName), isConst(_isConst) { }
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
        typeConverter["MACGrid"] = TType("float", "float*", false);
        typeConverter["FlagGrid"] = TType("int32", "int*", false);
        typeConverter["Grid"] = TType("float", "float*", false);
        typeConverter["Vec3"] = TType("float", "float*", true);
        typeConverter["Real"] = TType("float", "float", true);
        typeConverter["int"] = TType("int32", "int", true);
        typeConverter["float"] = TType("float", "float", true);


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

    string generateInParameter() {
        if(inIndex < 0)
            return "";
        else
            return "const " + tType.pName + " " + getInputName() + ", ";
    }

    string generateOutParameter() {
        if(outIndex < 0)
            return "";
        else
            return tType.pName + " " + getOutputName() + ", ";
    }

    string generateParameter() {
        return generateInParameter() + generateOutParameter();
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

    List<TArgument> tArguments;

    string func_name() {
        return convertToSnake_case(funcName);
    }

    string funcName_Functor() {
        return funcName + "_Functor";
    }

    string funcName_OP() {
        return funcName + "_OP";
    }

    string generateHeader() {
        string text;

        text += "template <typename Device>\r\n";
        text += "struct " + funcName_Functor() + " {\r\n";
        {
            string func_header = "\tvoid operator()(const Device& d, ";
            for(unsigned int i = 0; i < tArguments.size(); i++) {
                func_header += tArguments[i].generateParameter();
            }
            func_header = func_header.substr(0, func_header.size() - 2);            // Remove last ", "
            func_header += ");";

            text += func_header + "\r\n";
        }
        text += "};\r\n";

        return text;
    }

    string generateRegisterOP() {
        string text;

        text += "Register_OP(\"" + funcName + "\")\r\n";

        for(unsigned int i = 0; i < tArguments.size(); i++) {
            text += tArguments[i].generateInputLine();
        }

        for(unsigned int i = 0; i < tArguments.size(); i++) {
            text += tArguments[i].generateOutputLine();
        }

        {
            text += "\t.SetShapeFn([](::tensorflow::shape_inference::InferenceContext* c) {\r\n";
            for(unsigned int i = 0; i < tArguments.size(); i++) {
                text += tArguments[i].generateShapeInferenceLine();
            }
            text += "\t\treturn Status::OK();\r\n";
            text += "\t});\r\n";
        }

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

        {
            int inIndex = 0;
            int outIndex = 0;
            for(unsigned int i = 0; i < block.func.arguments.size(); i++) {
                TArgument argument = TArgument(&(block.func.arguments[i]));

                argument.inIndex = inIndex;
                inIndex++;
                if(!argument.isTypeConst()) {
                    argument.outIndex = outIndex;
                    outIndex++;
                }

                tArguments.push_back(argument);
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

        outfile << generateRegisterKernelCPU();

        outfile << std::endl;
        outfile << std::endl;

        outfile << generateRegisterKernelGPU();

        outfile.close();
    }
};

void processTensorFunction(const Block& block, const std::string& code, Sink& sink, std::vector<Instantiation>& inst) {
    TensorProcessor tensorProcessor = TensorProcessor(block, code, sink, inst);
    tensorProcessor.write();
}
