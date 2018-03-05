#ifndef _P_MT_SIMPLE_BLOCK_H
#define _P_MT_SIMPLE_BLOCK_H

#include "../prep.h"
#include <string>
#include <vector>

struct SimpleBlock {
    Block block;
    std::string tensorFuncName;
    std::string mantaFuncName;

    std::vector<std::string> newTypes;

    SimpleBlock(Block _block);
    SimpleBlock(const SimpleBlock& sBlock, std::string nType);
    SimpleBlock(const SimpleBlock& sBlock, Type& nType);

    std::string toString();
};

std::vector<SimpleBlock> templatePreprocessor(std::vector<SimpleBlock> &blocks);
std::vector<SimpleBlock> replaceGridBase(const Block& block);

#endif // _P_MT_SIMPLE_BLOCK_H


