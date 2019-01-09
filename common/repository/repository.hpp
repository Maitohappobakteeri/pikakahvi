#ifndef REPOSITORY_REPOSITORY_HPP
#define REPOSITORY_REPOSITORY_HPP


#include "db/db.hpp"


class Repository
{
public:
    Repository(bool isDebug, const Db::Database::DatabaseFilename& dbFilename);
    ~Repository();

protected:
    Db::Database* database_;
};


#endif // REPOSITORY_REPOSITORY_HPP
