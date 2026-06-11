#pragma once
#include <QWidget>
#include <QTableWidget>
#include <QSpinBox>
#include <QLineEdit>
#include <QDateTimeEdit>
#include <QLabel>

class LibrarySystem;

class SeatTab : public QWidget {
    Q_OBJECT
public:
    explicit SeatTab(LibrarySystem* lib, QWidget* parent = nullptr);
    void refreshGrid();

private slots:
    void onReserve();
    void onRelease();
    void onCellClicked(int row, int col);

private:
    void setupUI();
    LibrarySystem* library;
    QSpinBox* rowSpin, *colSpin;
    QLineEdit* readerEdit;
    QDateTimeEdit* startEdit, *endEdit;
    QTableWidget* grid;
    QLabel* statusLabel;
};
