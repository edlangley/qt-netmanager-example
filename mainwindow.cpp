#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <QNetworkReply>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    m_replyGotError(false),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    m_netManager = new QNetworkAccessManager(this);
}

MainWindow::~MainWindow()
{
    delete ui;
}

enum
{
    REQ_GET,
    REQ_POST
};

void MainWindow::doRequest(int reqType)
{
    QNetworkRequest request;
    request.setUrl(QUrl(ui->urlLineedit->text()));

    m_replyGotError = false;

    switch(reqType)
    {
    case REQ_GET:
        m_reply = m_netManager->get(request);
        break;
    case REQ_POST:
        m_reply = m_netManager->post(request, ui->headerText->toPlainText().toUtf8());
        break;
    default:
        return;
    }

    connect(m_reply, SIGNAL(finished()), this, SLOT(ReplyFinished()));
    connect(m_reply, SIGNAL(error(QNetworkReply::NetworkError)),
            this, SLOT(ReplyError(QNetworkReply::NetworkError)));
    connect(m_reply, SIGNAL(sslErrors(QList<QSslError>)),
            this, SLOT(ReplySSLError(QList<QSslError>)));
}

void MainWindow::ReplyFinished()
{
    ui->headerText->setPlainText("");
    QList<QByteArray> headerList = m_reply->rawHeaderList();
    for(int i = 0; i < headerList.length(); ++i)
    {
        QString headerNameValuePair(headerList[i]);
        headerNameValuePair += " :       ";
        headerNameValuePair += m_reply->rawHeader(headerList[i]);
        headerNameValuePair += "\n";

        ui->headerText->appendPlainText(headerNameValuePair);
    }

    ui->bodyText->setPlainText(QString(m_reply->readAll()));

    if(!m_replyGotError)
    {
        ui->statusBar->clearMessage();
        ui->statusBar->showMessage(QString("Got a reply"), 2000);
        ui->getButton->setEnabled(true);
        ui->postButton->setEnabled(true);
    }

    m_reply->deleteLater();
}

void MainWindow::ReplyError(QNetworkReply::NetworkError code)
{
    m_replyGotError = true;

    QString statusMsg;
    statusMsg.sprintf("Network error: %d (%s)", (int)code, m_reply->errorString().toUtf8().constData());

    ui->statusBar->clearMessage();
    ui->statusBar->showMessage(statusMsg);
    ui->getButton->setEnabled(true);
    ui->postButton->setEnabled(true);
}

void MainWindow::ReplySSLError(const QList<QSslError> & errors)
{
    m_replyGotError = true;

    QString statusMsg("SSL Network error");

    ui->statusBar->clearMessage();
    ui->statusBar->showMessage(statusMsg);
    ui->getButton->setEnabled(true);
    ui->postButton->setEnabled(true);
}

void MainWindow::on_getButton_clicked()
{
    doRequest(REQ_GET);

    ui->statusBar->showMessage(QString("Performing GET"));
    ui->getButton->setEnabled(false);
    ui->postButton->setEnabled(false);
}

void MainWindow::on_postButton_clicked()
{
    doRequest(REQ_POST);

    ui->statusBar->showMessage(QString("Performing POST"));
    ui->getButton->setEnabled(false);
    ui->postButton->setEnabled(false);
}
