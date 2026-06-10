#pragma once
#include <QDialog>
#include <QLineEdit>
#include <QCheckBox>

/**
 * 登录对话框
 * 支持读者登录和管理员登录
 */
class LoginDialog : public QDialog {
    Q_OBJECT

public:
    explicit LoginDialog(QWidget* parent = nullptr);

    bool isAdminLogin() const { return adminCheck->isChecked(); }
    QString getReaderId() const { return idEdit->text(); }
    QString getPassword() const { return pwdEdit->text(); }

private slots:
    void onLogin();
    void onRegister();

private:
    void setupUI();

    QLineEdit* idEdit = nullptr;
    QLineEdit* pwdEdit = nullptr;
    QCheckBox* adminCheck = nullptr;
};
