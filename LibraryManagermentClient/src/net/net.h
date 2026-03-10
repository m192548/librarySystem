#ifndef NET_H
#define NET_H
#include <QByteArray>
#include <QObject>
#include <QTcpSocket>
class Net : public QObject
{
    Q_OBJECT
public:
    static Net* getInstance();
    void openNet();
    void sendData(const QByteArray& data);
signals:
    void loginStatusChanged(const QJsonObject& replyData);
    void booksLoaded(const QJsonObject& replyData);
    void borrowInfoLoaded(const QVector<QVector<QString>>& records);
    void regeistStatus(int status);
    void addbookStatusChanged(int status);
    void deleteBooksStatusChanged(int status);
    void modefyBooksStatusChanged(const QJsonObject& replyData);
    void borrowBooksStatusChanged(const QJsonObject& replyData);
    void returnBooksStatusChanged(const QJsonObject& replyData);
    void borrowRecordSearched(QVector<QVector<QString>>& books);
    void personalInfoLoaded(const QJsonObject& replyData);
    void modefyPersonalInfoStatusChanged(int status);
    void modefyPasswordStatusChanged(int status);
    void getBookCount(const QJsonObject& replyData);
    void findBorrowCountByDate(const QJsonObject& replyData);
    void getReaderBorrowCount(const QJsonObject& replyData);
    void updateData();
public slots:
    void onReadReady();
private:
    explicit Net(QObject *parent = nullptr);
    Net(Net&)=delete;
    Net& operator=(Net&)=delete;
    void dealData(QJsonObject& replyData,int msg_type);
    void onLogin(QJsonObject& replyData);
    void onRegeist(QJsonObject& replyData);
    void onLoadBooks(QJsonObject& replyData);
    void onLoadBorrowInfo(QJsonObject& replyData);
    void onFindBooks(QJsonObject& replyData);
    void onAddBooks(QJsonObject& replyData);
    void onDeleteBooks(QJsonObject& replyData);
    void onModefyBooks(QJsonObject& replyData);
    void onBorrowBooks(QJsonObject& replyData);
    void onReturnBooks(QJsonObject& replyData);
    void onSearchBorrowRecord(QJsonObject& replyData);
    void onModefyPersonalInfo(QJsonObject& replyData);
    void onModefyPassword(QJsonObject& replyData);
    void onGetBookCount(QJsonObject& replyData);
    void onFindBorrowCountByDate(QJsonObject& replyData);
    void on_getReaderBorrowCount(QJsonObject& replyData);
private:
    static Net* m_net;
    QTcpSocket* socket;
    int dataByte=0;
    QByteArray recv_buffer;

signals:
};

#endif // NET_H
