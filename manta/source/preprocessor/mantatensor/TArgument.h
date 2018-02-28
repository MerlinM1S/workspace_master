#ifndef _P_MT_TARGUMENT_H
#define _P_MT_TARGUMENT_H

#include "TType.h"
#include "../prep.h"
#include <string>
#include "CodeGenerator.h"

class TArgument {
public:
    const TTypeOp tType;

    const Argument* argument;

    int inIndex;

private:
    TArgument(const TTypeOp _tType, const Argument* _argument);

public:
    static TArgument* create(const Argument* argument) {
	TTypeOp tType = TTypeOp::create(argument->type.toString());

	if(!tType.isUnkown() || argument->value.length() > 0) {
	    return new TArgument(tType, argument);
	} else {
	    return NULL;
	}
    }

    std::string getInputName() const;
    std::string getOutputName() const;

    std::string getInTensorName() const;
    std::string getOutTensorName() const;

    std::string getMantaName() const;
    std::string getDefaultValue() const;


    void addDimSizeLines(CodeGenerator& codeGenerator) const;

    void addInputLine(CodeGenerator& codeGenerator) const;
    void addOutputLine(CodeGenerator& codeGenerator) const;

    void addShapeInferenceLine(CodeGenerator& codeGenerator) const;

    std::string generateInHeaderParam() const;
    std::string generateOutHeaderParam() const;

    void addInTensorLines(CodeGenerator& codeGenerator) const;
    void addOutTensorLines(CodeGenerator& codeGenerator) const;


    std::string generateInParam() const;
    std::string generateOutParam() const;

    std::string generateMantaParam() const;

    void addCopyInToOutLines(CodeGenerator& codeGenerator) const;

    void addMantaVariableCreation(CodeGenerator& codeGenerator, std::string batch) const;

    void addCleanUp(CodeGenerator& codeGenerator) const;


    bool hasDefaultValue() const;
    bool isTypeConst() const;
    bool isTypeUnkown() const;


private:
    std::string getName() const;

    bool isPointer() const;

    std::string AddConstCast(std::string arrayPointer, bool constCast) const;
};

#endif // _P_MT_TARGUMENT_H

