#pragma once
#include <QString>
#include "core/LibrarySystem.h"

/**
 * 数据持久化类
 * 负责 JSON/文本文件的读写
 */
class DataPersistence {
public:
    static bool saveAll(LibrarySystem* sys, const QString& filePath);
    static bool loadAll(LibrarySystem* sys, const QString& filePath);
};
