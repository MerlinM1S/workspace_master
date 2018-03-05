#ifndef _P_MT_TTYPE_H
#define _P_MT_TTYPE_H

#include <string>
#include <map>

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

    static TTypeOp create(std::string sType) {
        std::map<std::string, TTypeOp> typeConverter;
        typeConverter["MACGrid"]    = TTypeOp(TTypeMACGrid);
        typeConverter["FlagGrid"]   = TTypeOp(TTypeFlagGrid);
        typeConverter["Grid<float>"]= TTypeOp(TTypeGridFloat);
        typeConverter["Grid<Real>"] = TTypeOp(TTypeGridFloat);
        typeConverter["Grid<int>"]  = TTypeOp(TTypeGridInt);
        typeConverter["Grid<Vec3>"] = TTypeOp(TTypeGridVec3);
        typeConverter["Grid<bool>"] = TTypeOp(TTypeGridBool);
        typeConverter["Vec3"]       = TTypeOp(TTypeVec3);
        typeConverter["Real"]       = TTypeOp(TTypeFloat);
        typeConverter["float"]      = TTypeOp(TTypeFloat);
        typeConverter["int"]        = TTypeOp(TTypeInt);
        typeConverter["bool"]       = TTypeOp(TTypeBool);


        if (typeConverter.find(sType) != typeConverter.end()) {
            return TTypeOp(typeConverter[sType]);
        } else {
            return TTypeOp(TTypeUnkown);
        }
    }

    TTypeOp() {}

    TTypeOp(const TType _tType) : tType(_tType) {
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


    bool isScalar() const {
        return promisedDims == 0;
    }

    bool isGrid() const {
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

    bool isUnkown() const {
        return tType == TTypeUnkown;
    }
};

#endif // _P_MT_TTYPE_H
