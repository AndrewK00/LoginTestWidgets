#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QJsonArray>
#include <QJsonObject>
#include <QMessageBox>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    ui->widgetMain->setVisible(false);
//    ui->label_text->setStyleSheet("QLabel { background-color : transparent; color : black; }");
//    ui->lineEmail->setStyleSheet("QLineEdit {  border: 1px solid #5686cb; border-radius: 5px; background-color: #174588;"
//  "color: #648dad;  }");
   // ui->lineEmail->setPlaceholderText("Email");
//    ui->linePass->setStyleSheet("QLineEdit {  border: 1px solid #5686cb; border-radius: 5px; background-color: #174588;"
//  "color: #648dad;  }");
//    ui->pushButton->setStyleSheet("QPushButton {  border: 1px solid #5686cb; border-radius: 5px; background-color: #588bd6;"
//  "color: white;  }");


    QObject::connect(&client,&HttpClient::emitError,this, &MainWindow::messageFromServer );
    QObject::connect(&client,&HttpClient::pageChanged,[&](QString page){

        if(page.compare("page1")==0){

            ui->widgetLogin->setDisabled(false);
            ui->widgetLogin->setVisible(true);
            ui->widgetMain->setDisabled(true);
            ui->widgetMain->setVisible(false);

        }else if(page.compare("page2")==0){

            ui->widgetMain->setEnabled(true);
            ui->widgetMain->setVisible(true);

            ui->widgetLogin->setDisabled(true);
            ui->widgetLogin->setVisible(false);
        }
    });
    QObject::connect(&client,&HttpClient::setHomePage,[&](QString name, QUrl avatar){ ui->user->setText(name);   qDebug()<<name<<avatar;});
    QObject::connect(this,&MainWindow::error,[&](QString error,QString code){

        QMessageBox messageBox;
        messageBox.critical(0,"Error " + code ,error);
        messageBox.setFixedSize(500,200);
    });



}

MainWindow::~MainWindow()
{
    delete ui;
}


void MainWindow::on_pushButton_clicked()
{

    loginPass(ui->lineEmail->text(),ui->linePass->text());

}


void MainWindow::on_pushButtonLogOut_clicked()
{

 logout();
}

void MainWindow::messageFromServer(const QJsonDocument &json, int responseCode)
{
    QJsonObject json_obj = json.object();
    if( responseCode==417){
        //        "first_errors": {
        //                "email": "Email is not a valid email address.",
        //                "password": "Password cannot be blank."
        //            }

        auto js = json_obj["first_errors"].toObject();
        emit error(js["email"].toString() + " \n" + js["password"].toString(),QString::number(responseCode));


    }else if(responseCode==404)
    {
        //        "name": "Not Found",
        //           "message": "Page not found.",
        //           "code": 0,
        //           "status": 404,

        auto js =json_obj["previous"].toObject();
        emit error(json_obj["message"].toString() + " \n" + js["message"].toString(),QString::number(responseCode));

    }else if(responseCode==401){

        //        "name": "Unauthorized",
        //            "message": "You did not authenticate",
        //            "code": 0,
        //            "status": 401

        // auto js =jsonObj["previous"].toObject();
        emit error(json_obj["name"].toString() + " \n" +
                json_obj["message"].toString() + " \n",
                //js["message"].toString()
                QString::number(responseCode));

    }else if(responseCode==0){

       emit error("Sorry, no internet connection\n", QString::number(responseCode) );
    }

}

void MainWindow::loginPass(const QString &login, const QString &Password)
{
    const QMap<QString,QString> header{{"Content-Type:","application/json"},{"Client-Company:","udimi"},{"Client-Device:","timetracker"},{"Client-Timezone-Name:","UTC"}};
    const auto body =  QString(R"({
  "email": "%1",
  "password": "%2"
})").arg(login).arg(Password);
    client.PostRequest("auth/login",header,body);

}

void MainWindow::logout()
{
    const QMap<QString,QString> header{{"Content-Type:","application/json"},{"Token:",client.getToken()}};
    //"anywhere": true  // система разлогинит юзера на всех устройствах /// Нехочу этого делать
    const QString body(R"({
        "anywhere": false
    })");
    client.PostRequest("auth/logout",header,body);

}

