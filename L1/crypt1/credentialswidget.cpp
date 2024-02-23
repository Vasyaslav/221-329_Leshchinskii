#include "credentialswidget.h"
#include "ui_credentialswidget.h"

credentialsWidget::credentialsWidget(const QString &url, QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::credentialsWidget)
{
    ui->setupUi(this);
    lockedImage = new QPixmap(":/icons/resources/padlock.png");
    unlockedImage = new QPixmap(":/icons/resources/opened_lock.png");
    lockState = false;
    ui->icon->setPixmap(lockedImage->scaled(49, 60, Qt::KeepAspectRatio));
    ui->url->setText(url);
}

credentialsWidget::~credentialsWidget()
{
    delete ui;
}
