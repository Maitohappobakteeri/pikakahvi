#include "repository.hpp"


Repository::Repository(bool isDebug, const Db::Database::DatabaseFilename& dbFilename)
{
    database_ = Db::Database::get_database(isDebug, dbFilename);
    database_->require_database();
}


Repository::~Repository()
{
    database_->release_database();
}
