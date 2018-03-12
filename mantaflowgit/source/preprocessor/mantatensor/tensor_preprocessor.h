#ifndef _P_MT_TENSOR_PREPROCESSOR_H
#define _P_MT_TENSOR_PREPROCESSOR_H

#include <string>
#include "targument.h"
#include "simple_block.h"
#include "tensor_preprocessor.h"
#include "string_util.h"

enum NameStyle {
    NS_name_style, NS_NameStyle, NS_Functor, NS_OP
};

class TensorPreprocessor {
protected:
    std::string mTensorFuncName;
    std::string mMantaFuncName;

    List<TArgument*> tArguments;

    TArgument* mArgumentWithHighestDims;

    bool mAddTimer;

    std::string mErrorMsg;		// Reason why this type of block can not be converted
    std::string mIgnoredMsg;		// Reason why this type of block can and should not be converted

    std::string getTensorFuncName(NameStyle nameStyle) const;

    virtual std::string getDeviceName() const = 0 ;

    void addFuncHeader(CodeGenerator& codeGenerator) const;

    void addHeader(CodeGenerator &codeGenerator) const;
    void addUsingNamespaces(CodeGenerator &codeGenerator) const;

public:
    TensorPreprocessor(const SimpleBlock& sBlock, bool _addTimer = false);

    virtual ~TensorPreprocessor();

    virtual std::string generateOpString() const = 0;

    bool canConvert() const;
    bool threwError() const;
    std::string getErrorMsg() const;
    std::string getIgnoredMsg() const { return mIgnoredMsg; }
};

#endif // _P_MT_TENSOR_PREPROCESSOR_H
