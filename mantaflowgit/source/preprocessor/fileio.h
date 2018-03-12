#ifndef _FILE_IO_H
#define _FILE_IO_H

#include <string>
#include <vector>
 
 
struct File {
public:
    File(const std::string& _sFile) : sFile(_sFile) { }      // /home/user/test.txt
    
    std::string getDirectory();                             // /home/user/
    std::string getFilename(bool inclExt = true);           // test.txt 
    std::string getExtension() const;                       // txt


    File toDirectory();        							// -> home/user/
    File makeRelative(File absoluteDir);					// /home/ -> user/test.txt
    File changeFilename(std::string newFilename, bool inclExt = true);        	// data -> /home/user/data.txt
    File moveDirectory(std::string folder);                                   	// tmp -> /home/user/tmp/test.txt
    
    std::vector<File> getFilesOfDirectory();
    
    std::string readFile();
    
    void writeFile(std::string& text);

    std::string toString() { return sFile; }

private:
    std::string sFile;
};

#endif // _FILE_IO_H
