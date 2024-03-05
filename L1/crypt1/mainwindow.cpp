#include "mainwindow.h"
#include "./ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    // qDebug() << m_jsonarray;
}

MainWindow::~MainWindow()
{
    delete ui;
}

bool MainWindow::decryptLoginPassword(QJsonObject &loginPassword, const QByteArray &aes256_key)
{
    // Функция для расшифровки конкретного логина И пароля
    QJsonParseError p_jsonErr;
    QByteArray encrString = QByteArray::fromHex(m_jsonarray[m_id].toObject()["logpass"].toString().toUtf8());
    QByteArray decrString;
    decryptByteArray(aes256_key, encrString, decrString);
    QJsonDocument jsonDoc = QJsonDocument::fromJson(decrString, &p_jsonErr);
    if (p_jsonErr.error != QJsonParseError::NoError) {
        return false;
    }
    loginPassword = jsonDoc.object();
    return true;
}

bool MainWindow::ReadJson(const QByteArray & aes256_key)
{
    // Функция для заполнения m_jsonarray расшифрованным .json файлом
    QFile jsonFile("credentials_enc2.json");
    jsonFile.open(QFile::ReadOnly);
    if (!jsonFile.isOpen())
        return false;

    QByteArray hexEcryptedBytes = jsonFile.readAll();
    QByteArray encryptedBytes = QByteArray::fromHex(hexEcryptedBytes);
    QByteArray decryptedBytes;
    int ret_code = decryptByteArray(aes256_key, encryptedBytes, decryptedBytes);
    if (!ret_code)
        return false;

    QJsonParseError p_jsonErr;
    QJsonDocument jsonDoc = QJsonDocument::fromJson(decryptedBytes, &p_jsonErr);
    if (p_jsonErr.error != QJsonParseError::NoError)
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
    ui->credentialList->clear();
    for (int i = 0; i < m_jsonarray.size(); i ++) {
        if (m_jsonarray[i].toObject()["url"].toString().contains(searchParam.trimmed())) {
            QListWidgetItem *newItem = new QListWidgetItem();
            credentialsWidget *itemWidget = new credentialsWidget(m_jsonarray[i].toObject()["url"].toString(), i);
            QObject::connect(itemWidget, &credentialsWidget::decryptLogOrPass, this, &MainWindow::decryptLogOrPass);
            newItem->setSizeHint(itemWidget->sizeHint());
            ui->credentialList->addItem(newItem);
            ui->credentialList->setItemWidget(newItem, itemWidget);
        }
    }
}

void MainWindow::on_search_editingFinished()
{
    // Функция вызывающаяся когда пользователь нажимает "enter" в строке поиска
    fillWidget(ui->search->text());
}

int MainWindow::decryptByteArray(
    const QByteArray & aes256_key,
    const QByteArray &encryptedBytes,
    QByteArray &decryptedBytes
    )
{
    // Функция для QByteArray
    // https://cryptii.com/pipes/aes-encryption
    // key: 12 1c d5 8c a2 f4 08 5f c7 b7 b7 4b dd e0 b2 00 2c 31 13 ff 7a d3 cc 5f 19 da 87 0d 73 6e 24 f8
    // iv:  d4 34 d6 21 f1 1c d3 e4 17 ad 12 30 ab 96 9e 23
    // QByteArray key_hex("121cd58ca2f4085fc7b7b74bdde0b2002c3113ff7ad3cc5f19da870d736e24f8");
    // QByteArray key_ba = QByteArray::fromHex(key_hex);
    unsigned char key[32] = {0};
    memcpy(key, aes256_key.data(), 32);

    QByteArray iv_hex("d434d621f11cd3e417ad1230ab969e23");
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
    // Функция проверяющая правильность введённого пин-кода и регулирующая что надо расшифровать
    // - весь файл, если пин-код вводят первый раз, или конкретный логин/пароль
    QByteArray hash = QCryptographicHash::hash(
        ui->edtPin->text().toUtf8(),
        QCryptographicHash::Sha256);
    qDebug() << "*** sha256 " << hash.toHex();
    if (this->m_id == -1) {
        if (ReadJson(hash)) { // Проверить что после расшифровки - норм json
            ui->stackedWidget->setCurrentIndex(0);
        } else { // А если не так то сказать что пин-код неверный
            ui->labelLogin->setText("Неверный пин-код");
            ui->labelLogin->setStyleSheet("color:red;");
        }
        fillWidget("");
    } else {
        QJsonObject decryptedJsonObject;
        if (!decryptLoginPassword(decryptedJsonObject, hash)) {
            ui->labelLogin->setText("Неверный пин-код");
            ui->labelLogin->setStyleSheet("color:red;");
        } else {
            if (this->m_field == credentialsWidget::LOGIN) {
                QGuiApplication::clipboard()->setText(decryptedJsonObject["login"].toString());
            } else {
                QGuiApplication::clipboard()->setText(decryptedJsonObject["password"].toString());
            }
            ui->stackedWidget->setCurrentIndex(0);
        }
    }
    ui->edtPin->setText(QString().fill('*', ui->edtPin->text().size()));
    ui->edtPin->clear();
    hash.setRawData(const_cast<char*>(QByteArray().fill('*', 32).data()), 32);
    hash = QByteArray();
}

void MainWindow::decryptLogOrPass(int id, credentialsWidget::FIELD field)
{
    // Слот принимающий сигнал для расшифровки и копирования конкретного логина/пароля
    qDebug() << "*** текущей id - " << id;
    ui->labelLogin->setText("Введите пин-код");
    ui->labelLogin->setStyleSheet("");
    this->m_id = id;
    this->m_field = field;
    ui->stackedWidget->setCurrentIndex(1);
}
