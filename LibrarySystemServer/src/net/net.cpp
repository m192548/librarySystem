#include "net.h"
#include "packet.h"
#include <QJsonDocument>
#include <QJsonValue>
#include "db.h"
#include <QJsonArray>
#include <QJsonObject>
#include <QDateTime>
#include <QDate>
Net::Net()
{
    client_count=0;
}
Net::~Net()
{

}
bool Net::openServer(const QHostAddress& address,const quint16 port)
{
    m_port=port;
    m_server_ip=address.toString();
    return listen(address,port);
}
void Net::closeServer()
{
    QHash<QTcpSocket*,int>::iterator it=m_clients.begin();
    while(it!=m_clients.end())
    {
        QTcpSocket* s=it.key();
        s->deleteLater();
        s=nullptr;
        m_clients.erase(it);
    }
    close();
}
void Net::onReadReady()
{
    QTcpSocket* socket=qobject_cast<QTcpSocket*>(sender());
    QByteArray recvData=socket->readAll();
    QJsonParseError error;

    QJsonDocument doc=QJsonDocument::fromJson(recvData,&error);
    qDebug()<<recvData.toStdString();
    if(error.error!=QJsonParseError::NoError)
    {
        qDebug()<<"JSON解析失败:"<<error.errorString();
        return;
    }
   QJsonObject data=doc.object();
   int msg_type=data["type"].toInt();
   QJsonObject reply;
   QString str=QString("收到客户端[%1]的请求消息:%2").arg(m_clients[socket]).arg(msg_type);
   qDebug()<<str;
   emit recvMessage(str);
   dealData(data,msg_type,reply);

   QJsonDocument docReply(reply);
   QByteArray replyByteArray=docReply.toJson(QJsonDocument::Compact);
   qint32 dataLen = replyByteArray.size();
   QByteArray lenData;
   lenData.resize(4);
   lenData[0] = (dataLen >> 24) & 0xFF;
   lenData[1] = (dataLen >> 16) & 0xFF;
   lenData[2] = (dataLen >> 8) & 0xFF;
   lenData[3] = dataLen & 0xFF;
   qDebug() << "长度前缀字节0：0x" << QString::number(static_cast<quint8>(lenData[0]), 16) << "(" << static_cast<qint8>(lenData[0]) << ")";
   qDebug() << "长度前缀字节1：0x" << QString::number(static_cast<quint8>(lenData[1]), 16) << "(" << static_cast<qint8>(lenData[1]) << ")";
   qDebug() << "长度前缀字节2：0x" << QString::number(static_cast<quint8>(lenData[2]), 16) << "(" << static_cast<qint8>(lenData[2]) << ")";
   qDebug() << "长度前缀字节3：0x" << QString::number(static_cast<quint8>(lenData[3]), 16) << "(" << static_cast<qint8>(lenData[3]) << ")";
   qDebug()<<"数据大小:"<<dataLen;



   if(reply["type"].toInt()==ADDBOOK_REPLY||
       reply["type"].toInt()==MODEFYBOOKS_REPLY||
       reply["type"].toInt()==DELETEBOOKS_REPLY||
       reply["type"].toInt()==BORROWBOOK_REPLY||
       reply["type"].toInt()==RETURNBOOK_REPLY)
   {
       QHash<QTcpSocket*,int>::iterator it=m_clients.begin();
       while(it!=m_clients.end())
       {

           QTcpSocket* client=it.key();
           // 3. 先发送长度，再发送JSON内容
           client->write(lenData);
           client->write(replyByteArray);
           client->flush(); // 强制刷出缓冲区
           it++;
       }
   }
   else
   {
       socket->write(lenData);
       socket->write(replyByteArray);
       socket->flush(); // 强制刷出缓冲区
   }

}
void Net::onError()
{

}
void Net::onDisconnected()
{
    QTcpSocket* sender_socket=qobject_cast<QTcpSocket*>(sender());
    QString str=QString("客户端%1断开连接").arg(m_clients[sender_socket]+1);
    emit recvMessage(str);
    m_clients.remove(sender_socket);
    sender_socket->deleteLater();
    sender_socket=nullptr;
    client_count--;
    emit clientCountChanged(client_count);
}
void Net::incomingConnection(qintptr socketDescriptor)
{
    QString str=QString("客户端%1连接上了").arg(client_count);
    emit recvMessage(str);
    QTcpSocket* socket=new QTcpSocket(this);
    if(!socket)
    {
        qDebug()<<"创建客户端套接字失败!";
        return;
    }
    if(!socket->setSocketDescriptor(socketDescriptor))
    {
        qDebug()<<"设置套接字描述符失败!";
        delete socket;
        socket=nullptr;
        return;
    }
    connect(socket,&QTcpSocket::readyRead,this,&Net::onReadReady);
    connect(socket,&QTcpSocket::errorOccurred,this,&Net::onError);
    connect(socket,&QTcpSocket::disconnected,this,&Net::onDisconnected);
    int id=m_clients.size()+1;
    m_clients.insert(socket,id);
    client_count++;
    emit clientCountChanged(client_count);

}
void Net::dealData(QJsonObject& jsonData,int msg_type,QJsonObject& replyJson)
{
    switch(msg_type)
    {
    case LOGIN_REQUEST:
        login(jsonData,replyJson);
        break;
    case REGEIST_REQUEST:
        regeist(jsonData,replyJson);
        break;
    case LOADBOOK_REQUEST:
        loadBooks(jsonData,replyJson);
        break;
    case FINDBOOK_REQUEST:
        findBooks(jsonData,replyJson);
        break;
    case ADDBOOK_REQUEST:
        addBook(jsonData,replyJson);
        break;
    case DELETEBOOKS_REQUEST:
        deleteBooks(jsonData,replyJson);
        break;
    case MODEFYBOOKS_REQUEST:
        modefyBooks(jsonData,replyJson);
        break;
    case LOADBORROWINFO_REQUEST:
        loadBorrowInfo(replyJson);
        break;
    case BORROWBOOK_REQUEST:
        borrowBook(jsonData,replyJson);
        break;
    case RETURNBOOK_REQUEST:
        returnBook(jsonData,replyJson);
        break;
    case SEARCHBORROWRECORD_REQUEST:
        searchBorrowRecords(jsonData,replyJson);
        break;
    case MODEFYPERSONALINFO_REQUEST:
         modefyPersonalInfomation(jsonData,replyJson);
        break;
    case MODEFYPASSWORD_REQUEST:
        modefyPassword(jsonData,replyJson);
        break;
    case GETBOOKCOUNT_REQUEST:
        getBookCount(jsonData,replyJson);
        break;
    case FINDMYBORROWINFO_REQUEST:
        findMyBorrowInfo(jsonData,replyJson);
        break;
    case FINDBORROWCOUNTBYDATE_REQUEST:
        findBorrowCountByDate(jsonData,replyJson);
        break;
    case GETREADERBORROWCOUNT_REQUEST:
        getReaderBorrowCount(replyJson);
        break;
    }
}
void Net::login(QJsonObject& data,QJsonObject& replyJson)
{
    QVector<QVector<QString>> res;
    QString account=data["account"].toString();
    QString permission=data["permission"].toString();
    QString sql=QString("select number,account,password,nickname,permission,name,phone,email,borrow_count,borrow_point from user"
                          " where account=\"%1\" and permission=\"%2\"")
                      .arg(account).arg(permission);
    DB::getInstance()->selectSql(sql,10,res);

    //设置响应数据
    replyJson["type"]=LOGIN_REPLY;
    if(res.size()!=0)
    {
        replyJson["res"]=SUCCESS;
        replyJson["number"]=res[0][0];
        replyJson["account"]=res[0][1];
        replyJson["password"]=res[0][2];
        replyJson["nickname"]=res[0][3];
        replyJson["permission"]=res[0][4];
        replyJson["name"]=res[0][5];
        replyJson["phone"]=res[0][6];
        replyJson["email"]=res[0][7];
        replyJson["borrow_count"]=res[0][8];
        replyJson["borrow_point"]=res[0][9];
        qDebug()<<res[0][2];

    }
    else
    {
        replyJson["res"]=ERROR;
    }

}
int Net::regeist(QJsonObject& data,QJsonObject& replyJson)
{
    replyJson["type"]=REGEIST_REPLY;

    QString number=data["number"].toString();
    QString account=data["account"].toString();
    QString password=data["password"].toString();
    QString nickname=data["nickname"].toString();
    QString name=data["name"].toString();
    QString phone=data["phone"].toString();
    QString email=data["email"].toString();
    QString create_time=data["create_time"].toString();
    //检测账号或者手机号是否重复
    QString checkSql=QString("select account,phone from user where account = \"%1\" or phone =\"%2\"").arg(account).arg(phone);
    QVector<QVector<QString>> res;

    DB::getInstance()->selectSql(checkSql,2,res);
    if(res.size()>0)
    {
        replyJson["res"]=ACCOUNT_REPEAT_ERROR;
        return -1;
    }
    QString sql=QString("insert into user (number,account,password,nickname,name,phone,email,create_time) values "
                          "(\"%1\",\"%2\",\"%3\",\"%4\",\"%5\",\"%6\",\"%7\",\"%8\")")
                      .arg(number).arg(account).arg(password).arg(nickname)
                      .arg(name).arg(phone).arg(email).arg(create_time);
    qDebug()<<sql;
    if(DB::getInstance()->execSql(sql))
    {
        replyJson["res"]=SUCCESS;
        return 0;
    }
    replyJson["res"]=ERROR;
    return -1;
}
void Net::loadBooks(QJsonObject& requestData,QJsonObject& replyJson)
{
    replyJson["type"]=LOADBOOK_REPLY;
    //获取图书总数
    QString sql="select count(book_no) from books";
    QVector<QVector<QString>> book_countRes;
    DB::getInstance()->selectSql(sql,1,book_countRes);
    replyJson["book_count"]=book_countRes[0][0].toInt();

    int count=requestData["count"].toInt();
    int fromIndex=requestData["from"].toInt();
    sql=QString("select book_no,book_name,author,"
                  "publisher,publish_date,category,"
                  "stock,available_quantity,current_borrowed_count,"
                  "total_borrowed_count,status "
                  " from books order by book_no asc"
                  " limit %1,%2").arg(fromIndex).arg(count);
    qDebug()<<sql;
    QVector<QVector<QString>> books;
    DB::getInstance()->selectSql(sql,11,books);
    qDebug()<<"book count:"<<book_countRes[0][0].toInt();
    qDebug()<<"load count:"<<books.size();
    if(books.size()>0)
    {
        QJsonArray booksArray;
        for(int i=0;i<books.size();i++)
        {

            QJsonObject obj;
            obj["book_no"]=books[i][0];
            obj["book_name"]=books[i][1];
            obj["author"]=books[i][2];
            obj["publisher"]=books[i][3];
            obj["publish_date"]=books[i][4];
            obj["category"]=books[i][5];
            obj["stock"]=books[i][6];
            obj["available_quantity"]=books[i][7];
            obj["current_borrowed_count"]=books[i][8];
            obj["total_borrowed_count"]=books[i][9];
            obj["status"]=books[i][10];

            booksArray.append(obj);
        }
        replyJson["array"]=booksArray;
    }
}
void Net::findBooks(QJsonObject& requestData,QJsonObject& replyData)
{
    QString str=requestData["str"].toString();
    int count =requestData["count"].toInt();
    int from=requestData["from"].toInt();
    QString sqlSelectCount,sql;
    QVector<QVector<QString>> book_countRes;
    if(str.isEmpty())//str为空则是普通用户的请求
    {
        //获取图书总数
        sqlSelectCount=QString("select count(book_no) "
                              "from books "
                              "where book_name = \"%1\" or category = \"%2\" "
                              "or author = \"%3\"")
                          .arg(requestData["book_name"].toString())
                          .arg(requestData["category"].toString())
                          .arg(requestData["author"].toString());
        //获取图书信息
        sql=QString("select book_no,book_name,author,"
                      "publisher,publish_date,category,"
                      "stock,available_quantity,current_borrowed_count,"
                      "total_borrowed_count,status "
                      "from books "
                      "where book_name = \"%1\" or category = \"%2\" "
                      "or author = \"%3\" "
                      "order by book_no "
                      "limit %4,%5")
                  .arg(requestData["book_name"].toString())
                  .arg(requestData["category"].toString())
                  .arg(requestData["author"].toString())
                  .arg(from).arg(count);
    }
    else
    {
        //获取图书总数
        sqlSelectCount=QString("select count(book_no) "
                      "from books "
                      "where book_name = \"%1\" or category = \"%1\" "
                      "or author = \"%1\"")
                  .arg(str);
        sql=QString("select book_no,book_name,author,"
                      "publisher,publish_date,category,"
                      "stock,available_quantity,current_borrowed_count,"
                      "total_borrowed_count,status "
                      "from books "
                      "where book_name = \"%1\" or category = \"%1\" "
                      "or author = \"%1\" "
                      "order by book_no "
                      "limit %2,%3")
                  .arg(str).arg(from)
                  .arg(count);
    }
    qDebug()<<sql;
    DB::getInstance()->selectSql(sqlSelectCount,1,book_countRes);
    if(!book_countRes.isEmpty())
    {
        replyData["book_count"]=book_countRes[0][0].toInt();
        qDebug()<<"count:"<<book_countRes[0][0].toInt();
    }

    else
        replyData["book_count"]=0;
    QVector<QVector<QString>> books;
    DB::getInstance()->selectSql(sql,11,books);

    replyData["type"]=FINDBOOK_REPLY;
    QJsonArray array;
    for(int i=0;i<books.size();i++)
    {
        QJsonObject obj;
        obj["book_no"]=books[i][0];
        obj["book_name"]=books[i][1];
        obj["author"]=books[i][2];
        obj["publisher"]=books[i][3];
        obj["publish_date"]=books[i][4];
        obj["category"]=books[i][5];
        obj["stock"]=books[i][6];
        obj["available_quantity"]=books[i][7];
        obj["current_borrowed_count"]=books[i][8];
        obj["total_borrowed_count"]=books[i][9];
        obj["status"]=books[i][10];

        array.append(obj);
    }
    replyData["array"]=array;
}
void Net::addBook(QJsonObject& requestData,QJsonObject& replyData)
{
    replyData["type"]=ADDBOOK_REPLY;
    QString number=requestData["book_no"].toString();
    if(DB::getInstance()->checkColumnRepeat("book_no","books",number))
    {
        replyData["res"]=BOOKNOREPEATE_ERROR;
        return;
    }
    QString name=requestData["book_name"].toString();
    QString author=requestData["author"].toString();
    QString publisher=requestData["publisher"].toString();
    QString publishDate=requestData["publish_date"].toString();
    QString category=requestData["category"].toString();
    int stock=requestData["stock"].toInt();
    int available_quantity=requestData["available_count"].toInt();
    int current_borrowed_count=requestData["current_borrowed_count"].toInt();
    int total_borrowed_count=requestData["total_borrowed_count"].toInt();
    int status=requestData["status"].toInt();

    QString sql=QString("insert into books (book_no,book_name,author,publisher,publish_date,"
                          "category,stock,available_quantity,current_borrowed_count,total_borrowed_count,status)"
                          "values(\"%1\",\"%2\",\"%3\",\"%4\",\"%5\","
                          "\"%6\",%7,%8,%9,%10,%11)")
                      .arg(number).arg(name).arg(author).arg(publisher)
                      .arg(publishDate).arg(category).arg(stock)
                      .arg(available_quantity).arg(current_borrowed_count).arg(total_borrowed_count).arg(status);
    qDebug()<<sql;
    if(!DB::getInstance()->execSql(sql))
    {
        replyData["res"]=ERROR;
        return;
    }
    replyData["res"]=SUCCESS;
    replyData["book_name"]=name;
    replyData["author"]=author;
    replyData["publisher"]=publisher;
    replyData["publish_date"]=publishDate;
    replyData["category"]=category;
    replyData["stock"]=stock;
    replyData["available_count"]=available_quantity;
    replyData["current_borrowed_count"]=current_borrowed_count;
    replyData["total_borrowed_count"]=total_borrowed_count;
    replyData["status"]=status;

}
void Net::deleteBooks(QJsonObject& requestData,QJsonObject& replyData)
{
    QJsonArray books_noArray=requestData["book_nos"].toArray();
    QString book_nos="(";
    for(int i=0;i<books_noArray.size();i++)
    {
        book_nos.append("\"");
        book_nos.append(books_noArray[i].toString());
        book_nos.append("\"");
        if(i<books_noArray.size()-1)
        {
            book_nos.append(',');
        }
    }
    book_nos.append(')');
    QString sql=QString("delete from books where book_no in %1").arg(book_nos);
    qDebug()<<sql;
    replyData["type"]=DELETEBOOKS_REPLY;
    if(DB::getInstance()->execSql(sql))
    {
        replyData["res"]=SUCCESS;
        replyData["book_nos"]=books_noArray;
    }
    else
    {
        replyData["res"]=ERROR;
    }
}
void Net::modefyBooks(QJsonObject& requestData,QJsonObject& replyData)
{
    QJsonArray books_array=requestData["array"].toArray();
    replyData["type"]=MODEFYBOOKS_REPLY;

    for(int i=0;i<books_array.size();i++)
    {
        QJsonObject bookInfo=books_array[i].toObject();

        QString book_no=bookInfo["book_no"].toString();
        QString book_name=bookInfo["book_name"].toString();
        QString author=bookInfo["author"].toString();
        QString publisher=bookInfo["publisher"].toString();
        QString publish_date=bookInfo["publish_date"].toString();
        QString category=bookInfo["category"].toString();
        int stock=bookInfo["stock"].toInt();
        int available_quantity=bookInfo["available_quantity"].toInt();
        int current_borrowed_count=bookInfo["current_borrowed_count"].toInt();
        int total_borrowed_count=bookInfo["total_borrowed_count"].toInt();
        int status=bookInfo["total_borrowed_count"].toString()=="可借"?1:0;
        QString sql=QString("update books set "
                              "book_name = \"%1\","
                              "author = \"%2\","
                              "publisher = \"%3\","
                              "publish_date = \"%4\","
                              "category = \"%5\","
                              "stock = %6,"
                              "available_quantity = %7,"
                              "current_borrowed_count = %8,"
                              "total_borrowed_count = %9,"
                              "status = %10 "
                              "where book_no = \"%11\"")
                          .arg(book_name).arg(author)
                          .arg(publisher).arg(publish_date).arg(category)
                          .arg(stock).arg(available_quantity).arg(current_borrowed_count)
                          .arg(total_borrowed_count).arg(status).arg(book_no);
        if(DB::getInstance()->execSql(sql))
        {
            replyData["res"]=SUCCESS;
            replyData["array"]=requestData["array"];
        }
        else
        {
            QString errorStr=DB::getInstance()->getError();
            qDebug()<<book_no<<"error:"<<errorStr;
            replyData["res"]=ERROR;
            replyData["error"]=errorStr;
        }

    }
    if(books_array.size()==0)replyData["res"]=ERROR;
}
void Net::loadBorrowInfo(QJsonObject& replyData)
{
    replyData["type"]=LOADBORROWINFO_REPLY;
    QJsonArray records;
    QVector<QVector<QString>> res;
    QString sql="select br.book_no,b.book_name,br.reader_no,u.name,br.borrow_time,"
                "br.return_time,br.due_time,br.deducted_Points,br.fine from borrow_record br "
                "left join books b on br.book_no = b.book_no "
                "left join user u on br.reader_no = number";

    DB::getInstance()->selectSql(sql,9,res);

    for(int i=0;i<res.size();i++)
    {
        QJsonObject obj;
        obj["book_no"]=res[i][0];
        obj["book_name"]=res[i][1];
        obj["reader_no"]=res[i][2];
        obj["reader_name"]=res[i][3];
        obj["borrow_time"]=QDateTime::fromString(res[i][4],"yyyy-MM-ddTHH:mm:ss.zzz").toString("yyyy-MM-dd HH:mm:ss");
        obj["return_time"]=QDateTime::fromString(res[i][5],"yyyy-MM-ddTHH:mm:ss.zzz").toString("yyyy-MM-dd HH:mm:ss");
        obj["due_time"]=QDateTime::fromString(res[i][6],"yyyy-MM-ddTHH:mm:ss.zzz").toString("yyyy-MM-dd HH:mm:ss");
        obj["deducted_Points"]=res[i][7];
        obj["fine"]=res[i][8];
        records.append(obj);
    }
    replyData["array"]=records;

}
void Net::borrowBook(QJsonObject& requestData,QJsonObject& replyData)
{
    qDebug()<<"book_no:"<<requestData["book_no"].toString();
    qDebug()<<"reader_no:"<<requestData["reader_no"].toString();
    //检查是否重复借阅该书
    QString check_sql=QString("select id from borrow_record where book_no = \"%1\" and "
                                "reader_no = \"%2\" and is_return = \"否\"")
                            .arg(requestData["book_no"].toString())
                            .arg(requestData["reader_no"].toString());
    QVector<QVector<QString>> res;
    DB::getInstance()->selectSql(check_sql,1,res);
    replyData["type"]=BORROWBOOK_REPLY;
    if(res.size()>0)
    {
        replyData["res"]=BORROWREPEATE_ERROR;
        return;
    }

    QString sql=QString("insert into borrow_record (book_no,reader_no,borrow_time,due_time)values"
                          "(\"%1\",\"%2\",\"%3\",\"%4\")")
                      .arg(requestData["book_no"].toString())
                      .arg(requestData["reader_no"].toString())
                      .arg(requestData["borrow_time"].toString())
                      .arg(requestData["due_time"].toString());

    if(DB::getInstance()->execSql(sql))
    {
        replyData["res"]=SUCCESS;
        replyData["book_no"]=requestData["book_no"];
        replyData["book_name"]=requestData["book_name"];
        replyData["reader_no"]=requestData["reader_no"];
        replyData["reader_name"]=requestData["reader_name"];
        replyData["borrow_time"]=requestData["borrow_time"];
        replyData["return_time"]="";
        replyData["due_time"]=requestData["due_time"];
        replyData["deducted_Points"]=0;
        replyData["fine"]=0.00;

    }
    else
    {
        replyData["res"]=ERROR;
    }
}
void Net::returnBook(QJsonObject& requestData,QJsonObject& replyData)
{
    QString book_no=requestData["book_no"].toString();
    QString reader_no=requestData["reader_no"].toString();
    QString return_time=requestData["return_time"].toString();

    QString sql=QString("update borrow_record set return_time = \"%1\" "
                          "where book_no = \"%2\" and reader_no = \"%3\" and return_time = null")
                      .arg(return_time).arg(book_no).arg(reader_no);
    qDebug()<<sql;
    replyData["type"]=RETURNBOOK_REPLY;
    if(DB::getInstance()->execSql(sql))
    {
        replyData["res"]=SUCCESS;
        replyData["book_no"]=book_no;
    }
    else
    {
        replyData["res"]=ERROR;
    }
}
void Net::searchBorrowRecords(QJsonObject& requestData,QJsonObject& replyData)
{
    QString book_no = requestData["book_no"].toString();
    QString reader_no = requestData["reader_no"].toString();
    QString book_type=requestData["book_type"].toString();
    QString return_status=requestData["return_status"].toString();

    QString sql;
    sql=QString("select br.book_no,b.book_name,br.reader_no,u.name,br.borrow_time,"
                  "br.return_time,br.due_time,deducted_Points,fine from borrow_record br "
                  "left join books b on b.book_no = br.book_no "
                  "left join user u on u.number=br.reader_no "
                  "where br.book_no = \"%1\" or "
                  "br.reader_no = \"%2\"")
              .arg(book_no).arg(reader_no);
    QString book_typeSqlStr=QString("b.category = \"%1\"").arg(book_type);
    QString return_typeSqlStr=QString("br.is_return = \"%1\"").arg(return_status=="未归还"?"否":"是");
    if(book_type=="全部"&&return_status!="全部")
    {
        sql=sql+" or "+return_typeSqlStr;
    }
    else if(book_type!="全部"&&return_status=="全部")
    {
        sql=sql+" or "+book_typeSqlStr;
    }
    else if(book_type!="全部"&&return_status!="全部")
    {
        sql=sql+" or "+return_typeSqlStr+" or "+book_typeSqlStr;
    }
    qDebug()<<sql;
    QVector<QVector<QString>> res;
    DB::getInstance()->selectSql(sql,9,res);

    replyData["type"]=SEARCHBORROWRECORD_REPLY;
    if(!res.isEmpty())
    {
        QJsonArray array;
        for(int i=0;i<res.size();i++)
        {
            QJsonObject obj;
            obj["book_no"]=res[i][0];
            obj["book_name"]=res[i][1];
            obj["reader_no"]=res[i][2];
            obj["reader_name"]=res[i][3];
            obj["borrow_time"]=res[i][4];
            obj["return_time"]=res[i][5];
            obj["due_time"]=res[i][6];
            obj["deducted_Points"]=res[i][7];
            obj["fine"]=res[i][8];

            array.append(obj);
        }
        replyData["array"]=array;
    }
}
void Net::modefyPersonalInfomation(QJsonObject& requestData,QJsonObject& replyData)
{
    QString number=requestData["number"].toString();
    QString nickname=requestData["nickname"].toString();
    QString name=requestData["name"].toString();
    QString phone=requestData["phone"].toString();
    QString email=requestData["email"].toString();

    QString sql=QString("update user set nickname = \"%1\",name = \"%2\",phone = \"%3\",email = \"%4\" "
                          "where number = \"%5\"")
                      .arg(nickname).arg(name).arg(phone).arg(email).arg(number);

    replyData["type"]=MODEFYPERSONALINFO_REPLY;
    if(DB::getInstance()->execSql(sql))
    {
        replyData["res"]=SUCCESS;
    }
    else
    {
        replyData["res"]=ERROR;
    }
}
void Net::modefyPassword(QJsonObject& requestData,QJsonObject& replyData)
{
    QString number=requestData["number"].toString();
    QString new_password=requestData["new_password"].toString();
    QString selectSql=QString("select password from user where number = \"%1\"").arg(number);

    replyData["type"]=MODEFYPASSWORD_REPLY;
    QString sql=QString("update user set password = \"%1\" where number = \"%2\"")
                      .arg(new_password).arg(number);

    if(DB::getInstance()->execSql(sql))
    {
        replyData["res"]=SUCCESS;
    }
    else
    {
        replyData["res"]=ERROR;
    }
}
void Net::getBookCount(QJsonObject& requestData,QJsonObject& replyData)
{
    QString col_name=requestData["col_name"].toString();
    QString count_type=requestData["count_type"].toString();

    QString sql=QString("select %1,sum(%2) as count from books group by %3")
        .arg(col_name).arg(count_type).arg(col_name);

    QVector<QVector<QString>> res;
    DB::getInstance()->selectSql(sql,2,res);
    replyData["type"]=GETBOOKCOUNT_REPLY;
    replyData["count_name"]=count_type;
    QJsonArray array;

    for(int i=0;i<res.size();i++)
    {
        QJsonObject obj;
        obj["category"]=res[i][0];
        obj["count"]=res[i][1].toInt();
        array.append(obj);
    }
    replyData["array"]=array;
}
void Net::findMyBorrowInfo(QJsonObject& requestData,QJsonObject& replyData)
{
    QString reader_no=requestData["reader_no"].toString();
    QString book_name=requestData["book_name"].toString();
    QString category=requestData["category"].toString();
    QString is_return=requestData["is_return"].toString();
    QString borrowDateRange=requestData["borrowDateRange"].toString();

    QString sql=QString("select br.book_no,b.book_name,br.reader_no,u.name,br.borrow_time,"
                  "br.return_time,br.due_time,deducted_Points,fine from borrow_record br "
                  "left join books b on b.book_no = br.book_no "
                  "left join user u on u.number=br.reader_no "
                          "where br.reader_no = \"%1\"").arg(reader_no);

    QString categorySqlStr=QString("b.category = \"%1\"").arg(category);
    QString isReturnSqlStr=QString("br.is_return = \"%1\"").arg(is_return=="已归还"?"是":"否");;
    if(category!="全部")
    {
        sql+=" and "+categorySqlStr;
    }

    if(is_return!="全部")
    {
        sql+=" and "+isReturnSqlStr;
    }

    int DateTimeRange=-1;
    if(borrowDateRange=="最近三天")DateTimeRange=3;
    else if(borrowDateRange=="最近一周")DateTimeRange=7;
    else if(borrowDateRange=="最近一个月")DateTimeRange=30;
    else if(borrowDateRange=="最近六个月")DateTimeRange=180;
    else if(borrowDateRange=="最近一年")DateTimeRange=365;

    if(DateTimeRange!=-1)
    {
        sql+=" and "+QString("br.borrow_time >= date_sub(now(),interval %1 day)").arg(DateTimeRange);
    }
    if(!book_name.isEmpty())
    {
        sql+=" or "+QString("b.book_name = \"%1\"").arg(book_name);
    }
    qDebug()<<sql;
    replyData["type"]=FINDMYBORROWINFO_REPLY;
    QVector<QVector<QString>> res;

    DB::getInstance()->selectSql(sql,9,res);
     qDebug()<<"res size:"<<res.size();
    QJsonArray array;

    for(int i=0;i<res.size();i++)
    {
        QJsonObject obj;
        obj["book_no"]=res[i][0];
        obj["book_name"]=res[i][1];
        obj["reader_no"]=res[i][2];
        obj["reader_name"]=res[i][3];
        obj["borrow_time"]=res[i][4];
        obj["return_time"]=res[i][5];
        obj["due_time"]=res[i][6];
        obj["deducted_Points"]=res[i][7];
        obj["fine"]=res[i][8];

        array.append(obj);
    }
    replyData["array"]=array;

}
void Net::findBorrowCountByDate(QJsonObject& requestData,QJsonObject& replyData)
{
    int days=requestData["days"].toInt();
    //初始化days天的借阅数量为0
    QVector<QVector<QString>> res;
    for(int i=0;i<days;i++)
    {
        QVector<QString> v;
        v.append(QDate::currentDate().addDays(0-i).toString("yyyy/MM/dd"));
        v.append("0");
        res.push_front(v);
    }
    QString sql=QString("select date(borrow_time) as borrow_date,count(id) as count from borrow_record "
                  "where borrow_time >= date_sub(curdate(), interval %1 day) "
                  "and borrow_time < date_add(curdate(), interval 1 DAY) "
                  "group by borrow_date "
                  "order by borrow_date")
              .arg(days);

    QVector<QVector<QString>> tempRes;
    DB::getInstance()->selectSql(sql,2,tempRes);
    replyData["type"]=FINDBORROWCOUNTBYDATE_REPLY;
    QJsonArray array;

    //根据日期设置查找到的数量
    for(int i=0;i<tempRes.size();i++)
    {
        tempRes[i][0]=QDate::fromString(tempRes[i][0],"yyyy-MM-dd").toString("yyyy/MM/dd");
        for(int j=0;j<res.size();j++)
        {
            if(res[j][0]==tempRes[i][0])
            {
                res[j][1]=tempRes[i][1];
                break;
            }
            qDebug()<<"res["<<j<<"]:"<<res[j]<<","<<"tempRes["<<i<<"]:"<<tempRes[i];
        }

    }
    for(int i=0;i<res.size();i++)
    {
        QJsonObject obj;
        obj["date"]=res[i][0];
        obj["count"]=res[i][1].toInt();
        array.append(obj);
    }
    replyData["array"]=array;
}
void Net::getReaderBorrowCount(QJsonObject& replyData)
{
    QString sql="select number,borrow_count from user order by borrow_count desc";
    QVector<QVector<QString>> res;

    DB::getInstance()->selectSql(sql,2,res);
    replyData["type"]=GETREADERBORROWCOUNT_REPLY;
    QJsonArray array;
    for(int i=0;i<res.size();i++)
    {
        QJsonObject obj;
        obj["reader_no"]=res[i][0];
        obj["count"]=res[i][1];
        array.append(obj);
    }
    replyData["array"]=array;
}

void Net::setPort(const int port)
{
    m_port=port;
}
void Net::setPassword(const QString password)
{
    m_password=password;
}
void Net::setServer_name(const QString server_name)
{
    m_server_name=server_name;
}
void Net::setServer_ip(const QString server_ip)
{
    m_server_ip=server_ip;
}
int Net::getPort(){return m_port;}
QString Net::getPassword(){return m_password;}
QString Net::getServer_name(){return m_server_name;}
QString Net::getServer_ip(){return m_server_ip;}
