#include "db.hpp"


#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <stdexcept>
#include "upgrade.hpp"

using Db::Database;


namespace
{
    std::string readFile(const std::string& filename)
    {
    	std::ifstream t(filename);
    	std::string str((std::istreambuf_iterator<char>(t)),
    	                 std::istreambuf_iterator<char>());

    	if (t.fail())
    	{
    		throw std::runtime_error(std::string("failed to read file: ") + filename);
    	}

        return str;
    }
}



//
// Database


std::map<Database::DatabaseFilename, std::unique_ptr<Database>> Database::databases_;


Database* Database::get_database(bool isDebug, const DatabaseFilename& filename)
{
    if(databases_.find(filename) == databases_.end())
    {
        databases_[filename] = std::unique_ptr<Database>(new Database(isDebug, filename));
    }

    return databases_[filename].get();
}


bool Database::require_database()
{
    openRequests_++;
    return open_database();
}


bool Database::release_database()
{
    openRequests_--;
    // TODO: Assert req >= 0

    if (openRequests_ == 0)
    {
        close_database();
        return true;
    }

    return false;
}


void Database::close_database_force()
{
    // TODO: Assert
    close_database();
}


void Database::open_database_force()
{
    // TODO: Assert
    open_database();
}


sqlite3* Database::database()
{
    return database_;
}


std::string Database::filename() const
{
    return filename_;
}


Database::Database(bool isDebug, const std::string& filename)
    :isDebug_(isDebug), filename_(filename), database_(nullptr)
{

}


Database::~Database()
{

}


bool Database::open_database()
{
    if (database_ != nullptr)
    {
        return true;
    }

    int rc = sqlite3_open(filename_.c_str(), &database_);
    bool result = rc == SQLITE_OK && upgradeToLatestVersionFromGenerated(database_, isDebug_);

    if (!result)
    {
        if (database_ != nullptr)
        {
            sqlite3_close(database_);
            database_ = nullptr;
        }
    }

    return result;
}


bool Database::close_database()
{
    if (database_ != nullptr)
    {
        sqlite3_close(database_);
        database_ = nullptr;
        return true;
    }

    return false;
}


bool Db::runString(sqlite3 *db, const std::string& script)
{
    return Db::runString(db, script, nullptr, nullptr);
}


bool Db::runString(sqlite3 *db, const std::string& script,
               void *object, int (*callback)(void*,int,char**,char**))
{
    char *zErrMsg = 0;
    int rc = sqlite3_exec(db, script.c_str(), callback, object, &zErrMsg);
    if (rc!=SQLITE_OK)
    {
        std::cerr <<"SQL error: " << sqlite3_errmsg(db) << std::endl;
        sqlite3_free(zErrMsg);
        return false;
    }

    return true;
}


bool Db::runScript(sqlite3 *db, const std::string& filename)
{
    std::string script;
    try
    {
        script = readFile(filename);
    }
    catch (const std::exception& e)
    {
        std::cerr << e.what() << std::endl;
        return false;
    }

    return Db::runString(db, script);
}


bool Db::runScript(sqlite3 *db, const std::string& filename,
               void *object, int (*callback)(void*,int,char**,char**))
{
    std::string script;
    try
    {
        script = readFile(filename);
    }
    catch (const std::exception& e)
    {
        std::cerr << e.what() << std::endl;
        return false;
    }

    return Db::runString(db, script, object, callback);
}


std::map<std::string, std::string> Db::mapExecCallbackArgs(int argc, char** argv, char** colNames)
{
    std::map<std::string, std::string> mapped;

    for (int i=0; i<argc; ++i)
    {
        // TODO: Nullable values
        if (argv[i] != nullptr)
        {
            mapped[colNames[i]] = argv[i];
        }
    }

    return mapped;
};
