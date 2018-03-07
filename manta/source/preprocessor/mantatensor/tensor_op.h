#ifndef _P_MT_TENSOR_OP_H
#define _P_MT_TENSOR_OP_H

#include <string>
#include <vector>
#include "string_util.h"


struct TensorOpParameter {
public:
    bool mKnownType;
    std::string mMantaType;
    std::string mName;
    std::string mDefaultValue;

    TensorOpParameter () { }
    TensorOpParameter (bool knownType, std::string mantaType, std::string name, std::string defaultValue) : mKnownType(knownType), mMantaType(mantaType), mName(name), mDefaultValue(defaultValue) { }

    static TensorOpParameter fromString(std::string text) {
        TensorOpParameter tensorOpType;

        std::vector<std::string> split = splitString(text, '!');

        if(split.size() == 4) {
            tensorOpType.mKnownType = split[0].compare("known") == 0;
            tensorOpType.mMantaType = split[1];
            tensorOpType.mName = split[2];
            tensorOpType.mDefaultValue = split[3];
        }

        return tensorOpType;
    }

    std::string toString() const {
        std::string textKnown = "";

        if(mKnownType) {
            textKnown += "known";
        } else {
            textKnown += "unknown";
        }

        return textKnown + "!" + mMantaType + "!" + mName + "!" + mDefaultValue;
    }

    bool hasDefaultValue() const {
        return mDefaultValue.length() > 0;
    }
};


struct TensorOp {
public:
    std::string mMantaFuncName;
    std::string mTensorFuncName;
    std::vector<std::string> mReturns;

    std::vector<TensorOpParameter> mParameters;

    TensorOp() { }

    TensorOp(std::string mantaFuncName, std::string tensorFuncName) : mMantaFuncName(mantaFuncName), mTensorFuncName(tensorFuncName) { }

    void addReturnType(std::string name) {
        mReturns.push_back(name);
    }

    void addParameter(bool knownType, std::string mantaType, std::string name, std::string defaultValue) {
        mParameters.push_back(TensorOpParameter(knownType, mantaType, name, defaultValue));
    }

    static TensorOp fromString(std::string text) {
        TensorOp tensorOp;

        std::vector<std::string> fSplit = splitString(text, ';');

        if(fSplit.size() == 4) {
            // Manta Function name
            tensorOp.mMantaFuncName = fSplit[0];

            // Returns
            {
                std::vector<std::string> split = splitString(fSplit[1], '/');
                for(size_t i = 0; i < split.size(); i++) {
                    tensorOp.mReturns.push_back(split[i]);
                }
            }

            // Tensor Function name
            tensorOp.mTensorFuncName = fSplit[2];

            {
                // Parameters
                std::vector<std::string> split = splitString(fSplit[3], '/');
                for(size_t i = 0; i < split.size(); i++) {
                    tensorOp.mParameters.push_back(TensorOpParameter::fromString(split[i]));
                }
            }
        }

        return tensorOp;
    }

    std::string toString() const {
        std::string text;

        // Manta Function name
        text += mMantaFuncName + ";";

        // Returns
        for(size_t i = 0; i < mReturns.size(); i++) {
            text += mReturns[i];
            if(mReturns.size() - 1 > i) {
                text += "/";
            }
        }

        // Tensor Function name
        text += ";" + mTensorFuncName + ";";

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
