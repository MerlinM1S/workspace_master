#include "ttype.h"
#include <string>
#include <map>

using namespace std;

map<string, TTypeOp> TTypeOp::createTypeConverter() {
    std::map<std::string, TTypeOp> map;
    map["MACGrid"]        = TTypeOp(TTypeMACGrid);
    map["FlagGrid"]       = TTypeOp(TTypeFlagGrid);
    map["Grid<float>"]    = TTypeOp(TTypeGridFloat);
    map["Grid<Real>"]     = TTypeOp(TTypeGridFloat);
    map["LevelsetGrid"]   = TTypeOp(TTypeLevelsetGrid);
    map["Grid<int>"]      = TTypeOp(TTypeGridInt);
    map["Grid<Vec3>"]     = TTypeOp(TTypeGridVec3);
    map["Vec3"]           = TTypeOp(TTypeVec3);
    map["Vec3i"]          = TTypeOp(TTypeVec3i);
    map["Real"]           = TTypeOp(TTypeFloat);
    map["float"]          = TTypeOp(TTypeFloat);
    map["int"]            = TTypeOp(TTypeInt);
    map["bool"]           = TTypeOp(TTypeBool);

    return map;
}

const map<string, TTypeOp> TTypeOp::typeConverter =  TTypeOp::createTypeConverter();


TTypeOp TTypeOp::create(const std::string sType) {
    map<string, TTypeOp>::const_iterator it = typeConverter.find(sType);
    if (it != typeConverter.end()) {
        return TTypeOp(it->second);
    } else {
        return TTypeOp(TTypeUnkown);
    }
}

TTypeOp::TTypeOp(const TType _tType) : mTType(_tType) {
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
        mPromisedDims = 4;          // batch, width, height, depth
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


bool TTypeOp::isScalar() const {
    return mPromisedDims == 0;
}

bool TTypeOp::isGrid() const {
    switch(mTType) {
    case TTypeMACGrid:
    case TTypeGridVec3:
    case TTypeFlagGrid:
    case TTypeGridFloat:
    case TTypeLevelsetGrid:
    case TTypeGridInt:
        return true;
    default:
        return false;
    }
}

bool TTypeOp::isUnkown() const {
    return mTType == TTypeUnkown;
}

