---
title: Linux的五种I/O模型
tags: 同步I/O,异步I/O,多路复用
---

**IO模型描述的是出现I/O等待时进程的状态以及处理数据的方式。围绕着进程的状态、数据准备到kernel buffer再到app buffer的两个阶段展开。其中数据复制到kernel buffer的过程称为数据准备阶段，数据从kernel buffer复制到user buffer的过程称为数据复制阶段。
对于一个network IO (这里我们以read举例)，它会涉及到两个系统对象，一个是调用这个IO的process (or thread)，另一个就是系统内核(kernel)。当一个read操作发生时，它会经历两个阶段：**
- 等待数据准备
- 将数据从内核拷贝到进程中

