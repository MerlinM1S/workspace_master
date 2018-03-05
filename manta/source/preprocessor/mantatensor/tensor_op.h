#ifndef _P_MT_TENSOR_OP_H
#define _P_MT_TENSOR_OP_H

#include <string>
#include <vector>
#include "util.h"


struct TensorOpType {
public:
    std::string name;
    std::string defaultValue;

    TensorOpType () { }
    TensorOpType (std::string _name, std::string _defaultValue) : name(_name), defaultValue(_defaultValue) { }

    static TensorOpType fromString(std::string text) {
	TensorOpType tensorOpType;

	std::vector<std::string> split = splitString(text, '=');

	if(split.size() == 2) {
		tensorOpType.name = split[0];
		tensorOpType.defaultValue = split[1];
	}
		
	return tensorOpType;
    }

    std::string toString() {
        return name + "=" + defaultValue;
    }
};


struct TensorOp {
public:
    std::string funcName;
    TensorOpType returnType;

    std::vector<TensorOpType> parameters;

    TensorOp() { }

    TensorOp(std::string _funcName) : funcName(_funcName) { }

    void setReturnType(std::string name, std::string defaultValue = "") {
         returnType.name = name;
         returnType.defaultValue = defaultValue;
    }

    void addParameter(std::string name, std::string defaultValue) {
	parameters.push_back(TensorOpType(name, defaultValue));
    }

    static TensorOp fromString(std::string text) {
	TensorOp tensorOp;

	std::vector<std::string> split = splitString(text, ',');

	if(split.size() >= 2) {
	   tensorOp.funcName = split[0];
	   tensorOp.returnType = TensorOpType::fromString(split[1]);

	   for(size_t i = 2; i < split.size(); i++) {
		tensorOp.parameters.push_back(TensorOpType::fromString(split[i]));
	   }
	}

	return tensorOp;
    }

    std::string toString() {
	std::string text;

        text += funcName + ",";
        text += returnType.toString() + ",";

        for(size_t i = 0; i < parameters.size(); i++) {
           text += parameters[i].toString();
           if(parameters.size() - 1 > i) {
              text += ",";
           }
        }

	return text;
    }
};
#endif // _P_MT_TENSOR_OP_H
