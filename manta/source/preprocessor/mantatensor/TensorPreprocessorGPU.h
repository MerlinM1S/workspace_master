#ifndef _P_MT_TENSORPREPROCESSORGPU_H
#define _P_MT_TENSORPREPROCESSORGPU_H

#include <string>
#include "CodeGenerator.h"
#include "TArgument.h"
#include "SimpleBlock.h"
#include "TensorPreprocessor.h"

class TensorProcessorGPU : public TensorProcessor {
private:
    std::string getDeviceName() const;

    void addIncludesEtc(CodeGenerator &codeGenerator) const;
    void addFuncImplementation(CodeGenerator& codeGenerator) const;

public:
    TensorProcessorGPU(const SimpleBlock& sBlock, const std::string& code, Sink& sink, bool _addTimer = false) : TensorProcessor(sBlock, code, sink, _addTimer) { }

    std::string generateString() const;
};

#endif // _P_MT_TENSORPREPROCESSORGPU_H
