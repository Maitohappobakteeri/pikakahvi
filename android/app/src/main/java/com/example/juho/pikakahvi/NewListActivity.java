package com.example.juho.pikakahvi;

import android.app.Activity;
import android.content.Intent;
import android.os.Bundle;
import android.view.View;
import android.widget.TextView;

public class NewListActivity extends Activity {
    public static final String LIST_NAME_MESSAGE = "com.example.pikakahvi.LIST_NAME_MESSAGE";

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_new_list);
    }

    public void finishEditing(View view) {
        TextView tv = findViewById(R.id.editListName);
        String text = tv.getText().toString();

        Intent resultIntent = new Intent();
        resultIntent.putExtra(LIST_NAME_MESSAGE, text);

        setResult(RESULT_OK, resultIntent);
        finish();
    }
}
