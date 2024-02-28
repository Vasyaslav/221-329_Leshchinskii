#include "credentialswidget.h"
#include "ui_credentialswidget.h"

credentialsWidget::credentialsWidget(const QString &url, const QString &logpass, QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::credentialsWidget)
{
    this->logpass = logpass;
    ui->setupUi(this);
    lockedImage = new QPixmap(":/icons/resources/padlock.png");
    unlockedImage = new QPixmap(":/icons/resources/opened_lock.png");
    lockState = true;
    ui->icon->setPixmap(lockedImage->scaled(49, 60, Qt::KeepAspectRatio));
    ui->url->setText(url);
    ui->icon->installEventFilter(this);
}

credentialsWidget::~credentialsWidget()
{
    delete ui;
}

bool credentialsWidget::eventFilter(QObject *object, QEvent *event)
{
    if (object == ui->icon && (event->type() == QEvent::MouseButtonPress)) {
        changeLockState();
    }

    return false;
}

void credentialsWidget::changeLockState()
{
    if (lockState) lockState = false;
    else lockState = true;
    if (lockState) {
        ui->icon->setPixmap(lockedImage->scaled(49, 60, Qt::KeepAspectRatio));
        QByteArray decBytes;
        decryptFile(this->logpass.toUtf8(), decBytes);
        qDebug() << "*** logpass" << decBytes;
    }
    else {
        ui->icon->setPixmap(unlockedImage->scaled(49, 60, Qt::KeepAspectRatio));
    }
}

int credentialsWidget::decryptFile(
    const QByteArray &encryptedBytes,
    QByteArray &decryptedBytes
    )
{
    // Функция для расшифровки файла
    // https://cryptii.com/pipes/aes-encryption
    // key: 12 1c d5 8c a2 f4 08 5f c7 b7 b7 4b dd e0 b2 00 2c 31 13 ff 7a d3 cc 5f 19 da 87 0d 73 6e 24 f8
    // iv:  d3 e3 40 51 e0 e6 e2 81 fe f4 ab bb 14 40 13 36
    QByteArray key_hex("121cd58ca2f4085fc7b7b74bdde0b2002c3113ff7ad3cc5f19da870d736e24f8");
    QByteArray key_ba = QByteArray::fromHex(key_hex);
    unsigned char key[32] = {0};
    memcpy(key, key_ba.data(), 32);

    QByteArray iv_hex("d3e34051e0e6e281fef4abbb14401336");
    QByteArray iv_ba = QByteArray::fromHex(iv_hex);
    unsigned char iv[16] = {0};
    memcpy(iv, iv_ba.data(), 16);

    EVP_CIPHER_CTX *ctx;
    ctx = EVP_CIPHER_CTX_new();
    if (!EVP_DecryptInit_ex2(ctx, EVP_aes_256_cbc(), key, iv, NULL)) {
        qDebug() << "*** DecryptInit Error ";
        EVP_CIPHER_CTX_free(ctx);
        return 0;
    }

    const int buffer_len = 256;
    unsigned char enc_buffer[buffer_len] = {0};
    unsigned char dec_buffer[buffer_len] = {0};
    int enc_len, dec_len;
    QDataStream enc_stream(encryptedBytes);
    QDataStream dec_stream(&decryptedBytes, QIODevice::ReadWrite);
    enc_len = enc_stream.readRawData(reinterpret_cast<char*>(enc_buffer), buffer_len);
    while (enc_len > 0) {
        if (!EVP_DecryptUpdate(ctx, dec_buffer, &dec_len, enc_buffer, enc_len)) {
            qDebug() << "*** DecryptUpdate Error ";
            EVP_CIPHER_CTX_free(ctx);
            return 0;
        }
        dec_stream.writeRawData(reinterpret_cast<char*>(dec_buffer), dec_len);
        enc_len = enc_stream.readRawData(reinterpret_cast<char*>(enc_buffer), buffer_len);
    }
    if (!EVP_DecryptFinal_ex(ctx, dec_buffer, &dec_len)) {
        EVP_CIPHER_CTX_free(ctx);
        return 0;
    }
    dec_stream.writeRawData(reinterpret_cast<char*>(dec_buffer), dec_len);
    EVP_CIPHER_CTX_free(ctx);
    return 1;
}
