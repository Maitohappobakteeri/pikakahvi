
#include "repository/taskrepository.hpp"

#include <cstdio>

#ifndef SKIP_BOOSTTEST
#include <boost/test/unit_test.hpp>
#endif

struct repository_taskrepository_fixture {
  repository_taskrepository_fixture()
  {
      std::remove("unit-test.sqlite");
      repo = new TaskRepository(false, "unit-test.sqlite");
  }

  ~repository_taskrepository_fixture()
  {
      delete repo;
  }

  TaskRepository* repo;
};


BOOST_FIXTURE_TEST_SUITE(repository_taskrepository, repository_taskrepository_fixture)

BOOST_AUTO_TEST_CASE( new_repo )
{
    BOOST_CHECK(repo->allTasks().empty());
    BOOST_CHECK(repo->allLists().size() == 1);
}

BOOST_AUTO_TEST_CASE( add_list )
{
    std::string name = "  Sum List Sum List";
    TaskList list = repo->createList(name);
    BOOST_CHECK(repo->allLists().size() == 2);
    BOOST_CHECK(list.name == name);
}

BOOST_AUTO_TEST_CASE( get_lists )
{
    BOOST_CHECK(repo->allLists().size() == 1);
    TaskList list = repo->allLists()[0];
    BOOST_CHECK(list.name == "generic");
}

BOOST_AUTO_TEST_CASE( add_task )
{
    std::string title = "xxx123!!!!'\"'!!!'''";
    int listId = repo->allLists()[0].id;

    repo->createTask(title, listId);
    std::vector<Task> tasks = repo->allTasks();
    BOOST_CHECK(tasks.size() == 1);
    BOOST_CHECK(tasks[0].title == title);
    BOOST_CHECK(tasks[0].listId == listId);
    BOOST_CHECK(tasks[0].timeDone == 0);
    BOOST_CHECK(tasks[0].priority == 1);
}

BOOST_AUTO_TEST_CASE( mark_done )
{
    Task task = repo->createTask("777", repo->allLists()[0].id);
    repo->markDone(task.id);
    BOOST_CHECK(repo->getTask(task.id).done);
    BOOST_CHECK(repo->getTask(task.id).timeDone > 0);
}


BOOST_AUTO_TEST_CASE( undo_done )
{
    Task task = repo->createTask("777", repo->allLists()[0].id);

    repo->markDone(task.id);
    BOOST_CHECK(repo->getTask(task.id).done);

    repo->undoTask(task.id);
    BOOST_CHECK(!repo->getTask(task.id).done);
}


BOOST_AUTO_TEST_CASE( set_task_priority )
{
    Task task = repo->createTask("123", repo->allLists()[0].id);
    repo->setPriority(task.id, 3);
    BOOST_CHECK(repo->getTask(task.id).priority == 3);
}


BOOST_AUTO_TEST_CASE( mark_list_deprecated )
{
    TaskList list = repo->createList("This is a list");
    repo->markListDeprecated(list.id);
    BOOST_CHECK(repo->getList(list.id).deprecated);
}


BOOST_AUTO_TEST_SUITE_END()
