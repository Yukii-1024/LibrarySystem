# LibrarySystem — 高校图书馆智能管理系统

基于 **C++17 / Qt6** 的图书馆管理系统，是数据结构课程大作业。系统内部集成了 **8 种手写数据结构**（链表、栈、队列、二叉排序树、哈希表、图、堆、稀疏矩阵），实现图书管理、读者管理、借阅归还、座位预约、热门排行、图书推荐等功能，并配有实时的**数据结构可视化面板**、**消息队列面板**和**操作日志面板**。

---

## 快速开始

### 环境要求

| 组件 | 版本 | 说明 |
|------|------|------|
| Windows | 10 / 11 64 位 | 操作系统 |
| MinGW-w64 | GCC 11.5.0 | C++ 编译器 |
| Qt | 6.8.2 MinGW 64-bit | GUI 框架 |
| CMake | ≥ 3.16 | 构建系统 |
| Ninja | 任意版本 | 构建工具 |

### 直接运行

```powershell
$env:PATH = "C:\Qt\6.8.2\mingw_64\bin;$env:PATH"
C:\Projects\LibrarySystem\build\LibrarySystem.exe
```

### 从源码编译

```powershell
$env:PATH = "C:\Tool\pandacpp\mingw64\bin;C:\Tool\mingw64\bin;C:\Qt\6.8.2\mingw_64\bin;" + $env:PATH
cd build
cmake .. -G Ninja
cmake --build .
```

---

## 用户指南

### 登录与权限

| 账号 | 密码 | 角色 | 权限 |
|------|------|------|------|
| `admin` | `admin123` | 管理员 | 增删改图书、增删读者、管理所有座位 |
| `2024001` | `123456` | 读者 | 借阅/归还、预约座位（同一时段仅一个） |
| `2024002` ~ `2024004` | `123456` | 读者 | 同上 |

- 关闭登录窗口程序自动退出
- 管理员权限由账号自动判断，无需手动勾选
- 新用户可点击「注册新读者」自助注册

### 功能标签页（左侧 6 个）

| 标签 | 管理员 | 普通读者 |
|:---|:---|:---|
| 图书管理 | 增删改查全部可见 | 仅搜索框 + 图书列表（只读） |
| 读者管理 | 可见 | **完全隐藏** |
| 借阅/归还 | 可见 | 可见（借书/还书/撤销） |
| 图书推荐 | 可见 | 可见（输入 ISBN 获取推荐） |
| 热门排行 | 可见 | 可见（Top N 热门图书） |
| 座位预约 | 可预约多个座位 | 同 时段仅一个座位，释放一键自动 |

### 座位预约

- 自动使用登录者学号，无需手动输入
- 网格颜色：🟢 绿色=空闲　🔴 红色=他人占用　🟠 橙色=你的座位
- 普通读者：预约后点「释放座位」自动找到并释放唯一座位
- 管理员：可管理所有座位

### 视图面板（默认关闭，从 "视图" 菜单开启）

| 面板 | 内容 |
|:---|:---|
| 数据结构可视化 | BST / 哈希表 / 堆 / 图 / 栈 / 队列 / 稀疏矩阵 的实时状态 |
| 消息队列 | FIFO 消息队列处理历史，展示并发安全机制 |
| 操作日志 | 所有重要操作记录，支持「全部」/「仅管理员」筛选 |

### 数据持久化

- 文件 → 保存修改：写入 SQLite (`library.db`)
- 文件 → 放弃修改：回滚到上次保存
- 关闭窗口时如有未保存修改会提示

---

## 软件架构

```
QApplication
  └── MainWindow
        ├── QTabWidget (6个功能标签)
        │     ├── BookTab        ←→ LibrarySystem
        │     ├── ReaderTab      ←→ (仅管理员可见)
        │     ├── BorrowTab      ←→
        │     ├── RecommendTab   ←→
        │     ├── HotRankTab     ←→
        │     └── SeatTab        ←→
        │
        └── QSplitter (右侧视图面板，默认隐藏)
              ├── VisualPanel      — 数据结构可视化
              ├── MessageQueueTab  — 消息队列
              └── LogPanel         — 操作日志

LibrarySystem (核心业务层)
  ├── 8 种数据结构实例
  ├── MessageQueueProcessor (并发安全)
  ├── OperationLogger (操作审计)
  └── DataPersistence (SQLite)
```

### 数据结构与业务映射

| 数据结构 | 存储形态 | 应用场景 |
|:---|:---|:---|
| **LinkedList** | 单向链表 | 图书/读者/借阅记录的主存储 |
| **Stack** | 链栈 | 操作撤销 (Undo) |
| **Queue** | 链队列 | 预约排队、消息队列 |
| **BST (AVL)** | 平衡二叉树 | 索书号索引与范围查询 |
| **HashTable** | 链地址法 + djb2 | 学号索引、ISBN/书名索引 |
| **Graph** | 邻接表 | 共同借阅推荐网络 |
| **MaxHeap** | 完全二叉树数组 | 热门图书 Top K 排行 |
| **SparseMatrix** | 三元组压缩 | 座位分布管理 |

---

## 项目结构

```
LibrarySystem/
├── main.cpp
├── CMakeLists.txt
├── icon.png                     # 应用图标
├── README.md
├── model/
│   ├── Book.h                   # 图书实体
│   ├── Reader.h                 # 读者实体
│   ├── BorrowRecord.h           # 借阅记录
│   └── Seat.h                   # 座位实体（Free/Occupied）
├── datastruct/
│   ├── Common.h                 # 状态码与枚举
│   ├── LinkedList.h             # 单向链表
│   ├── Stack.h                  # 链栈
│   ├── Queue.h                  # 链队列 (FIFO)
│   ├── BST.h                    # AVL 平衡树
│   ├── HashTable.h              # 哈希表 (djb2 + 链地址)
│   ├── Graph.h                  # 无向带权图 (邻接表)
│   ├── Heap.h                   # 最大堆
│   └── SparseMatrix.h           # 稀疏矩阵 (三元组)
├── core/
│   ├── LibrarySystem.h/.cpp     # 业务中枢
│   ├── MessageQueue.h/.cpp      # 消息队列处理器
│   ├── OperationLog.h/.cpp      # 操作日志记录
│   └── SeedData.h/.cpp          # 初始数据 (30本书)
├── ui/
│   ├── MainWindow.h/.cpp        # 主窗口
│   ├── LoginDialog.h/.cpp       # 登录对话框
│   ├── BookTab.h/.cpp           # 图书管理标签
│   ├── ReaderTab.h/.cpp         # 读者管理标签
│   ├── BorrowTab.h/.cpp         # 借阅/归还标签
│   ├── SeatTab.h/.cpp           # 座位预约标签
│   ├── HotRankTab.h/.cpp        # 热门排行标签
│   ├── RecommendTab.h/.cpp      # 图书推荐标签
│   ├── VisualPanel.h/.cpp       # 数据结构可视化面板
│   ├── MessageQueueTab.h/.cpp   # 消息队列面板
│   └── LogPanel.h/.cpp          # 操作日志面板
├── persistence/
│   └── DataPersistence.h/.cpp   # SQLite 持久化
├── utils/
│   └── Logger.h/.cpp            # 调试日志
└── build/                       # 构建输出
    └── LibrarySystem.exe
```
