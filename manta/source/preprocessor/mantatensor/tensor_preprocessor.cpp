#include "tensor_preprocessor.h"
#include "util.h"

using namespace std;

string TensorPreprocessor::getTensorFuncName(NameStyle nameStyle) const {
    switch(nameStyle) {
    case NS_name_style:
        return convertToSnake_case(mTensorFuncName);
    case NS_NameStyle: {
        string result = mTensorFuncName;
        result[0] = toupper(result[0]);
        return result;
    }
    case NS_Functor:
        return mTensorFuncName + "_Functor";
    case NS_OP:
        return mTensorFuncName + "_OP";
    default:
        return mTensorFuncName;
    }
}



void TensorPreprocessor::addHeader(CodeGenerator &codeGenerator) const {
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


void TensorPreprocessor::addUsingNamespaces(CodeGenerator &codeGenerator) const {
    codeGenerator.addLine("using namespace tensorflow;");
    codeGenerator.addLine("using namespace Manta;");
    codeGenerator.addLine("using namespace mantatensor;");
}


void TensorPreprocessor::addFuncHeader(CodeGenerator& codeGenerator) const {
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


TensorPreprocessor::TensorPreprocessor(const SimpleBlock& sBlock, bool addTimer) : mAddTimer(addTimer) {
    const Block* block = &sBlock.mBlock;
    mTensorFuncName = convertToCamelCase(sBlock.mTensorFuncName);
    mMantaFuncName = sBlock.mMantaFuncName;

    mArgumentWithHighestDims = 0;
    int highestDims = -1;

    mErrorMsg = "";

    if(block->func.returnType.name.length() == 0) {
        mIgnoredMsg = "Constructors can not be converted.";
        return;
    }

    if(block->parent) {
        mIgnoredMsg = "Methods can not be converted.";
        return;
    }

    if(block->func.returnType.name.compare("void") != 0) {
        mErrorMsg = "Functions may not have a return value.";
        return;
    }

    {
        int inIndex = 0;
        int outIndex = 0;
        for(unsigned int i = 0; i < block->func.arguments.size(); i++) {
            TArgument* argument = TArgument::create(&(block->func.arguments[i]));

            if(!argument) {
                mErrorMsg = block->func.arguments[i].type.toString() + " is not a valid parameter type.";
                return;
            }


            // Set Indicies
            if(!argument->isTypeUnkown()) {
                argument->setInIndex(inIndex);
                inIndex++;

                if(!argument->isTypeConst()) {
                    argument->setOutIndex(outIndex);
                    outIndex++;
                }
            }

            tArguments.push_back(argument);

            if(argument->mTType.mPromisedDims > highestDims) {
                highestDims = argument->mTType.mPromisedDims ;
                mArgumentWithHighestDims = argument;
            }
        }

        if(!mArgumentWithHighestDims || mArgumentWithHighestDims->mTType.mPromisedDims < 3) {
            mErrorMsg = "Functions require at least one parameter of type Grid.";
            return;
        }

    }
}

TensorPreprocessor::~TensorPreprocessor() {
    for(size_t i = 0; i < tArguments.size(); i++) {
        delete (tArguments[i]);
    }
}

bool TensorPreprocessor::canConvert() const {
    return mErrorMsg.length() == 0 && mIgnoredMsg.length() == 0;
}

bool TensorPreprocessor::threwError() const {
    return mErrorMsg.length() != 0;
}

string TensorPreprocessor::getErrorMsg() const {
    return mErrorMsg;
}

