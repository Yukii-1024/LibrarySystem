#pragma once
#include <QString>
#include <QFile>
#include <QTextStream>
#include <QMutex>
#include <QDateTime>

/**
 * 简单日志工具（单例模式）
 * 记录关键操作到日志文件
 */
class Logger {
public:
    static Logger& instance();

    void log(const QString& level, const QString& message);
    void info(const QString& msg) { log("INFO", msg); }
    void warn(const QString& msg) { log("WARN", msg); }
    void error(const QString& msg) { log("ERROR", msg); }

private:
    Logger();
    ~Logger();
    QMutex mutex;
    QFile file;
};
