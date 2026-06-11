#include "LoginDialog.h"
#include "core/LibrarySystem.h"
#include "model/Reader.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QFormLayout>
#include <QLabel>
#include <QPushButton>
#include <QMessageBox>
#include <QDialogButtonBox>

LoginDialog::LoginDialog(LibrarySystem* lib, QWidget* parent)
    : QDialog(parent)
    , library(lib)
{
    setWindowTitle(QString::fromUtf8("登录 - 高校图书馆智能管理系统"));
    setFixedSize(420, 280);
    setupUI();
}

void LoginDialog::setupUI()
{
    auto* mainLayout = new QVBoxLayout(this);
    mainLayout->setSpacing(12);

    auto* title = new QLabel(QString::fromUtf8("\U0001F510 用户登录"), this);
    title->setStyleSheet("font-size: 18px; font-weight: bold;");
    title->setAlignment(Qt::AlignCenter);
    mainLayout->addWidget(title);

    auto* formLayout = new QFormLayout();
    formLayout->setSpacing(8);

    idEdit = new QLineEdit(this);
    idEdit->setPlaceholderText(QString::fromUtf8("请输入学号或工号"));
    formLayout->addRow(QString::fromUtf8("学号/工号:"), idEdit);

    pwdEdit = new QLineEdit(this);
    pwdEdit->setEchoMode(QLineEdit::Password);
    pwdEdit->setPlaceholderText(QString::fromUtf8("请输入密码"));
    formLayout->addRow(QString::fromUtf8("密码:"), pwdEdit);

    mainLayout->addLayout(formLayout);

    adminCheck = new QCheckBox(QString::fromUtf8("以管理员身份登录"), this);
    mainLayout->addWidget(adminCheck);

    auto* btnLayout = new QHBoxLayout();
    auto* loginBtn = new QPushButton(QString::fromUtf8("登录"), this);
    loginBtn->setDefault(true);
    auto* regBtn = new QPushButton(QString::fromUtf8("注册新读者"), this);
    btnLayout->addWidget(loginBtn);
    btnLayout->addWidget(regBtn);
    mainLayout->addLayout(btnLayout);

    connect(loginBtn, &QPushButton::clicked, this, &LoginDialog::onLogin);
    connect(regBtn, &QPushButton::clicked, this, &LoginDialog::onRegister);
}

void LoginDialog::onLogin()
{
    QString id = idEdit->text().trimmed();
    QString pwd = pwdEdit->text();

    if (id.isEmpty() || pwd.isEmpty()) {
        QMessageBox::warning(this,
            QString::fromUtf8("输入错误"),
            QString::fromUtf8("请填写学号/工号和密码"));
        return;
    }

    if (!library) {
        // No backend, accept as before (fallback)
        accept();
        return;
    }

    bool readerIsAdmin = false;
    bool valid = library->verifyLogin(id, pwd, readerIsAdmin);

    if (!valid) {
        QMessageBox::warning(this,
            QString::fromUtf8("登录失败"),
            QString::fromUtf8("学号/工号或密码错误，请重试"));
        return;
    }

    if (adminCheck->isChecked() && !readerIsAdmin) {
        QMessageBox::warning(this,
            QString::fromUtf8("权限不足"),
            QString::fromUtf8("该账号不是管理员，请取消管理员选项后重试"));
        return;
    }

    accept();
}

void LoginDialog::onRegister()
{
    // Registration dialog
    QDialog dlg(this);
    dlg.setWindowTitle(QString::fromUtf8("注册新读者"));
    dlg.setFixedSize(360, 250);

    auto* layout = new QVBoxLayout(&dlg);
    auto* form = new QFormLayout();

    auto* regIdEdit = new QLineEdit(&dlg);
    regIdEdit->setPlaceholderText(QString::fromUtf8("学号/工号"));
    form->addRow(QString::fromUtf8("学号/工号:"), regIdEdit);

    auto* regPwdEdit = new QLineEdit(&dlg);
    regPwdEdit->setEchoMode(QLineEdit::Password);
    regPwdEdit->setPlaceholderText(QString::fromUtf8("设置密码"));
    form->addRow(QString::fromUtf8("密码:"), regPwdEdit);

    auto* regNameEdit = new QLineEdit(&dlg);
    regNameEdit->setPlaceholderText(QString::fromUtf8("真实姓名"));
    form->addRow(QString::fromUtf8("姓名:"), regNameEdit);

    auto* regDeptEdit = new QLineEdit(&dlg);
    regDeptEdit->setPlaceholderText(QString::fromUtf8("院系/部门"));
    form->addRow(QString::fromUtf8("院系:"), regDeptEdit);

    layout->addLayout(form);

    auto* buttons = new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel, &dlg);
    buttons->button(QDialogButtonBox::Ok)->setText(QString::fromUtf8("注册"));
    buttons->button(QDialogButtonBox::Cancel)->setText(QString::fromUtf8("取消"));
    layout->addWidget(buttons);

    connect(buttons, &QDialogButtonBox::accepted, &dlg, [&]() {
        QString id = regIdEdit->text().trimmed();
        QString pwd = regPwdEdit->text();
        QString name = regNameEdit->text().trimmed();
        QString dept = regDeptEdit->text().trimmed();

        if (id.isEmpty() || pwd.isEmpty() || name.isEmpty()) {
            QMessageBox::warning(&dlg,
                QString::fromUtf8("输入错误"),
                QString::fromUtf8("请填写学号、密码和姓名"));
            return;
        }

        if (library && library->findReader(id)) {
            QMessageBox::warning(&dlg,
                QString::fromUtf8("注册失败"),
                QString::fromUtf8("该学号/工号已被注册"));
            return;
        }

        if (library) {
            auto* reader = new Reader(id.toStdString(), pwd.toStdString(),
                                      name.toStdString(), dept.toStdString(), false);
            library->addReader(reader);
        }

        // Auto-fill login form
        idEdit->setText(id);
        pwdEdit->setText(pwd);

        dlg.accept();
        QMessageBox::information(this,
            QString::fromUtf8("注册成功"),
            QString::fromUtf8("读者 '%1' 注册成功，请登录").arg(name));
    });

    connect(buttons, &QDialogButtonBox::rejected, &dlg, &QDialog::reject);

    dlg.exec();
}
