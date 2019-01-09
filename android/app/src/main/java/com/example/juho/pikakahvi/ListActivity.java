package com.example.juho.pikakahvi;

import android.app.Activity;
import android.content.Context;
import android.content.Intent;
import android.os.Bundle;
import android.support.v7.widget.LinearLayoutManager;
import android.support.v7.widget.RecyclerView;
import android.support.v7.widget.helper.ItemTouchHelper;
import android.view.View;

public class ListActivity extends Activity {
    public static final String EXTRA_MESSAGE = "com.example.pikakahvi.MESSAGE";
    public static final String LISTID_MESSAGE = "com.example.pikakahvi.LISTID_MESSAGE";
    static final int NEW_THING_REQUEST = 1;

    RecyclerView mRecyclerView;
    RecyclerView.LayoutManager mLayoutManager;
    MyAdapter mAdapter;
    Things things;
    int listId;

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_list);

        things = new Things(getApplicationContext());

        mRecyclerView = (RecyclerView) findViewById(R.id.mainlista);

        // use this setting to improve performance if you know that changes
        // in content do not change the layout size of the RecyclerView
        mRecyclerView.setHasFixedSize(true);

        // use a linear layout manager
        mLayoutManager = new LinearLayoutManager(this);
        mRecyclerView.setLayoutManager(mLayoutManager);

        // specify an adapter (see also next example)
        Intent intent = getIntent();
        listId = Integer.valueOf(intent.getStringExtra(LISTID_MESSAGE));
        mAdapter = new MyAdapter(getApplicationContext(), things, listId);
        mRecyclerView.setAdapter(mAdapter);
        ItemTouchHelper touchHelper = new ItemTouchHelper(new ItemTouchHelper.Callback() {
            @Override
            public int getMovementFlags(RecyclerView recyclerView, RecyclerView.ViewHolder viewHolder) {
                return makeMovementFlags(
                        0,
                        ItemTouchHelper.LEFT | ItemTouchHelper.RIGHT
                );
            }

            @Override
            public boolean onMove(RecyclerView recyclerView, RecyclerView.ViewHolder viewHolder, RecyclerView.ViewHolder target) {
                return false;
            }

            @Override
            public void onSwiped(RecyclerView.ViewHolder viewHolder, int direction) {
                MyAdapter.ViewHolder vh = (MyAdapter.ViewHolder) viewHolder;

                if (direction == ItemTouchHelper.LEFT) {
                    things.decreasePriority((int) vh.doneButton.getTag());
                }
                else if (direction == ItemTouchHelper.RIGHT) {
                    things.incrementPriority((int) vh.doneButton.getTag());
                }
            }
        });
        touchHelper.attachToRecyclerView(mRecyclerView);

        MainActivity.initDB(getApplicationContext().getFilesDir() + "/" + MainActivity.REAL_DB_NAME,
                BuildConfig.DEBUG);
    }

    @Override
    protected void onStart() {
        super.onStart();

        things.update();
    }

    @Override
    protected void onDestroy()
    {
        super.onDestroy();

        MainActivity.closeDB();
    }

    public void onClickAdd(View view)
    {
        Intent intent = new Intent(this, NewThingActivity.class);
        intent.putExtra(EXTRA_MESSAGE, "woop");
        startActivityForResult(intent, NEW_THING_REQUEST);
    }

    public void doneThing(View view) {
        things.markDone((int) view.getTag());
    }

    @Override
    protected void onActivityResult(int requestCode, int resultCode, Intent data) {
        // Check which request we're responding to
        if (requestCode == NEW_THING_REQUEST) {
            // Make sure the request was successful
            if (resultCode == RESULT_OK) {
                String s = data.getStringExtra("thing");
                things.addThing(s, listId);
            }
        }
    }
}
