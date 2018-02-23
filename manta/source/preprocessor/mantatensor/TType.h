#ifndef _P_MT_TTYPE_H
#define _P_MT_TTYPE_H

#include <string>

enum TType {
    TTypeUnkown, TTypeMACGrid, TTypeFlagGrid, TTypeGridFloat, TTypeGridInt, TTypeGridVec3, TTypeGridBool, TTypeVec3, TTypeFloat, TTypeInt, TTypeBool
};

class TTypeOp {
public:
    std::string cName;
    std::string name;            // type name
    std::string hName;           // header type name
    bool isConst;
    int promisedDims;

    std::string pName;           // paramter type name

    TType tType;


    bool isScalar() {
        return promisedDims == 0;
    }

    bool isGrid() {
        switch(tType) {
        case TTypeMACGrid:
        case TTypeGridVec3:
        case TTypeFlagGrid:
        case TTypeGridFloat:
        case TTypeGridBool:
        case TTypeGridInt:
            return true;
        default:
            return false;
        }
    }

    TTypeOp() {}

    TTypeOp(TType _tType) : tType(_tType) {
        switch(tType) {
        case TTypeUnkown:
            promisedDims = 0;
            break;
        case TTypeMACGrid:
            cName = "MACGrid";
            name = "float";
            hName = "float";
            isConst = false;
            promisedDims = 5;
            break;
        case TTypeGridVec3:
            cName = "Grid<Vec3>";
            name = "float";
            hName = "float";
            isConst = false;
            promisedDims = 5;
            break;
        case TTypeFlagGrid:
            cName = "FlagGrid";
            name = "int";
            hName = "int32";
            isConst = false;
            promisedDims = 4;
            break;
        case TTypeGridFloat:
            cName = "Grid<float>";
            name = "float";
            hName = "float";
            isConst = false;
            promisedDims = 4;
            break;
        case TTypeGridBool:
            cName = "Grid<bool>";
            name = "bool";
            hName = "bool";
            isConst = false;
            promisedDims = 4;
            break;
        case TTypeGridInt:
            cName = "Grid<int>";
            name = "int";
            hName = "int32";
            isConst = false;
            promisedDims = 4;
            break;
        case TTypeVec3:
            cName = "Vec3";
            name = "float";
            hName = "float";
            isConst = true;
            promisedDims = 2;
            break;
        case TTypeFloat:
            cName = "float";
            name = "float";
            hName = "float";
            isConst = true;
            promisedDims = 1;
            break;
        case TTypeInt:
            cName = "int";
            name = "int";
            hName = "int32";
            isConst = true;
            promisedDims = 1;
            break;
        case TTypeBool:
            cName = "bool";
            name = "bool";
            hName = "bool";
            isConst = true;
            promisedDims = 1;
            break;
        }

        pName = name;
        if(!isScalar())
            pName += "*";
    }

    TTypeOp(std::string _cName, std::string _name, std::string _hName, bool _isConst, int _promisedDims) : cName(_cName), name(_name), hName(_hName), isConst(_isConst), promisedDims(_promisedDims) {
        pName = name;
        if(!isScalar())
            pName += "*";
    }

    bool isUnkown() {
        return tType == TTypeUnkown;
    }
};

#endif // _P_MT_TTYPE_H
