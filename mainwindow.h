#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QNetworkAccessManager>
#include <QNetworkReply>

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT
    
public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();
    
public slots:
//    void ReplyFinished(QNetworkReply *reply);
    void ReplyFinished();
    void ReplyError(QNetworkReply::NetworkError code);
    void ReplySSLError(const QList<QSslError> & errors);

    void on_getButton_clicked();
    void on_postButton_clicked();

private:
    void doRequest(int reqType);

    QNetworkAccessManager *m_netManager;
    QNetworkReply *m_reply;
    bool m_replyGotError;

    Ui::MainWindow *ui;
};

#endif // MAINWINDOW_H
