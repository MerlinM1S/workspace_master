#include "prep.h"
// using ofstream constructors.
#include <iostream>
#include <fstream>
#include <map>
#include <sstream>
#include <algorithm>
#include <vector>


#define SSTR( x ) static_cast< std::ostringstream & >( \
        ( std::ostringstream() << std::dec << x ) ).str()

using namespace std;


class CodeGenerator {
public:
    int intend;

    ostringstream os;

public:
    CodeGenerator() : intend(0) { }

    void addLine(string text, int intendMod = 0) {
        if(intendMod < 0)
            intend += intendMod;

        insertIntend();
        os << text << endl;

        if(intendMod > 0)
            intend += intendMod;
    }

    void addText(string text) {
        os << text;
    }

    void newLine() {
        os << endl;
    }

    string toString() {
        return os.str();
    }

private:
    void insertIntend() {
        for(int i = 0; i < intend; i++) {
            os << "\t";
        }
    }
};

class StringList {
private:
    vector<string> sList;

public:
    void add(string text) {
        if(text.length() > 0) {
            sList.push_back(text);
        }
    }

    string toString() {
        string text = "";
        for(size_t i = 0; i < sList.size(); i++) {
            text += sList[i];
            if(sList.size() - 1 > i) {
                text += ", ";
            }
        }
        return text;
    }
};



void stringReplace(string& source, string const& find, string const& replace)
{
    for(string::size_type i = 0; (i = source.find(find, i)) != string::npos;)
    {
        source.replace(i, find.length(), replace);
        i += replace.length();
    }
}


string convertToSnake_case(string camelCase) {
    std::string str(1, tolower(camelCase[0]));

    // First place underscores between contiguous lower and upper case letters.
    // For example, `_LowerCamelCase` becomes `_Lower_Camel_Case`.
    for (string::iterator it = camelCase.begin() + 1; it != camelCase.end(); ++it) {
      if (isupper(*it) && *(it-1) != '_' && (islower(*(it-1)) || islower(*(it+1)))) {
        str += "_";
      }
      str += *it;
    }

    // Then convert it to lower case.
    std::transform(str.begin(), str.end(), str.begin(), ::tolower);

    return str;
}



struct SimpleBlock {
    Block block;
    string tensorFuncName;
    string mantaFuncName;

    vector<string> newTypes;

    SimpleBlock(Block _block) : block(_block) {
        mantaFuncName = block.func.name;
        if(block.func.isTemplated()) {
            mantaFuncName += "<>";
        }

        tensorFuncName = block.func.name;
    }

    SimpleBlock(SimpleBlock sBlock, string nType) : block(sBlock.block) {
        mantaFuncName = block.func.name;
        tensorFuncName = block.func.name;

        newTypes = sBlock.newTypes;
        newTypes.push_back(nType);

        if(!newTypes.empty()) {
            mantaFuncName += "<";

            for(size_t i = 0; i < newTypes.size(); i++) {
                mantaFuncName += newTypes[i];
                if(newTypes.size() > i + 1)
                    mantaFuncName += ", ";


                string NewType = newTypes[i];
                NewType[0] = toupper(NewType[0]);
                tensorFuncName += NewType;
            }

            mantaFuncName += ">";
        }
    }

    SimpleBlock(SimpleBlock sBlock, Type& nType) : block(sBlock.block) {
        mantaFuncName = sBlock.mantaFuncName;
        tensorFuncName = sBlock.tensorFuncName;

        string NewType = nType.toString();          // TODO replace ... not correct
        stringReplace(NewType, "<", "");
        stringReplace(NewType, ">", "");
        NewType[0] = toupper(NewType[0]);
        tensorFuncName += NewType;
    }
};


vector<SimpleBlock> templatePreprocessor(vector<SimpleBlock> &blocks) {
    vector<SimpleBlock> result = blocks;

    string dataTypes[] = {"int", "float", "Vec3"};

    while(result[0].block.func.isTemplated()) {
        SimpleBlock cSBlock = result[0];
        Block* cBlock = &cSBlock.block;

        result.erase (result.begin());

        string removedTemplate = cBlock->func.templateTypes[0].name;
        cBlock->func.templateTypes._data.erase(cBlock->func.templateTypes._data.begin());

        for(size_t i = 0; i < 3; i++) {
            string nType = dataTypes[i];

            SimpleBlock nBlock = SimpleBlock(cSBlock, nType);

            for(size_t j = 0; j < cBlock->func.arguments.size();  j++) {
                Argument argument = cBlock->func.arguments[j];

                if(removedTemplate.compare(argument.type.name) == 0) {
                    argument.type.name = nType;
                }

                List<Type> nTemplates;
                for(size_t k = 0; k < argument.type.templateTypes.size(); k++) {
                    Type cType = argument.type.templateTypes[k];

                    if(removedTemplate.compare(cType.name) == 0) {
                        cType.name = nType;
                    }

                    nTemplates.push_back(cType);
                }
                argument.type.templateTypes = nTemplates;

//                replace_if(templates.begin(), templates.end(), removedTemplate, nType);

                nBlock.block.func.arguments[j] = argument;
            }

            result.push_back(nBlock);
        }
    }

    // Print
//    for(size_t i = 0; i < result.size(); i++) {
//        cout << result[i].mantaFuncName << "/ " << result[i].tensorFuncName << ": ";

//        for(size_t j = 0; j < result[i].block.func.arguments.size();  j++) {

//            cout << result[i].block.func.arguments[j].type.name;


//            if(result[i].block.func.arguments[j].type.isTemplated()) {
//                cout << "<";
//                for(size_t k = 0; k < result[i].block.func.arguments[j].type.templateTypes.size(); k++) {
//                    cout << result[i].block.func.arguments[j].type.templateTypes[k].name;
//                }
//                cout << ">";
//            }


//            cout << ", ";
//        }
//        cout << endl;
//    }


    return result;
}




vector<SimpleBlock> replaceGridBase(const Block& block) {

    Type dataTypes[3];
    dataTypes[0].name = "MACGrid";
    dataTypes[1].name = "Grid";
    {
        Type type;
        type.name = "Vec3";
        dataTypes[1].templateTypes.push_back(type);
    }
    dataTypes[2].name = "Grid";
    {
        Type type;
        type.name = "float";
        dataTypes[2].templateTypes.push_back(type);
    }



    vector<SimpleBlock> result;

    result.push_back(SimpleBlock(block));

    List<Argument> arguments = block.func.arguments;

    for(size_t i = 0; i < arguments.size();  i++) {
        if(arguments[i].type.name.compare("GridBase") == 0) {
            vector<SimpleBlock> nResult;

            for(int k = 0; k< 3; k++) {
                Argument argument = arguments[i];
                argument.type.name = dataTypes[k].name;
                argument.type.templateTypes = dataTypes[k].templateTypes;
                for(size_t j = 0; j < result.size();  j++) {
                    SimpleBlock nBlock = SimpleBlock(result[j], argument.type);

                    nBlock.block.func.arguments[i] = argument;

                    nResult.push_back(nBlock);
                }
            }

            result = nResult;
        }
    }


//        for(size_t i = 0; i < result.size(); i++) {
//            cout << result[i].mantaFuncName << "/ " << result[i].tensorFuncName << ": ";

//            for(size_t j = 0; j < result[i].block.func.arguments.size();  j++) {

//                cout << result[i].block.func.arguments[j].type.name;


//                if(result[i].block.func.arguments[j].type.isTemplated()) {
//                    cout << "<";
//                    for(size_t k = 0; k < result[i].block.func.arguments[j].type.templateTypes.size(); k++) {
//                        cout << result[i].block.func.arguments[j].type.templateTypes[k].name;
//                    }
//                    cout << ">";
//                }


//                cout << ", ";
//            }
//            cout << endl;
//        }


    return result;
}



enum TType {
    TTypeUnkown, TTypeMACGrid, TTypeFlagGrid, TTypeGridFloat, TTypeGridInt, TTypeGridVec3, TTypeGridBool, TTypeVec3, TTypeFloat, TTypeInt, TTypeBool
};

class TTypeOp {
public:
    string cName;
    string name;            // type name
    string hName;           // header type name
    bool isConst;
    int promisedDims;

    string pName;           // paramter type name

    TType tType;


    bool isScalar() {
        return promisedDims == 0;
    }

    TTypeOp() {}

    TTypeOp(TType _tType) : tType(_tType) {
        switch(tType) {
        case TTypeUnkown:
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

    TTypeOp(string _cName, string _name, string _hName, bool _isConst, int _promisedDims) : cName(_cName), name(_name), hName(_hName), isConst(_isConst), promisedDims(_promisedDims) {
        pName = name;
        if(!isScalar())
            pName += "*";
    }

    bool isUnkown() {
        return tType == TTypeUnkown;
    }

};

class TArgument {
public:
    TTypeOp tType;

    const Argument* argument;

    int inIndex;
    int outIndex;


private:
    TArgument(TTypeOp _tType, const Argument* _argument) : tType(_tType), argument(_argument), inIndex(-1), outIndex(-1) {  }

public:
    static TArgument* create(const Argument* argument) {
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

        if (typeConverter.find(argument->type.toString()) != typeConverter.end()) {
            return new TArgument(typeConverter[argument->type.toString()], argument);
        } else if (argument->value.length() > 0){
            return new TArgument(TTypeOp(TTypeUnkown), argument);
        } else {
            return NULL;
        }
    }

    bool hasDefaultValue() {
        return argument->value.length() > 0;
    }


    bool isValid() {
        return tType.name.length() > 0;
    }

    void applyIndex(int &_inIndex, int &_outIndex) {
        if(tType.isUnkown()) {
            return;
        }

        inIndex = _inIndex;
        _inIndex++;

        if(!isTypeConst()) {
            outIndex = _outIndex;
            _outIndex++;
        }
    }

    bool isTypeConst() {
        return argument->type.isConst || tType.isConst;
    }

    string getInputName() {
        return "in_" + convertToSnake_case(argument->name);
    }

    string getOutputName() {
        return "out_" + convertToSnake_case(argument->name);
    }

    string getInTensorName() {
        return getInputName() + "_tensor";
    }

    string getOutTensorName() {
        return getOutputName() + "_tensor";
    }

    void addDimSizeLines(CodeGenerator& codeGenerator) {
        string dimNames[4] = {"batches", "depth", "height", "width"};
        for(int i = 0; i < 4; i++) {
            string lineOp = "long " + dimNames[i] + " = ";

            if(tType.promisedDims > i)
               lineOp += getInTensorName() + ".shape().dim_size(" + SSTR(i) + ");";
            else
               lineOp += "-1;";

            codeGenerator.addLine(lineOp);
        }


        codeGenerator.addLine("DimSize dimSize = DimSize(batches, depth, height, width);");    // TODO might be: batches, width, depth, height
    }

    void addInputLine(CodeGenerator& codeGenerator) {
        if(inIndex >= 0) {
            codeGenerator.addLine(".Input(\"" + getInputName() + ": " + tType.hName + "\")");
        }
    }

    void addOutputLine(CodeGenerator& codeGenerator) {
        if(outIndex >= 0) {
            codeGenerator.addLine(".Output(\"" + getOutputName() + ": " + tType.hName + "\")");
        }
    }

    void addShapeInferenceLine(CodeGenerator& codeGenerator) {
        if(outIndex >= 0 && inIndex >= 0) {
            codeGenerator.addLine("c->set_output(" + SSTR(outIndex) + ", c->input(" + SSTR(inIndex) + "));");
        }
    }

    string generateInHeaderParam() {
        if(inIndex < 0)
            return "";
        else
            return "const " + tType.pName + " " + getInputName();
    }

    string generateOutHeaderParam() {
        if(outIndex < 0)
            return "";
        else
            return tType.pName + " " + getOutputName();
    }

    void addInTensorLines(CodeGenerator& codeGenerator) {
        if(inIndex < 0) {
            return;
        }

        codeGenerator.addLine("const Tensor& " + getInTensorName() + " = context->input(" + SSTR(inIndex) + ");");

        if(tType.isScalar()) {
            codeGenerator.addLine("const " + tType.pName + " " + getInputName() + " = " + getInTensorName() + ".scalar<" + tType.name + ">().data()[0];");
        } else {
            codeGenerator.addLine("const " + tType.pName + " " + getInputName() + " = " + getInTensorName() + ".flat<" + tType.name + ">().data();");
        }

        codeGenerator.newLine();
    }

    void addOutTensorLines(CodeGenerator& codeGenerator) {
        if(outIndex < 0) {
            return;
        }

        codeGenerator.addLine("Tensor* " + getOutTensorName() + " = NULL;");
        codeGenerator.addLine("OP_REQUIRES_OK(context, context->allocate_output(" + SSTR(outIndex) + ", " + getInTensorName() + ".shape(), &" + getOutTensorName() + "));");
        codeGenerator.addLine(tType.pName + " " + getOutputName() + " = " + getOutTensorName() + "->flat<" + tType.name + ">().data();");

        codeGenerator.newLine();
    }


    string generateInParam() {
        if(inIndex < 0)
            return "";
        else
            return getInputName();
    }

    string generateOutParam() {
        if(outIndex < 0)
            return "";
        else
            return getOutputName();
    }

    string generateMantaParam() {
        string text = "";

        if(tType.isUnkown()) {
            text += argument->value;
        } else {
            if(argument->type.isPointer)
                text += "&";
            text += argument->name;
        }


        return text;
    }

    void addCopyInToOutLines(CodeGenerator& codeGenerator) {
        if(outIndex >= 0 && inIndex >= 0) {
            codeGenerator.addLine("for(int i = 0; i < dimSize.lengthOf(" + SSTR(tType.promisedDims) + "); i++) {", 1);
            codeGenerator.addLine(getOutputName() + "[i] = " + getInputName() + "[i];");
            codeGenerator.addLine("}", -1);
            codeGenerator.newLine();
        }
    }

    void addMantaVariableCreation(CodeGenerator& codeGenerator, string batch) {
        if(tType.isUnkown())
            return;

        string baseVariableName;
        if(outIndex >= 0) {
            baseVariableName = getOutputName();
        } else if (inIndex >= 0){
            baseVariableName = getInputName();
        } else {
            return;
        }

        bool constCast = inIndex >= 0;

        switch(tType.tType) {
        case TTypeGridInt:
        case TTypeGridFloat:
        case TTypeGridBool:
        case TTypeFlagGrid:
            AddDefaultCheck(codeGenerator, tType.cName + "(&fluidSolver, " + AddConstCast(baseVariableName + " + dimSize.batchToIndex(4, " + batch + ")", constCast) + ", true);");
            break;
        case TTypeGridVec3:
        case TTypeMACGrid:
            AddDefaultCheck(codeGenerator, tType.cName + "(&fluidSolver, (Vec3*) (" + AddConstCast(baseVariableName + " + dimSize.batchToIndex(4, " + batch + ")", constCast) +  "), true);");
            break;
        case TTypeVec3:
            codeGenerator.addLine(tType.cName + " " + argument->name + " = Vec3(" +  baseVariableName + " + (3 * " + batch + "));");
            break;
        default:
            codeGenerator.addLine(tType.cName + " " + argument->name + " = " + baseVariableName + "[" + batch + "];");
            break;
        }
    }



private:
    string AddConstCast(string arrayPointer, bool constCast) {
        if(constCast) {
            return "const_cast<" + tType.pName + ">(" +  arrayPointer + ")";
        } else {
            return arrayPointer;
        }
    }

    void AddDefaultCheck(CodeGenerator& codeGenerator, string makeString) {
        if(hasDefaultValue()) {
            codeGenerator.addLine(tType.cName + " " + argument->name + " = NULL");
            codeGenerator.addLine("if(" + getInputName() + ") {", 1);
            codeGenerator.addLine(argument->name + " = " + makeString);
            codeGenerator.addLine("}", -1);
        } else {
            codeGenerator.addLine(tType.cName + " " + argument->name + " = " + makeString);
        }
    }
};


class TensorProcessor {
private:
    string tensorFuncName;
    string mantaFuncName;
    string filename;

    List<TArgument*> tArguments;

    TArgument* argumentWithHighestDims;

    bool convertable;

    string tensor_func_name() {
        return convertToSnake_case(tensorFuncName);
    }

    string TensorFuncName() {
        string result = tensorFuncName;
        result[0] = toupper(result[0]);
        return result;
    }

    string tensorFuncName_Functor() {
        return tensorFuncName + "_Functor";
    }

    string tensorFuncName_OP() {
        return tensorFuncName + "_OP";
    }

    enum DeviceType {
        DeviceTypeGeneral, DeviceTypeCPU, DeviceTypeGPU
    };

    void addIncludesEtc(CodeGenerator &codeGenerator) {
        codeGenerator.addLine("#define EIGEN_USE_THREADS");
        codeGenerator.newLine();
        codeGenerator.addLine("#if GOOGLE_CUDA");
        codeGenerator.addLine("#define EIGEN_USE_GPU");
        codeGenerator.addLine("#endif");
        codeGenerator.newLine();
        codeGenerator.newLine();
        codeGenerator.addLine("#include \"dim_size.h\"\r\n");
        codeGenerator.newLine();
        codeGenerator.addLine("#include \"tensorflow/core/framework/op.h\"");
        codeGenerator.addLine( "#include \"tensorflow/core/framework/shape_inference.h\"");
        codeGenerator.addLine("#include \"tensorflow/core/framework/op_kernel.h\"");
        codeGenerator.newLine();
        codeGenerator.addLine("using namespace tensorflow;");
        codeGenerator.addLine("using namespace Manta;");
    }

    void addHeader(CodeGenerator &codeGenerator) {
        StringList arguments;
        string opLine = "";

        codeGenerator.addLine("template <typename Device>");
        codeGenerator.addLine("struct " + tensorFuncName_Functor() + " {", 1);
        opLine += "void operator()(";
        arguments.add("const Device& d");

        arguments.add("const DimSize dimSize");
        for(size_t i = 0; i < tArguments.size(); i++) {
            arguments.add(tArguments[i]->generateInHeaderParam());
            arguments.add(tArguments[i]->generateOutHeaderParam());
        }
        opLine += arguments.toString();
        opLine += ");";

        codeGenerator.addLine(opLine);
        codeGenerator.addLine("};", -1);
    }

    void addRegisterOP(CodeGenerator& codeGenerator) {
        codeGenerator.addLine("REGISTER_OP(\"" + TensorFuncName() + "\")", 1);

        for(size_t i = 0; i < tArguments.size(); i++) {
            tArguments[i]->addInputLine(codeGenerator);
        }

        for(size_t i = 0; i < tArguments.size(); i++) {
            tArguments[i]->addOutputLine(codeGenerator);
        }

        {
            codeGenerator.addLine(".SetShapeFn([](::tensorflow::shape_inference::InferenceContext* c) {", 1);
            for(size_t i = 0; i < tArguments.size(); i++) {
                tArguments[i]->addShapeInferenceLine(codeGenerator);
            }
            codeGenerator.addLine("return Status::OK();");
            codeGenerator.addLine("});", -1);
            codeGenerator.addLine("", -1);
        }
    }

    void addUsingDefs(CodeGenerator& codeGenerator) {
        codeGenerator.addLine("using CPUDevice = Eigen::ThreadPoolDevice;");
        codeGenerator.addLine("using GPUDevice = Eigen::GpuDevice;");
    }


    string getDeviceName(DeviceType deviceType) {
        switch(deviceType) {
        case DeviceTypeCPU:
            return "CPUDevice";
        case DeviceTypeGPU:
            return "GPUDevice";
        default:
            return "Device";
        }
    }

    void addFuncHeader(CodeGenerator& codeGenerator, DeviceType deviceType) {
        StringList arguments;
        string opLine = "";
        string deviceName = getDeviceName(deviceType);

        codeGenerator.addLine("template <>");
        opLine += "void " + tensorFuncName_Functor() + "<" + deviceName + ">::operator()(";
        arguments.add("const " + deviceName + "& d");

        arguments.add("const DimSize dimSize");
        for(size_t i = 0; i < tArguments.size(); i++) {
            arguments.add(tArguments[i]->generateInHeaderParam());
            arguments.add(tArguments[i]->generateOutHeaderParam());
        }
        opLine += arguments.toString();
        opLine += ") {";

        codeGenerator.addLine(opLine, 1);
    }


    void addFuncImplementationCPU(CodeGenerator& codeGenerator) {
        addFuncHeader(codeGenerator, DeviceTypeCPU);


        // Copy In to Out
        for(size_t i = 0; i < tArguments.size(); i++) {
            tArguments[i]->addCopyInToOutLines(codeGenerator);
        }

        codeGenerator.addLine("FluidSolver fluidSolver = FluidSolver(Vec3i(dimSize.width, dimSize.height, dimSize.depth));");
        codeGenerator.newLine();

        codeGenerator.addLine("for(int i_b = 0; i_b < dimSize.batches; i_b++) {", 1);

        // Transform In-types to Tensorflow-types
        for(size_t i = 0; i < tArguments.size(); i++) {
            tArguments[i]->addMantaVariableCreation(codeGenerator, "i_b");
        }

        codeGenerator.newLine();

        // Func Call
        {
            string opLine = "";
            StringList arguments;

            opLine += mantaFuncName + "(";

            for(size_t i = 0; i < tArguments.size(); i++) {
                arguments.add(tArguments[i]->generateMantaParam());
            }
            opLine += arguments.toString();

            opLine += ");";

            codeGenerator.addLine(opLine);
        }

        codeGenerator.addLine("}", -1);
        codeGenerator.addLine("}", -1);
    }


    void addFuncOP(CodeGenerator& codeGenerator) {

        codeGenerator.addLine("template <typename Device>");
        codeGenerator.addLine("class " + tensorFuncName_OP() + " : public OpKernel {");
        codeGenerator.addLine("public:", 1);
        codeGenerator.addLine("explicit " + tensorFuncName_OP() +"(OpKernelConstruction* context) : OpKernel(context) {}");
        codeGenerator.newLine();
        codeGenerator.addLine("void Compute(OpKernelContext* context) override {", 1);

        // InTensor
        for(size_t i = 0; i < tArguments.size(); i++) {
            tArguments[i]->addInTensorLines(codeGenerator);
        }

        // OutTensor
        for(size_t i = 0; i < tArguments.size(); i++) {
            tArguments[i]->addOutTensorLines(codeGenerator);
        }


        // DimSize
        argumentWithHighestDims->addDimSizeLines(codeGenerator);

        codeGenerator.newLine();

        // Function call
        {
            string funcCall = "";
            StringList arguments;

            funcCall += tensorFuncName_Functor() + "<Device>()(";

            arguments.add("context->eigen_device<Device>()");
            arguments.add("dimSize");

            for(size_t i = 0; i < tArguments.size(); i++) {
                arguments.add(tArguments[i]->generateInParam());
                arguments.add(tArguments[i]->generateOutParam());
            }
            funcCall += arguments.toString();
            funcCall += ");";

            codeGenerator.addLine(funcCall);
        }


        codeGenerator.addLine("}", -1);
        codeGenerator.addLine("};", -1);
    }

    void addRegisterKernelCPU(CodeGenerator& codeGenerator) {
        codeGenerator.addLine("REGISTER_KERNEL_BUILDER(Name(\"" + TensorFuncName() + "\").Device(DEVICE_CPU), " + tensorFuncName_OP() + "<CPUDevice>);");
    }

    void addRegisterKernelGPU(CodeGenerator& codeGenerator) {
        codeGenerator.addLine("#if GOOGLE_CUDA");
        codeGenerator.newLine();
        codeGenerator.addLine("REGISTER_KERNEL_BUILDER(Name(\"" + TensorFuncName() + "\").Device(DEVICE_GPU), " + tensorFuncName_OP() + "<GPUDevice>);");
        codeGenerator.newLine();
        codeGenerator.addLine("#endif");
    }

public:
    TensorProcessor(const SimpleBlock& sBlock, const std::string& code, Sink& sink) {
        const Block* block = &sBlock.block;
        tensorFuncName = sBlock.tensorFuncName;
        mantaFuncName = sBlock.mantaFuncName;

        argumentWithHighestDims = 0;
        int highestDims = -1;

        {
            int inIndex = 0;
            int outIndex = 0;
            for(unsigned int i = 0; i < block->func.arguments.size(); i++) {
                TArgument* argument = TArgument::create(&(block->func.arguments[i]));

                if(!argument) {
                    convertable = false;
                    return;
                }

                argument->applyIndex(inIndex, outIndex);

                tArguments.push_back(argument);

                if(argument->tType.promisedDims > highestDims) {
                    highestDims = argument->tType.promisedDims ;
                    argumentWithHighestDims = argument;
                }
            }
        }
        convertable = true;

        filename = sink.getFilename() + "wat";
    }

    bool canConvert() {
        return convertable;
    }

    string generateString() {
        CodeGenerator codeGenerator;

        codeGenerator.newLine();
        codeGenerator.newLine();

        codeGenerator.addLine("} // namespace");

        codeGenerator.newLine();
        codeGenerator.newLine();

        codeGenerator.addLine("// -------------------------- TENSORFLOW OP ------------------------- ");

        codeGenerator.newLine();

        addIncludesEtc(codeGenerator);

        codeGenerator.newLine();

        addHeader(codeGenerator);

        codeGenerator.newLine();

        addRegisterOP(codeGenerator);

        codeGenerator.newLine();
        codeGenerator.newLine();

        addUsingDefs(codeGenerator);

        codeGenerator.newLine();
        codeGenerator.newLine();

        addFuncImplementationCPU(codeGenerator);

        codeGenerator.newLine();
        codeGenerator.newLine();

        addFuncOP(codeGenerator);

        codeGenerator.newLine();
        codeGenerator.newLine();

        addRegisterKernelCPU(codeGenerator);

//        codeGenerator.newLine();
//        codeGenerator.newLine();

//        addRegisterKernelGPU(codeGenerator);

        codeGenerator.newLine();


        codeGenerator.addLine("// ------------------------------------------------------------------ ");

        codeGenerator.newLine();
        codeGenerator.newLine();


        codeGenerator.addLine("namespace Manta {");


        return codeGenerator.toString();
    }

    void write() {
        std::ofstream outfile (filename.c_str());

        outfile << generateString();

        outfile.close();
    }

    string getOpName() {
        return tensor_func_name();
    }

    ~TensorProcessor() {
        for(size_t i = 0; i < tArguments.size(); i++) {
            delete (tArguments[i]);
        }
    }
};


void processTensorFunction(const Block& block, const string& code, Sink& sink) {
    if(gMTType != MTTensor)
        return;

    vector<SimpleBlock> simpleBlocks = replaceGridBase(block);
    simpleBlocks = templatePreprocessor(simpleBlocks);

    for(size_t i = 0; i < simpleBlocks.size(); i++) {
        TensorProcessor tensorProcessor = TensorProcessor(simpleBlocks[i], code, sink);

        if(!tensorProcessor.canConvert()) {
            cout << "Could not convert: " << block.func.name << endl;
            return;
        }

        sink.buildInfo << tensorProcessor.getOpName() << endl;
        sink.inplace << tensorProcessor.generateString();
    }



    //tensorProcessor.write();
}
