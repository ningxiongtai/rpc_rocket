# rocket

## 1.总览
rocket 是基于 C++11 开发的一款多线程的异步 RPC 框架，它旨在高效、简洁的同时，又保持至极高的性能。

rocket 同样是基于主从 Reactor 架构，底层采用 epoll 实现 IO 多路复用。应用层则基于 protobuf 自定义 rpc 通信协议，同时也将支持简单的 HTTP 协议。



## 2. 前置准备
### 2.1 环境搭建
- 开发环境：Linux，可以是虚拟机。推荐跟我保持一致，Centos8 的虚拟机. 包含必要的 C++ 开发工具，如 GCC/G++(至少能支持到 C++11 语法的版本)
- 开发工具：VsCode，通过 ssh 远程连接 linux 机器

### 2.2 依赖库的安装
#### 2.2.1 protobuf
protobuf 推荐使用 3.19.4 及其以上：

安装过程：
```
wget  https://github.com/protocolbuffers/protobuf/releases/download/v3.19.4/protobuf-cpp-3.19.4.tar.gz

tar -xzvf protobuf-cpp-3.19.4.tar.gz
```

我们需要指定 安装路径在 `/usr` 目录下:
```
cd protobuf-cpp-3.19.4

./configure -prefix=/usr/local

make -j4 

sudo make install
```

安装完成后，你可以找到头文件将位于 `/usr/include/google` 下，库文件将位于 `/usr/lib` 下。

#### 2.2.2 tinyxml
项目中使用到了配置模块，采用了 xml 作为配置文件。因此需要安装 libtinyxml 解析 xml 文件。

```
wget https://udomain.dl.sourceforge.net/project/tinyxml/tinyxml/2.6.2/tinyxml_2_6_2.zip

unzip tinyxml_2_6_2.zip

```

