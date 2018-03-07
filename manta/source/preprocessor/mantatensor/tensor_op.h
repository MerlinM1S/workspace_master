#ifndef _P_MT_TENSOR_OP_H
#define _P_MT_TENSOR_OP_H

#include <string>
#include <vector>


struct TensorOpParameter {
public:
    bool mKnownType;
    std::string mMantaType;
    std::string mName;
    std::string mDefaultValue;

    TensorOpParameter () { }
    TensorOpParameter (bool knownType, std::string mantaType, std::string name, std::string defaultValue) : mKnownType(knownType), mMantaType(mantaType), mName(name), mDefaultValue(defaultValue) { }

    static TensorOpParameter fromString(std::string text);

    bool hasDefaultValue() const;

    std::string toString() const;
};


struct TensorOp {
public:
    std::string mMantaFuncName;
    std::string mTensorFuncName;
    std::vector<std::string> mReturns;

    std::vector<TensorOpParameter> mParameters;

    TensorOp() { }

    TensorOp(std::string mantaFuncName, std::string tensorFuncName) : mMantaFuncName(mantaFuncName), mTensorFuncName(tensorFuncName) { }

    static TensorOp fromString(std::string text);

    void addReturnType(std::string name);

    void addParameter(bool knownType, std::string mantaType, std::string name, std::string defaultValue);


    std::string toString() const;
};
#endif // _P_MT_TENSOR_OP_H
