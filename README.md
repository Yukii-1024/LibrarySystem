# LibrarySystem — 高校图书馆智能管理系统

基于 **C++17 / Qt6** 的图书馆管理系统，是数据结构课程大作业。系统内部集成了 **8 种手写数据结构**（链表、栈、队列、二叉排序树、哈希表、图、堆、稀疏矩阵），实现图书管理、读者管理、借阅归还、座位预约、热门排行、图书推荐等功能，并配有实时的**数据结构可视化面板**。

---

## 目录

- [如何使用](#如何使用)
  - [环境要求](#环境要求)
  - [直接运行](#直接运行)
  - [从源码编译](#从源码编译)
  - [使用说明](#使用说明)
- [软件架构](#软件架构)
  - [整体架构](#整体架构)
  - [分层设计](#分层设计)
  - [数据结构映射](#数据结构映射)
  - [模块说明](#模块说明)
  - [类关系图](#类关系图)
- [项目结构](#项目结构)

---

## 如何使用

### 环境要求

| 组件 | 版本 | 说明 |
|------|------|------|
| Windows | 10 / 11 64 位 | 操作系统 |
| MinGW-w64 | GCC 11.5.0 MSVCRT | C++ 编译器（必须与 Qt 共用 MSVCRT） |
| Qt | 6.8.2 MinGW 64-bit | GUI 框架 |
| CMake | ≥ 3.16 | 构建系统 |
| Ninja | 任意版本 | 构建工具 |

> ⚠️ **重要**：Qt 6.8.2 的 MinGW 版本使用 MSVCRT 运行时，编译器也必须使用 MSVCRT 版本，否则会出现堆损坏（`0xc0000374`）导致程序崩溃。项目已配置为使用 `C:\Tool\pandacpp\mingw64`（GCC 11.5.0 MSVCRT）。

### 直接运行

如果你已经编译好，`build/` 目录下包含所有必要的 DLL 文件：

```powershell
# 添加 Qt bin 到 PATH（如果还没加）
$env:PATH = "C:\Qt\6.8.2\mingw_64\bin;$env:PATH"

# 运行
C:\Projects\LibrarySystem\build\LibrarySystem.exe
```

或者直接双击 `build\LibrarySystem.exe`（前提是 `build/` 目录下有 windeployqt 部署的所有 DLL）。

### 从源码编译

```powershell
# 方式一：使用项目提供的构建脚本（推荐）
.\build_msvcrt.bat

# 方式二：手动构建
$env:PATH = "C:\Qt\6.8.2\mingw_64\bin;C:\Tool\pandacpp\mingw64\bin;C:\Tool\mingw64\bin;$env:PATH"
cd build
cmake .. -G Ninja -DCMAKE_BUILD_TYPE=Debug `
    -DCMAKE_CXX_COMPILER="C:/Tool/pandacpp/mingw64/bin/g++.exe"
ninja

# 部署 Qt 运行时（使程序可在任意位置运行）
windeployqt --release --no-translations LibrarySystem.exe
```

### 使用说明

1. **启动程序**：双击 `LibrarySystem.exe`，出现登录界面
2. **登录**：输入学号/工号和密码（暂无预设账户，注册功能待实现，直接点登录即可进入）
3. **主界面**：
   - **左侧**：功能标签页（图书管理、读者管理、借阅/归还、座位预约、热门排行、图书推荐）
   - **右侧**：数据结构可视化面板，实时显示数据在 BST、哈希表、堆、图、栈、队列、座位矩阵中的状态
4. **菜单栏**：文件（保存/加载数据）、视图（切换可视化面板）、帮助

---

## 软件架构

### 整体架构

```
┌─────────────────────────────────────────────────────┐
│                    UI Layer (Qt6)                     │
│  MainWindow  LoginDialog  VisualPanel               │
├─────────────────────────────────────────────────────┤
│                  Core Layer (Business)                │
│                  LibrarySystem                       │
├─────────────────────────────────────────────────────┤
│              Data Structure Layer                     │
│  LinkedList  Stack  Queue  BST  HashTable  Graph     │
│  Heap  SparseMatrix                                  │
├─────────────────────────────────────────────────────┤
│                 Model Layer                           │
│  Book  Reader  BorrowRecord  Seat                   │
├─────────────────────────────────────────────────────┤
│              Persistence Layer                        │
│  DataPersistence  Logger                             │
└─────────────────────────────────────────────────────┘
```

### 分层设计

| 层 | 职责 | 依赖方向 |
|----|------|----------|
| **UI** | Qt Widgets 界面，处理用户交互和数据结构可视化 | → Core, Model |
| **Core** | 业务逻辑中心，整合所有数据结构，提供统一 API | → DataStruct, Model |
| **DataStruct** | 手写数据结构模板类，通用、可复用 | — |
| **Model** | 纯数据实体类，不含业务逻辑 | — |
| **Persistence** | 文件读写、日志记录 | → Model |

### 数据结构映射

每个数据结构在系统中都有明确的应用场景：

| 数据结构 | 应用场景 | 核心操作 |
|----------|----------|----------|
| **LinkedList** (双向链表) | 图书基础列表、读者列表、借阅记录表 | 增删改查、条件筛选 |
| **Stack** (顺序栈) | 操作历史 Undo/Redo | push / pop 操作记录 |
| **Queue** (链队列) | 热门图书预约排队（每本书一个队列） | FIFO 先到先服务 |
| **BST** (二叉排序树) | 按索书号快速检索图书、范围查询 | 插入、删除、查找、中序遍历 |
| **HashTable** (链地址法) | 读者登录验证（学号→读者）、按书名查找 | O(1) 查找、动态扩容 |
| **Graph** (邻接表) | 图书推荐系统（共同借阅关系） | 添加边、权重更新、TopN 推荐 |
| **MaxHeap** (最大堆) | 热门图书排行榜 Top K | 插入、堆化、更新键值 |
| **SparseMatrix** | 图书馆座位分布管理 | 坐标映射、预约/释放 |

### 模块说明

#### Model 层 (`model/`)

| 类 | 关键字段 | 说明 |
|----|----------|------|
| `Book` | isbn, callNumber, title, author, borrowCount | 图书实体，索书号用于 BST 索引，borrowCount 用于堆排行 |
| `Reader` | id, password, name, currentBorrow, isAdmin | 读者实体，id 用于哈希表索引 |
| `BorrowRecord` | recordId, readerId, bookISBN, returned | 借阅记录，存储在链表中 |
| `Seat` | row, col, readerId, startTime, endTime | 座位实体，存储在稀疏矩阵中 |

#### DataStruct 层 (`datastruct/`)

所有数据结构均为**模板类**，不依赖特定数据类型，均可独立复用：

- `LinkedList<T>` — 双向链表，支持头插、尾插、条件删除、遍历
- `Stack<T>` — 顺序栈，支持动态扩容、移动语义
- `Queue<T>` — 链队列，FIFO
- `BST<T, KeyFunc>` — 二叉排序树，预留 AVL 旋转操作，可升级为平衡树
- `HashTable<T, KeyFunc>` — 链地址哈希表（djb2 算法），负载因子 0.75 自动扩容
- `Graph` — 无向带权图（邻接表），支持顶点增删、边加权、推荐算法
- `MaxHeap<T, KeyFunc>` — 最大堆，支持建堆、插入、弹出、键值更新、TopN
- `SparseMatrix<T>` — 基于 `std::map` 的坐标映射，支持转为稠密矩阵

#### Core 层 (`core/`)

`LibrarySystem` 是整个系统的**业务中枢**，继承自 `QObject`，通过 Qt 信号将操作传递给可视化面板：

```
addBook()    → LinkedList.append + BST.insert + HashTable.insert + Graph.addVertex + Heap.insert
removeBook() → LinkedList.remove + BST.remove + HashTable.remove + Graph.removeVertex
borrowBook() → 检查库存 → 更新计数 → 生成借阅记录 → 更新堆 → 更新图 → Undo栈记录
returnBook() → 恢复库存 → 标记归还 → 更新堆 → Undo栈记录
undoOp()     → Stack.pop → 反向执行借/还操作
recommend()  → Graph.recommend (共同借阅关联度排序)
getHotBooks()→ Heap.getTopN
```

#### UI 层 (`ui/`)

| 类 | 继承 | 说明 |
|----|------|------|
| `MainWindow` | QMainWindow | 主窗口，左右分栏布局（功能 Tab + 可视化面板），菜单栏 |
| `LoginDialog` | QDialog | 登录弹窗，支持管理员/读者身份切换 |
| `VisualPanel` | QWidget | 数据结构可视化面板，7 个 QGraphicsView 分别展示不同数据结构的实时状态 |

### 类关系图

```
QApplication
    │
    └── MainWindow (QMainWindow)
            │
            ├── LibrarySystem (QObject) ── 业务中枢
            │       │
            │       ├── LinkedList<Book*>          ── 图书链表
            │       ├── LinkedList<Reader*>        ── 读者链表
            │       ├── LinkedList<BorrowRecord*>  ── 借阅记录链表
            │       ├── Stack<OperationRecord>     ── Undo栈
            │       ├── BST<Book>                  ── 索书号索引
            │       ├── HashTable<Reader>          ── 读者学号索引
            │       ├── HashTable<Book>            ── 书名索引
            │       ├── Graph                      ── 推荐图
            │       ├── MaxHeap<Book>              ── 热门排行堆
            │       ├── SparseMatrix<Seat>         ── 座位矩阵
            │       └── map<string, Queue<QString>>── 预约队列
            │
            ├── VisualPanel (QWidget) ── 可视化面板
            │       ├── QTabWidget (7个Tab)
            │       ├── QGraphicsScene × 7
            │       └── QGraphicsView × 7
            │
            └── LoginDialog (QDialog) ── 登录界面
```

---

## 项目结构

```
LibrarySystem/
├── main.cpp                    # 程序入口
├── CMakeLists.txt              # CMake 构建配置
├── build_msvcrt.bat            # 一键编译脚本
├── README.md
├── model/                      # 数据模型层
│   ├── Book.h
│   ├── Reader.h
│   ├── BorrowRecord.h
│   └── Seat.h
├── datastruct/                 # 手写数据结构层
│   ├── Common.h                # 通用枚举与状态码
│   ├── LinkedList.h            # 双向链表
│   ├── Stack.h                 # 顺序栈
│   ├── Queue.h                 # 链队列
│   ├── BST.h                   # 二叉排序树（可升级AVL）
│   ├── HashTable.h             # 哈希表（链地址法）
│   ├── Graph.h                 # 无向带权图（邻接表）
│   ├── Heap.h                  # 最大堆（优先队列）
│   └── SparseMatrix.h          # 稀疏矩阵
├── core/                       # 业务逻辑层
│   ├── LibrarySystem.h
│   └── LibrarySystem.cpp
├── ui/                         # 用户界面层
│   ├── MainWindow.h/.cpp       # 主窗口
│   ├── LoginDialog.h/.cpp      # 登录对话框
│   └── VisualPanel.h/.cpp      # 数据结构可视化面板
├── persistence/                # 持久化层
│   ├── DataPersistence.h/.cpp  # 数据存取
│   └── utils/
│       ├── Logger.h/.cpp       # 日志工具
└── build/                      # 构建输出（已忽略）
    └── LibrarySystem.exe
```
