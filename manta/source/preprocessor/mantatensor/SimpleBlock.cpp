#include "../prep.h"
#include <string>
#include <vector>
#include <iostream>

#include "SimpleBlock.h"


vector<SimpleBlock> templatePreprocessor(vector<SimpleBlock> &blocks) {
    vector<SimpleBlock> result = blocks;

    string dataTypes[] = {"int", "float", "Vec3"};

    while(result[0].block.func.isTemplated()) {
        SimpleBlock cSBlock = result[0];
        Block* cBlock = &cSBlock.block;

        result.erase (result.begin());

        string removedTemplate = cBlock->func.templateTypes[0].name;
        cBlock->func.templateTypes._data.erase(cBlock->func.templateTypes._data.begin());

        for(size_t i = 0; i < 3; i++) {
            string nType = dataTypes[i];

            SimpleBlock nBlock = SimpleBlock(cSBlock, nType);

            for(size_t j = 0; j < cBlock->func.arguments.size();  j++) {
                Argument argument = cBlock->func.arguments[j];

                if(removedTemplate.compare(argument.type.name) == 0) {
                    argument.type.name = nType;
                }

                List<Type> nTemplates;
                for(size_t k = 0; k < argument.type.templateTypes.size(); k++) {
                    Type cType = argument.type.templateTypes[k];

                    if(removedTemplate.compare(cType.name) == 0) {
                        cType.name = nType;
                    }

                    nTemplates.push_back(cType);
                }
                argument.type.templateTypes = nTemplates;

                nBlock.block.func.arguments[j] = argument;
            }

            result.push_back(nBlock);
        }
    }


    return result;
}




vector<SimpleBlock> replaceGridBase(const Block& block) {

    Type dataTypes[3];
    dataTypes[0].name = "MACGrid";
    dataTypes[1].name = "Grid";
    {
        Type type;
        type.name = "Vec3";
        dataTypes[1].templateTypes.push_back(type);
    }
    dataTypes[2].name = "Grid";
    {
        Type type;
        type.name = "float";
        dataTypes[2].templateTypes.push_back(type);
    }



    vector<SimpleBlock> result;

    result.push_back(SimpleBlock(block));

    List<Argument> arguments = block.func.arguments;

    for(size_t i = 0; i < arguments.size();  i++) {
        if(arguments[i].type.name.compare("GridBase") == 0) {
            vector<SimpleBlock> nResult;

            for(int k = 0; k< 3; k++) {
                Argument argument = arguments[i];
                argument.type.name = dataTypes[k].name;
                argument.type.templateTypes = dataTypes[k].templateTypes;
                for(size_t j = 0; j < result.size();  j++) {
                    SimpleBlock nBlock = SimpleBlock(result[j], argument.type);

                    nBlock.block.func.arguments[i] = argument;

                    nResult.push_back(nBlock);
                }
            }

            result = nResult;
        }
    }

    return result;
}
