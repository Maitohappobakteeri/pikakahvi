package com.example.juho.pikakahvi;

import android.content.Context;
import android.database.Cursor;
import android.database.sqlite.SQLiteDatabase;

import com.example.juho.pikakahvi.data.Task;

import java.util.ArrayList;
import java.util.List;

public class Things {
    List<Task> items = new ArrayList<>();
    List<FilteredList<Task>> flists = new ArrayList<>();

    Things(Context context) {
        initItems();
    }

    List<Task> getItems() {
        return items;
    }

    public void addFilteredList(FilteredList<Task> flist) {
        flists.add(flist);
        flist.onResetSource(items);
    }

    public void removeFilteredList(FilteredList<Task> flist) {
        flists.remove(flist);
    }

    void addThing(String thing, int listId) {
        Task task = MainActivity.createThing(thing, listId);
        items.add(task);
        onTaskUpdated(task);
    }

    public void update() {
        initItems();
    }

    void markDone(int id) {
        MainActivity.markDone(id);

        Task task = findTask(id);
        task.done = true;

        onTaskUpdated(task);
    }

    void undoTask(int id) {
        MainActivity.undoTask(id);

        Task task = findTask(id);
        task.done = false;

        onTaskUpdated(task);
    }

    void incrementPriority(int id) {
        Task task = findTask(id);

        if (MainActivity.setTaskPriority(id, task.priority + 1))
        {
            task.priority++;
        }

        onTaskUpdated(task);
    }

    void decreasePriority(int id) {
        Task task = findTask(id);

        if (MainActivity.setTaskPriority(id, task.priority - 1))
        {
            task.priority--;
        }

        onTaskUpdated(task);
    }

    private Task findTask(int id) {
        for (Task task: items) {
            if (task.id == id) {
                return task;
            }
        }
        return null;
    }

    private void onTaskUpdated(Task task) {
        for (FilteredList flist : flists) {
            flist.onUpdateSourceItem(task);
        }
    }

    private void initItems() {
        items.clear();
        items.addAll(MainActivity.getThings());

        for (FilteredList flist : flists) {
            flist.onResetSource(items);
        }
    }
}
