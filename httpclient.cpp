#include "httpclient.h"
#include <QNetworkAccessManager>
#include <QNetworkRequest>
#include <QUrlQuery>
#include <QJsonArray>
#include <QJsonObject>

HttpClient::HttpClient(QObject *parent): QObject(parent) {}



void HttpClient::PostRequest(const QString &add,const QMap<QString, QString> &header,const QString &params) {



    if(manager==nullptr)
        manager = new QNetworkAccessManager(this);

    QNetworkRequest request(QUrl("https://api.quwi.com/v2/"+add));
    request.setRawHeader("User-Agent", "Windows 10");
    request.setHeader(QNetworkRequest::ContentTypeHeader, "application/json");

    {
        QMapIterator<QString, QString> itr(header);
        while (itr.hasNext()) {
            itr.next();
            request.setRawHeader(itr.key().toUtf8(),itr.value().toUtf8());
        }
    }

    const QByteArray postData(params.toUtf8());



    auto rep = manager->post(request, postData);



    QObject::connect(rep, &QNetworkReply::finished, [=](){
	    const int HttpStatusCode = rep->attribute(QNetworkRequest::HttpStatusCodeAttribute).toInt();
        const auto allReplay = rep->readAll();
        if(HttpStatusCode==0){
            emit emitError(QJsonDocument::fromJson(R"({})"),HttpStatusCode);

        }else if( rep->error() == QNetworkReply::NoError){
            QJsonObject jsonObj = QJsonDocument::fromJson(allReplay).object();
            token = jsonObj["token"].toString();
            if(!token.isEmpty()){
                emit pageChanged("page2");
                QJsonObject js = jsonObj["app_init"].toObject()["user"].toObject();
                emit setHomePage(js["name"].toString(),js["avatar_url"].toString());
            } else emit pageChanged("page1");

        }
        else{
            emit emitError(QJsonDocument::fromJson(allReplay),HttpStatusCode);
        }

        QObject::connect(manager,&QNetworkAccessManager::destroyed,[&]( ){
        	manager=nullptr;});
        QObject::connect(rep,&QNetworkReply::destroyed,[&]( ){
            manager->deleteLater();});
        rep->deleteLater();

        delete rep;
    });


}


