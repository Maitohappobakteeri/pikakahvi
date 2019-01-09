package com.example.juho.pikakahvi;

import java.util.ArrayList;
import java.util.Collection;
import java.util.Comparator;
import java.util.List;

public abstract class FilteredList<T> {
    private ArrayList<T> list;

    FilteredList() {
        list = new ArrayList<>();
    }

    public void onResetSource(Collection<T> source) {
        list.clear();

        for (T item : source) {
            if (filter(item)) {
                insert(item);
            }
        }

        onReset();
    }

    public void onAddSourceItem(T item) {
        if (filter(item)) {
            onAdd(insert(item));
        }
    }

    public void onRemoveSourceItem(T item) {
        int i = list.indexOf(item);
        if (i >= 0) {
            list.remove(i);
            onRemove(i);
        }
    }

    public void onUpdateSourceItem(T item) {
        // TODO: This is a mess
        int i = list.indexOf(item);
        if (i >= 0) {
            if (!filter(item)) {
                list.remove(i);
                onRemove(i);
            }
            else {
                if (comparator == null) {
                    onUpdate(i);
                }
                else {
                    list.remove(i);
                    onRemove(i);
                    onAdd(insert(item));
                }
            }
        }
        else {
            if (filter(item)) {
                onAdd(insert(item));
            }
        }
    }

    private int insert(T item) {
        // TODO: binary search

        if (comparator != null) {
            int index = 0;
            for (; index<list.size(); ++index) {
                if (comparator.compare(item, list.get(index)) >= 0) {
                    break;
                }
            }

            list.add(index, item);
            return index;
        }
        else {
            list.add(item);
            return list.size() - 1;
        }
    }

    public T getItem(int index) {
        return list.get(index);
    }

    public int size() {
        return list.size();
    }

    protected Comparator<T> comparator;
    protected abstract boolean filter(T t);
    protected abstract void onReset();
    protected abstract void onAdd(int index);
    protected abstract void onRemove(int index);
    protected abstract void onUpdate(int index);
}
