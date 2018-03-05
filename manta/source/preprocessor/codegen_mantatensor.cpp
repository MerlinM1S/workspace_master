#include "prep.h"
// using ofstream constructors.
#include <iostream>     // for cout
#include <algorithm>
#include <string>

#include "mantatensor/tensor_preprocessor_cpu.h"
#include "mantatensor/tensor_preprocessor_gpu.h"

using namespace std;

void processFunctionMantatensorCPU(const Block& block, Sink& sink) {
    vector<SimpleBlock> simpleBlocks = replaceGridBase(block);
    simpleBlocks = templatePreprocessor(simpleBlocks);

    for(size_t i = 0; i < simpleBlocks.size(); i++) {
        TensorPreprocessorCPU tensorProcessor = TensorPreprocessorCPU(simpleBlocks[i], false);

        if(!tensorProcessor.canConvert()) {
            if(tensorProcessor.threwError()) {
                cout << "Failure: " << block.func.name << ": " << tensorProcessor.getErrorMsg() << endl;
            }
            return;
        }

        sink.mtBuildInfo << tensorProcessor.generateBuildString();
        sink.mtCustomOps << tensorProcessor.generateOpString();
    }

    cout << "Success: " << block.func.name << endl;
}

void processFunctionMantatensorGPU(const Block& block, Sink& sink) {
    vector<SimpleBlock> simpleBlocks = replaceGridBase(block);
    simpleBlocks = templatePreprocessor(simpleBlocks);

    for(size_t i = 0; i < simpleBlocks.size(); i++) {
        TensorPreprocessorGPU tensorProcessor = TensorPreprocessorGPU(simpleBlocks[i], false);

        if(!tensorProcessor.canConvert()) {
            return;
        }

        sink.mtCustomOps << tensorProcessor.generateOpString();
    }
}


void processFunctionMantatensor(const Block& block, const string& code, Sink& sink) {
    if(gMTType == MTTF_CPU) {
        processFunctionMantatensorCPU(block, sink);
    } else if(gMTType == MTTF_GPU) {
        processFunctionMantatensorGPU(block, sink);
    }
}
