#ifndef HTTPCLIENT_H
#define HTTPCLIENT_H

#include <QObject>
#include <QMap>
#include <QNetworkReply>
#include <QJsonDocument>



class HttpClient: public QObject
{
    Q_OBJECT
public:
    HttpClient(QObject *parent=nullptr);
    void PostRequest(const QString &add, const QMap<QString, QString> &header, const QString &params);

    QString getToken()const {return token;}

    signals:
    void emitError(QJsonDocument js,int responseCode );
    void setHomePage(QString name, QUrl avatar);
    void pageChanged(QString page);




private:
    QNetworkAccessManager *manager{nullptr};
    QString token;
};

#endif // HTTPCLIENT_H
