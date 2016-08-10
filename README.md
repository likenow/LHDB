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
