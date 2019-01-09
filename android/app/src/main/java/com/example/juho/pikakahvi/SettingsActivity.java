package com.example.juho.pikakahvi;

import android.app.Activity;
import android.app.AlertDialog;
import android.content.DialogInterface;
import android.os.Bundle;
import android.text.format.DateFormat;
import android.view.View;
import android.widget.TextView;
import android.widget.Toast;

import java.io.File;
import java.io.FileInputStream;
import java.io.FileOutputStream;
import java.io.IOException;
import java.io.InputStream;
import java.io.OutputStream;
import java.util.Calendar;
import java.util.Date;

import static com.example.juho.pikakahvi.MainActivity.REAL_DB_NAME;
import static com.example.juho.pikakahvi.MainActivity.closeDBForce;
import static com.example.juho.pikakahvi.MainActivity.returnDBForce;

public class SettingsActivity extends Activity {

    public String DB_PATH;

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_settings);
        TextView tv = findViewById(R.id.settingsVersionText);
        tv.setText("Version: " + BuildConfig.GitBranch + " " + BuildConfig.GitVersion);

        View resetDbButton = findViewById(R.id.settingsResetDb);
        resetDbButton.setVisibility(BuildConfig.DEBUG ? View.VISIBLE : View.INVISIBLE);

        // TODO: Make shared, now copied from MainActivity
        DB_PATH = getApplicationContext().getFilesDir() + "/" + REAL_DB_NAME;
    }

    public void confirmReset(View view)
    {
        new AlertDialog.Builder(this)
                .setTitle("Reset DB")
                .setMessage("Are you sure?")
                .setIcon(android.R.drawable.ic_dialog_alert)
                .setPositiveButton(android.R.string.yes, new DialogInterface.OnClickListener() {
                    public void onClick(DialogInterface dialog, int whichButton) {
                        resetDBDebug();
                    }})
                .setNegativeButton(android.R.string.no, null).show();
    }

    public void confirmImport(View view)
    {
        new AlertDialog.Builder(this)
                .setTitle("Import DB")
                .setMessage("Are you sure?")
                .setIcon(android.R.drawable.ic_dialog_alert)
                .setPositiveButton(android.R.string.yes, new DialogInterface.OnClickListener() {
                    public void onClick(DialogInterface dialog, int whichButton) {
                        importDB();
                    }})
                .setNegativeButton(android.R.string.no, null).show();
    }

    public void confirmExport(View view)
    {
        new AlertDialog.Builder(this)
                .setTitle("Export DB")
                .setMessage("Are you sure?")
                .setIcon(android.R.drawable.ic_dialog_alert)
                .setPositiveButton(android.R.string.yes, new DialogInterface.OnClickListener() {
                    public void onClick(DialogInterface dialog, int whichButton) {
                        exportDB();
                    }})
                .setNegativeButton(android.R.string.no, null).show();
    }

    public void resetDBDebug() {
        boolean wasOpen = closeDBForce();
        File dbFile = new File(DB_PATH);
        dbFile.delete();

        if (wasOpen)
        {
            returnDBForce(DB_PATH, BuildConfig.DEBUG);
        }

        Toast.makeText(SettingsActivity.this, "DB reset", Toast.LENGTH_SHORT).show();
    }

    public void exportDB() {
        File src = new File(getApplicationContext().getFilesDir() + "/" + REAL_DB_NAME);
        File dst = new File(getExternalFilesDir(null).getAbsolutePath() + "/" + "pikakahvi.sqlite");

        try
        {
            dst.createNewFile();
            copy(src, dst);
        }
        catch (IOException e)
        {
            Toast.makeText(SettingsActivity.this,
                    "Failed to export DB: " + e.getMessage(), Toast.LENGTH_LONG).show();
        }

        Toast.makeText(SettingsActivity.this, "DB exported", Toast.LENGTH_SHORT).show();
    }

    public void importDB() {
        boolean wasOpen = closeDBForce();

        if (!BuildConfig.DEBUG)
        {
            backupDB();
        }

        File dbFile = new File(DB_PATH);
        dbFile.delete();


        File dst = new File(getApplicationContext().getFilesDir() + "/" + REAL_DB_NAME);
        File src = new File(getExternalFilesDir(null).getAbsolutePath() + "/" + "pikakahvi.sqlite");

        try
        {
            dst.createNewFile();
            copy(src, dst);
        }
        catch (IOException e)
        {
            Toast.makeText(SettingsActivity.this,
                    "Failed to import DB: " + e.getMessage(), Toast.LENGTH_LONG).show();
        }

        if (wasOpen)
        {
            returnDBForce(DB_PATH, BuildConfig.DEBUG);
        }

        Toast.makeText(SettingsActivity.this, "DB imported", Toast.LENGTH_SHORT).show();
    }

    public void backupDB()
    {
        File src = new File(getApplicationContext().getFilesDir() + "/" + REAL_DB_NAME);
        File dst = new File(getExternalFilesDir(null).getAbsolutePath()
                + "/" + "pikakahvi-"
                + DateFormat.format("yyyy-MM-dd-hh-mm-ss", Calendar.getInstance().getTime())
                + ".sqlite");

        try
        {
            dst.createNewFile();
            copy(src, dst);
        }
        catch (IOException e)
        {
            Toast.makeText(SettingsActivity.this,
                    "Failed to backup DB: " + e.getMessage(), Toast.LENGTH_LONG).show();
        }
    }

    // Based on https://stackoverflow.com/questions/9292954/how-to-make-a-copy-of-a-file-in-android
    public static void copy(File src, File dst) throws IOException {
        try (InputStream in = new FileInputStream(src)) {
            try (OutputStream out = new FileOutputStream(dst)) {
                // Transfer bytes from in to out
                byte[] buf = new byte[1024];
                int len;
                while ((len = in.read(buf)) > 0) {
                    out.write(buf, 0, len);
                }
            }
        }
    }
}
