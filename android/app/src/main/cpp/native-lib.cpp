#include "common.hpp"
#include "db/db.hpp"
#include "db/upgrade.hpp"
#include "repository/taskrepository.hpp"
#include "generated/data-templates/task.h"
#include "task/tasks.hpp"
#include "generated.hpp"

#include <jni.h>
#include <string>
#include <vector>
#include <repository/repositoryconfig.hpp>


Tasks* tasks;
unsigned int taskRefCount = 0;


template <class T>
jobject to_java_arraylist(JNIEnv* env, const std::vector<T>& cppObjects)
{
    jclass cls_arraylist = env->FindClass("java/util/ArrayList");
    jmethodID init_arraylist = env->GetMethodID(cls_arraylist, "<init>", "()V");
    jobject obj_arraylist = env->NewObject(cls_arraylist, init_arraylist, "");
    jmethodID arraylist_add = env->GetMethodID(cls_arraylist, "add", "(Ljava/lang/Object;)Z");

    for (const T& cppObj: cppObjects)
    {
        jobject obj = cpp_to_java(env, cppObj);
        env->CallBooleanMethod(obj_arraylist, arraylist_add, obj);
        env->DeleteLocalRef(obj);
    }

    env->DeleteLocalRef(cls_arraylist);

    return obj_arraylist;
}


extern "C" JNIEXPORT jboolean

JNICALL
Java_com_example_juho_pikakahvi_MainActivity_initDB(
        JNIEnv *env,
        jobject,
        jstring dbname,
        jboolean debug)
{
    RepoConfig::commonDatabaseFilename = env->GetStringUTFChars(dbname, 0);
    RepoConfig::useDebugUpgrades = (bool) debug;

    if (taskRefCount == 0)
    {
        assert(tasks == nullptr);
        tasks = new Tasks();
    }

    taskRefCount++;

    return true;
}


extern "C" JNIEXPORT jboolean

JNICALL
Java_com_example_juho_pikakahvi_MainActivity_closeDB(
        JNIEnv *env,
        jobject /* this */)
{
    assert(taskRefCount > 0);
    taskRefCount--;

    if (taskRefCount == 0)
    {
        delete tasks;
        tasks = nullptr;
    }

    return true;
}


extern "C" JNIEXPORT jboolean

JNICALL
Java_com_example_juho_pikakahvi_MainActivity_returnDBForce(
        JNIEnv *env,
        jobject,
        jstring dbname,
        jboolean debug)
{
    Db::Database* db = Db::Database::get_database(RepoConfig::useDebugUpgrades,
                                                  RepoConfig::commonDatabaseFilename);
    db->open_database_force();

    assert(tasks == nullptr);
    tasks = new Tasks();

    return true;
}


extern "C" JNIEXPORT jboolean

JNICALL
Java_com_example_juho_pikakahvi_MainActivity_closeDBForce(
        JNIEnv *env,
        jobject /* this */)
{
    assert(tasks != nullptr);
    delete tasks;
    tasks = nullptr;

    Db::Database* db = Db::Database::get_database(RepoConfig::useDebugUpgrades,
                                                  RepoConfig::commonDatabaseFilename);
    db->close_database_force();

    return true;
}


extern "C" JNIEXPORT jobject

JNICALL
Java_com_example_juho_pikakahvi_MainActivity_getThings(
        JNIEnv *env,
        jobject /* this */)
{
    return to_java_arraylist(env, tasks->allTasks());
}


extern "C" JNIEXPORT jobject

JNICALL
Java_com_example_juho_pikakahvi_MainActivity_createThing(
        JNIEnv *env,
        jobject /* this */,
        jstring title,
        jint listId)

{
    return cpp_to_java(env, tasks->createTask(env->GetStringUTFChars(title, 0), (int) listId));
}

extern "C" JNIEXPORT jboolean

JNICALL
Java_com_example_juho_pikakahvi_MainActivity_markDone(
        JNIEnv *env,
        jobject /* this */,
        jint id)

{
    tasks->markDone(id);
    return true;
}


extern "C" JNIEXPORT jboolean

JNICALL
Java_com_example_juho_pikakahvi_MainActivity_undoTask(
        JNIEnv *env,
        jobject /* this */,
        jint id)

{
    tasks->undoTask(id);
    return true;
}


extern "C" JNIEXPORT jboolean

JNICALL
Java_com_example_juho_pikakahvi_MainActivity_setTaskPriority(
        JNIEnv *env,
        jobject /* this */,
        jint id,
        jint priority)

{
    return tasks->setPriority(id, priority);
}


extern "C" JNIEXPORT jobject

JNICALL
Java_com_example_juho_pikakahvi_MainActivity_getList(
        JNIEnv *env,
        jobject /* this */,
        jint id)
{
    return cpp_to_java(env, tasks->getList(id));
}


extern "C" JNIEXPORT jobject

JNICALL
Java_com_example_juho_pikakahvi_MainActivity_getLists(
        JNIEnv *env,
        jobject /* this */)
{
    return to_java_arraylist(env, tasks->getLists());
}


extern "C" JNIEXPORT jboolean

JNICALL
Java_com_example_juho_pikakahvi_MainActivity_markListDeprecated(
        JNIEnv *env,
        jobject /* this */,
        jint id)
{
    tasks->markListDeprecated((int) id);
    return true;
}


extern "C" JNIEXPORT jboolean

JNICALL
Java_com_example_juho_pikakahvi_MainActivity_createNewList(
        JNIEnv *env,
        jobject /* this */,
        jstring name)
{
    tasks->createList(env->GetStringUTFChars(name, 0));
    return true;
}