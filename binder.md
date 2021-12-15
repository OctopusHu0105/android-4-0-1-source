请联系 octopushu@163.com

### Binder 概要

binder 是 android 上进程间通讯的一种方式。有以下三种特色：

1. Binder 通信机制采用 C/S 架构；

2. Binder 通信机制中主要涉及到 4 个角色 Client、Server、Service Manager 和 Binder 驱动，其中 Client、Server、Service Manager 运行在用户空间，Binder 驱动运行在内核空间；

- Client 代表客户端进程
- Server 代表客户端进程提供各种服务
- Service Manager 用来管理各种系统服务vim 
- Binder 驱动提供进程间通信的能力

3. 用户空间的 Client、Server、ServiceManager 通过 open、mmap 和 ioctl 等标准文件操作来访问 /dev/binder，进而实现进程间通信。

##逻辑框架图
 

   client、server 和 serviceManager 之间通讯都是基于 binder 机制。server 注册服务和 client 获取服务的过程都需要 serviceManager，这里的 serviceManager 是这个binder 通讯的大管家，也是 binder 通讯机制的守护进程。当 serviceManager 启动之后，client 端和 server 端通信时都需要先获取 serviceManager 接口，才能开始通信服务。

1. 注册服务：server 进程要先注册 service 到 serviceManager。该过程中 server 是客户端，serviceManager 是服务端。

2. 获取服务：client 进程使用某个 service 前，须先向 serviceManager 中获取相应的 service。该过程中 client 是客户端，serviceManager 是服务端。

3. 使用服务：client 根据得到的 service 信息建立与 service 所在的 server 进程通信的通路，然后就可以直接与 service 交互。该过程中 client 是客户端，server 是服务端。
        
   client、server 和 serviceManager 彼此之间不是直接交互的，而是都通过与 binder 驱动进行交互的，从而实现IPC通信方式。其中 binder 驱动位于内核空间，client、server 和 serviceManager 位于用户空间。  binder 驱动和 serviceManager 整个系统平台的基础架构，client 和 server 是应用层，开发人员只需自定义实现 client、Server 端，借助平台基础架构便可以直接进行 IPC 通信。









