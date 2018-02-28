#ifndef _P_MT_TENSORPREPROCESSOR_H
#define _P_MT_TENSORPREPROCESSOR_H

#include <string>
#include "CodeGenerator.h"
#include "TArgument.h"
#include "SimpleBlock.h"
#include "TensorPreprocessor.h"

enum NameStyle {
    NS_name_style, NS_NameStyle, NS_Functor, NS_OP
};

class TensorProcessor {
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
    TensorProcessor(const SimpleBlock& sBlock, const std::string& code, Sink& sink, bool _addTimer = false);

    virtual ~TensorProcessor();

    virtual std::string generateOpString() const = 0;

    bool canConvert() const;
    bool threwError() const;
    std::string getErrorMsg() const;
};

#endif // _P_MT_TENSORPREPROCESSOR_H
