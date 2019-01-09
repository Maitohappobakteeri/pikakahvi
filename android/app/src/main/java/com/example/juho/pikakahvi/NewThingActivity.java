package com.example.juho.pikakahvi;

import android.app.Activity;
import android.content.Intent;
import android.os.Bundle;
import android.view.View;
import android.widget.TextView;

public class NewThingActivity extends Activity {
    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_new_thing);

        // Get the Intent that started this activity and extract the string
        Intent intent = getIntent();
        String message = intent.getStringExtra(ListActivity.EXTRA_MESSAGE);
    }

    public void finishThing(View view) {
        TextView tv = findViewById(R.id.editText);
        String text = tv.getText().toString();

        Intent resultIntent = new Intent();
        resultIntent.putExtra("thing", text);

        setResult(RESULT_OK, resultIntent);
        finish();
    }
}
