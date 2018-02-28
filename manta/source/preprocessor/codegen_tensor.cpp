#include "prep.h"
// using ofstream constructors.
#include <iostream>     // for cout
#include <algorithm>
#include <string>

#include "mantatensor/TensorPreprocessorCPU.h"
#include "mantatensor/TensorPreprocessorGPU.h"

using namespace std;

void processTensorFunctionCPU(const Block& block, const string& code, Sink& sink) {
    vector<SimpleBlock> simpleBlocks = replaceGridBase(block);
    simpleBlocks = templatePreprocessor(simpleBlocks);

    for(size_t i = 0; i < simpleBlocks.size(); i++) {
        TensorProcessorCPU tensorProcessor = TensorProcessorCPU(simpleBlocks[i], code, sink, false);

        if(!tensorProcessor.canConvert()) {
            if(tensorProcessor.threwError()) {
                cout << "Failure: " << block.func.name << ": " << tensorProcessor.getErrorMsg() << endl;
            }
            return;
        }

        sink.buildInfo << tensorProcessor.generateBuildString();
        sink.inplace << tensorProcessor.generateOpString();
    }

    cout << "Success: " << block.func.name << endl;
}

void processTensorFunctionGPU(const Block& block, const string& code, Sink& sink) {
    vector<SimpleBlock> simpleBlocks = replaceGridBase(block);
    simpleBlocks = templatePreprocessor(simpleBlocks);

    for(size_t i = 0; i < simpleBlocks.size(); i++) {
        TensorProcessorGPU tensorProcessor = TensorProcessorGPU(simpleBlocks[i], code, sink, false);

        if(!tensorProcessor.canConvert()) {
            return;
        }

        sink.inplace << tensorProcessor.generateOpString();
    }
}


void processTensorFunction(const Block& block, const string& code, Sink& sink) {
    if(gMTType == MTTF_CPU) {
        processTensorFunctionCPU(block, code, sink);
    } else if(gMTType == MTTF_GPU) {
        processTensorFunctionGPU(block, code, sink);
    }
}
