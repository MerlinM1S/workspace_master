#ifndef _P_MT_TENSOR_PREPROCESSOR_GPU_H
#define _P_MT_TENSOR_PREPROCESSOR_GPU_H

#include <string>
#include "code_generator.h"
#include "targument.h"
#include "simple_block.h"
#include "tensor_preprocessor.h"

class TensorPreprocessorGPU : public TensorPreprocessor {
private:
    std::string getDeviceName() const;

    void addIncludesEtc(CodeGenerator &codeGenerator) const;
    void addFuncImplementation(CodeGenerator& codeGenerator) const;

public:
    TensorPreprocessorGPU(const SimpleBlock& sBlock, const std::string& code, Sink& sink, bool _addTimer = false) : TensorPreprocessor(sBlock, code, sink, _addTimer) { }

    std::string generateOpString() const;
};

#endif // _P_MT_TENSOR_PREPROCESSOR_GPU_H
