#pragma once
#include <QWidget>
#include <QTableWidget>
#include <QLineEdit>
#include <QSpinBox>
#include <QLabel>

class LibrarySystem;

class RecommendTab : public QWidget {
    Q_OBJECT
public:
    explicit RecommendTab(LibrarySystem* lib, QWidget* parent = nullptr);

private slots:
    void onRecommend();

private:
    void setupUI();
    LibrarySystem* library;
    QLineEdit* isbnEdit;
    QSpinBox* topSpin;
    QTableWidget* table;
    QLabel* statusLabel;
};
