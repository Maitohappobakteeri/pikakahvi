#!/usr/bin/env bash

DIR="$( cd "$( dirname "${BASH_SOURCE[0]}" )" >/dev/null && pwd )"
cd "$DIR"

rm -Rf generated/data
mkdir -p generated/data

JAVA_DATA_DIR="../android/app/src/main/java/com/example/juho/pikakahvi/data"
CPP_JAVA_DIR="../android/app/src/main/cpp"

rm $DIR/$JAVA_DATA_DIR/*.java
rm "$CPP_JAVA_DIR/generated.hpp"

# Generate headers
for FILE in $(find data-templates -name *.template); do
    cd "$DIR"
    CLASSNAME="$(python -c "import generate_data_from_template as x;print(x.open_template(\"$FILE\").className);")"
    HEADERFILE="generated/$(echo $FILE | sed -r 's/(.*)\.template/\1.h/g')"
    JAVAFILE="generated/$(echo $FILE | sed -r 's/(.*)\/.*\.template/\1\/'$CLASSNAME'.java/g')"
    mkdir -p "$(dirname $HEADERFILE)"
    python3 generate_data_from_template.py cpp "$FILE" > "$DIR/$HEADERFILE"
    python3 generate_data_from_template.py java "$FILE" > "$DIR/$JAVAFILE"

    JAVAFILE_BASE="$(basename $JAVAFILE)"
    JAVAFILE_BASE="${JAVAFILE_BASE^}"
    cp "$DIR/$JAVAFILE" "$DIR/$JAVA_DATA_DIR/$JAVAFILE_BASE"
    echo "Copying $DIR/$HEADERFILE to $(basename $FILE) in $DIR/$(dirname $FILE)"
done
cd "$DIR"

echo "
#ifndef CPP_TO_JAVA_GENERATED_HPP
#define CPP_TO_JAVA_GENERATED_HPP

#include <jni.h>
#include <string>
#include <vector>

using std::string;

jstring to_jni_object(JNIEnv *env, const std::string& str) {
    return env->NewStringUTF(str.c_str());
}

template <class T>
T get_and_convert_java_field(JNIEnv *env, jclass cls, jobject obj, const char* fieldName);

template <>
int get_and_convert_java_field<int>(JNIEnv *env, jclass cls, jobject obj, const char* fieldName) {
    jfieldID fId = env->GetFieldID(cls, fieldName, \"I\");
    return env->GetIntField(obj, fId);
}

template <>
bool get_and_convert_java_field<bool>(JNIEnv *env, jclass cls, jobject obj, const char* fieldName) {
    jfieldID fId = env->GetFieldID(cls, fieldName, \"Z\");
    return env->GetBooleanField(obj, fId);
}

template <>
string get_and_convert_java_field<string>(JNIEnv *env, jclass cls, jobject obj, const char* fieldName) {
    jfieldID fId = env->GetFieldID(cls, fieldName, \"Ljava/lang/String;\");
    return env->GetStringUTFChars((jstring) env->GetObjectField(obj, fId), 0);
}

template <class T>
T java_to_cpp(JNIEnv *env, jobject jobj);

" > "$CPP_JAVA_DIR/generated.hpp"

for FILE in $(find data-templates -name *.template); do
    cd "$DIR"
    HEADERFILE="generated/$(echo $FILE | sed -r 's/(.*)\.template/\1.h/g')"
    echo "#include \"$HEADERFILE\"" >> "$CPP_JAVA_DIR/generated.hpp"
    python3 generate_data_from_template.py cpp-to-java "$FILE" >> "$CPP_JAVA_DIR/generated.hpp"
done
cd "$DIR"

echo "
#endif // CPP_TO_JAVA_GENERATED_HPP
" >> "$CPP_JAVA_DIR/generated.hpp"

rm generated/data-templates/*.java
