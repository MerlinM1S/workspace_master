#ifndef _P_MT_TTYPE_H
#define _P_MT_TTYPE_H

#include <string>
#include <map>

enum TType {
    TTypeUnkown, TTypeMACGrid, TTypeGridVec3, TTypeFlagGrid, TTypeGridFloat, TTypeLevelsetGrid, TTypeGridInt, TTypeVec3, TTypeVec3i, TTypeFloat, TTypeInt, TTypeBool
};

class TTypeOp {
private:
    static std::map<std::string, TTypeOp> createTypeConverter();

    static const std::map<std::string, TTypeOp> typeConverter;

public:
    std::string mMantaName;
    std::string mTensorName; 
    std::string mTensorRegisterName;  
    bool mIsConst;				// is const type (such as int, real, Vec3, etc.)
    int mPromisedDims;

    std::string mParameterName;

    TType mTType;

    static TTypeOp create(std::string sType);

    TTypeOp() {}

    TTypeOp(const TType _tType);


    bool isScalar() const;

    bool isGrid() const;

    bool isUnkown() const;
};

#endif // _P_MT_TTYPE_H
