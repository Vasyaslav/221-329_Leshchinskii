/********************************************************************************
** Form generated from reading UI file 'credentialswidget.ui'
**
** Created by: Qt User Interface Compiler version 6.6.2
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_CREDENTIALSWIDGET_H
#define UI_CREDENTIALSWIDGET_H

#include <QtCore/QVariant>
#include <QtWidgets/QApplication>
#include <QtWidgets/QGridLayout>
#include <QtWidgets/QLabel>
#include <QtWidgets/QLineEdit>
#include <QtWidgets/QSpacerItem>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_credentialsWidget
{
public:
    QGridLayout *gridLayout;
    QLabel *icon;
    QLabel *url;
    QLineEdit *login;
    QLineEdit *password;
    QSpacerItem *horizontalSpacer;

    void setupUi(QWidget *credentialsWidget)
    {
        if (credentialsWidget->objectName().isEmpty())
            credentialsWidget->setObjectName("credentialsWidget");
        credentialsWidget->resize(887, 78);
        QSizePolicy sizePolicy(QSizePolicy::Policy::Preferred, QSizePolicy::Policy::Preferred);
        sizePolicy.setHorizontalStretch(0);
        sizePolicy.setVerticalStretch(0);
        sizePolicy.setHeightForWidth(credentialsWidget->sizePolicy().hasHeightForWidth());
        credentialsWidget->setSizePolicy(sizePolicy);
        credentialsWidget->setMinimumSize(QSize(0, 0));
        gridLayout = new QGridLayout(credentialsWidget);
        gridLayout->setObjectName("gridLayout");
        icon = new QLabel(credentialsWidget);
        icon->setObjectName("icon");

        gridLayout->addWidget(icon, 0, 0, 2, 1);

        url = new QLabel(credentialsWidget);
        url->setObjectName("url");
        QFont font;
        font.setPointSize(12);
        url->setFont(font);

        gridLayout->addWidget(url, 0, 1, 1, 1);

        login = new QLineEdit(credentialsWidget);
        login->setObjectName("login");
        login->setEchoMode(QLineEdit::Password);

        gridLayout->addWidget(login, 1, 1, 1, 1);

        password = new QLineEdit(credentialsWidget);
        password->setObjectName("password");
        password->setEchoMode(QLineEdit::Password);

        gridLayout->addWidget(password, 1, 2, 1, 1);

        horizontalSpacer = new QSpacerItem(498, 17, QSizePolicy::Policy::Expanding, QSizePolicy::Policy::Minimum);

        gridLayout->addItem(horizontalSpacer, 1, 3, 1, 1);


        retranslateUi(credentialsWidget);

        QMetaObject::connectSlotsByName(credentialsWidget);
    } // setupUi

    void retranslateUi(QWidget *credentialsWidget)
    {
        credentialsWidget->setWindowTitle(QCoreApplication::translate("credentialsWidget", "Form", nullptr));
        icon->setText(QCoreApplication::translate("credentialsWidget", "TextLabel", nullptr));
        url->setText(QCoreApplication::translate("credentialsWidget", "<html><head/><body><p><span style=\" color:#0000ff;\">TextLabel</span></p></body></html>", nullptr));
        login->setText(QCoreApplication::translate("credentialsWidget", "......", nullptr));
        password->setText(QCoreApplication::translate("credentialsWidget", "......", nullptr));
    } // retranslateUi

};

namespace Ui {
    class credentialsWidget: public Ui_credentialsWidget {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_CREDENTIALSWIDGET_H
