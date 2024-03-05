#include "credentialswidget.h"
#include "ui_credentialswidget.h"

credentialsWidget::credentialsWidget(const QString &url, const int id, QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::credentialsWidget)
{
    ui->setupUi(this);
    ui->url->setText(url);
    this->m_id = id;
}

credentialsWidget::~credentialsWidget()
{
    delete ui;
}

void credentialsWidget::on_loginCopy_clicked()
{
    qDebug() << "*** Достаём логин из индекса - " << this->m_id;
    emit(decryptLogOrPass(this->m_id, FIELD::LOGIN));
}


void credentialsWidget::on_passwordCopy_clicked()
{
    qDebug() << "*** Достаём пароль из индекса - " << this->m_id;
    emit(decryptLogOrPass(this->m_id, FIELD::PASSWORD));
}

