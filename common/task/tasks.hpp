#ifndef TASK_TASKS_HPP
#define TASK_TASKS_HPP


#include "repository/taskrepository.hpp"

#include <memory>


class ITasks
{
    virtual ~ITasks() { };

    virtual std::vector<Task> allTasks() = 0;
    virtual Task getTask(int taskId) = 0;

    virtual Task createTask(const std::string title, int listId) = 0;
    virtual void markDone(int taskId) = 0;
    virtual void undoTask(int taskId) = 0;
    virtual void setPriority(int taskId, int priority) = 0;

    virtual std::vector<TaskList> getLists() = 0;
    virtual TaskList getList(int listId) = 0;
    virtual TaskList createList(const std::string name) = 0;
    virtual void markListDeprecated(int listId) = 0;
};


class Tasks
{
public:
    Tasks();
    ~Tasks();

    std::vector<Task> allTasks();
    Task getTask(int taskId);

    Task createTask(const std::string title, int listId);
    void markDone(int taskId);
    void undoTask(int taskId);

    // Currently priority is in range [0,4]
    //    Return false if priority is too small or large
    bool setPriority(int taskId, int priority);

    std::vector<TaskList> getLists();
    TaskList getList(int listId);
    TaskList createList(const std::string name);
    void markListDeprecated(int listId);

private:
    std::unique_ptr<TaskRepository> repository_;
};


#endif // TASK_TASKS_HPP
