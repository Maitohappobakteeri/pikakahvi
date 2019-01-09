#ifndef TASKREPOSITORY_HPP
#define TASKREPOSITORY_HPP


#include "db/db.hpp"
#include "repository.hpp"
#include "generated/data-templates/task.h"
#include "generated/data-templates/list.h"

#include <vector>


class TaskRepository: public Repository
{
public:
    TaskRepository(bool isDebug, const Db::Database::DatabaseFilename& dbFilename);
    ~TaskRepository();

    std::vector<Task> allTasks();
    Task getTask(int taskId);

    // TODO: Just update the whole task?
    Task createTask(const std::string title, int listId);
    void markDone(int taskId);
    void undoTask(int taskId);
    void setPriority(int taskId, int priority);

    std::vector<TaskList> allLists();
    TaskList getList(int listId);
    TaskList createList(const std::string name);
    void markListDeprecated(int listId);
};


#endif
