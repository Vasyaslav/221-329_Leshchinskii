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
#include <QClipboard>

#include "credentialswidget.h"

QT_BEGIN_NAMESPACE
namespace Ui {
class MainWindow;
}
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    bool decryptLoginPassword(QJsonObject &loginPassword, const QByteArray & aes256_key);
    bool ReadJson(const QByteArray & aes256_key);
    void fillWidget(QString);
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void on_search_editingFinished();

    void on_edtPin_returnPressed();

    void decryptLogOrPass(int id, credentialsWidget::FIELD field);

private:
    Ui::MainWindow *ui;
    QJsonArray m_jsonarray;
    int m_id = -1;
    credentialsWidget::FIELD m_field;
    int decryptByteArray(const QByteArray & aes256_key,
                    const QByteArray&,
                    QByteArray&);
};
#endif // MAINWINDOW_H
