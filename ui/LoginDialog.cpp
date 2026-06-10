#include "LoginDialog.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QPushButton>
#include <QMessageBox>

LoginDialog::LoginDialog(QWidget* parent)
    : QDialog(parent)
{
    setWindowTitle(QString::fromUtf8("登录 - 高校图书馆智能管理系统"));
    setFixedSize(400, 250);
    setupUI();
}

void LoginDialog::setupUI()
{
    auto* mainLayout = new QVBoxLayout(this);

    auto* title = new QLabel(QString::fromUtf8("\U0001F510 用户登录"), this);
    title->setStyleSheet("font-size: 18px; font-weight: bold;");
    title->setAlignment(Qt::AlignCenter);
    mainLayout->addWidget(title);

    auto* idLayout = new QHBoxLayout();
    idLayout->addWidget(new QLabel(QString::fromUtf8("学号/工号:"), this));
    idEdit = new QLineEdit(this);
    idEdit->setPlaceholderText(QString::fromUtf8("请输入学号或工号"));
    idLayout->addWidget(idEdit);
    mainLayout->addLayout(idLayout);

    auto* pwdLayout = new QHBoxLayout();
    pwdLayout->addWidget(new QLabel(QString::fromUtf8("密码:      "), this));
    pwdEdit = new QLineEdit(this);
    pwdEdit->setEchoMode(QLineEdit::Password);
    pwdEdit->setPlaceholderText(QString::fromUtf8("请输入密码"));
    pwdLayout->addWidget(pwdEdit);
    mainLayout->addLayout(pwdLayout);

    adminCheck = new QCheckBox(QString::fromUtf8("以管理员身份登录"), this);
    mainLayout->addWidget(adminCheck);

    auto* btnLayout = new QHBoxLayout();
    auto* loginBtn = new QPushButton(QString::fromUtf8("登录"), this);
    auto* regBtn = new QPushButton(QString::fromUtf8("注册新读者"), this);
    btnLayout->addWidget(loginBtn);
    btnLayout->addWidget(regBtn);
    mainLayout->addLayout(btnLayout);

    connect(loginBtn, &QPushButton::clicked, this, &LoginDialog::onLogin);
    connect(regBtn, &QPushButton::clicked, this, &LoginDialog::onRegister);
}

void LoginDialog::onLogin()
{
    if (idEdit->text().isEmpty() || pwdEdit->text().isEmpty()) {
        QMessageBox::warning(this,
            QString::fromUtf8("输入错误"),
            QString::fromUtf8("请填写学号/工号和密码"));
        return;
    }
    accept();
}

void LoginDialog::onRegister()
{
    QMessageBox::information(this,
        QString::fromUtf8("注册"),
        QString::fromUtf8("注册功能将在后续实现"));
}
