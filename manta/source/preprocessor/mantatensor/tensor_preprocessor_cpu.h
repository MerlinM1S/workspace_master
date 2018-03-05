#ifndef _P_MT_TENSOR_PREPROCESSOR_CPU_H
#define _P_MT_TENSOR_PREPROCESSOR_CPU_H

#include <string>
#include "code_generator.h"
#include "targument.h"
#include "simple_block.h"
#include "tensor_preprocessor.h"


class TensorPreprocessorCPU : public TensorPreprocessor {
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
    TensorPreprocessorCPU(const SimpleBlock& sBlock, const std::string& code, Sink& sink, bool _addTimer = false) : TensorPreprocessor(sBlock, code, sink, _addTimer) { }

    std::string generateOpString() const;
    std::string generateBuildString() const;
};

#endif // _P_MT_TENSOR_PREPROCESSOR_CPU_H
