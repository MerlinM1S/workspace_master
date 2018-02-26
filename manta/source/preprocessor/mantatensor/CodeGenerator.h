#ifndef _P_MT_CODEGENERATOR_H
#define _P_MT_CODEGENERATOR_H

#include <string>
#include <sstream>

class CodeGenerator {
public:
    int intend;

    std::ostringstream os;

public:
    CodeGenerator() : intend(0) { }

    void addLine(std::string text, int intendMod = 0) {
        if(intendMod < 0)
            changeIntend(intendMod);

        insertIntend();
        os << text << std::endl;

        if(intendMod > 0)
            changeIntend(intendMod);
    }

    void changeIntend(int intendMod) {
        intend += intendMod;
    }

    void addText(std::string text) {
        os << text;
    }

    void newLine() {
        os << std::endl;
    }

    std::string toString() const {
        return os.str();
    }

private:
    void insertIntend() {
        for(int i = 0; i < intend; i++) {
            os << "\t";
        }
    }
};

#endif // _P_MT_CODEGENERATOR_H
