#pragma once
#include <QWidget>
#include <QLineEdit>
#include <QSpinBox>
#include <QTableWidget>
#include <QLabel>
#include <QPushButton>
#include <QGroupBox>

class LibrarySystem;

class BookTab : public QWidget {
    Q_OBJECT
public:
    explicit BookTab(LibrarySystem* lib, QWidget* parent = nullptr);
    void refreshTable();
    void setAdminMode(bool admin);

private slots:
    void onAdd();
    void onDelete();
    void onEdit();
    void onSearch();
    void onRangeQuery();

private:
    void setupUI();
    LibrarySystem* library;
    QGroupBox* formBox;
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
    QPushButton* addBtn;
    QPushButton* editBtn;
    QString editingISBN; // 非空表示正在编辑该ISBN的图书
};
