#ifndef _P_MT_TENSOR_OP_H
#define _P_MT_TENSOR_OP_H

#include <string>
#include <vector>
#include "util.h"


struct TensorOpType {
public:
    std::string mName;
    std::string mDefaultValue;

    TensorOpType () { }
    TensorOpType (std::string name, std::string defaultValue) : mName(name), mDefaultValue(defaultValue) { }

    static TensorOpType fromString(std::string text) {
	TensorOpType tensorOpType;

	std::vector<std::string> split = splitString(text, '=');

	if(split.size() == 2) {
		tensorOpType.mName = split[0];
		tensorOpType.mDefaultValue = split[1];
	}
		
	return tensorOpType;
    }

    std::string toString() {
        return mName + "=" + mDefaultValue;
    }
};


struct TensorOp {
public:
    std::string mFuncName;
    TensorOpType mReturnType;

    std::vector<TensorOpType> mParameters;

    TensorOp() { }

    TensorOp(std::string funcName) : mFuncName(funcName) { }

    void setReturnType(std::string name, std::string defaultValue = "") {
         mReturnType.mName = name;
         mReturnType.mDefaultValue = defaultValue;
    }

    void addParameter(std::string name, std::string defaultValue) {
	mParameters.push_back(TensorOpType(name, defaultValue));
    }

    static TensorOp fromString(std::string text) {
	TensorOp tensorOp;

	std::vector<std::string> split = splitString(text, ',');

	if(split.size() >= 2) {
	   tensorOp.mFuncName = split[0];
	   tensorOp.mReturnType = TensorOpType::fromString(split[1]);

	   for(size_t i = 2; i < split.size(); i++) {
		tensorOp.mParameters.push_back(TensorOpType::fromString(split[i]));
	   }
	}

	return tensorOp;
    }

    std::string toString() {
	std::string text;

        text += mFuncName + ",";
        text += mReturnType.toString() + ",";

        for(size_t i = 0; i < mParameters.size(); i++) {
           text += mParameters[i].toString();
           if(mParameters.size() - 1 > i) {
              text += ",";
           }
        }

	return text;
    }
};
#endif // _P_MT_TENSOR_OP_H
