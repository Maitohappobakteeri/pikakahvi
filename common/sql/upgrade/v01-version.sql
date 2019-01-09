create table version (
    id integer(1) default 1 not null,
    version integer,
    
    constraint pk_version primary key (id),
    constraint ck_id_1 check (id=1)
);

insert into version (version) values (1);
