create index idx_category
    on thing (list);

create index idx_thing_done
    on thing (done);

create index idx_list_deprecated
    on list (deprecated);
