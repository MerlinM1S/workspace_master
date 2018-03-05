#ifndef _P_MT_TENSOR_PREPROCESSOR_H
#define _P_MT_TENSOR_PREPROCESSOR_H

#include <string>
#include "code_generator.h"
#include "targument.h"
#include "simple_block.h"
#include "tensor_preprocessor.h"

enum NameStyle {
    NS_name_style, NS_NameStyle, NS_Functor, NS_OP
};

class TensorPreprocessor {
protected:
    std::string tensorFuncName;
    std::string mantaFuncName;
    std::string filename;

    List<TArgument*> tArguments;

    TArgument* argumentWithHighestDims;
    TArgument* returnArgument;

    bool addTimer;

    std::string errorMsg;
    std::string ignoredMsg;

    std::string getTensorFuncName(NameStyle nameStyle) const;

    virtual std::string getDeviceName() const = 0 ;

    void addFuncHeader(CodeGenerator& codeGenerator) const;

    void addHeader(CodeGenerator &codeGenerator) const;
    void addUsingNamespaces(CodeGenerator &codeGenerator) const;

public:
    TensorPreprocessor(const SimpleBlock& sBlock, const std::string& code, Sink& sink, bool _addTimer = false);

    virtual ~TensorPreprocessor();

    virtual std::string generateOpString() const = 0;

    bool canConvert() const;
    bool threwError() const;
    std::string getErrorMsg() const;
};

#endif // _P_MT_TENSOR_PREPROCESSOR_H
