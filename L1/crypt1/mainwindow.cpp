#include "mainwindow.h"
#include "credentialswidget.h"
#include "./ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    qDebug() << ReadJson();
    qDebug() << m_jsonarray;
    fillWidget("");

}

MainWindow::~MainWindow()
{
    delete ui;
}

bool MainWindow::ReadJson()
{
    QFile jsonFile("credentials.json");
    jsonFile.open(QFile::ReadOnly);
    if (!jsonFile.isOpen())
        return false;
    QByteArray hexEcryptedBytes = jsonFile.readAll();
    QByteArray encryptedBytes = QByteArray::fromHex(hexEcryptedBytes);
    QByteArray decryptedBytes;
    int ret_code = decryptFile(encryptedBytes, decryptedBytes);
    QJsonDocument jsonDoc = QJsonDocument::fromJson(jsonFile.readAll());
    m_jsonarray = jsonDoc.object()["credentials"].toArray();
    for (auto item: m_jsonarray) {
        qDebug() << item.toObject()["login"];
    }
    jsonFile.close();

    return true;
}

void MainWindow::fillWidget(QString searchParam)
{
    for (int i = 0; i < m_jsonarray.size(); i ++) {
        if (m_jsonarray[i].toObject()["url"].toString().contains(searchParam.trimmed())) {
            QListWidgetItem *newItem = new QListWidgetItem();
            credentialsWidget *itemWidget = new credentialsWidget(m_jsonarray[i].toObject()["url"].toString());
            newItem->setSizeHint(itemWidget->sizeHint());
            ui->credentialList->addItem(newItem);
            ui->credentialList->setItemWidget(newItem, itemWidget);
        }
    }
}

void MainWindow::on_search_editingFinished()
{
    ui->credentialList->clear();
    fillWidget(ui->search->text());
}

int MainWindow::decryptFile(
        const QByteArray &encryptedBytes,
        QByteArray &decryptedBytes
        )
{
    // https://cryptii.com/pipes/aes-encryption
    // key: 20 a8 d0 4b 86 0a d3 4a a4 d1 a5 94 7c a2 db 2a b4 01 62 cd 3b 07 8d 61 71 76 85 83 f7 0e 7c 81
    // iv:  0c e9 ad 97 cf e4 d7 bb 84 86 1b 14 4b 69 62 d4
    QByteArray hex = QByteArray::toHex(20a8d04b860ad34aa4d1a5947ca2db2ab40162cd3b078d6171768583f70e7c81);
    return 1;
}

