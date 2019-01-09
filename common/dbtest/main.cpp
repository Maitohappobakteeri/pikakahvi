#include <sqlite3.h>


#include "db/db.hpp"
#include "db/upgrade.hpp"
#include "repository/repositoryconfig.hpp"
#include "task/tasks.hpp"

#include <iostream>
#include <fstream>
#include <string>
#include <vector>


using namespace std;
using namespace Db;


int main(int argc, char** argv)
{
	if (argc > 1)
	{
		printUpgrades("sql/upgrade");
		return 0;
	}

	sqlite3 *db;
	char *zErrMsg = 0;
	int rc;

	rc = sqlite3_open("dbtest.db", &db);

	if( rc )
	{
		cout<<"Can't open database: "<<sqlite3_errmsg(db)<<"\n";
	}
	else
	{
		cout<<"Open database successfully\n\n";
	}

	upgradeToLatestVersion(db, "sql/upgrade", true);

	RepoConfig::commonDatabaseFilename = "dbtest.db";
	Tasks* tasks = new Tasks();

	for (Task task: tasks->allTasks())
	{
		tasks->createTask("'hai'hai", 1);
	}

	for (Task task: tasks->allTasks())
	{
		std::cout << task.toString() << std::endl;
	}

	sqlite3_close(db);

	return 0;
}
