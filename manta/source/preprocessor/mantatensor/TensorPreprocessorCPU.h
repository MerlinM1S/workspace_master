#ifndef _P_MT_TENSORPREPROCESSORCPU_H
#define _P_MT_TENSORPREPROCESSORCPU_H

#include <string>
#include "CodeGenerator.h"
#include "TArgument.h"
#include "SimpleBlock.h"
#include "TensorPreprocessor.h"


class TensorProcessorCPU : public TensorProcessor {
private:
    std::string getDeviceName() const;

    void addIncludesEtc(CodeGenerator &codeGenerator) const;
    void addRegisterOP(CodeGenerator& codeGenerator) const;
    void addUsingDefs(CodeGenerator& codeGenerator) const;
    void addFuncImplementation(CodeGenerator& codeGenerator) const;
    void addFuncOP(CodeGenerator& codeGenerator) const;
    void addRegisterKernelCPU(CodeGenerator& codeGenerator) const;
    void addRegisterKernelGPU(CodeGenerator& codeGenerator) const;

public:
    TensorProcessorCPU(const SimpleBlock& sBlock, const std::string& code, Sink& sink, bool _addTimer = false) : TensorProcessor(sBlock, code, sink, _addTimer) { }

    std::string generateOpString() const;
    std::string generateBuildString() const;
};

#endif // _P_MT_TENSORPREPROCESSORCPU_H
