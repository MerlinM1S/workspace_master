#include "code_util.h"
#include "string_util.h"

std::string typeToString(Type type) {
    std::string text = type.name;
    if(type.isTemplated()) {
        text += "<";

        StringList templateTypes;
        for(size_t i = 0; i < type.templateTypes.size(); i++) {
            templateTypes.add(typeToString(type.templateTypes[i]));
        }
        text += templateTypes.toString();

        text += ">";
    }
    return text;
}
