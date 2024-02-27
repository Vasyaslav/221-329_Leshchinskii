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
    // Считывание из json файла
    QFile jsonFile("credentials_enc.json");
    jsonFile.open(QFile::ReadOnly);
    if (!jsonFile.isOpen())
        return false;

    QByteArray hexEcryptedBytes = jsonFile.readAll();
    QByteArray encryptedBytes = QByteArray::fromHex(hexEcryptedBytes);
    QByteArray decryptedBytes;
    int ret_code = decryptFile(encryptedBytes, decryptedBytes);
    if (!ret_code)
        return false;

    QJsonDocument jsonDoc = QJsonDocument::fromJson(decryptedBytes);
    qDebug() << decryptedBytes;
    m_jsonarray = jsonDoc.object()["credentials"].toArray();
    for (auto item: m_jsonarray) {
        qDebug() << item.toObject()["login"];
    }
    jsonFile.close();

    return true;
}

void MainWindow::fillWidget(QString searchParam)
{
    // Функция выводящая только те виджеты, которые содержат подстроку из строки поиска в названии сайта
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
    // Функция вызывающаяся когда пользователь нажимает "enter" в строке поиска
    ui->credentialList->clear();
    fillWidget(ui->search->text());
}

int MainWindow::decryptFile(
    const QByteArray &encryptedBytes,
    QByteArray &decryptedBytes
    )
{
    // Функция для расшифровки файла
    // https://cryptii.com/pipes/aes-encryption
    // key: 20 a8 d0 4b 86 0a d3 4a a4 d1 a5 94 7c a2 db 2a b4 01 62 cd 3b 07 8d 61 71 76 85 83 f7 0e 7c 81
    // iv:  0c e9 ad 97 cf e4 d7 bb 84 86 1b 14 4b 69 62 d4
    QByteArray key_hex("20a8d04b860ad34aa4d1a5947ca2db2ab40162cd3b078d6171768583f70e7c81");
    QByteArray key_ba = QByteArray::fromHex(key_hex);
    unsigned char key[32] = {0};
    memcpy(key, key_ba.data(), 32);

    QByteArray iv_hex("0ce9ad97cfe4d7bb84861b144b6962d4");
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
        dec_stream << QByteArray(reinterpret_cast<char*>(dec_buffer), dec_len);
        enc_len = enc_stream.readRawData(reinterpret_cast<char*>(enc_buffer), buffer_len);
    }
    if (!EVP_DecryptFinal_ex(ctx, dec_buffer, &dec_len)) {
        EVP_CIPHER_CTX_free(ctx);
        return 0;
    }
    EVP_CIPHER_CTX_free(ctx);
    return 1;
}
