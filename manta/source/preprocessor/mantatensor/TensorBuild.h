#ifndef _P_MT_TENSORBUILD_H
#define _P_MT_TENSORBUILD_H

#include <string>

std::string generateBuildString(std::string sInCPUDir, std::string sInGPUDir, std::string sOutFileDir);
std::string generatePythonString(std::string sInCPUDir, std::string sLibFile);

#endif // _P_MT_TENSORBUILD_H
