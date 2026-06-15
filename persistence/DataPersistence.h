#pragma once
#include <QString>
#include <QSqlDatabase>

class LibrarySystem;

class DataPersistence {
public:
    /// Open (or create) the SQLite database at @p dbPath.
    /// Creates tables if they don't exist, and a default admin
    /// account when the readers table is completely empty.
    static bool initDatabase(const QString& dbPath = "library.db");

    /// Save all in-memory state into the database (overwrites).
    static bool saveAll(LibrarySystem* sys);

    /// Load from database into LibrarySystem (replaces in-memory state).
    static bool loadAll(LibrarySystem* sys);

    /// Close the current database connection.
    static void close();

    /// Return the current database path.
    static QString currentPath();

private:
    static void createTables();
    static void clearAll(LibrarySystem* sys);
    static QString dbPath_;
};
