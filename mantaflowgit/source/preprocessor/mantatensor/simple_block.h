#ifndef _P_MT_SIMPLE_BLOCK_H
#define _P_MT_SIMPLE_BLOCK_H

#include "../prep.h"
#include <string>
#include <vector>

struct SimpleBlock {
    Block mBlock;
    std::string mTensorFuncName;
    std::string mMantaFuncName;

    std::vector<std::string> mNewTypes;

    SimpleBlock(Block block);
    SimpleBlock(const SimpleBlock& sBlock, std::string nType);
    SimpleBlock(const SimpleBlock& sBlock, Type& nType);

    std::string toString();
};

std::vector<SimpleBlock> templatePreprocessor(std::vector<SimpleBlock> &blocks);
std::vector<SimpleBlock> replaceGridBase(const Block& block);

#endif // _P_MT_SIMPLE_BLOCK_H


