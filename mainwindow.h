#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "httpclient.h"

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

signals:
    void pageChanged(QString page);
    void error(QString error,QString code);
    void setHomePage(QString userName, QUrl avatar);

private slots:
    void on_pushButton_clicked();
    void on_pushButtonLogOut_clicked();
     void messageFromServer(const QJsonDocument& json, int responseCode);

private:
    void loginPass(const QString &login, const QString &Password);
    void logout();
    Ui::MainWindow *ui;
    HttpClient client;
};
#endif // MAINWINDOW_H
