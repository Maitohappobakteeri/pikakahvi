alter table thing
    rename to thing_tmp;

-- List table for grouping things
create table list (
    id integer primary key,
    name varchar(32) not null unique
);

insert into list
    (name)
values
    ('generic');

-- New thing table
create table thing (
    id integer primary key,
    title varchar(256),
    done integer(1) default 0,
    list int not null,
    constraint fk_list foreign key (list) references list(id)
);

create index idx_category
    on thing (list);

-- Convert old things
insert into thing
    (title, done, list)
select title, done, (select (id) from list where name = 'generic') from thing_tmp;

drop table thing_tmp;
