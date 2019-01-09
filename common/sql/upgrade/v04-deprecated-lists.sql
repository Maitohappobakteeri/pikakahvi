drop index idx_category;
alter table list
    rename to list_tmp;

create table list (
    id integer primary key,
    name varchar(32) not null unique,
    deprecated integer(1) default 0
);

insert into list
    (id, name)
select id, name from list_tmp;

drop table list_tmp;
