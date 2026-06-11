#pragma once
#include <QWidget>
#include <QLineEdit>
#include <QSpinBox>
#include <QTableWidget>
#include <QLabel>

class LibrarySystem;

class BookTab : public QWidget {
    Q_OBJECT
public:
    explicit BookTab(LibrarySystem* lib, QWidget* parent = nullptr);
    void refreshTable();

private slots:
    void onAdd();
    void onDelete();
    void onSearch();
    void onRangeQuery();

private:
    void setupUI();
    LibrarySystem* library;
    QLineEdit* isbnEdit;
    QLineEdit* callNumEdit;
    QLineEdit* titleEdit;
    QLineEdit* authorEdit;
    QLineEdit* pubEdit;
    QSpinBox* stockSpin;
    QLineEdit* searchEdit;
    QLineEdit* rangeLowEdit;
    QLineEdit* rangeHighEdit;
    QTableWidget* table;
    QLabel* statusLabel;
};
