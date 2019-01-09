#include "taskrepository.hpp"


#include <chrono>


// TODO: Updating this SQL when renaming things feels silly! Refactor names to const variables
// TODO: Read SQL-strings from somewhere else, just insert the names^ here!


// TODO: Move this to somewhere else
namespace {
    int unix_time()
    {
        auto now = std::chrono::system_clock::now();
        auto epoch = now.time_since_epoch();
        auto seconds = std::chrono::duration_cast<std::chrono::seconds>(epoch);
        return seconds.count();
    }
}


TaskRepository::TaskRepository(bool isDebug, const Db::Database::DatabaseFilename& dbFilename)
    :Repository(isDebug, dbFilename)
{

}


TaskRepository::~TaskRepository()
{

}


std::vector<Task> TaskRepository::allTasks()
{
    const char* query = "select task.* from task inner join list on task.list = list.id where list.deprecated = 0;";
    return database_->runStringAndCompileResults<Task>(query);
}


Task TaskRepository::getTask(int taskId)
{
    std::string query = std::string()
        + "select task.* from task where id = "
        + std::to_string(taskId) + ";";
    return database_->runStringAndCompileResults<Task>(query)[0];
}


Task TaskRepository::createTask(const std::string title, int listId)
{
    std::vector<Task> tasks = database_->runStringAndCompileResults<Task>(
        std::string("insert into task (title, list) values (")
        + toSqlString(title)
        + ", " + std::to_string(listId) + ""
        + "); select * from task order by id desc limit 1;");

    return tasks.front();
}


void TaskRepository::markDone(int taskId)
{
    Db::runString(database_->database(),
        std::string("update task set done = 1 where id = ")
        + toSqlString(taskId)
        + ";");
    Db::runString(database_->database(),
        std::string("update task set time_done = ")
        + toSqlString(unix_time())
        + " where id = "
        + toSqlString(taskId)
        + ";");
}


void TaskRepository::undoTask(int taskId)
{
    Db::runString(database_->database(),
        std::string("update task set done = 0 where id = ")
        + toSqlString(taskId)
        + ";");
}


void TaskRepository::setPriority(int taskId, int priority)
{
    Db::runString(database_->database(),
        std::string("update task set priority = ")
        + toSqlString(priority)
        + " where id = "
        + toSqlString(taskId)
        + ";");
}


std::vector<TaskList> TaskRepository::allLists()
{
    const char* query = "select * from list where deprecated = 0;";
    return database_->runStringAndCompileResults<TaskList>(query);
}


TaskList TaskRepository::getList(int listId)
{
    std::string query = std::string()
        + "select * from list where id = "
        + std::to_string(listId) + ";";
    return database_->runStringAndCompileResults<TaskList>(query)[0];
}


TaskList TaskRepository::createList(const std::string name)
{
    std::string query = std::string("insert into list (name) values (")
        + toSqlString(name)
        + "); select * from list order by id desc limit 1;";
    return database_->runStringAndCompileResults<TaskList>(query).front();
}


void TaskRepository::markListDeprecated(int listId)
{
    std::string query = std::string("update list set deprecated = 1 where id = ")
        + toSqlString(listId)
        + ";";
    Db::runString(database_->database(), query);
}
