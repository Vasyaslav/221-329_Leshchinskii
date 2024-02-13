#include "mainwindow.h"
#include "./ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    ReadJson();

    //ui->listView->setModel(m_cridentials);
}

MainWindow::~MainWindow()
{
    delete ui;
}

/* Функция считывает данные из json файла в QList */
bool MainWindow::ReadJson()
{
    QFile jsonFile("credentials.json");
    jsonFile.open(QFile::ReadOnly);
    if (!jsonFile.isOpen())
        return false;

    QJsonDocument jsonDoc = QJsonDocument::fromJson(jsonFile.readAll());
    QJsonObject rootObject = jsonDoc.object();
    QJsonArray records = rootObject["credentials"].toArray();
    for (auto item: records) {
        qDebug() << item;
    }

    jsonFile.close();

    return true;
}
