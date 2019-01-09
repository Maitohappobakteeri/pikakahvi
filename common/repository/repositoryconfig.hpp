#ifndef REPOSITORY_REPOSITORYCONFIG_HPP
#define REPOSITORY_REPOSITORYCONFIG_HPP


#include "db/db.hpp"

#include <string>


namespace RepoConfig
{
    extern bool useDebugUpgrades;
    extern Db::Database::DatabaseFilename commonDatabaseFilename;
}


#endif // REPOSITORY_REPOSITORY_HPP
