#pragma once
#include <QDialog>
#include <QLineEdit>
#include <QCheckBox>

class LibrarySystem;

class LoginDialog : public QDialog {
    Q_OBJECT

public:
    explicit LoginDialog(LibrarySystem* lib, QWidget* parent = nullptr);

    bool isAdminLogin() const { return adminCheck->isChecked(); }
    QString getReaderId() const { return idEdit->text(); }
    QString getPassword() const { return pwdEdit->text(); }

private slots:
    void onLogin();
    void onRegister();

private:
    void setupUI();

    LibrarySystem* library = nullptr;
    QLineEdit* idEdit = nullptr;
    QLineEdit* pwdEdit = nullptr;
    QCheckBox* adminCheck = nullptr;
};
