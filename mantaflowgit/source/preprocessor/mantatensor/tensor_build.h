#ifndef _P_MT_TENSOR_BUILD_H
#define _P_MT_TENSOR_BUILD_H

#include <string>

std::string generateBuildString(std::string sInCPUDir, std::string sInGPUDir, std::string sOutFileDir);
std::string generatePythonString(std::string sInCPUDir, std::string sLibFile);

#endif // _P_MT_TENSOR_BUILD_H
