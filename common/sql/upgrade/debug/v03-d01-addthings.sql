insert into list
    (name)
values
    ('love'),
    ('liquid');

insert into thing
    (title, list)
values
    ('Something generic', (select (id) from list where name = 'generic')),
    ('Another generic thing', (select (id) from list where name = 'generic'));

insert into thing
    (title, list)
values
    ('trap', (select (id) from list where name = 'love')),
    ('tea', (select (id) from list where name = 'liquid')),
    ('coffee', (select (id) from list where name = 'liquid'));
