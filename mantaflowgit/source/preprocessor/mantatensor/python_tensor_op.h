#ifndef _P_MT_PYTHON_TENSOR_OP_H
#define _P_MT_PYTHON_TENSOR_OP_H

#include "tensor_op.h"
#include "code_generator.h"
#include <string>
#include <vector>

struct PythonDefParameter {
    bool mKnownType;
    std::string mMantaType;
    std::string mName;
    std::string mDefaultValue;
    bool mIsReturnValue;

    std::string mMantaDefaultValue;

    PythonDefParameter(TensorOpParameter& parameter);

    void validateDefaultValue();

    void setIsReturnValue(bool isReturnValue);

    std::string getDefParameter() const;

    std::string getMantaParameter() const;

    std::string getTupleName() const;

    std::string getParameterName() const;
};

class PythonTensorOp {
private:
    std::string mMantaFuncName;
    std::string mTensorFuncName;

    std::vector<std::string> mReturns;
    std::vector<PythonDefParameter> mParameters;

public:
    PythonTensorOp(TensorOp& tensorOp);

    void addModuleAssignment(CodeGenerator& codeGenerator, std::string moduleName) const;

    void addCommentDef(CodeGenerator& codeGenerator) const;

    void addFunctionDef(CodeGenerator& codeGenerator) const;

private:
    std::string getMantatensorFuncName() const;

};

#endif // _P_MT_PYTHON_TENSOR_OP_H
