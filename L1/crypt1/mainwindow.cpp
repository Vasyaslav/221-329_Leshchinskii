#include "mainwindow.h"
#include "credentialswidget.h"
#include "./ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    // qDebug() << m_jsonarray;
    fillWidget("");

}

MainWindow::~MainWindow()
{
    delete ui;
}

bool MainWindow::ReadJson(const QByteArray & aes256_key)
{
    // Считывание из json файла
    QFile jsonFile("credentials_enc2.json");
    jsonFile.open(QFile::ReadOnly);
    if (!jsonFile.isOpen())
        return false;

    QByteArray hexEcryptedBytes = jsonFile.readAll();
    QByteArray encryptedBytes = QByteArray::fromHex(hexEcryptedBytes);
    QByteArray decryptedBytes;
    int ret_code = decryptFile(aes256_key, encryptedBytes, decryptedBytes);
    if (!ret_code)
        return false;

    QJsonParseError * p_jsonErr;
    QJsonDocument jsonDoc = QJsonDocument::fromJson(decryptedBytes, p_jsonErr);
    if (p_jsonErr->error != QJsonParseError::NoError)
        return false;
    m_jsonarray = jsonDoc.object()["credentials"].toArray(); /*
    for (auto item: m_jsonarray) {
        qDebug() << item.toObject()["login"];
    } */
    jsonFile.close();

    return true;
}

void MainWindow::fillWidget(QString searchParam)
{
    // Функция выводящая только те виджеты, которые содержат подстроку из строки поиска в названии сайта
    for (int i = 0; i < m_jsonarray.size(); i ++) {
        if (m_jsonarray[i].toObject()["url"].toString().contains(searchParam.trimmed())) {
            QListWidgetItem *newItem = new QListWidgetItem();
            credentialsWidget *itemWidget = new credentialsWidget(m_jsonarray[i].toObject()["url"].toString(),
                                                                  m_jsonarray[i].toObject()["logpass"].toString());
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
    const QByteArray & aes256_key,
    const QByteArray &encryptedBytes,
    QByteArray &decryptedBytes
    )
{
    // Функция для расшифровки файла
    // https://cryptii.com/pipes/aes-encryption
    // key: 12 1c d5 8c a2 f4 08 5f c7 b7 b7 4b dd e0 b2 00 2c 31 13 ff 7a d3 cc 5f 19 da 87 0d 73 6e 24 f8
    // iv:  d3 e3 40 51 e0 e6 e2 81 fe f4 ab bb 14 40 13 36
    // QByteArray key_hex("121cd58ca2f4085fc7b7b74bdde0b2002c3113ff7ad3cc5f19da870d736e24f8");
    // QByteArray key_ba = QByteArray::fromHex(key_hex);
    unsigned char key[32] = {0};
    memcpy(key, aes256_key.data(), 32);

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

void MainWindow::on_edtPin_returnPressed()
{
    //Использовать пин-код
    QByteArray hash = QCryptographicHash::hash(
        ui->edtPin->text().toUtf8(),
        QCryptographicHash::Sha256);
    qDebug() << "*** sha256 " << hash;
    // TODO расшифровать файл и проверить верность пин-кода
    // qDebug() << ReadJson(hash);
    if (ReadJson(hash)) { // Проверить что после расшифровки - норм json
        ui->stackedWidget->setCurrentIndex(0);
    } else { // А если не так то сказать что пин-код неверный
        ui->labelLogin->setText("Неверный пин-код");
        ui->labelLogin->setStyleSheet("color:red;");
    }
    ui->edtPin->setText(QString().fill('*', ui->edtPin->text().size()));
    ui->edtPin->clear();
    hash.setRawData(const_cast<char*>(QByteArray().fill('*', 32).data()), 32);
    hash = QByteArray();
}

