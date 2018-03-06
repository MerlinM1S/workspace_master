#ifndef _P_MT_TENSOR_OP_H
#define _P_MT_TENSOR_OP_H

#include <string>
#include <vector>
#include "string_util.h"


struct TensorOpParameter {
public:
    std::string mName;
    std::string mDefaultValue;

    TensorOpParameter () { }
    TensorOpParameter (std::string name, std::string defaultValue) : mName(name), mDefaultValue(defaultValue) { }

    static TensorOpParameter fromString(std::string text) {
        TensorOpParameter tensorOpType;

        std::vector<std::string> split = splitString(text, '=');

        if(split.size() == 2) {
            tensorOpType.mName = split[0];
            tensorOpType.mDefaultValue = split[1];
        }

        return tensorOpType;
    }

    std::string toString() const {
        return mName + "=" + mDefaultValue;
    }

    bool hasDefaultValue() const {
        return mDefaultValue.length() > 0;
    }
};


struct TensorOp {
public:
    std::string mFuncName;
    std::vector<std::string> mReturns;

    std::vector<TensorOpParameter> mParameters;

    TensorOp() { }

    TensorOp(std::string funcName) : mFuncName(funcName) { }

    void addReturnType(std::string name) {
        mReturns.push_back(name);
    }

    void addParameter(std::string name, std::string defaultValue) {
        mParameters.push_back(TensorOpParameter(name, defaultValue));
    }

    static TensorOp fromString(std::string text) {
        TensorOp tensorOp;

        std::vector<std::string> fSplit = splitString(text, ';');

        if(fSplit.size() == 3) {
            // Returns
            {
                std::vector<std::string> split = splitString(fSplit[0], '/');
                for(size_t i = 0; i < split.size(); i++) {
                    tensorOp.mReturns.push_back(split[i]);
                }
            }

            // Function name
            tensorOp.mFuncName = fSplit[1];

            {
                // Parameters
                std::vector<std::string> split = splitString(fSplit[2], '/');
                for(size_t i = 0; i < split.size(); i++) {
                    tensorOp.mParameters.push_back(TensorOpParameter::fromString(split[i]));
                }
            }
        }

        return tensorOp;
    }

    std::string toString() const {
        std::string text;

        // Returns
        for(size_t i = 0; i < mReturns.size(); i++) {
            text += mReturns[i];
            if(mReturns.size() - 1 > i) {
                text += "/";
            }
        }

        // Function name
        text += ";" + mFuncName + ";";

        // Parameters
        for(size_t i = 0; i < mParameters.size(); i++) {
            text += mParameters[i].toString();
            if(mParameters.size() - 1 > i) {
                text += "/";
            }
        }

        return text;
    }
};
#endif // _P_MT_TENSOR_OP_H
