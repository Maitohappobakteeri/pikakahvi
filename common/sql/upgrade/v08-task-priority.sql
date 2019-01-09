alter table task
    add column priority integer not null default 1 check (priority >= 0);
