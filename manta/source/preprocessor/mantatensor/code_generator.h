#ifndef _P_MT_CODE_GENERATOR_H
#define _P_MT_CODE_GENERATOR_H

#include <string>
#include <sstream>

class CodeGenerator {
public:
    int mIntend;

    std::ostringstream mOS;

public:
    CodeGenerator() : mIntend(0) { }

    void addLine(std::string text, int intendMod = 0) {
        if(intendMod < 0)
            changeIntend(intendMod);

        insertIntend();
        mOS << text << std::endl;

        if(intendMod > 0)
            changeIntend(intendMod);
    }

    void changeIntend(int intendMod) {
        mIntend += intendMod;
    }

    void addText(std::string text) {
        mOS << text;
    }

    void newLine() {
        mOS << std::endl;
    }

    std::string toString() const {
        return mOS.str();
    }

private:
    void insertIntend() {
        for(int i = 0; i < mIntend; i++) {
            mOS << "\t";
        }
    }
};

#endif // _P_MT_CODE_GENERATOR_H
