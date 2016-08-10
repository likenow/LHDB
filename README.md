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


