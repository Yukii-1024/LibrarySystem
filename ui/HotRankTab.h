#pragma once
#include <QWidget>
#include <QTableWidget>
#include <QSpinBox>
#include <QLabel>

class LibrarySystem;

class HotRankTab : public QWidget {
    Q_OBJECT
public:
    explicit HotRankTab(LibrarySystem* lib, QWidget* parent = nullptr);
    void refreshRanking();

private:
    void setupUI();
    LibrarySystem* library;
    QSpinBox* topSpin;
    QTableWidget* table;
    QLabel* statusLabel;
};
