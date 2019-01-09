-- Only this table should have the word 'thing'
alter table thing
    rename to task;

-- Rename index
drop index idx_thing_done;
create index idx_task_done
    on task (done);
