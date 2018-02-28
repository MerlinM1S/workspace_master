#include "TensorBuild.h"
#include "util.h"
#include <vector>
#include <algorithm>
#include <sstream>
#include "../fileio.h"
#include "TensorOp.h"

using namespace std;

bool isNotCFile (const File& file) {
    return file.getExtension().compare("h") && file.getExtension().compare("cpp") ;
}

bool isNotBuildFile (const File& file) {
    return file.getExtension().compare("build");
}

vector<TensorOp> getCustomTensorOps(vector<File> inBuildFiles) {
    inBuildFiles.erase(std::remove_if(inBuildFiles.begin(), inBuildFiles.end(), isNotBuildFile), inBuildFiles.end());

    vector<TensorOp> result;
    for (size_t i = 0; i < inBuildFiles.size(); i++) {
        File file = inBuildFiles[i];

        string buildFileContent = file.readFile();

        vector<string> strings = splitString(buildFileContent, '\n');
        for(size_t i = 0; i < strings.size() - 1; i++) {
            result.push_back(TensorOp::fromString(strings[i]));
        }
    }

    return result;
}


string generateBuildString(string sInCPUDir, string sInGPUDir, string sOutFileDir) {
    File inCPUDir = File(sInCPUDir);
    File inGPUDir = File(sInGPUDir);
    File outFileDir =  File(sOutFileDir);

    vector<File> inCPUFiles = inCPUDir.getFilesOfDirectory();

    inCPUFiles.erase(std::remove_if(inCPUFiles.begin(), inCPUFiles.end(), isNotCFile), inCPUFiles.end());

    stringstream cpuSourcesStream;
    {
        cpuSourcesStream << "\tsrcs = [";

        for (size_t i = 0; i < inCPUFiles.size(); i++) {
            File file = inCPUFiles[i];

            cpuSourcesStream << "\"" << file.makeRelative(outFileDir).toString() << "\"";

            if(i + 1 < inCPUFiles.size()) {
                cpuSourcesStream << ", ";
            }
        }
        cpuSourcesStream << "]," << endl;
    }

    stringstream gpuSourcesStream;
    {
        vector<File> inGPUFiles = inGPUDir.getFilesOfDirectory();
        inGPUFiles.erase(std::remove_if(inGPUFiles.begin(), inGPUFiles.end(), isNotCFile), inGPUFiles.end());

        gpuSourcesStream << "\tgpu_srcs = [";

        for (size_t i = 0; i < inGPUFiles.size(); i++) {
            File file = inGPUFiles[i];

            gpuSourcesStream << "\"" << file.makeRelative(outFileDir).toString() << "\"";

            if(i + 1 < inGPUFiles.size()) {
                gpuSourcesStream << ", ";
            }
        }
        gpuSourcesStream << "]," << endl;
    }

    stringstream buildStream;

    buildStream << "load(\"//tensorflow:tensorflow.bzl\", \"tf_custom_op_library\")" << endl << endl;
    buildStream << "tf_custom_op_library(" << endl;
    buildStream << "\tname = \"mantatensor.so\"," << endl;
    buildStream << cpuSourcesStream.str();
//    buildStream << gpuSourcesStream.str();
    buildStream << ")" << endl << endl;

    return buildStream.str();
}

string generatePythonString(std::string sInCPUDir, std::string sLibFile) {
    File inCPUDir = File(sInCPUDir);
    File libFile =  File(sLibFile);

    vector<File> inCPUFiles = inCPUDir.getFilesOfDirectory();

    vector<TensorOp> customTensorOps = getCustomTensorOps(inCPUFiles);

    stringstream pyStream;

    pyStream << "import tensorflow as tf" << endl << endl;
    pyStream << "mantatensor_module = tf.load_op_library('" << libFile.toString() << "')" << endl;
    for(size_t i = 0; i < customTensorOps.size(); i++) {
        pyStream <<"manta_" << customTensorOps[i].funcName << " = mantatensor_module." << customTensorOps[i].funcName << endl;
    }

    return pyStream.str();
}
