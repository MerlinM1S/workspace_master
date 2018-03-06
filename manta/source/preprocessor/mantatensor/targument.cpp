#include "targument.h"
#include "string_util.h"

TArgument::TArgument(TTypeOp _tType, const Argument* _argument) : mTType(_tType), mArgument(_argument), mInIndex(-1), mOutIndex(-1) {  }

bool TArgument::hasDefaultValue() const {
    return mArgument->value.length() > 0;
}

bool TArgument::isTypeConst() const {
    return mArgument->type.isConst || mTType.mIsConst;
}

bool TArgument::isTypeUnkown() const {
    return mTType.isUnkown();
}

string TArgument::getInputName() const {
    return "in_" + convertToSnake_case(mArgument->name);
}

string TArgument::getOutputName() const {
    return "out_" + convertToSnake_case(mArgument->name);
}

string TArgument::TArgument::getInTensorName() const {
    return getInputName() + "_tensor";
}

string TArgument::getOutTensorName() const {
    return getOutputName() + "_tensor";
}

string TArgument::getMantaName() const {
    return mArgument->name;
}

string TArgument::getDefaultValue() const {
    return mArgument->value;
}

void TArgument::addDimSizeLines(CodeGenerator& codeGenerator) const {
    string dimNames[4] = {"batches", "depth", "height", "width"};


    for(int i = 0; i < 4; i++) {
        string lineOp = "long " + dimNames[i] + " = ";

        if(mTType.mPromisedDims > i) {
           lineOp += getInTensorName() + ".shape().dim_size(" + SSTR(i) + ");";
        } else {
            lineOp += "-1;";
        }

        codeGenerator.addLine(lineOp);
    }


    codeGenerator.addLine("DimSize dimSize = DimSize(batches, depth, height, width);");    // TODO might be: batches, width, depth, height
}

void TArgument::addInputLine(CodeGenerator& codeGenerator) const {
    if(mInIndex >= 0) {
        codeGenerator.addLine(".Input(\"" + getInputName() + ": " + mTType.mTensorRegisterName + "\")");
    }
}

void TArgument::addOutputLine(CodeGenerator& codeGenerator) const {
    if(mOutIndex >= 0) {
        codeGenerator.addLine(".Output(\"" + getOutputName() + ": " + mTType.mTensorRegisterName + "\")");
    }
}

void TArgument::addShapeInferenceLine(CodeGenerator& codeGenerator) const {
    if(mOutIndex >= 0 && mInIndex >= 0) {
        codeGenerator.addLine("c->set_output(" + SSTR(mOutIndex) + ", c->input(" + SSTR(mInIndex) + "));");
    }
}

string TArgument::generateInHeaderParam() const {
    if(mInIndex < 0)
        return "";
    else
        return "const " + mTType.mParameterName + " " + getInputName();
}

string TArgument::generateOutHeaderParam() const {
    if(mOutIndex < 0)
        return "";
    else
        return mTType.mParameterName + " " + getOutputName();
}

void TArgument::addInTensorLines(CodeGenerator& codeGenerator) const {
    if(mInIndex < 0) {
        return;
    }

    codeGenerator.addLine("const Tensor& " + getInTensorName() + " = context->input(" + SSTR(mInIndex) + ");");

    if(hasDefaultValue() && isPointer()) {
        codeGenerator.addLine("const " + mTType.mParameterName + " " + getInputName() + " = NULL;");
        codeGenerator.addLine("if(" + getInTensorName() + ".shape().dims() > 0) {", 1);
        codeGenerator.addLine(getInputName() + " = " + getInTensorName() + ".flat<" + mTType.mTensorName + ">().data();");
        codeGenerator.addLine("}", -1);
    } else {
        if(mTType.isScalar()) {
            codeGenerator.addLine("const " + mTType.mParameterName + " " + getInputName() + " = " + getInTensorName() + ".scalar<" + mTType.mTensorName + ">().data()[0];");
        } else {
            codeGenerator.addLine("const " + mTType.mParameterName + " " + getInputName() + " = " + getInTensorName() + ".flat<" + mTType.mTensorName + ">().data();");
        }
    }

    codeGenerator.newLine();
}

void TArgument::addOutTensorLines(CodeGenerator& codeGenerator) const {
    if(mOutIndex < 0) {
        return;
    }

    codeGenerator.addLine("Tensor* " + getOutTensorName() + " = NULL;");
    codeGenerator.addLine("OP_REQUIRES_OK(context, context->allocate_output(" + SSTR(mOutIndex) + ", " + getInTensorName() + ".shape(), &" + getOutTensorName() + "));");
    codeGenerator.addLine(mTType.mParameterName + " " + getOutputName() + " = " + getOutTensorName() + "->flat<" + mTType.mTensorName + ">().data();");

    codeGenerator.newLine();
}


string TArgument::generateInParam() const {
    if(mInIndex < 0)
        return "";
    else
        return getInputName();
}

string TArgument::generateOutParam() const {
    if(mOutIndex < 0)
        return "";
    else
        return getOutputName();
}

string TArgument::generateMantaParam() const {
    if(mTType.isUnkown()) {
        return mArgument->value;
    } else {
        return getName();
    }
}

void TArgument::addCopyInToOutLines(CodeGenerator& codeGenerator) const {
    if(!isTypeConst() && mInIndex >= 0) {
        codeGenerator.addLine("for(int i = 0; i < dimSize.lengthOf(" + SSTR(mTType.mPromisedDims) + "); i++) {", 1);
        codeGenerator.addLine(getOutputName() + "[i] = " + getInputName() + "[i];");
        codeGenerator.addLine("}", -1);
        codeGenerator.newLine();
    }
}

void TArgument::addMantaVariableCreation(CodeGenerator& codeGenerator, string batch) const {
    if(mTType.isUnkown())
        return;

    string baseVariableName;
    if(!isTypeConst()) {
        baseVariableName = getOutputName();
    } else if (mInIndex >= 0){
        baseVariableName = getInputName();
    } else {
        return;
    }

    string makeCode;

    switch(mTType.mTType) {
    case TTypeGridInt:
    case TTypeGridFloat:
    case TTypeFlagGrid:
        makeCode = mTType.mMantaName + "(&fluidSolver, " + AddConstCast(baseVariableName + " + dimSize.batchToIndex(4, " + batch + ")", isTypeConst()) + ", true);";
        break;
    case TTypeGridVec3:
    case TTypeMACGrid:
         makeCode = mTType.mMantaName + "(&fluidSolver, (Vec3*) (" + AddConstCast(baseVariableName + " + dimSize.batchToIndex(4, " + batch + ")", isTypeConst()) +  "), true);";
        break;
    case TTypeVec3:
        makeCode = "Vec3(" +  baseVariableName + " + (3 * " + batch + "));";
        break;
    default:
        makeCode = baseVariableName + "[" + batch + "];";
        break;
    }

    if(isPointer()) {
        if(hasDefaultValue()) {
            codeGenerator.addLine(mTType.mMantaName + "* " + getName() + " = NULL;");
            codeGenerator.addLine("if(" + getInputName() + ") {", 1);
            codeGenerator.addLine(getName() + " = new " + makeCode);
            codeGenerator.addLine("}", -1);
        } else {
            codeGenerator.addLine(mTType.mMantaName + "* " + getName() + " = new " + makeCode);
        }
    } else {
        codeGenerator.addLine(mTType.mMantaName + " " + getName() + " = " + makeCode);
    }
}

void TArgument::addCleanUp(CodeGenerator& codeGenerator) const {
    if(isPointer() && !isTypeUnkown()) {
        codeGenerator.addLine("delete " + getName() + ";");
    }
}


string TArgument::getName() const {
    return mArgument->name;
}

bool TArgument::isPointer() const {
    return mArgument->type.isPointer;
}

string TArgument::AddConstCast(string arrayPointer, bool constCast) const {
    if(constCast) {
        return "const_cast<" + mTType.mParameterName + ">(" +  arrayPointer + ")";
    } else {
        return arrayPointer;
    }
}

