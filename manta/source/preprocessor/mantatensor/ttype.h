#ifndef _P_MT_TTYPE_H
#define _P_MT_TTYPE_H

#include <string>
#include <map>
#include <iostream> //only used for cout

enum TType {
    TTypeUnkown, TTypeMACGrid, TTypeGridVec3, TTypeFlagGrid, TTypeGridFloat, TTypeLevelsetGrid, TTypeGridInt, TTypeVec3, TTypeVec3i, TTypeFloat, TTypeInt, TTypeBool
};

class TTypeOp {
public:
    std::string mMantaName;
    std::string mTensorName; 
    std::string mTensorRegisterName;  
    bool mIsConst;				// is const type (such as int, real, Vec3, etc.)
    int mPromisedDims;

    std::string mParameterName;

    TType mTType;

    static TTypeOp create(std::string sType) {
        std::map<std::string, TTypeOp> typeConverter;
        typeConverter["MACGrid"]        = TTypeOp(TTypeMACGrid);
        typeConverter["FlagGrid"]       = TTypeOp(TTypeFlagGrid);
        typeConverter["Grid<float>"]    = TTypeOp(TTypeGridFloat);
        typeConverter["Grid<Real>"]     = TTypeOp(TTypeGridFloat);
        typeConverter["LevelsetGrid"]   = TTypeOp(TTypeLevelsetGrid);
        typeConverter["Grid<int>"]      = TTypeOp(TTypeGridInt);
        typeConverter["Grid<Vec3>"]     = TTypeOp(TTypeGridVec3);
        typeConverter["Vec3"]           = TTypeOp(TTypeVec3);
        typeConverter["Vec3i"]          = TTypeOp(TTypeVec3i);
        typeConverter["Real"]           = TTypeOp(TTypeFloat);
        typeConverter["float"]          = TTypeOp(TTypeFloat);
        typeConverter["int"]            = TTypeOp(TTypeInt);
        typeConverter["bool"]           = TTypeOp(TTypeBool);


        if (typeConverter.find(sType) != typeConverter.end()) {
            return TTypeOp(typeConverter[sType]);
        } else {
            return TTypeOp(TTypeUnkown);
        }
    }

    TTypeOp() {}

    TTypeOp(const TType _tType) : mTType(_tType) {
        switch(mTType) {
        case TTypeUnkown:
            mPromisedDims = 0;
            break;
        case TTypeMACGrid:
            mMantaName = "MACGrid";
            mTensorName = "float";
            mTensorRegisterName = "float";
            mIsConst = false;
            mPromisedDims = 5;		// batch, width, height, depth, (x,y,z)
            break;
        case TTypeGridVec3:
            mMantaName = "Grid<Vec3>";
            mTensorName = "float";
            mTensorRegisterName = "float";
            mIsConst = false;
            mPromisedDims = 5;		// batch, width, height, depth, (x,y,z)
            break;
        case TTypeFlagGrid:
            mMantaName = "FlagGrid";
            mTensorName = "int";
            mTensorRegisterName = "int32";
            mIsConst = false;
            mPromisedDims = 4;
            break;
        case TTypeGridFloat:
            mMantaName = "Grid<float>";
            mTensorName = "float";
            mTensorRegisterName = "float";
            mIsConst = false;
            mPromisedDims = 4;		// batch, width, height, depth
            break;
        case TTypeLevelsetGrid:
            mMantaName = "LevelsetGrid";
            mTensorName = "float";
            mTensorRegisterName = "float";
            mIsConst = false;
            mPromisedDims = 4;		// batch, width, height, depth
            break;
        case TTypeGridInt:
            mMantaName = "Grid<int>";
            mTensorName = "int";
            mTensorRegisterName = "int32";
            mIsConst = false;
            mPromisedDims = 4;		// batch, width, height, depth
            break;
        case TTypeVec3:
            mMantaName = "Vec3";
            mTensorName = "float";
            mTensorRegisterName = "float";
            mIsConst = true;
            mPromisedDims = 2;		// batch, (x,y,z)
            break;
        case TTypeVec3i:
            mMantaName = "Vec3i";
            mTensorName = "int";
            mTensorRegisterName = "int32";
            mIsConst = true;
            mPromisedDims = 2;		// batch, (x,y,z)
            break;
        case TTypeFloat:
            mMantaName = "float";
            mTensorName = "float";
            mTensorRegisterName = "float";
            mIsConst = true;
            mPromisedDims = 1;		// batch
            break;
        case TTypeInt:
            mMantaName = "int";
            mTensorName = "int";
            mTensorRegisterName = "int32";
            mIsConst = true;
            mPromisedDims = 1;		// batch
            break;
        case TTypeBool:
            mMantaName = "bool";
            mTensorName = "bool";
            mTensorRegisterName = "bool";
            mIsConst = true;
            mPromisedDims = 1;		// batch
            break;
        }

        mParameterName = mTensorName;
        if(!isScalar())
            mParameterName += "*";
    }


    bool isScalar() const {
        return mPromisedDims == 0;
    }

    bool isGrid() const {
        switch(mTType) {
        case TTypeMACGrid:
        case TTypeGridVec3:
        case TTypeFlagGrid:
        case TTypeGridFloat:
        case TTypeGridInt:
            return true;
        default:
            return false;
        }
    }

    bool isUnkown() const {
        return mTType == TTypeUnkown;
    }
};

#endif // _P_MT_TTYPE_H
