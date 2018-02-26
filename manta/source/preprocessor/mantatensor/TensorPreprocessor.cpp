#include "TensorPreprocessor.h"

using namespace std;

string TensorProcessor::getTensorFuncName(NameStyle nameStyle) const {
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



void TensorProcessor::addHeader(CodeGenerator &codeGenerator) const {
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


void TensorProcessor::addUsingNamespaces(CodeGenerator &codeGenerator) const {
    codeGenerator.addLine("using namespace tensorflow;");
    codeGenerator.addLine("using namespace Manta;");
    codeGenerator.addLine("using namespace mantatensor;");
}


void TensorProcessor::addFuncHeader(CodeGenerator& codeGenerator) const {
    StringList arguments;
    string opLine = "";

    codeGenerator.addLine("template <>");
    opLine += "void " + getTensorFuncName(NS_Functor) + "<" + getDeviceName() + ">::operator()(";
    arguments.add("const " + getDeviceName() + "& d");

    arguments.add("const DimSize dimSize");
    for(size_t i = 0; i < tArguments.size(); i++) {
        arguments.add(tArguments[i]->generateInHeaderParam());
        arguments.add(tArguments[i]->generateOutHeaderParam());
    }
    opLine += arguments.toString();
    opLine += ") {";

    codeGenerator.addLine(opLine, 1);
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

TensorProcessor::~TensorProcessor() {
    for(size_t i = 0; i < tArguments.size(); i++) {
        delete (tArguments[i]);
    }
}

bool TensorProcessor::canConvert() const {
    return errorMsg.length() == 0 && ignoredMsg.length() == 0;
}

bool TensorProcessor::threwError() const {
    return errorMsg.length() != 0;
}

string TensorProcessor::getErrorMsg() const {
    return errorMsg;
}

