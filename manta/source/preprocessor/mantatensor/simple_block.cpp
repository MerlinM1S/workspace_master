#include "../prep.h"
#include <string>
#include <vector>
#include <iostream>
#include "util.h"

#include "simple_block.h"

using namespace std;



SimpleBlock::SimpleBlock(Block _block) : block(_block) {
    mantaFuncName = block.func.name;
    if(block.func.isTemplated()) {
        mantaFuncName += "<>";
    }

    tensorFuncName = block.func.name;
}

SimpleBlock::SimpleBlock(const SimpleBlock& sBlock, string nType) : block(sBlock.block) {
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

SimpleBlock::SimpleBlock(const SimpleBlock& sBlock, Type& nType) : block(sBlock.block) {
    mantaFuncName = sBlock.mantaFuncName;
    tensorFuncName = sBlock.tensorFuncName;

    string NewType = nType.toString();          // TODO replace ... not correct
    stringReplace(NewType, "<", "");
    stringReplace(NewType, ">", "");
    NewType[0] = toupper(NewType[0]);
    tensorFuncName += NewType;
}

string SimpleBlock::toString() {
    ostringstream os;

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
