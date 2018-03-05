#include "targument.h"
#include "util.h"

TArgument::TArgument(TTypeOp _tType, const Argument* _argument) : tType(_tType), argument(_argument), inIndex(-1) {  }

bool TArgument::hasDefaultValue() const {
    return argument->value.length() > 0;
}

bool TArgument::isTypeConst() const {
    return argument->type.isConst || tType.isConst;
}

bool TArgument::isTypeUnkown() const {
    return tType.isUnkown();
}

string TArgument::getInputName() const {
    return "in_" + convertToSnake_case(argument->name);
}

string TArgument::getOutputName() const {
    return "out_" + convertToSnake_case(argument->name);
}

string TArgument::TArgument::getInTensorName() const {
    return getInputName() + "_tensor";
}

string TArgument::getOutTensorName() const {
    return getOutputName() + "_tensor";
}

string TArgument::getMantaName() const {
    return argument->name;
}

string TArgument::getDefaultValue() const {
    return argument->value;
}

void TArgument::addDimSizeLines(CodeGenerator& codeGenerator) const {
    string dimNames[4] = {"batches", "depth", "height", "width"};


    for(int i = 0; i < 4; i++) {
        string lineOp = "long " + dimNames[i] + " = ";

        if(tType.promisedDims > i) {
           lineOp += getInTensorName() + ".shape().dim_size(" + SSTR(i) + ");";
        } else {
            lineOp += "-1;";
        }

        codeGenerator.addLine(lineOp);
    }


    codeGenerator.addLine("DimSize dimSize = DimSize(batches, depth, height, width);");    // TODO might be: batches, width, depth, height
}

void TArgument::addInputLine(CodeGenerator& codeGenerator) const {
    if(inIndex >= 0) {
        codeGenerator.addLine(".Input(\"" + getInputName() + ": " + tType.hName + "\")");
    }
}

void TArgument::addOutputLine(CodeGenerator& codeGenerator) const {
    if(!isTypeConst()) {
        codeGenerator.addLine(".Output(\"" + getOutputName() + ": " + tType.hName + "\")");
    }
}

void TArgument::addShapeInferenceLine(CodeGenerator& codeGenerator) const {
    if(!isTypeConst() && inIndex >= 0) {
        codeGenerator.addLine("c->set_output(0, c->input(" + SSTR(inIndex) + "));");
    }
}

string TArgument::generateInHeaderParam() const {
    if(inIndex < 0)
        return "";
    else
        return "const " + tType.pName + " " + getInputName();
}

string TArgument::generateOutHeaderParam() const {
    if(isTypeConst())
        return "";
    else
        return tType.pName + " " + getOutputName();
}

void TArgument::addInTensorLines(CodeGenerator& codeGenerator) const {
    if(inIndex < 0) {
        return;
    }

    codeGenerator.addLine("const Tensor& " + getInTensorName() + " = context->input(" + SSTR(inIndex) + ");");

    if(hasDefaultValue() && isPointer()) {
        codeGenerator.addLine("const " + tType.pName + " " + getInputName() + " = NULL;");
        codeGenerator.addLine("if(" + getInTensorName() + ".shape().dims() > 0) {", 1);
        codeGenerator.addLine(getInputName() + " = " + getInTensorName() + ".flat<" + tType.name + ">().data();");
        codeGenerator.addLine("}", -1);
    } else {
        if(tType.isScalar()) {
            codeGenerator.addLine("const " + tType.pName + " " + getInputName() + " = " + getInTensorName() + ".scalar<" + tType.name + ">().data()[0];");
        } else {
            codeGenerator.addLine("const " + tType.pName + " " + getInputName() + " = " + getInTensorName() + ".flat<" + tType.name + ">().data();");
        }
    }

    codeGenerator.newLine();
}

void TArgument::addOutTensorLines(CodeGenerator& codeGenerator) const {
    if(isTypeConst()) {
        return;
    }

    codeGenerator.addLine("Tensor* " + getOutTensorName() + " = NULL;");
    codeGenerator.addLine("OP_REQUIRES_OK(context, context->allocate_output(0, " + getInTensorName() + ".shape(), &" + getOutTensorName() + "));");
    codeGenerator.addLine(tType.pName + " " + getOutputName() + " = " + getOutTensorName() + "->flat<" + tType.name + ">().data();");

    codeGenerator.newLine();
}


string TArgument::generateInParam() const {
    if(inIndex < 0)
        return "";
    else
        return getInputName();
}

string TArgument::generateOutParam() const {
    if(isTypeConst())
        return "";
    else
        return getOutputName();
}

string TArgument::generateMantaParam() const {
    if(tType.isUnkown()) {
        return argument->value;
    } else {
        return getName();
    }
}

void TArgument::addCopyInToOutLines(CodeGenerator& codeGenerator) const {
    if(!isTypeConst() && inIndex >= 0) {
        codeGenerator.addLine("for(int i = 0; i < dimSize.lengthOf(" + SSTR(tType.promisedDims) + "); i++) {", 1);
        codeGenerator.addLine(getOutputName() + "[i] = " + getInputName() + "[i];");
        codeGenerator.addLine("}", -1);
        codeGenerator.newLine();
    }
}

void TArgument::addMantaVariableCreation(CodeGenerator& codeGenerator, string batch) const {
    if(tType.isUnkown())
        return;

    string baseVariableName;
    if(!isTypeConst()) {
        baseVariableName = getOutputName();
    } else if (inIndex >= 0){
        baseVariableName = getInputName();
    } else {
        return;
    }

    string makeCode;

    switch(tType.tType) {
    case TTypeGridInt:
    case TTypeGridFloat:
    case TTypeGridBool:
    case TTypeFlagGrid:
        makeCode = tType.cName + "(&fluidSolver, " + AddConstCast(baseVariableName + " + dimSize.batchToIndex(4, " + batch + ")", isTypeConst()) + ", true);";
        break;
    case TTypeGridVec3:
    case TTypeMACGrid:
         makeCode = tType.cName + "(&fluidSolver, (Vec3*) (" + AddConstCast(baseVariableName + " + dimSize.batchToIndex(4, " + batch + ")", isTypeConst()) +  "), true);";
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
            codeGenerator.addLine(tType.cName + "* " + getName() + " = NULL;");
            codeGenerator.addLine("if(" + getInputName() + ") {", 1);
            codeGenerator.addLine(getName() + " = new " + makeCode);
            codeGenerator.addLine("}", -1);
        } else {
            codeGenerator.addLine(tType.cName + "* " + getName() + " = new " + makeCode);
        }
    } else {
        codeGenerator.addLine(tType.cName + " " + getName() + " = " + makeCode);
    }
}

void TArgument::addCleanUp(CodeGenerator& codeGenerator) const {
    if(isPointer()) {
        codeGenerator.addLine("delete " + getName() + ";");
    }
}


string TArgument::getName() const {
    return argument->name;
}

bool TArgument::isPointer() const {
    return argument->type.isPointer;
}

string TArgument::AddConstCast(string arrayPointer, bool constCast) const {
    if(constCast) {
        return "const_cast<" + tType.pName + ">(" +  arrayPointer + ")";
    } else {
        return arrayPointer;
    }
}

