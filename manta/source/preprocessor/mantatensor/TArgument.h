#ifndef _P_MT_TARGUMENT_H
#define _P_MT_TARGUMENT_H

#include "TType.h"
#include "../prep.h"
#include <map>
#include <string>
#include "CodeGenerator.h"

class TArgument {
public:
    TTypeOp tType;

    const Argument* argument;

    int inIndex;
    int outIndex;


private:
    TArgument(TTypeOp _tType, const Argument* _argument);

public:
    static TArgument* create(const Argument* argument) {
        std::map<std::string, TTypeOp> typeConverter;
        typeConverter["MACGrid"]    = TTypeOp(TTypeMACGrid);
        typeConverter["FlagGrid"]   = TTypeOp(TTypeFlagGrid);
        typeConverter["Grid<float>"]= TTypeOp(TTypeGridFloat);
        typeConverter["Grid<Real>"] = TTypeOp(TTypeGridFloat);
        typeConverter["Grid<int>"]  = TTypeOp(TTypeGridInt);
        typeConverter["Grid<Vec3>"] = TTypeOp(TTypeGridVec3);
        typeConverter["Grid<bool>"] = TTypeOp(TTypeGridBool);
        typeConverter["Vec3"]       = TTypeOp(TTypeVec3);
        typeConverter["Real"]       = TTypeOp(TTypeFloat);
        typeConverter["float"]      = TTypeOp(TTypeFloat);
        typeConverter["int"]        = TTypeOp(TTypeInt);
        typeConverter["bool"]       = TTypeOp(TTypeBool);


        if (typeConverter.find(argument->type.toString()) != typeConverter.end()) {
            return new TArgument(typeConverter[argument->type.toString()], argument);
        } else if (argument->value.length() > 0){
            return new TArgument(TTypeOp(TTypeUnkown), argument);
        } else {
            return NULL;
        }
    }

    bool hasDefaultValue();
    bool isValid();
    bool isTypeConst();

    void applyIndex(int &_inIndex, int &_outIndex);


    std::string getInputName();
    std::string getOutputName();

    std::string getInTensorName();
    std::string getOutTensorName();

    void addDimSizeLines(CodeGenerator& codeGenerator);

    void addInputLine(CodeGenerator& codeGenerator);
    void addOutputLine(CodeGenerator& codeGenerator);

    void addShapeInferenceLine(CodeGenerator& codeGenerator);

    std::string generateInHeaderParam();
    std::string generateOutHeaderParam();

    void addInTensorLines(CodeGenerator& codeGenerator);
    void addOutTensorLines(CodeGenerator& codeGenerator);


    std::string generateInParam();
    std::string generateOutParam();

    std::string generateMantaParam();

    void addCopyInToOutLines(CodeGenerator& codeGenerator);

    void addMantaVariableCreation(CodeGenerator& codeGenerator, std::string batch);

    void addCleanUp(CodeGenerator& codeGenerator);


private:
    std::string getName();

    bool isPointer();

    std::string AddConstCast(std::string arrayPointer, bool constCast);
};

#endif // _P_MT_TARGUMENT_H

