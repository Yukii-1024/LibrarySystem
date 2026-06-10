#include "Logger.h"

Logger& Logger::instance()
{
    static Logger inst;
    return inst;
}

Logger::Logger()
{
    file.setFileName("library_log.txt");
    file.open(QIODevice::WriteOnly | QIODevice::Append | QIODevice::Text);
}

Logger::~Logger()
{
    if (file.isOpen()) file.close();
}

void Logger::log(const QString& level, const QString& message)
{
    QMutexLocker locker(&mutex);
    if (!file.isOpen()) return;
    QTextStream out(&file);
    QString timestamp = QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss");
    out << "[" << timestamp << "] [" << level << "] " << message << "\n";
    out.flush();
}
