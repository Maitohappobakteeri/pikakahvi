package com.example.juho.pikakahvi;

import android.app.Activity;
import android.app.AlertDialog;
import android.content.DialogInterface;
import android.content.Intent;
import android.os.Bundle;
import android.view.View;
import android.widget.TextView;
import android.widget.Toast;

import com.example.juho.pikakahvi.data.TaskList;

public class EditListActivity extends Activity {
    public static final String LISTID_MESSAGE = "com.example.pikakahvi.LISTID_MESSAGE";

    private TaskList thingList;

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_edit_list);

        Intent intent = getIntent();
        int listId = Integer.valueOf(intent.getStringExtra(LISTID_MESSAGE));

        MainActivity.initDB(getApplicationContext().getFilesDir() + "/" + MainActivity.REAL_DB_NAME,
                BuildConfig.DEBUG);

        thingList = MainActivity.getList(listId);

        TextView tv = findViewById(R.id.editListName);
        tv.setText(thingList.name);
    }

    @Override
    protected void onStart() {
        super.onStart();
    }

    @Override
    protected void onDestroy()
    {
        super.onDestroy();
        MainActivity.closeDB();
    }

    public void onMarkDeprecated(View view) {

        new AlertDialog.Builder(this)
                .setTitle("Mark " + thingList.name + " as deprecated")
                .setMessage("Are you sure?")
                .setIcon(android.R.drawable.ic_dialog_alert)
                .setPositiveButton(android.R.string.yes, new DialogInterface.OnClickListener() {
                    public void onClick(DialogInterface dialog, int whichButton) {
                        MainActivity.markListDeprecated(thingList.id);
                        Toast.makeText(EditListActivity.this, "Marked deprecated", Toast.LENGTH_SHORT).show();
                    }})
                .setNegativeButton(android.R.string.no, null).show();
    }
}
