package com.example.juho.pikakahvi;

import android.support.v7.widget.RecyclerView;
import android.view.LayoutInflater;
import android.view.ViewGroup;
import android.widget.Button;
import android.widget.LinearLayout;
import android.widget.TextView;

import com.example.juho.pikakahvi.data.Task;

import java.util.Comparator;

public class RecentlyDoneAdapter extends RecyclerView.Adapter<RecentlyDoneAdapter.ViewHolder> {
    private Dataset mDataset = new Dataset();


    public static class ViewHolder extends RecyclerView.ViewHolder {
        public TextView mTextView;
        public Button undoButton;
        public ViewHolder(LinearLayout v) {
            super(v);
            mTextView = v.findViewById(R.id.textView);
            undoButton = v.findViewById(R.id.undoButton);
        }
    }

    private class Dataset extends FilteredList<Task> {

        Dataset() {
            comparator = new Comparator<Task>() {
                @Override
                public int compare(Task task, Task t1) {
                    return task.timeDone - t1.timeDone;
                }
            };
        }

        @Override
        protected void onReset() {
            RecentlyDoneAdapter.this.notifyDataSetChanged();
        }

        @Override
        protected void onRemove(int i) {
            RecentlyDoneAdapter.this.notifyItemRemoved(i);
        }

        @Override
        protected void onAdd(int i) {
            RecentlyDoneAdapter.this.notifyItemInserted(i);
        }

        @Override
        protected void onUpdate(int i) {
            RecentlyDoneAdapter.this.notifyItemChanged(i);
        }

        @Override
        protected boolean filter(Task thing) {
            return thing.done;
        }
    }

    public RecentlyDoneAdapter(Things things)
    {
        things.addFilteredList(mDataset);
    }

    @Override
    public RecentlyDoneAdapter.ViewHolder onCreateViewHolder(ViewGroup parent,
                                                             int viewType) {
        LinearLayout v = (LinearLayout) LayoutInflater.from(parent.getContext())
                .inflate(R.layout.recent_list_item, parent, false);

        ViewHolder vh = new ViewHolder(v);
        return vh;
    }

    // Replace the contents of a view (invoked by the layout manager)
    @Override
    public void onBindViewHolder(ViewHolder holder, int position) {
        // - get element from your dataset at this position
        // - replace the contents of the view with that element
        holder.mTextView.setText(mDataset.getItem(position).title);
        holder.undoButton.setTag(new Integer(mDataset.getItem(position).id));
    }

    // Return the size of your dataset (invoked by the layout manager)
    @Override
    public int getItemCount() {
        return mDataset.size();
    }
}
