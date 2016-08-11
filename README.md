# LHDB

  一个数据库工具

framework:

LHArray:自定义数组,使用双向链表实现的一个动态C数组(考虑到链表占用内存太大换成C数组)。

LHDictionary:自定义字典,哈希表实现的一个动态C字典。

LHSqlite:数据库主体操作库,使用C语言实现,结合了LHArray,LHDictionary。

LHModel:字典转模型工具。

LHSqliteManager:管理多个数据库。

NSObject+LHSqlite:数据库所有api。




LHDB是一个对象存储型数据库。

NSObject+LHSqlite是对NSObject写的扩展,所以所有的数据库操作都可以直接通过类或者对象来调用。


API:
    + (BOOL)lh_openDB;
    
    打开一个数据库,任何数据库操作之前必须打开数据库否则执行失败,打开成功后LHSqliteRef会持有一个数据库连接.当其他线程需要执行数据库操作时,必须再次调用此方法。
    
    + (BOOL)lh_closeDB;

    打开数据库后,使用此方法关闭.

    + (BOOL)lh_createTable;

    建表,会根据model中所有的属性创建column。

    + (BOOL)lh_createTableWithContraints:(NSDictionary<NSString*,NSArray<NSNumber*>*>*)contraints;

    建表,根据传入的contraints为对应的column添加约束

    + (BOOL)lh_beginTransaction;

    开启手动管理事务,当执行大量insert，update，delete语句时建议打开。

    + (BOOL)lh_commit;

    提交事务

    增，删，改，查:

    NSObject+LHSqlite中有对应的api

LHSqliteRef:
    这是一个C写的对象,一个sqliteRef对象管理一个数据库文件。sqliteRef对象可以从LHSqliteManager单列中获取。
    NSObject+LHSqlite中不带sqliteRef参数的api运行在默认的数据库,如果希望操作其他的数据库,可以使用带参数的api传入一个sqliteRef对象，也可以更改LHSqliteManager的sqlitePath路径，重写了sqlitePaht的set方法，当更改路径时默认的currentSqlite也会随之更改。
    
    LHSqliteRef包装了sqlite3常用的api,并提供了一个锁用于维护线程安全,打开数据库是会启动这个锁,关闭时释放。内部提供了一个LHDictionary字典对象用于缓存sqlite3_stmt,提升了执行速度。

    LHSqliteRef和外部的数据交互由LHArray和LHDictionary来完成,因为LHSqliteRef所使用的数据需要是特定的结构体对象,所有我自己维护了一个字典和数组对象,并为NSArray及NSDictionary写了扩展用于和自定义的容器转换。

    LHSqliteRef还提供了WAL模式,是需要执行 LHSqliteSetPropertyWAL 并传入true。

LHArray:
    最开始采用的数据结构是双向链表,不过由于链表内存使用量过大而且数据的内存地址不连续插入效率并不比C数组快，所以数据结构换成了数组。不过始终没有解决数组插入效率的问题。

LHDictionary:
    数据结构采用的哈希表,借鉴了openssl的哈希算法,使用链表去解决hash冲突。在容量超过bucket的1.75时重构内存。

LHModel:
    对model的解析,内部使用runtime的model进行解析及转化.

    
