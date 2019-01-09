package com.example.juho.pikakahvi;

import android.app.Activity;
import android.content.Context;
import android.content.Intent;
import android.os.Bundle;
import android.support.v7.widget.LinearLayoutManager;
import android.support.v7.widget.RecyclerView;
import android.view.View;

public class RecentlyDoneActivity extends Activity {
    RecyclerView mRecyclerView;
    RecyclerView.LayoutManager mLayoutManager;
    RecentlyDoneAdapter mAdapter;
    Things things;

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_recently_done);

        things = new Things(getApplicationContext());

        mRecyclerView = (RecyclerView) findViewById(R.id.recentList);
        mRecyclerView.setHasFixedSize(true);

        mLayoutManager = new LinearLayoutManager(this);
        mRecyclerView.setLayoutManager(mLayoutManager);

        mAdapter = new RecentlyDoneAdapter(things);
        mRecyclerView.setAdapter(mAdapter);

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

    public void undoTask(View view) {
        things.undoTask((int) view.getTag());
    }
}
