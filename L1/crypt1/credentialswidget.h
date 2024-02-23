#ifndef CREDENTIALSWIDGET_H
#define CREDENTIALSWIDGET_H

#include <QWidget>
#include <QPixmap>

namespace Ui {
class credentialsWidget;
}

class credentialsWidget : public QWidget
{
    Q_OBJECT

public:
    explicit credentialsWidget(const QString &url, QWidget *parent = nullptr);
    ~credentialsWidget();

private:
    Ui::credentialsWidget *ui;
    bool lockState;
    QPixmap *lockedImage;
    QPixmap *unlockedImage;
};

#endif // CREDENTIALSWIDGET_H
