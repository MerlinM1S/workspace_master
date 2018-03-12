#include "tensor_op.h"

#include <string>
#include <vector>
#include "string_util.h"

using namespace std;


TensorOpParameter TensorOpParameter::fromString(std::string text) {
    TensorOpParameter tensorOpType;

    vector<string> split = splitString(text, '!');

    if(split.size() == 4) {
        tensorOpType.mKnownType = split[0].compare("known") == 0;
        tensorOpType.mMantaType = split[1];
        tensorOpType.mName = split[2];
        tensorOpType.mDefaultValue = split[3];
    }

    return tensorOpType;
}

string TensorOpParameter::toString() const {
    string textKnown = "";

    if(mKnownType) {
        textKnown += "known";
    } else {
        textKnown += "unknown";
    }

    return textKnown + "!" + mMantaType + "!" + mName + "!" + mDefaultValue;
}

bool TensorOpParameter::hasDefaultValue() const {
    return mDefaultValue.length() > 0;
}

void TensorOp::addReturnType(std::string name) {
    mReturns.push_back(name);
}

void TensorOp::addParameter(bool knownType, std::string mantaType, std::string name, std::string defaultValue) {
    mParameters.push_back(TensorOpParameter(knownType, mantaType, name, defaultValue));
}

TensorOp TensorOp::fromString(std::string text) {
    TensorOp tensorOp;

    vector<string> fSplit = splitString(text, ';');

    if(fSplit.size() == 4) {
        // Manta Function name
        tensorOp.mMantaFuncName = fSplit[0];

        // Returns
        {
            vector<string> split = splitString(fSplit[1], '/');
            for(size_t i = 0; i < split.size(); i++) {
                tensorOp.mReturns.push_back(split[i]);
            }
        }

        // Tensor Function name
        tensorOp.mTensorFuncName = fSplit[2];

        {
            // Parameters
            vector<string> split = splitString(fSplit[3], '/');
            for(size_t i = 0; i < split.size(); i++) {
                tensorOp.mParameters.push_back(TensorOpParameter::fromString(split[i]));
            }
        }
    }

    return tensorOp;
}

string TensorOp::toString() const {
    string text;

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
