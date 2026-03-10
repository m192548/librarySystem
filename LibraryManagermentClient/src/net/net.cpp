#include "net.h"
#include "packet.h"
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QByteArray>
Net* Net::m_net=nullptr;
Net::Net(QObject *parent)
    : QObject{parent}
{
    socket=nullptr;
    openNet();
}
Net* Net::getInstance()
{
    if(!m_net)
    {
        m_net=new Net;
    }
    return m_net;
}
void Net::openNet()
{
    socket=new QTcpSocket(this);
    socket->connectToHost(QHostAddress::LocalHost,1925);
    connect(socket,&QTcpSocket::readyRead,this,&Net::onReadReady);
}
void Net::onReadReady()
{

        recv_buffer.append(socket->readAll());
        if(dataByte==0&&recv_buffer.size()>=4)
        {
            // 解析4字节长度前缀
            dataByte=(static_cast<quint8>(recv_buffer[0]) << 24) |
                       (static_cast<quint8>(recv_buffer[1]) << 16) |
                       (static_cast<quint8>(recv_buffer[2]) << 8) |
                       static_cast<quint8>(recv_buffer[3]);

            qDebug() << "读取的字节0：0x" << QString::number(static_cast<quint8>(recv_buffer[0]), 16) << "(" << static_cast<qint8>(recv_buffer[0]) << ")";
            qDebug() << "读取的字节1：0x" << QString::number(static_cast<quint8>(recv_buffer[1]), 16) << "(" << static_cast<qint8>(recv_buffer[1]) << ")";
            qDebug() << "读取的字节2：0x" << QString::number(static_cast<quint8>(recv_buffer[2]), 16) << "(" << static_cast<qint8>(recv_buffer[2]) << ")";
            qDebug() << "读取的字节3：0x" << QString::number(static_cast<quint8>(recv_buffer[3]), 16) << "(" << static_cast<qint8>(recv_buffer[3]) << ")";
            // 移除长度前缀
            recv_buffer.remove(0, 4);
        }
        qDebug()<<"数据大小:"<<dataByte;
        if(dataByte>0&&recv_buffer.size()>=dataByte)
        {
            // 提取完整的JSON数据
            QByteArray jsonData = recv_buffer.left(dataByte);
            // 清空缓冲区（处理剩余粘包数据）
            recv_buffer.remove(0, dataByte);
            dataByte = 0;
            QJsonParseError error;
            QJsonDocument doc=QJsonDocument::fromJson(jsonData,&error);
            if(error.error!=QJsonParseError::NoError)
            {
                qDebug()<<"解析json数据失败!"<<error.errorString();
                return;
            }
            QJsonObject data=doc.object();
            int msg_type=data["type"].toInt();
            qDebug()<<"收到响应消息:"<<msg_type;
            dealData(data,msg_type);
        }




}
void Net::dealData(QJsonObject& replyData,int msg_type)
{
    switch(msg_type)
    {
    case LOGIN_REPLY:
        onLogin(replyData);     //处理登录响应
        break;
    case REGEIST_REPLY:
        onRegeist(replyData);   //处理注册响应
        break;
    case LOADBOOK_REPLY:
        onLoadBooks(replyData); //处理加载图书信息响应
        break;
    case FINDBOOK_REPLY:
        onFindBooks(replyData); //处理查询图书响应
        break;
    case ADDBOOK_REPLY:
        onAddBooks(replyData); //处理添加图书响应
        break;
    case DELETEBOOKS_REPLY:
         onDeleteBooks(replyData); //处理删除图书响应
         break;
    case MODEFYBOOKS_REPLY:
        onModefyBooks(replyData);
        break;
    case LOADBORROWINFO_REPLY:
        onLoadBorrowInfo(replyData);
        break;
    case BORROWBOOK_REPLY:
        onBorrowBooks(replyData);
        break;
    case RETURNBOOK_REPLY:
        onReturnBooks(replyData);
        break;
    case SEARCHBORROWRECORD_REPLY:
    case FINDMYBORROWINFO_REPLY:
        onSearchBorrowRecord(replyData);
        break;
    case MODEFYPERSONALINFO_REPLY:
        onModefyPersonalInfo(replyData);
        break;
    case MODEFYPASSWORD_REPLY:
        onModefyPassword(replyData);
        break;
    case GETBOOKCOUNT_REPLY:
        onGetBookCount(replyData);
        break;
    case FINDBORROWCOUNTBYDATE_REPLY:
        findBorrowCountByDate(replyData);
        break;
    case GETREADERBORROWCOUNT_REPLY:
        on_getReaderBorrowCount(replyData);
        break;

    }
}
void Net::onLogin(QJsonObject& replyData)
{
    emit loginStatusChanged(replyData);
    emit personalInfoLoaded(replyData);
}
void Net::onRegeist(QJsonObject& replyData)
{
    emit regeistStatus(replyData["res"].toInt());
}
void Net::onLoadBooks(QJsonObject& replyData)
{
    emit booksLoaded(replyData);
}
void Net::onFindBooks(QJsonObject& replyData)
{
    //直接调用onLoadBooks
    onLoadBooks(replyData);
}
void Net::onAddBooks(QJsonObject& replyData)
{
    emit addbookStatusChanged(replyData["res"].toInt());
    emit updateData();
}
void Net::sendData(const QByteArray& data)
{
   //qint32 dataLen = data.size();
   //QByteArray lenData;
   //lenData.resize(4);
   //lenData[0] = (dataLen >> 24) & 0xFF;
   //lenData[1] = (dataLen >> 16) & 0xFF;
   //lenData[2] = (dataLen >> 8) & 0xFF;
   //lenData[3] = dataLen & 0xFF;

   //// 3. 先发送长度，再发送JSON内容
   //socket->write(lenData);
    socket->write(data);
    socket->flush(); // 强制刷出缓冲区

}

void Net::onDeleteBooks(QJsonObject& replyData)
{
    emit deleteBooksStatusChanged(replyData["res"].toInt());
    emit updateData();
}
void Net::onModefyBooks(QJsonObject& replyData)
{
    emit modefyBooksStatusChanged(replyData);
    emit updateData();
}
void Net::onLoadBorrowInfo(QJsonObject& replyData)
{
    QJsonArray recordArray=replyData["array"].toArray();
    QVector<QVector<QString>> records;
    foreach(const QJsonValue& value, recordArray)
    {
        if (value.isObject())
        {
            QJsonObject bookObj = value.toObject();
            QString book_no = bookObj["book_no"].toString();
            QString book_name = bookObj["book_name"].toString();
            QString reader_no = bookObj["reader_no"].toString();
            QString reader_name = bookObj["reader_name"].toString();
            QString borrow_time = bookObj["borrow_time"].toString("yyyy-MM-dd HH:mm:ss");
            QString return_time = bookObj["return_time"].toString("yyyy-MM-dd HH:mm:ss");
            QString due_time = bookObj["due_time"].toString("yyyy-MM-dd HH:mm:ss");
            QString deducted_Points = bookObj["deducted_Points"].toString();
            QString fine = bookObj["fine"].toString();

            QVector<QString> record;
            QStringList list{book_no,book_name,reader_no,reader_name,borrow_time,return_time,due_time,deducted_Points,fine};
            record.append(list);
            records.append(record);
        }

    }
    if(!records.empty()) emit borrowInfoLoaded(records);
}
void Net::onBorrowBooks(QJsonObject& replyData)
{
    emit borrowBooksStatusChanged(replyData);
}
void Net::onReturnBooks(QJsonObject& replyData)
{
    emit returnBooksStatusChanged(replyData);
}
void Net::onSearchBorrowRecord(QJsonObject& replyData)
{
    QJsonArray jsonRecords=replyData["array"].toArray();
    QVector<QVector<QString>> records;
    foreach(const QJsonValue& value, jsonRecords)
    {
        if (value.isObject())
        {
            QJsonObject recordObj = value.toObject();
            QString book_no = recordObj["book_no"].toString();
            QString book_name = recordObj["book_name"].toString();
            QString reader_no = recordObj["reader_no"].toString();
            QString reader_name = recordObj["reader_name"].toString();
            QString borrow_time = recordObj["borrow_time"].toString();
            QString return_time = recordObj["return_time"].toString();
            QString due_time = recordObj["due_time"].toString();
            QString deducted_Points = recordObj["deducted_Points"].toString();
            QString fine = recordObj["fine"].toString();

            QVector<QString> record;
            QStringList list{book_no,book_name,reader_no,reader_name,borrow_time,return_time,due_time,deducted_Points,fine};
            record.append(list);
            records.append(record);
        }

    }
    emit borrowRecordSearched(records);
}
void Net::onModefyPersonalInfo(QJsonObject& replyData)
{
    emit modefyPersonalInfoStatusChanged(replyData["res"].toInt());
}
void Net::onModefyPassword(QJsonObject& replyData)
{
    emit modefyPasswordStatusChanged(replyData["res"].toInt());
}
void Net::onGetBookCount(QJsonObject& replyData)
{
    emit getBookCount(replyData);
}
void Net::onFindBorrowCountByDate(QJsonObject& replyData)
{
    emit findBorrowCountByDate(replyData);
}
void Net::on_getReaderBorrowCount(QJsonObject& replyData)
{
    emit getReaderBorrowCount(replyData);
}
