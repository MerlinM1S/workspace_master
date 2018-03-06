#include <stdio.h>
#include <string.h>
#include <iostream>
#include <fstream>
#include <sstream>

#include <vector>

#include "fileio.h"

// linux
#include <sys/types.h>
#include <dirent.h>

using namespace std;

string File::getDirectory() {
    return sFile.substr(0, sFile.find_last_of('/') + 1);
}

File File::toDirectory() {
    return File(getDirectory());
}

File File::makeRelative(File absoluteDir) {
    string sPath = absoluteDir.toString();

    if(sPath.length()  > sFile.length() || sPath.compare(sFile.substr(0, sPath.length()))) {
        return File(sFile);
    } else {
        return File(sFile.substr(sPath.length(), sFile.length() - sPath.length()));
    }

    return File(getDirectory());
}

string File::getFilename(bool inclExt) {
    int iDir = sFile.find_last_of('/') + 1;

    if(inclExt) {
        return sFile.substr(iDir, sFile.length() - iDir);
    } else {
        int iExt = sFile.find_last_of('.');
        return sFile.substr(iDir, iExt - iDir);
    }
}

string File::getExtension() const {
    int iExt = sFile.find_last_of('.') + 1;
    return sFile.substr(iExt, sFile.length() - iExt);
}


File File::changeFilename(std::string newFilename, bool inclExt) {
    if(inclExt) {
        return File(getDirectory() + newFilename);
    } else {
        return File(getDirectory() + newFilename + '.' + getExtension());
    }
}

File File::moveDirectory(std::string folder) {
    return File(getDirectory() + folder + '/' + getFilename() + '.' + getExtension());
}

vector<File> File::getFilesOfDirectory() {
    vector<File> files;

    DIR* dirp = opendir(getDirectory().c_str());
    struct dirent* dp;

    if (dirp != NULL) {
        while ((dp = readdir(dirp)) != NULL) {
            if(!strcmp(dp->d_name, ".") || !strcmp(dp->d_name, ".."))
                continue;

            files.push_back(File(getDirectory() + dp->d_name));
        }
    }

    return files;
}

string File::readFile() {
    std::ifstream file(toString().c_str());
    std::stringstream buffer;

    buffer << file.rdbuf();
    return buffer.str();
}

void File::writeFile(string& text) {
    ofstream ofs(toString().c_str(), ios::binary | ios::out);
    if (!ofs.good()) {
        cerr << "preprocessor error: Can't write to file '" << toString() << "'" << endl;
    } else {
        ofs << text;
        ofs.close();
    }
}
