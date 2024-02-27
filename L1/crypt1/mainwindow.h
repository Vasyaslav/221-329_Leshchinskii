#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QFile>
#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonValue>
#include <QDebug>
#include <openssl/evp.h>

QT_BEGIN_NAMESPACE
namespace Ui {
class MainWindow;
}
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    bool ReadJson();
    void fillWidget(QString);
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void on_search_editingFinished();

private:
    Ui::MainWindow *ui;
    QJsonArray m_jsonarray;
    int decryptFile(const QByteArray&, QByteArray&);
};
#endif // MAINWINDOW_H
