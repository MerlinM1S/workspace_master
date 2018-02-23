/******************************************************************************
 *
 * MantaFlow fluid solver framework
 * Copyright 2011-2014 Tobias Pfaff, Nils Thuerey 
 *
 * This program is free software, distributed under the terms of the
 * GNU General Public License (GPL) 
 * http://www.gnu.org/licenses
 *
 * Preprocessor Declarations
 *
 ******************************************************************************/

#ifndef _PREP_H
#define _PREP_H

#include "util.h"
#include "code.h"
#include "tokenize.h"
#include <string>

using namespace std;

enum BlockType {
    BlockTypeNone, BlockTypeKernel, BlockTypePython, BlockTypeTKernel, BlockTypeTPython
};

// from main.cpp
enum MType { MTNone = 0, MTTBB, MTOpenMP, MTTF_CPU, MTTF_GPU};
extern std::string gFilename;
extern bool gDebugMode;
extern MType gMTType;
extern bool gDocMode;

// functions from merge.cpp
void generateMerge(int num, char* files[]);

// functions from tokenize.cpp
void processText(const std::string& text, int baseline, Sink& sink, const Class *parent, std::vector<Instantiation>& inst);

// functions from parse.cpp
void parseBlock(const BlockType blockType, const std::vector<Token>& tokens, const Class *parent, Sink& sink, std::vector<Instantiation>& inst);

// functions from codegen_XXX.cpp
void processKernel(const Block& block, const std::string& code, Sink& sink);
void processPythonFunction(const Block& block, const std::string& code, Sink& sink, std::vector<Instantiation>& inst);
void processPythonVariable(const Block& block, Sink& sink);
void processPythonClass(const Block& block, const std::string& code, Sink& sink, std::vector<Instantiation>& inst);
void processPythonInstantiation(const Block& block, const Type& aliasType, Sink& sink, std::vector<Instantiation>& inst);
void processPythonAlias(const Block& block, const Type& aliasType, const std::string& aliasName, Sink& sink);
void postProcessInstantiations(Sink& sink, std::vector<Instantiation>& inst);

void processTensorFunction(const Block& block, const string& code, Sink& sink);



inline const string ToString(BlockType blockType)
{
    switch (blockType)
    {
        case BlockTypeKernel: return "KERNEL";
        case BlockTypePython: return "PYTHON";
        case BlockTypeTKernel: return "TENSOR KERNEL";
        case BlockTypeTPython: return "TENSOR PYTHON";
        default: return "[Unknown BlockType]";
    }
}

#endif // _PREP_H
