#  The Nixy Library

这是一个小巧灵活的 跨平台/编译器的 C++基础库

* 目前支持的OS: Win32/64/CE, Linux
* 目前支持的CC: VC(8.0+), gcc

## nixyx

    - Nixy的构建系统, 使用 Qt 5.1.1 编写
    - 脚本可以使用ini和js, 其中ini不支持条件判断和循环(当然了,这货只是个ini), 仅用于最简单的情况
    - 目前支持生成: Makefile, Visual Studio项目文件(Format Version 9.00 以上)

## nixycore

    - Nixy的核心部分, 基于泛型思想搭建的C++库
    - 目标: 代码轻巧,功能灵活,使用方便

    > container            ...done        - 各种泛型容器
    > finalizer            ...done        - 资源管理，包括通用rc和gc实现等
    > memory               ...done        - 内存管理，包括内存池、通用内存分配器、rc智能指针等
    > preprocessor	       ...done        - 预处理器，用于对编译器的预处理阶段编程，一般用于自动代码生成
    > typemanip	           ...done        - 类型操作，用于对编译器的编译期编程，实现各种泛型技巧
    > thread               ...done        - 线程管理，包括原子类、各种线程同步对象、线程类等
    > utility              ...done        - 各种实用小工具
    > al(Abstract Layer)   ...ongoing     - 抽象层，提供跨平台的兼容适配
    > general              ...ongoing     - 全库的通用部分
    > algorithm	           ...working     - 提供一些基本的算法
    > delegate	           ...working     - 委托，包括泛化仿函数、绑定器等
    > bugfix               ...preparing   - 用于简化调试和排错
    > pattern              ...preparing   - 模式，包括一部分可以通用化的设计模式
    > random               ...preparing   - 随机数生成器
    > time                 ...preparing   - 时间管理
