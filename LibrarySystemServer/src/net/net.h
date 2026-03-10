#ifndef NET_H
#define NET_H
#include <QTcpSocket>
#include <QTcpServer>
#include <QHash>
class Net:public QTcpServer
{
    Q_OBJECT
public:
    Net();
    ~Net();
    void getClientCount()const;
    bool openServer(const QHostAddress& address=QHostAddress::Any,const quint16 port=0);
    void closeServer();
    void setPort(const int port);
    void setPassword(const QString password);
    void setServer_name(const QString server_name);
    void setServer_ip(const QString server_ip);
    int getPort();
    QString getPassword();
    QString getServer_name();
    QString getServer_ip();
public slots:
    void onReadReady();
    void onError();
    void onDisconnected();
signals:
    void clientCountChanged(int count);
    void recvMessage(const QString& str);
protected:
    void incomingConnection(qintptr socketDescriptor)override;
private:
    void dealData(QJsonObject& jsonData,int msg_type,QJsonObject& replyJson);
    void login(QJsonObject& data,QJsonObject& replyJson);
    int  regeist(QJsonObject& data,QJsonObject& replyJson);
    void loadBooks(QJsonObject& requestData,QJsonObject& replyJson);
    void findBooks(QJsonObject& requestData,QJsonObject& replyData);
    void addBook(QJsonObject& requestData,QJsonObject& replyData);
    void deleteBooks(QJsonObject& requestData,QJsonObject& replyData);
    void modefyBooks(QJsonObject& requestData,QJsonObject& replyData);
    void loadBorrowInfo(QJsonObject& replyData);
    void borrowBook(QJsonObject& requestData,QJsonObject& replyData);
    void returnBook(QJsonObject& requestData,QJsonObject& replyData);
    void searchBorrowRecords(QJsonObject& requestData,QJsonObject& replyData);
    void modefyPersonalInfomation(QJsonObject& requestData,QJsonObject& replyData);
    void modefyPassword(QJsonObject& requestData,QJsonObject& replyData);
    void getBookCount(QJsonObject& requestData,QJsonObject& replyData);
    void findMyBorrowInfo(QJsonObject& requestData,QJsonObject& replyData);
    void findBorrowCountByDate(QJsonObject& requestData,QJsonObject& replyData);
    void getReaderBorrowCount(QJsonObject& replyData);
private:
    QHash<QTcpSocket*,int> m_clients;
    int client_count;
    int m_port;
    QString m_password;
    QString m_server_name;
    QString m_server_ip;
};

#endif // NET_H
