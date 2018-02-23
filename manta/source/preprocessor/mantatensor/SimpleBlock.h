#ifndef _P_MT_SIMPLEBLOCK_H
#define _P_MT_SIMPLEBLOCK_H

#include "../prep.h"
#include <string>
#include <vector>
#include <iostream>
#include "util.h"

struct SimpleBlock {
    Block block;
    std::string tensorFuncName;
    std::string mantaFuncName;

    std::vector<std::string> newTypes;

    SimpleBlock(Block _block) : block(_block) {
        mantaFuncName = block.func.name;
        if(block.func.isTemplated()) {
            mantaFuncName += "<>";
        }

        tensorFuncName = block.func.name;
    }

    SimpleBlock(SimpleBlock sBlock, std::string nType) : block(sBlock.block) {
        mantaFuncName = block.func.name;
        tensorFuncName = block.func.name;

        newTypes = sBlock.newTypes;
        newTypes.push_back(nType);

        if(!newTypes.empty()) {
            mantaFuncName += "<";

            for(size_t i = 0; i < newTypes.size(); i++) {
                mantaFuncName += newTypes[i];
                if(newTypes.size() > i + 1)
                    mantaFuncName += ", ";


                string NewType = newTypes[i];
                NewType[0] = toupper(NewType[0]);
                tensorFuncName += NewType;
            }

            mantaFuncName += ">";
        }
    }

    SimpleBlock(SimpleBlock sBlock, Type& nType) : block(sBlock.block) {
        mantaFuncName = sBlock.mantaFuncName;
        tensorFuncName = sBlock.tensorFuncName;

        std::string NewType = nType.toString();          // TODO replace ... not correct
        stringReplace(NewType, "<", "");
        stringReplace(NewType, ">", "");
        NewType[0] = toupper(NewType[0]);
        tensorFuncName += NewType;
    }

    std::string toString() {
    	std::ostringstream os;

        os << mantaFuncName << "/ " << tensorFuncName << ": ";

        for(size_t i = 0; i < block.func.arguments.size();  i++) {
            os << block.func.arguments[i].type.name;

            if(block.func.arguments[i].type.isTemplated()) {
                os << "<";
                for(size_t j = 0; j < block.func.arguments[i].type.templateTypes.size(); j++) {
                    os << block.func.arguments[i].type.templateTypes[j].name;
                }
                os << ">";
            }

            os << ", ";
        }
        os << endl;
        

	return os.str();
    }
};

std::vector<SimpleBlock> templatePreprocessor(std::vector<SimpleBlock> &blocks);
std::vector<SimpleBlock> replaceGridBase(const Block& block);

#endif // _P_MT_SIMPLEBLOCK_H
