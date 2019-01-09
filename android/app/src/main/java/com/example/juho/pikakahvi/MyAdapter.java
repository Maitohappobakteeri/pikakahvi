package com.example.juho.pikakahvi;

import android.content.Context;
import android.support.v7.widget.RecyclerView;
import android.support.v7.widget.helper.ItemTouchHelper;
import android.view.LayoutInflater;
import android.view.View;
import android.view.ViewGroup;
import android.widget.Button;
import android.widget.LinearLayout;
import android.widget.TextView;

import com.example.juho.pikakahvi.data.Task;

import java.util.Comparator;
import java.util.List;

public class MyAdapter extends RecyclerView.Adapter<MyAdapter.ViewHolder> {
    private Dataset mDataset = new Dataset();
    private int listId = -1;
    private int[] priorityColors;

    // Provide a reference to the views for each data item
    // Complex data items may need more than one view per item, and
    // you provide access to all the views for a data item in a view holder
    public static class ViewHolder extends RecyclerView.ViewHolder {
        // each data item is just a string in this case
        public LinearLayout layout;
        public TextView mTextView;
        public Button doneButton;
        public ViewHolder(LinearLayout v) {
            super(v);
            layout = v.findViewById(R.id.texti);
            mTextView = v.findViewById(R.id.textView);
            doneButton = v.findViewById(R.id.doneButton);
        }
    }

    private class Dataset extends FilteredList<Task> {

        Dataset() {
            comparator = new Comparator<Task>() {
                @Override
                public int compare(Task task, Task t1) {
                    return task.priority - t1.priority;
                }
            };
        }

        @Override
        protected void onReset() {
            MyAdapter.this.notifyDataSetChanged();
        }

        @Override
        protected void onRemove(int i) {
            MyAdapter.this.notifyItemRemoved(i);
        }

        @Override
        protected void onAdd(int i) {
            MyAdapter.this.notifyItemInserted(i);
        }

        @Override
        protected void onUpdate(int i) {
            MyAdapter.this.notifyItemChanged(i);
        }

        @Override
        protected boolean filter(Task thing) {
            return MyAdapter.this.listId == thing.listId && !thing.done;
        }
    }

    // Provide a suitable constructor (depends on the kind of dataset)
    public MyAdapter(Context context, Things things, int listId)
    {
        things.addFilteredList(mDataset);
        this.listId = listId;

        priorityColors = new int[5];
        priorityColors[0] = context.getColor(R.color.colorPriority0);
        priorityColors[1] = context.getColor(R.color.colorPriority1);
        priorityColors[2] = context.getColor(R.color.colorPriority2);
        priorityColors[3] = context.getColor(R.color.colorPriority3);
        priorityColors[4] = context.getColor(R.color.colorPriority4);
    }

    // Create new views (invoked by the layout manager)
    @Override
    public MyAdapter.ViewHolder onCreateViewHolder(ViewGroup parent,
                                                   int viewType) {
        // create a new view
        LinearLayout v = (LinearLayout) LayoutInflater.from(parent.getContext())
                .inflate(R.layout.texti, parent, false);

        ViewHolder vh = new ViewHolder(v);
        return vh;
    }

    // Replace the contents of a view (invoked by the layout manager)
    @Override
    public void onBindViewHolder(ViewHolder holder, int position) {
        // - get element from your dataset at this position
        // - replace the contents of the view with that element
        holder.mTextView.setText(mDataset.getItem(position).title);
        holder.doneButton.setTag(new Integer(mDataset.getItem(position).id));

        int priority = mDataset.getItem(position).priority;
        int color = priorityColors[Math.max(Math.min(priority, 4), 0)];
        holder.layout.setBackgroundColor(color);
    }

    // Return the size of your dataset (invoked by the layout manager)
    @Override
    public int getItemCount() {
        return mDataset.size();
    }
}
