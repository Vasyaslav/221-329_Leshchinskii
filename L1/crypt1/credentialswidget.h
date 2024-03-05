#ifndef CREDENTIALSWIDGET_H
#define CREDENTIALSWIDGET_H

#include <QWidget>
#include <QPixmap>
#include <QJsonObject>
#include <QJsonDocument>
#include <openssl/evp.h>

namespace Ui {
class credentialsWidget;
}

class credentialsWidget : public QWidget
{
    Q_OBJECT

public:
    explicit credentialsWidget(const QString &url, const int id, QWidget *parent = nullptr);
    ~credentialsWidget();

    enum FIELD {
        LOGIN, PASSWORD
    };

private slots:
    void on_loginCopy_clicked();

    void on_passwordCopy_clicked();


signals:
    void decryptLogOrPass(int id, FIELD field);

private:
    Ui::credentialsWidget *ui;
    int m_id = -1;
};

#endif // CREDENTIALSWIDGET_H
