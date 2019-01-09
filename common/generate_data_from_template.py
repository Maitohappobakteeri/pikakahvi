#!/usr/bin/env python3


import sys


INDENTATION = " " * 4


STRUCT_TEMPLATE = "\
#ifndef {headerGuard}_HPP\n\
#define {headerGuard}_HPP\n\
#include \"db/db.hpp\"\n\
#include \"db/conversions.hpp\"\n\
#include <string>\n\
using std::string;\n\
struct {cppName} {{\n\
{indentation}{cppName}() {{\n\
\n\
{indentation}}}\n\
{indentation}{cppName}({args}) {{\n\
{constructorAssignments}\
{indentation}}}\n\
{indentation}{cppName}(int argc, char **argv, char **azColName){{\n\
{indentation}{indentation}auto mapped = Db::mapExecCallbackArgs(argc, argv, azColName);\n\
{constructorAssignments2}\
{indentation}}}\n\
{indentation}string toString() const {{\n\
{indentation}{indentation}return std::string(\"{cppName}\\n\")\n\
{fieldsAndValues}\n\
{indentation}}}\n\
{fieldDeclarations}\
}};\n\
#endif // {headerGuard}_HPP\
"

JAVA_TEMPLATE = "\
package com.example.juho.pikakahvi.data;\n\
public class {cppName} {{\n\
{fieldDeclarations}\
{indentation}{cppName}() {{\n\
\n\
{indentation}}}\n\
{indentation}{cppName}({args}) {{\n\
{constructorAssignments}\
{indentation}}}\n\
}}\n\
"


CPP_TO_JAVA_TEMPLATE = "\
inline jobject cpp_to_java(JNIEnv *env, const {cppName}& cppObj) {{\n\
{indentation}jmethodID constructor;\n\
{indentation}jclass cls = env->FindClass(\"com/example/juho/pikakahvi/data/{cppName}\");\n\
{indentation}constructor = env->GetMethodID( cls, \"<init>\", \"({javaParam})V\");\n\
{createObj}\
{indentation}jobject obj = env->NewObject(cls, constructor, {constructorArgs});\n\
{indentation}env->DeleteLocalRef(cls);\n\
{deleteObj}\
{indentation}return obj;\n\
}}\n\
template <>\n\
inline {cppName} java_to_cpp<{cppName}>(JNIEnv *env, jobject jobj) {{\n\
{indentation}jclass cls = env->FindClass(\"com/example/juho/pikakahvi/data/{cppName}\");\n\
{getFields}\
{indentation}env->DeleteLocalRef(cls);\n\
{indentation}return {cppName}({cppConstructorArgs});\n\
}}\n\
"


cppToJavaType = {
    "bool": "boolean",
    "string": "String",
    "int": "int"
}

cppToJniType = {
    "string": "jstring",
}

cppToJniConstructorType = {
    "int": "I",
    "string": "Ljava/lang/String;",
    "bool": "Z"
}


class Field:
    def __init__(self, type, cppName, columnName, index):
        self.type = type
        self.cppName = cppName
        self.columnName = columnName
        self.index = index


class Template:
    def __init__(self, str):
        lines = [l for l in str.split("\n") if len(l) > 0]
        self.className, self.tableName = lines[0].split()
        self.fields = [Field(*l.split(), idx) for idx, l in enumerate(lines[1:])]


def open_template(filename):
    with open(filename) as f:
        return Template(f.read())


def generate_cpp_struct(str):
    template = Template(str)
    className = template.className
    tableName = template.tableName
    fields = template.fields

    argsStr = ", " \
        .join("{type} {name}".format(type=f.type, name=f.cppName) for f in fields)

    constructorAssignments = ""
    for field in fields:
        constructorAssignments += 2 * INDENTATION \
            + "this->{name} = {name};\n".format(
                type=field.type,
                name=field.cppName
                )

    constructorAssignments2 = ""
    for field in fields:
        constructorAssignments2 += 2 * INDENTATION \
            + "this->{name} = default_conversion<{type}>(mapped[\"{columnName}\"]);\n".format(
                type=field.type,
                name=field.cppName,
                columnName=field.columnName
                )

    fieldsStr = ""
    for field in fields:
        fieldsStr += INDENTATION + "{type} {name};\n".format(
            type=field.type,
            name=field.cppName
        )

    fieldsAndValues = " + \"\\n\"\n".join(
        3 * INDENTATION + "+ \"" + INDENTATION + "{name} = \" + ::toString({name})".format(name=f.cppName) for f in fields
    ) + ";"

    print(STRUCT_TEMPLATE.format(
        indentation=INDENTATION,
        cppName=className,
        args=argsStr,
        constructorAssignments=constructorAssignments,
        constructorAssignments2=constructorAssignments2,
        fieldDeclarations=fieldsStr,
        headerGuard=className.upper(),
        fieldsAndValues=fieldsAndValues
    ))


def generate_cpp_to_java(str):
    template = Template(str)
    className = template.className
    tableName = template.tableName
    fields = template.fields

    paramStr = "" \
        .join(cppToJniConstructorType[f.type] for f in fields)

    argsStr = ", " \
        .join(f.cppName if f.type in cppToJniType else "cppObj." + f.cppName for f in fields)

    createObjects = ""
    for field in fields:
        if field.type in cppToJniType:
            createObjects += INDENTATION \
                + "auto {name} = to_jni_object(env, cppObj.{name});\n".format(
                    name=field.cppName
                    )
    deleteObjects = ""
    for field in fields:
        if field.type in cppToJniType:
            deleteObjects += INDENTATION \
                + "env->DeleteLocalRef({name});\n".format(
                    name=field.cppName
                    )
    constructorAssignments2 = ""
    for field in fields:
        constructorAssignments2 += 2 * INDENTATION \
            + "this->{name} = default_conversion<{type}>(mapped[\"{columnName}\"]);\n".format(
                type=field.type,
                name=field.cppName,
                columnName=field.columnName
                )

    fieldsStr = ""
    for field in fields:
        fieldsStr += INDENTATION + "{type} {name};\n".format(
            type=field.type,
            name=field.cppName
        )

    getFields = ""
    for field in fields:
        getFields += INDENTATION \
            + "auto {name} = get_and_convert_java_field<{type}>(env, cls, jobj, \"{name}\");\n".format(
                type=field.type,
                name=field.cppName
                )

    cppConstructorArgs = ", ".join(f.cppName for f in fields)

    print(CPP_TO_JAVA_TEMPLATE.format(
        indentation=INDENTATION,
        cppName=className,
        javaParam=paramStr,
        createObj=createObjects,
        deleteObj=deleteObjects,
        constructorArgs=argsStr,
        getFields=getFields,
        cppConstructorArgs=cppConstructorArgs
    ))


def generate_java_class(str):
    template = Template(str)
    className = template.className
    tableName = template.tableName

    fields = template.fields
    for field in fields:
        field.type = cppToJavaType[field.type]

    argsStr = ", " \
        .join("{type} {name}".format(type=f.type, name=f.cppName) for f in fields)

    constructorAssignments = ""
    for field in fields:
        constructorAssignments += 2 * INDENTATION \
            + "this.{name} = {name};\n".format(
                type=field.type,
                name=field.cppName
                )

    constructorAssignments2 = ""
    for field in fields:
        constructorAssignments2 += 2 * INDENTATION \
            + "this->{name} = default_conversion<{type}>(mapped[\"{columnName}\"]);\n".format(
                type=field.type,
                name=field.cppName,
                columnName=field.columnName
                )

    fieldsStr = ""
    for field in fields:
        fieldsStr += INDENTATION + "public {type} {name};\n".format(
            type=field.type,
            name=field.cppName
        )

    print(JAVA_TEMPLATE.format(
        indentation=INDENTATION,
        cppName=className,
        args=argsStr,
        constructorAssignments=constructorAssignments,
        constructorAssignments2=constructorAssignments2,
        fieldDeclarations=fieldsStr,
        headerGuard=className.upper()
    ))


if __name__ == "__main__":
    with open(sys.argv[2]) as f:
        if sys.argv[1] == "cpp":
            generate_cpp_struct(f.read())
        elif sys.argv[1] == "java":
            generate_java_class(f.read())
        elif sys.argv[1] == "cpp-to-java":
            generate_cpp_to_java(f.read())
        else:
            raise RuntimeError("no type")
