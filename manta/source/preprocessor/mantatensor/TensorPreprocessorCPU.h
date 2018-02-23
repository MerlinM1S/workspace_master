#ifndef _P_MT_TENSORPREPROCESSORCPU_H
#define _P_MT_TENSORPREPROCESSORCPU_H

#include <string>
#include "CodeGenerator.h"
#include "TArgument.h"
#include "SimpleBlock.h"

enum NameStyle {
    NS_name_style, NS_NameStyle, NS_Functor, NS_OP
};


class TensorProcessor {
private:
    std::string tensorFuncName;
    std::string mantaFuncName;
    std::string filename;

    List<TArgument*> tArguments;

    TArgument* argumentWithHighestDims;

    std::string errorMsg;
    std::string ignoredMsg;

    bool addTimer;

    std::string getTensorFuncName(NameStyle nameStyle);


    void addIncludesEtc(CodeGenerator &codeGenerator);
    void addHeader(CodeGenerator &codeGenerator);
    void addRegisterOP(CodeGenerator& codeGenerator);
    void addUsingDefs(CodeGenerator& codeGenerator);
    void addFuncHeader(CodeGenerator& codeGenerator);
    void addFuncImplementationCPU(CodeGenerator& codeGenerator);
    void addFuncOP(CodeGenerator& codeGenerator);
    void addRegisterKernelCPU(CodeGenerator& codeGenerator);
    void addRegisterKernelGPU(CodeGenerator& codeGenerator);

public:
    TensorProcessor(const SimpleBlock& sBlock, const std::string& code, Sink& sink, bool _addTimer = false);

    ~TensorProcessor();

    std::string generateString();

    std::string getOpName();

    bool canConvert();
    bool threwError();
    std::string getErrorMsg();
};

#endif // _P_MT_TENSORPREPROCESSORCPU_H
