/******************************************************************************
 *
 * MantaFlow fluid solver framework
 * Copyright 2011 Tobias Pfaff, Nils Thuerey 
 *
 * This program is free software, distributed under the terms of the
 * GNU General Public License (GPL) 
 * http://www.gnu.org/licenses
 *
 * Preprocessor Main
 *
 ******************************************************************************/

#include <string>
#include <sstream>
#include <fstream>
#include <streambuf>
#include <iostream>
#include <cstdlib>
#include <cstring>

#include <algorithm>

#include "fileio.h"
#include "prep.h"

using namespace std;

string gFilename;
bool gDebugMode = false;
bool gDocMode;
bool gIsHeader;
MType gMTType = MTNone;


void usage() {
	cerr << "preprocessor error: Unknown parameters." << endl;
	cerr << "  Usage : prep generate <dbg_mode> <mt_type> <inputdir> <inputfile> <outputfile>" << endl;
	cerr << "     or : prep docgen <dbg_mode> <mt_type> <inputdir> <inputfile> <outputfile>" << endl;
        cerr << "     or : prep link <regfiles...>" << endl;
        cerr << "     or : prep build <cpuinputdir> <gpuinputdir> <libfile> <outputdir>" << endl;
        exit(1);
}

void doMerge(int argc, char* argv[]) {    
	if (argc < 3) usage();
	
	generateMerge(argc-2, &argv[2]);
}

void doGenerate(int argc, char* argv[], bool docs) {
	gDocMode = docs;
	gDebugMode = false;
	gMTType    = MTNone;
	if (argc != 7) usage();
	
	// set constants
	const string indir(argv[4]), infile(argv[5]), outfile(argv[6]);
	bool isPython = infile.size() > 3 && !infile.compare(infile.size()-3, 3, ".py");

	// only enable in cmake's PREP_DEBUG mode (passed via cmd line option dbg_mode)
	gDebugMode = atoi(argv[2]) != 0;
	if (!strcmp(argv[3],"TBB")) gMTType = MTTBB;
        if (!strcmp(argv[3],"OPENMP")) gMTType = MTOpenMP;
        if (!strcmp(argv[3],"TF_CPU")) gMTType = MTTF_CPU;
        if (!strcmp(argv[3],"TF_GPU")) gMTType = MTTF_GPU;

	// load complete file into buffer    
	gFilename = indir+infile;
	string text = readFile(gFilename);
	if (text.empty()) {
		cerr << "preprocessor error: Can't read file '" << infile << "'" << endl;
		exit(1);
	}
	// pad text for easier lexing lookups
	text += "\n\n\n";
	
	Sink sink(infile,outfile);
	if (gDocMode) {
		sink.inplace << "/*! \\file " + infile + " */\n";
        } else {
		sink.inplace << "\n\n\n\n\n// DO NOT EDIT !\n";
		sink.inplace << "// This file is generated using the MantaFlow preprocessor (prep generate).";
		sink.inplace << "\n\n\n\n\n";
	}

        if(gMTType == MTTF_CPU) {
            sink.inplace << "#define Manta MantaTF_CPU" << endl << endl;
        } else if(gMTType == MTTF_GPU) {
            sink.inplace << "#define Manta MantaTF_GPU" << endl << endl;
        }
	
        if (isPython) {
                // python file, only registering
                replaceAll(text, "\n", "\\n");
                replaceAll(text, "\r", "");
                replaceAll(text, "\t", "\\t");
                replaceAll(text, "\"", "<qtm>"); // split into two ops to avoid interference
                replaceAll(text, "<qtm>", "\\\"");
                sink.link << "#include \"registry.h\"\n";
                sink.link << "static const Pb::Register _reg(\"" + infile + "\", \"" + text + "\");\n";
        } else {
                if (!gDocMode) {
			sink.link << "#include \"" + infile + "\"\n";
                        if (!gDebugMode && gMTType != MTTF_CPU && gMTType != MTTF_GPU)
				sink.inplace << "#line 1 \"" << indir << infile << "\"\n";
		}
		std::vector<Instantiation> inst;
		processText(text, 1, sink, 0, inst);
		postProcessInstantiations(sink, inst);
	}
	sink.write();
}

void doRegister(int argc, char* argv[]) {
	if(argc<3) { 
		cerr << "Wrong call for prep register, not enough arguments" << endl; 
		exit(1); 
	}
	std::ofstream output(argv[argc-1]); // full path from call
	std::string newl("\n"); 
	
	std::stringstream RegistrationsDefs;
	std::stringstream Registrations;
	for (int i = 2; i < argc-1; i++) {
		std::ifstream input(argv[i]);

		for (std::string line; getline(input, line); )	{
			int pos = line.find("void PbRegister_");
			if (pos != (int)std::string::npos) {
				std::string lineRegEnd = line.substr( pos, line.length() );
				int endpos = lineRegEnd.find("{");
				std::string lineFunc = lineRegEnd.substr( 0, endpos );

				RegistrationsDefs << "\t\textern " << lineFunc << ";" << newl;
				replaceAll(lineFunc, "void ", "");
				Registrations << "\t\t" << lineFunc << ";" << newl;
			}
		}
		input.close();
	}

	// add external declarations
	output << "extern \"C\" {\n";
	output << RegistrationsDefs.str();
	output << "}\n\n";

	// add dummy function calling all of them
	output << "namespace Pb {\n";
	output << "\tvoid MantaEnsureRegistration()\n\t{\n";
	output << Registrations.str();
	output << "\t}\n";
	output << "}\n";
	output.close();
}

bool isNotCFile (const File& file) {
    return file.getExtension().compare("h") && file.getExtension().compare("cpp") ;
}

bool isNotBuildFile (const File& file) {
    return file.getExtension().compare("build");
}

std::vector<std::string> split_string(const std::string& str,
                                      const std::string& delimiter)
{
    std::vector<std::string> strings;

    std::string::size_type pos = 0;
    std::string::size_type prev = 0;
    while ((pos = str.find(delimiter, prev)) != std::string::npos)
    {
        strings.push_back(str.substr(prev, pos - prev));
        prev = pos + delimiter.length();
    }

    // To get the last substring (or only, if delimiter is not found)
    //strings.push_back(str.substr(prev));

    return strings;
}

vector<string> getCustomOpsNames(vector<File> inBuildFiles) {
    inBuildFiles.erase(std::remove_if(inBuildFiles.begin(), inBuildFiles.end(), isNotBuildFile), inBuildFiles.end());

    vector<string> result;
    for (size_t i = 0; i < inBuildFiles.size(); i++) {
        File file = inBuildFiles[i];

        string buildFileContent = file.readFile();

        vector<string> strings = split_string(buildFileContent, "\n");

        result.insert( result.end(), strings.begin(), strings.end() );
    }

    return result;
}

void createTensorBuild(int argc, char* argv[]) {
    if(argc != 6) {
        usage();
        exit(1);
    }


    File inCPUDir = File(argv[2]);
    File inGPUDir = File(argv[3]);
    File libFile =  File(argv[4]);
    File outFileDir =  File(argv[5]);
    File outFileBUILD = outFileDir.changeFilename("BUILD");
    File outFilePy = outFileDir.changeFilename("mantatensor.py");

    vector<File> inCPUFiles = inCPUDir.getFilesOfDirectory();

    vector<string> customOpsNames = getCustomOpsNames(inCPUFiles);

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
    buildStream << gpuSourcesStream.str();
    buildStream << ")" << endl << endl;

    string buildStr = buildStream.str();
    outFileBUILD.writeFile(buildStr);



    stringstream pyStream;

    pyStream << "import tensorflow as tf" << endl << endl;
    pyStream << "mantatensor_module = tf.load_op_library('" << libFile.toString() << "')" << endl;
    for(size_t i = 0; i < customOpsNames.size(); i++) {
        pyStream <<"manta_" << customOpsNames[i] << " = mantatensor_module." << customOpsNames[i] << endl;
    }

    string pyStr = pyStream.str();

    outFilePy.writeFile(pyStr);
}


int main(int argc, char* argv[]) {
	// command line options
	if (argc < 2) usage();

	// prep modes:
	// - preprocessing of header / cpp files, "main" mode
	// - generating doxygen info
	// - merge / link generated functions
	// - create dummy registry call to prevent linkers from discarding python bindings
	if (!strcmp(argv[1],"generate")) 
		doGenerate(argc, argv, false);
	else if (!strcmp(argv[1],"docgen"))
		doGenerate(argc, argv, true);
	else if (!strcmp(argv[1],"link")) 
		doMerge(argc, argv);
	else if (!strcmp(argv[1], "register"))
		doRegister(argc, argv);
        else if (!strcmp(argv[1], "build"))
            createTensorBuild(argc, argv);
        else
		usage();
	
	return 0;
}
 
