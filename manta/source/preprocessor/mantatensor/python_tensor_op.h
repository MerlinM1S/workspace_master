#ifndef _P_MT_PYTHON_TENSOR_OP_H
#define _P_MT_PYTHON_TENSOR_OP_H

#include "tensor_op.h"
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
    bool mKnownType;
    std::string mMantaType;
    std::string mName;
    std::string mDefaultValue;
    bool mIsReturnValue;

    std::string mMantaDefaultValue;

    PythonDefParameter(TensorOpParameter& parameter) {
        mKnownType = parameter.mKnownType;
        mMantaType = parameter.mMantaType;
        mName = trim(parameter.mName);
        mMantaDefaultValue = trim(parameter.mDefaultValue);;
        mDefaultValue = mMantaDefaultValue;
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
        if(!mKnownType) {
            return "";
        }

        std::string param = mName;

//        if(hasDefaultValue() && !mIsReturnValue) {
//            param += " = " + mDefaultValue;
//        }
        return param;
    }

    std::string getMantaParameter() const {
        std::string param = "";

        if(!mIsReturnValue) {
            param += "const ";
        }

        param += mMantaType + " ";

        param += mName;

        if(mMantaDefaultValue.length() > 0) {
            param += "=" + mMantaDefaultValue;
        }
        return param;
    }

    std::string getTupleName() const {
        if(mIsReturnValue && mKnownType) {
            return mName;
        } else {
            return "";
        }
    }

    std::string getParameterName() const {
        if(mKnownType) {
            return mName;
        } else {
            return "";
        }
    }
};

class PythonTensorOp {
private:
    std::string mMantaFuncName;
    std::string mTensorFuncName;

    std::vector<std::string> mReturns;
    std::vector<PythonDefParameter> mParameters;

public:
    PythonTensorOp(TensorOp& tensorOp) {
        mMantaFuncName = tensorOp.mMantaFuncName;
        mTensorFuncName = tensorOp.mTensorFuncName;
        mReturns = tensorOp.mReturns;

        for(size_t i = 0; i < tensorOp.mParameters.size(); i++) {
            PythonDefParameter parameter = PythonDefParameter(tensorOp.mParameters[i]);
            parameter.setIsReturnValue(std::find(mReturns.begin(), mReturns.end(), tensorOp.mParameters[i].mName) != mReturns.end());
            mParameters.push_back(parameter);
        }
    }

    void addModuleAssignment(CodeGenerator& codeGenerator, std::string moduleName) const {
        codeGenerator.addLine(getMantatensorFuncName() + " = " + moduleName + "." + mTensorFuncName);
    }

    void addCommentDef(CodeGenerator& codeGenerator) const {
        std::string commentLine = "";
        commentLine += "# void " + mMantaFuncName + "(";

        StringList parameters;
        for(size_t i = 0; i < mParameters.size(); i++) {
            parameters.add(mParameters[i].getMantaParameter());
        }
        commentLine += parameters.toString();
        commentLine += ");";

        codeGenerator.addLine(commentLine);
    }

    void addFunctionDef(CodeGenerator& codeGenerator) const {
        addCommentDef(codeGenerator);
        if(mReturns.size() <= 0) {
            codeGenerator.addLine("# Could not define " + mTensorFuncName + " because of missing return value.");
            return;
        }

        // def f(x):
        {
            std::string defLine = "";
            defLine += "def " + mTensorFuncName + "(";

            StringList parameters;
            for(size_t i = 0; i < mParameters.size(); i++) {
                parameters.add(mParameters[i].getDefParameter());
            }
            defLine += parameters.toString();

            defLine += + "):";
            codeGenerator.addLine(defLine, 1);
        }

        // assign(x, f(x))
        if(mReturns.size() <= 1) {      // One output
            std::string opLine = "return tf.assign(" + mReturns[0] + ", " + getMantatensorFuncName() + "(";

            StringList parameters;
            for(size_t i = 0; i < mParameters.size(); i++) {
                parameters.add(mParameters[i].getParameterName());
            }
            opLine += parameters.toString();

            opLine += ")";
            opLine += ")";
            codeGenerator.addLine(opLine);
        } else {                     // multiple outputs
            std::string tupleLine = "output = (";
            {
                StringList tupleList;
                for(size_t i = 0; i < mParameters.size(); i++) {
                    tupleList.add(mParameters[i].getTupleName());
                }
                tupleLine += tupleList.toString();
                tupleLine += ")";
            }
            codeGenerator.addLine(tupleLine);

            std::string funcCallLine = "funcCall = " + getMantatensorFuncName() + "(";
            {
                StringList parameters;
                for(size_t i = 0; i < mParameters.size(); i++) {
                    parameters.add(mParameters[i].getParameterName());
                }
                funcCallLine += parameters.toString();
                funcCallLine += ")";
            }
            codeGenerator.addLine(funcCallLine);


            codeGenerator.addLine("return tf.contrib.framework.nest.map_structure(lambda state, var: tf.assign(var, state), funcCall, output, check_types=False)");
        }
        codeGenerator.changeIntend(-1);
    }

private:
    std::string getMantatensorFuncName() const {
        return "manta_" + mTensorFuncName;
    }

};

#endif // _P_MT_PYTHON_TENSOR_OP_H
