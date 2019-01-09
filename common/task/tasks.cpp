#include "tasks.hpp"


#include "repository/repositoryconfig.hpp"


Tasks::Tasks()
{
    repository_ = std::make_unique<TaskRepository>(
        RepoConfig::useDebugUpgrades,
        RepoConfig::commonDatabaseFilename
    );
}


Tasks::~Tasks()
{

}


std::vector<Task> Tasks::allTasks()
{
    return repository_->allTasks();
}


Task Tasks::createTask(const std::string title, int listId)
{
    return repository_->createTask(title, listId);
}


void Tasks::markDone(int taskId)
{
    return repository_->markDone(taskId);
}


void Tasks::undoTask(int taskId)
{
    return repository_->undoTask(taskId);
}


bool Tasks::setPriority(int taskId, int priority)
{
    if (priority < 0 || priority > 4)
    {
        return false;
    }

    repository_->setPriority(taskId, priority);
    return true;
}


TaskList Tasks::getList(int listId)
{
    return repository_->getList(listId);
}


std::vector<TaskList> Tasks::getLists()
{
    return repository_->allLists();
}


TaskList Tasks::createList(const std::string name)
{
    return repository_->createList(name);
}


void Tasks::markListDeprecated(int listId)
{
    return repository_->markListDeprecated(listId);
}
