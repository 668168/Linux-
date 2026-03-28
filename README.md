# Linux 设备驱动学习路线

![20191103_094602_84](image/20191103_094602_84.png)

这个仓库不是单纯的代码归档，而是一个用来学习 Linux 设备驱动开发的学习工作区。内容覆盖驱动基础概念、章节笔记、可编译示例、用户态测试程序，以及一些历史资料和补充实例。

如果你的目标是“看懂驱动代码，并且能自己动手改例子、跑实验”，建议按下面的路线推进，而不是一上来就直接看复杂总线驱动。

## 学习目标

通过这个仓库，建议逐步完成下面几件事：

- 建立 Linux 设备驱动的整体认知
- 看懂字符设备驱动的基本结构
- 理解 `open`、`read`、`write`、`ioctl`、`poll`、异步通知等常见接口
- 理解并发、阻塞、中断、内存映射等关键机制
- 逐步进入平台总线、I2C、USB、块设备、网络设备等更复杂主题
- 能独立编译、加载、测试、修改一个简单驱动示例

## 仓库内容

- [`docs/`](docs/): 学习笔记、章节总结、概念说明
- [`code/`](code/): 主要示例代码，通常按章节组织
- [`simple/`](simple/): 更零散、偏练手的样例和书中原始代码
- [`实例代码/`](实例代码/): 额外的中文驱动和实验样例
- [`image/`](image/) 和 [`reference/`](reference/): 配图和参考资料

如果你准备新增内容，优先把总结和理解写到 `docs/`，把代码实验放到最接近的章节目录中。

## 推荐学习路线

### 第一阶段：建立基础认知

先读这些内容，把驱动开发的基本背景补上：

1. [Linux设备驱动概述及开发环境构建](docs/Linux设备驱动概述及开发环境构建.md)
2. [驱动设计的硬件基础](docs/驱动设计的硬件基础.md)
3. [Linux内核及内核编程](docs/Linux内核及内核编程.md)
4. [Linux内核模块](docs/Linux内核模块.md)
5. [Linux文件系统与设备文件](docs/Linux文件系统与设备文件.md)

这一阶段的目标不是记住所有细节，而是先理解：

- 驱动在内核里的位置
- 模块如何编译和加载
- 设备文件和驱动接口是什么关系

### 第二阶段：先吃透字符设备驱动

字符设备驱动是最适合入门和练手的部分，建议重点看：

1. [字符设备驱动](docs/字符设备驱动.md)
2. [Linux设备驱动中的并发控制](docs/Linux设备驱动中的并发控制.md)
3. [Linux设备驱动中的阻塞与非阻塞I/O](docs/Linux设备驱动中的阻塞与非阻塞IO.md)
4. [Linux设备驱动中的异步通知与异步I/O](docs/Linux设备驱动中的异步通知与异步IO.md)
5. [中断与时钟](docs/中断与时钟.md)
6. [内存与I/O访问](docs/内存与IO访问.md)

推荐结合下面这些例子一起看：

- [`code/06/globalmem驱动`](code/06/globalmem驱动)
- [`code/06/包含2个globalmem设备的驱动`](code/06/包含2个globalmem设备的驱动)
- [`code/08/globalfifo驱动`](code/08/globalfifo驱动)
- [`code/08/poll应用程序范例`](code/08/poll应用程序范例)
- [`code/09/支持异步通知的globalfifo`](code/09/支持异步通知的globalfifo)
- [`code/09/异步通知应用程序范例`](code/09/异步通知应用程序范例)

建议在这个阶段重点回答这些问题：

- 驱动是怎么注册字符设备的
- `file_operations` 里每个回调在什么时候被调用
- 阻塞和非阻塞访问在代码里怎么体现
- `poll` 和异步通知分别解决什么问题
- 自旋锁、信号量、等待队列分别用在什么场景

### 第三阶段：理解驱动模型和总线框架

在字符设备基础打稳之后，再进入更抽象的驱动模型：

1. [Linux设备驱动的软件架构思想](docs/Linux设备驱动的软件架构思想.md)
2. [Linux I2C核心、总线与设备驱动](docs/LinuxI2C核心总线与设备驱动.md)
3. [USB主机、设备与Gadget驱动](docs/USB主机设备与Gadget驱动.md)
4. [I2C、SPI、USB驱动架构类比](docs/I2CSPIUSB驱动架构类比.md)
5. [ARM Linux设备树](docs/ARMLinux设备树.md)
6. [Linux电源管理的系统架构和驱动](docs/Linux电源管理的系统架构和驱动.md)

这一阶段的重点是建立“设备、驱动、总线、匹配、枚举”这些关系，而不是只盯着单个 API。

### 第四阶段：专题驱动和系统能力

最后再按兴趣或项目需要深入：

1. [Linux块设备驱动](docs/Linux块设备驱动.md)
2. [Linux网络设备驱动](docs/Linux网络设备驱动.md)
3. [Linux芯片级移植及底层驱动](docs/Linux芯片级移植及底层驱动.md)
4. [Linux设备驱动的调试](docs/Linux设备驱动的调试.md)

如果你的工作和嵌入式 SoC 平台相关，这一阶段通常会和设备树、中断、时钟、电源管理一起反复交叉学习。

## 建议实践方式

每学一个主题，尽量按这个节奏推进：

1. 先读对应 `docs/` 笔记，建立概念框架
2. 再找 `code/` 或 `simple/` 里的最近示例，对照阅读源码
3. 在示例目录里执行 `make modules`
4. 如果目录提供测试程序，继续编译并运行测试
5. 自己修改一个小点，再重新编译验证行为变化
6. 把结论和踩坑记录回写到 `docs/` 或对应目录说明中

不要只停留在“看过代码”。对驱动学习来说，编译、加载、调用、观察日志，这几个动作非常关键。

## 常用命令

大多数代码样例都需要进入具体目录单独构建，没有统一的仓库级命令。

```sh
cd code/06/包含2个globalmem设备的驱动
make modules
make ioctl_test
```

常见命令：

- `make modules`：编译当前目录内核模块
- `make clean`：清理编译产物
- `make insmod` / `make rmmod`：加载或卸载模块
- `make ioctl_test` 或 `make test_ioctl`：构建并运行用户态测试

说明：

- 不同目录支持的目标不同，需要先看本目录 `Makefile`
- 某些老示例依赖旧版本内核接口，不能直接在新内核上无修改编译
- 涉及 `insmod`、`rmmod`、创建设备节点时通常需要 root 权限

## 章节导航

- [Linux设备驱动概述及开发环境构建](docs/Linux设备驱动概述及开发环境构建.md)
- [Linux设备驱动的开发环境构建](docs/Linux设备驱动概述及开发环境构建/Linux设备驱动的开发环境构建.md)
- [驱动设计的硬件基础](docs/驱动设计的硬件基础.md)
- [Linux内核及内核编程](docs/Linux内核及内核编程.md)
- [Linux内核模块](docs/Linux内核模块.md)
- [Linux文件系统与设备文件](docs/Linux文件系统与设备文件.md)
- [字符设备驱动](docs/字符设备驱动.md)
- [Linux设备驱动中的并发控制](docs/Linux设备驱动中的并发控制.md)
- [Linux设备驱动中的阻塞与非阻塞I/O](docs/Linux设备驱动中的阻塞与非阻塞IO.md)
- [Linux设备驱动中的异步通知与异步I/O](docs/Linux设备驱动中的异步通知与异步IO.md)
- [中断与时钟](docs/中断与时钟.md)
- [内存与I/O访问](docs/内存与IO访问.md)
- [Linux设备驱动的软件架构思想](docs/Linux设备驱动的软件架构思想.md)
- [Linux块设备驱动](docs/Linux块设备驱动.md)
- [Linux网络设备驱动](docs/Linux网络设备驱动.md)
- [Linux I2C核心、总线与设备驱动](docs/LinuxI2C核心总线与设备驱动.md)
- [USB主机、设备与Gadget驱动](docs/USB主机设备与Gadget驱动.md)
- [I2C、SPI、USB驱动架构类比](docs/I2CSPIUSB驱动架构类比.md)
- [ARM Linux设备树](docs/ARMLinux设备树.md)
- [Linux电源管理的系统架构和驱动](docs/Linux电源管理的系统架构和驱动.md)
- [Linux芯片级移植及底层驱动](docs/Linux芯片级移植及底层驱动.md)
- [Linux设备驱动的调试](docs/Linux设备驱动的调试.md)
- [勘误](docs/勘误.md)

## 参考

- <https://github.com/gatieme/LDD-LinuxDeviceDrivers>
- <https://mp.weixin.qq.com/s/fuPgpugqJeezBJkn4JfKsQ>
- <http://www.embstu.xyz/2018/07/09/car-build-devlop-env/>
- 《Linux 设备驱动开发详解》
- 《冯国进 Linux驱动程序开发实例》
- 《ARM嵌入式Linux设备驱动实例开发》
- 《嵌入式Linux设备驱动开发详解》
- 《Linux驱动开发入门与实战》
