cmake_minimum_required(VERSION 3.16)
project(LibrarySystem VERSION 1.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

# Windows UTF-8 支持（仅 MSVC）
if(WIN32 AND MSVC)
    add_compile_options(/utf-8)
endif()

# 查找 Qt（优先 Qt6，回退 Qt5）
find_package(Qt6 QUIET COMPONENTS Core Widgets Gui Sql)
if(NOT Qt6_FOUND)
    find_package(Qt5 REQUIRED COMPONENTS Core Widgets Gui)
    set(QT_VERSION_MAJOR 5)
else()
    set(QT_VERSION_MAJOR 6)
endif()

set(SOURCES
    main.cpp
    ui/MainWindow.cpp
    ui/LoginDialog.cpp
    ui/VisualPanel.cpp
    ui/BookTab.cpp
    ui/ReaderTab.cpp
    ui/BorrowTab.cpp
    ui/SeatTab.cpp
    ui/HotRankTab.cpp
    ui/RecommendTab.cpp
    core/LibrarySystem.cpp
    persistence/DataPersistence.cpp
    utils/Logger.cpp
)

set(HEADERS
    model/Book.h
    model/Reader.h
    model/BorrowRecord.h
    model/Seat.h
    datastruct/Common.h
    datastruct/LinkedList.h
    datastruct/Stack.h
    datastruct/Queue.h
    datastruct/BST.h
    datastruct/HashTable.h
    datastruct/Graph.h
    datastruct/Heap.h
    datastruct/SparseMatrix.h
    ui/MainWindow.h
    ui/LoginDialog.h
    ui/VisualPanel.h
    ui/BookTab.h
    ui/ReaderTab.h
    ui/BorrowTab.h
    ui/SeatTab.h
    ui/HotRankTab.h
    ui/RecommendTab.h
    core/LibrarySystem.h
    persistence/DataPersistence.h
    utils/Logger.h
)

add_executable(${PROJECT_NAME} ${SOURCES} ${HEADERS})

target_include_directories(${PROJECT_NAME} PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_CURRENT_SOURCE_DIR}/model
    ${CMAKE_CURRENT_SOURCE_DIR}/datastruct
    ${CMAKE_CURRENT_SOURCE_DIR}/core
    ${CMAKE_CURRENT_SOURCE_DIR}/ui
    ${CMAKE_CURRENT_SOURCE_DIR}/visualization
    ${CMAKE_CURRENT_SOURCE_DIR}/persistence
    ${CMAKE_CURRENT_SOURCE_DIR}/utils
)

target_link_libraries(${PROJECT_NAME} PRIVATE
    Qt::Core
    Qt::Widgets
    Qt::Gui
    Qt::Sql
)

# Windows 控制台输出（调试用）
if(WIN32)
    set_property(TARGET ${PROJECT_NAME} PROPERTY WIN32_EXECUTABLE FALSE)
endif()
