#include "upgrade.hpp"


#include "db.hpp"
#include "generated/generate_sql_header.hpp"

#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <map>
#include <set>
#include <regex>
#include <stdio.h>
#include <dirent.h>


using namespace Db;


namespace
{
    static int callback(void *NotUsed, int argc, char **argv, char **azColName)
    {
    	int i;
    	for(i=0; i<argc; i++)
    	{
    		std::cout << azColName[i]<<" = " << (argv[i] ? argv[i] : "NULL")<<"\n";
    	}
    	std::cout<<"\n";
    	return 0;
    }

    static int callbackForOne(void *voidString, int argc, char **argv, char **azColName)
    {
        if (argc != 1)
        {
            throw std::runtime_error("expected argc == 1 for callback from sql");
        }

        std::string *string = static_cast<std::string*>(voidString);
        *string = argv[0];
        return 0;
    }

    std::vector<std::string> listDir(const std::string& dirname)
    {
        struct dirent *de;
        DIR *dr = opendir(dirname.c_str());

        if (dr == NULL)
        {
            throw std::runtime_error(std::string("could not read directory: ") + dirname);
        }

        std::vector<std::string> items;
        while ((de = readdir(dr)) != NULL)
        {
            items.push_back(de->d_name);
        }
        closedir(dr);

        return items;
    }

    std::map<int, std::string> listUpgrades(const std::string& upgradeDir)
    {
        std::map<int, std::string> upgrades;
        std::vector<std::string> filenames = listDir(upgradeDir);
        for (std::string filename : filenames)
        {
            std::regex upgradeRegex("v([0-9]+)-(.*)\\.sql");
            std::smatch results;
            if (std::regex_match(filename, results, upgradeRegex))
            {
                int version = std::stoi(results[1]);
                if (upgrades.find(version) != upgrades.end())
                {
                    throw std::runtime_error(std::string("duplicate version: ") + filename);
                }

                upgrades[version] = filename;
            }
        }

        int nextVersion = 1;
        for (const auto& upgrade : upgrades)
        {
            if (nextVersion != upgrade.first)
            {
                throw std::runtime_error(std::string("missing version: ") + std::to_string(nextVersion));
            }

            ++nextVersion;
        }

        return upgrades;
    }

    std::map<int, std::map<int, std::string>> listDebugUpgrades(const std::string& upgradeDir,
        const std::map<int, std::string>& upgrades)
    {
        std::map<int, std::map<int, std::string>> debugUpgrades;
        std::vector<std::string> filenames = listDir(upgradeDir + "/debug");
        for (std::string filename : filenames)
        {
            std::regex upgradeRegex("v([0-9]+)-d([0-9]+)-(.*)\\.sql");
            std::smatch results;
            if (std::regex_match(filename, results, upgradeRegex))
            {
                int version = std::stoi(results[1]);
                int debugVersion = std::stoi(results[2]);
                if (debugUpgrades.find(version) != debugUpgrades.end()
                    && debugUpgrades[version].find(debugVersion) != debugUpgrades[version].end())
                {
                    throw std::runtime_error(std::string("duplicate version: ") + filename);
                }

                if (upgrades.find(version) == upgrades.end())
                {
                    throw std::runtime_error(std::string("missing baseupgrade: ") + filename);
                }

                debugUpgrades[version][debugVersion] = filename;
            }
        }

        for (const auto& baseUpgrade : upgrades)
        {
            int nextVersion = 1;
            for (const auto& upgrade : upgrades)
            {
                if (nextVersion != upgrade.first)
                {
                    throw std::runtime_error(std::string("missing version: ") + std::to_string(nextVersion));
                }

                ++nextVersion;
            }
        }

        return debugUpgrades;
    }

    void updateVersion(sqlite3 *db, int version)
    {
        std::string updateVersion = std::string("update version set version = ")
            + std::to_string(version) + ";";
        char *zErrMsg = 0;

        int rc = sqlite3_exec(db, updateVersion.c_str(), nullptr, nullptr, &zErrMsg);
        if( rc!=SQLITE_OK )
        {
            std::cerr <<"SQL error: " << sqlite3_errmsg(db) << std::endl;
            sqlite3_free(zErrMsg);
            throw std::runtime_error("failed to update db version");
        }
    }
}


bool Db::upgradeToLatestVersion(sqlite3 *db, const std::string& upgradeDir, bool debug)
{
    const char* selectVersion = "select (version) from version;";
    char *zErrMsg = 0;

    std::string versionString;
    int rc = sqlite3_exec(db, selectVersion, callbackForOne, &versionString, &zErrMsg);
    if( rc!=SQLITE_OK )
    {
        std::cerr <<"SQL error: " << sqlite3_errmsg(db) << std::endl;
        sqlite3_free(zErrMsg);
    }

    int currentVersion = 0;

    if (!versionString.empty())
    {
        currentVersion = std::stoi(versionString);
    }

    auto upgrades = listUpgrades(upgradeDir);
    auto debugUpgrades = listDebugUpgrades(upgradeDir, upgrades);

    for (auto& upgradePair : upgrades)
    {
        int version = upgradePair.first;
        if (currentVersion >= version)
        {
            std::cout << "Skipping upgrade: " << upgradePair.second << std::endl;
            continue;
        }

        std::cout << "Running upgrade: " << upgradePair.second << std::endl;
        if (!runScript(db, upgradeDir + "/" + upgradePair.second))
        {
            return false;
        }

        updateVersion(db, version);

        if (debug && debugUpgrades.find(version) != debugUpgrades.end())
        {
            for (auto& debugUpgradePair : debugUpgrades[version])
            {
                int debugVersion = debugUpgradePair.first;

                std::cout << "Running debug upgrade: " << debugUpgradePair.second << std::endl;
                if (!runScript(db, upgradeDir + "/debug/" + debugUpgradePair.second))
                {
                    return false;
                }
            }
        }
    }

    return true;
}


bool Db::printUpgrades(const std::string& upgradeDir)
{
    auto upgrades = listUpgrades(upgradeDir);
    auto debugUpgrades = listDebugUpgrades(upgradeDir, upgrades);

    for (auto& upgradePair : upgrades)
    {
        int version = upgradePair.first;
        std::cout << version << " upgrade " << upgradePair.second << std::endl;

        if (debugUpgrades.find(version) != debugUpgrades.end())
        {
            for (auto& debugUpgradePair : debugUpgrades[version])
            {
                int debugVersion = debugUpgradePair.first;
                std::cout << debugVersion << " debug " << debugUpgradePair.second << std::endl;
            }
        }
    }

    return true;
}


bool Db::upgradeToLatestVersionFromGenerated(sqlite3 *db, bool debug)
{
    const char* selectVersion = "select (version) from version;";
    char *zErrMsg = 0;

    std::string versionString;
    int rc = sqlite3_exec(db, selectVersion, callbackForOne, &versionString, &zErrMsg);
    if( rc!=SQLITE_OK )
    {
        std::cerr <<"SQL error: " << sqlite3_errmsg(db) << std::endl;
        sqlite3_free(zErrMsg);
    }

    int currentVersion = 0;
    int upgradeVersion = 0;
    bool skipDebugUpgrades = false;

    if (!versionString.empty())
    {
        currentVersion = std::stoi(versionString);
    }

    for (SqlUpgrade upgrade : sqlUpgrades)
    {
        if (upgrade.type == "upgrade")
        {
            upgradeVersion = upgrade.version;

            if (currentVersion >= upgradeVersion)
            {
                std::cout << "Skipping upgrade: " << upgrade.version << std::endl;
                skipDebugUpgrades = true;
                continue;
            }

            skipDebugUpgrades = false;

            std::cout << "Running upgrade: " << upgrade.version << std::endl;
            if (!runString(db, upgrade.script))
            {
                return false;
            }

            updateVersion(db, upgradeVersion);
        }
        else if (upgrade.type == "debug")
        {
            if (!debug || skipDebugUpgrades)
            {
                std::cout << "Skipping debug upgrade: " << upgrade.version << std::endl;
                continue;
            }

            std::cout << "Running debug upgrade: " << upgrade.version << std::endl;
            if (!runString(db, upgrade.script))
            {
                return false;
            }
        }
    }

    return true;
}
