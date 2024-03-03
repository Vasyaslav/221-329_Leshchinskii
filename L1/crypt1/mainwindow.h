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
#include <QCryptographicHash>

QT_BEGIN_NAMESPACE
namespace Ui {
class MainWindow;
}
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    bool ReadJson(const QByteArray & aes256_key);
    void fillWidget(QString);
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void on_search_editingFinished();

    void on_edtPin_returnPressed();

private:
    Ui::MainWindow *ui;
    QJsonArray m_jsonarray;
<<<<<<< HEAD
    int decryptFile(const QByteArray & aes256_key,
                    const QByteArray&,
                    QByteArray&);
=======
    int decryptFile(const QByteArray &, QByteArray&);
>>>>>>> 81f56eaa79414baa99c0efea131872945b281f93
};
#endif // MAINWINDOW_H
