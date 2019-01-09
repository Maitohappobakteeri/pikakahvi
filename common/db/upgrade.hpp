#ifndef UPGRADE_HPP
#define UPGRADE_HPP


#include <sqlite3.h>

#include <string>


namespace Db {
    bool upgradeToLatestVersion(sqlite3 *db, const std::string& upgradeDir, bool debug);
    bool upgradeToLatestVersionFromGenerated(sqlite3 *db, bool debug);
    bool printUpgrades(const std::string& upgradeDir);
}


#endif //UPGRADE_HPP
