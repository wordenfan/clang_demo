# clang_demo

### 测试列表：
- malloc
- calloc （创建结构体数组）
- realloc（传参或不传参创建结构体数组）
- pthread_test （多线程文件通讯）
- phread_deadlock_test （死锁后robust接管）

### Tips:
- Q: error: implicit declaration of function 'pthread_mutex_consistent'
- A: 在macOs中并不支持pthread_mutexattr_setrobust函数，所以只能通过远程调试在Linux环境调试