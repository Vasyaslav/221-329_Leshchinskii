#ifndef CREDENTIALSWIDGET_H
#define CREDENTIALSWIDGET_H

#include <QWidget>
#include <QPixmap>
#include <QJsonObject>
#include <QJsonValue>
#include <QJsonDocument>
#include <openssl/evp.h>

namespace Ui {
class credentialsWidget;
}

class credentialsWidget : public QWidget
{
    Q_OBJECT

public:
    explicit credentialsWidget(const QString &url, const QString &logpass, QWidget *parent = nullptr);
    ~credentialsWidget();

private:
    QString logpass;
    Ui::credentialsWidget *ui;
    bool lockState;
    QPixmap *lockedImage;
    QPixmap *unlockedImage;
    bool eventFilter(QObject *object, QEvent *event);
    void changeLockState();
    int decryptFile(const QByteArray &encryptedBytes, QByteArray &decryptedBytes);
};

#endif // CREDENTIALSWIDGET_H
