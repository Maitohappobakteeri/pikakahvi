package com.example.juho.pikakahvi;

import android.app.Activity;
import android.content.Context;
import android.content.Intent;
import android.content.res.AssetManager;
import android.os.Bundle;
import android.os.Environment;
import android.support.v7.widget.RecyclerView;
import android.util.Log;
import android.view.LayoutInflater;
import android.view.View;
import android.widget.Button;
import android.widget.LinearLayout;
import android.widget.TextView;

import com.example.juho.pikakahvi.data.Task;
import com.example.juho.pikakahvi.data.TaskList;

import java.io.File;
import java.io.FileInputStream;
import java.io.FileOutputStream;
import java.io.IOException;
import java.io.InputStream;
import java.io.OutputStream;
import java.util.List;

public class MainActivity extends Activity {
    static final int NEW_LIST_REQUEST = 1;
    static final int SETTINGS_REQUEST = 2;
    static final int EDIT_LIST_REQUEST = 3;

    public static final String DATABASE_NAME = "pikakahvi.db";
    public static final String DEBUG_DATABASE_NAME = "pikakahvi-debug.db";
    public static final String REAL_DB_NAME =
            BuildConfig.DEBUG
            ? DEBUG_DATABASE_NAME
            : DATABASE_NAME;

    public String DB_PATH;


    // Used to load the 'native-lib' library on application startup.
    static {
        System.loadLibrary("native-lib");
    }

    RecyclerView mRecyclerView;
    RecyclerView.LayoutManager mLayoutManager;
    MyAdapter mAdapter;
    Things things;

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_main);

        DB_PATH = getApplicationContext().getFilesDir() + "/" + REAL_DB_NAME;

        initDB(DB_PATH, BuildConfig.DEBUG);

        initMenuItems();

        LinearLayout tt = findViewById(R.id.menuTricks);

        LayoutInflater.from(this).inflate(R.layout.recent_button, tt);
        LayoutInflater.from(this).inflate(R.layout.addthinglist, tt);
        LayoutInflater.from(this).inflate(R.layout.settings_button, tt);
    }

    @Override
    protected void onDestroy()
    {
        super.onDestroy();

        closeDB();
    }

    @Override
    protected void onActivityResult(int requestCode, int resultCode, Intent data) {
        // Check which request we're responding to
        if (requestCode == NEW_LIST_REQUEST) {
            // Make sure the request was successful
            if (resultCode == RESULT_OK) {
                String s = data.getStringExtra(NewListActivity.LIST_NAME_MESSAGE);
                createNewList(s);

                initMenuItems();
            }
        }
        else if (requestCode == SETTINGS_REQUEST)
        {
            // Allow returning without any additional confirms
            //if (resultCode == RESULT_OK) {
                initMenuItems();
            //}
        }
        else if (requestCode == EDIT_LIST_REQUEST)
        {
            initMenuItems();
        }
    }

    public void onClickList(View view)
    {
        Intent intent = new Intent(this, ListActivity.class);
        intent.putExtra(ListActivity.LISTID_MESSAGE, (String) view.getTag());
        startActivity(intent);
    }

    public void onEditList(View view)
    {
        Intent intent = new Intent(this, EditListActivity.class);
        intent.putExtra(EditListActivity.LISTID_MESSAGE, (String) view.getTag());
        startActivityForResult(intent, EDIT_LIST_REQUEST);
    }

    public void addThingList(View view) {
        Intent intent = new Intent(this, NewListActivity.class);
        startActivityForResult(intent, NEW_LIST_REQUEST);
    }

    public void openSettings(View view) {
        Intent intent = new Intent(this, SettingsActivity.class);
        startActivityForResult(intent, SETTINGS_REQUEST);
    }

    public void openRecentlyDone(View view) {
        Intent intent = new Intent(this, RecentlyDoneActivity.class);
        startActivity(intent);
    }

    private void initMenuItems()
    {
        List<TaskList> lists = getLists();
        LinearLayout ll = findViewById(R.id.menuItems);
        ll.removeAllViews();

        for (TaskList list : lists)
        {
            LayoutInflater.from(this).inflate(R.layout.chooselist, ll);

            View cl = ll.getChildAt(ll.getChildCount() - 1);

            TextView tv = cl.findViewById(R.id.chooseListName);
            tv.setText(list.name);
            tv.setTag(Integer.toString(list.id));

            Button bt = cl.findViewById(R.id.editButton);
            bt.setTag(Integer.toString(list.id));
        }
    }


    public static native List<Task> getThings();
    public static native TaskList getList(int listId);
    public static native boolean markListDeprecated(int listId);
    public static native List<TaskList> getLists();
    public static native Task createThing(String title, int listId);
    public static native boolean createNewList(String name);
    public static native boolean markDone(int id);
    public static native boolean setTaskPriority(int id, int priority);
    public static native boolean undoTask(int id);
    public static native boolean initDB(String dbname, boolean debug);
    public static native boolean closeDB();

    public static native boolean returnDBForce(String dbname, boolean debug);
    public static native boolean closeDBForce();
}
