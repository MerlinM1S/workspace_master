#ifndef _P_MT_PYTHON_TENSOR_OP_H
#define _P_MT_PYTHON_TENSOR_OP_H

#include "tensor_op.h"
#include "string_util.h"
#include "code_generator.h"
#include <vector>
#include <algorithm>

template <typename T>
bool canLexicalCast(const std::string& s, T& result)
{
    std::stringstream ss(s);

    return !(ss >> result).fail() && (ss >> std::ws).eof();
}

template <typename T>
bool canLexicalCast(const std::string& s)
{
    std::stringstream ss(s);

    T result;

    return !(ss >> result).fail() && (ss >> std::ws).eof();
}

template <>
bool canLexicalCast(const std::string& s, bool& result)
{
    std::stringstream ss(s);

    return !(ss >> std::boolalpha >> result).fail() && (ss >> std::ws).eof();
}

struct PythonDefParameter {
    std::string mName;
    std::string mDefaultValue;
    bool mIsReturnValue;

    PythonDefParameter(TensorOpParameter& parameter) {
        mName = trim(parameter.mName);
        mDefaultValue = trim(parameter.mDefaultValue);
        mIsReturnValue = false;

        validateDefaultValue();
    }

    void validateDefaultValue() {
        bool bResult;
        if(canLexicalCast<bool>(mDefaultValue, bResult)) {
            if(bResult)
                mDefaultValue = "True";
            else
                mDefaultValue = "False";

            return;
        }

        float fResult;
        if(canLexicalCast<float>(mDefaultValue, fResult)) {
            mDefaultValue = SSTR(fResult);
            return;
        }

        double dResult;
        if(canLexicalCast<double>(mDefaultValue, dResult)) {
            mDefaultValue = SSTR((float) dResult);
            return;
        }

        if(canLexicalCast<int>(mDefaultValue))
            return;

        mDefaultValue = "";
    }

    void setIsReturnValue(bool isReturnValue) {
        mIsReturnValue = isReturnValue;
    }

    std::string getDefParameter() const {
        std::string param = mName;

        if(hasDefaultValue() && !mIsReturnValue) {
            param += " = " + mDefaultValue;
        }
        return param;
    }

    bool hasDefaultValue() const {
        return mDefaultValue.length() > 0;
    }
};

class PythonTensorOp {
private:
    std::string mFuncName;

    std::vector<std::string> mReturns;
    std::vector<PythonDefParameter> mParameters;

public:
    PythonTensorOp(TensorOp& tensorOp) {
        mFuncName = tensorOp.mFuncName;
        mReturns = tensorOp.mReturns;

        for(size_t i = 0; i < tensorOp.mParameters.size(); i++) {
            PythonDefParameter parameter = PythonDefParameter(tensorOp.mParameters[i]);
            parameter.setIsReturnValue(std::find(mReturns.begin(), mReturns.end(), tensorOp.mParameters[i].mName) != mReturns.end());
            mParameters.push_back(parameter);
        }
    }

    void addModuleAssignment(CodeGenerator& codeGenerator, std::string moduleName) const {
        codeGenerator.addLine(getMantaFuncName() + " = " + moduleName + "." + mFuncName);
    }

    void addFunctionDef(CodeGenerator& codeGenerator) const {
        if(mReturns.size() <= 0) {
            codeGenerator.addLine("# Could not define " + mFuncName + " because of missing return value.");
            return;
        }

        // def f(x):
        {
            std::string defLine = "";
            defLine += "def " + mFuncName + "(";

            StringList parameters;
            for(size_t i = 0; i < mParameters.size(); i++) {
                parameters.add(mParameters[i].getDefParameter());
            }
            defLine += parameters.toString();

            defLine += + "):";
            codeGenerator.addLine(defLine, 1);
        }

        // assign(x, f(x))
        if(mReturns.size() <= 1) {
            std::string opLine = "return tf.assign(" + mReturns[0] + ", " + getMantaFuncName() + "(";

            StringList parameters;
            for(size_t i = 0; i < mParameters.size(); i++) {
                parameters.add(mParameters[i].mName);
            }
            opLine += parameters.toString();

            opLine += ")";
            opLine += ")";
            codeGenerator.addLine(opLine);
        } else {
        }
        codeGenerator.changeIntend(-1);
    }

private:
    std::string getMantaFuncName() const {
        return "manta_" + mFuncName;
    }

};

#endif // _P_MT_PYTHON_TENSOR_OP_H
