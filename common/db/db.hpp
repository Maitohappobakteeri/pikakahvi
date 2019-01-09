#ifndef DB_HPP
#define DB_HPP


#include <sqlite3.h>

#include <string>
#include <map>
#include <vector>
#include <memory>


namespace Db {
    template <class T>
    static int sqlExecEmplaceCallback(void *ptr, int argc, char **argv, char **azColName)
    {
        std::vector<T>* vec = (std::vector<T>*) ptr;
        vec->emplace_back(argc, argv, azColName);
        return 0;
    }


    bool runString(sqlite3 *db, const std::string& script);
    bool runString(sqlite3 *db, const std::string& script,
                   void *object, int (*callback)(void*,int,char**,char**));

    bool runScript(sqlite3 *db, const std::string& filename);
    bool runScript(sqlite3 *db, const std::string& filename,
                   void *object, int (*callback)(void*,int,char**,char**));

    std::map<std::string, std::string> mapExecCallbackArgs(int argc, char** argv, char** colNames);


    template <class T>
    std::vector<T> runScriptAndCompileResults(sqlite3 *db, const std::string& filename)
    {
        std::vector<T> results;
        runScript(db, filename, &results, sqlExecEmplaceCallback<T>);
        return results;
    }


    template <class T>
    std::vector<T> runStringAndCompileResults(sqlite3 *db, const std::string& script)
    {
        std::vector<T> results;
        runString(db, script, &results, sqlExecEmplaceCallback<T>);
        return results;
    }


    class Database
    {
        friend std::unique_ptr<Database>::deleter_type;

    public:
        using DatabaseFilename = std::string;

        template <class T>
        std::vector<T> runScriptAndCompileResults(const std::string& filename)
        {
            std::vector<T> results;
            runScript(database_, filename, &results, sqlExecEmplaceCallback<T>);
            return results;
        }


        template <class T>
        std::vector<T> runStringAndCompileResults(const std::string& script)
        {
            std::vector<T> results;
            runString(database_, script, &results, sqlExecEmplaceCallback<T>);
            return results;
        }

        static Database* get_database(bool isDebug, const DatabaseFilename& filename);
        bool require_database();
        bool release_database();
        void close_database_force();
        void open_database_force();

        sqlite3* database();
        std::string filename() const;

    private:
        Database(bool isDebug, const DatabaseFilename& filename);
        ~Database();

        bool open_database();
        bool close_database();

        static std::map<DatabaseFilename, std::unique_ptr<Database>> databases_;

        bool isDebug_;
        std::string filename_;
        sqlite3* database_;
        int openRequests_ = 0;
    };
}


#endif //DB_HPP
