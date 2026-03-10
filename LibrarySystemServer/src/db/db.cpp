#include "db.h"
#include <QDebug>
#include <QSqlError>
DB* DB::m_db=nullptr;

DB::DB()
{
    //设置数据库相关属性
    m_mysql=QSqlDatabase::addDatabase("QMYSQL","connect1");
    m_mysql.setDatabaseName("librarymanagersystem");
    m_mysql.setHostName("localhost");
    m_mysql.setUserName("root");
    m_mysql.setPassword("192548");
    if(!m_mysql.open())
    {
        qDebug()<<"打开数据库失败!";
    }
    query=QSqlQuery(m_mysql);
}

DB* DB::getInstance()
{
    if(!m_db)
    {
        m_db=new DB;
    }
    return m_db;

}

DB::~DB()
{
    if(m_db)
    {
        //释放内存
        delete m_db;
        m_db=nullptr;
        //关闭数据库
        m_mysql.close();
    }
}
bool DB::execSql(const QString& sql)
{
    if(!query.exec(sql))
    {
       qDebug()<<query.lastError().text();
        return false;
    }
    return true;
}
void DB::selectSql(const QString& sql,int col,QVector<QVector<QString>>& res)
{
    if(query.exec(sql))
    {
        while(query.next())
        {
            QVector<QString> val;
            for(int i=0;i<col;i++)
            {
                val.push_back(query.value(i).toString());
            }
            res.push_back(val);
        }
    }
    else
    {
        qDebug()<<query.lastError().text();
    }
}
bool DB::checkColumnRepeat(const QString column_name,const QString table_name,const QString value)
{
    QString sql=QString("select id from \"%1\" where \"%2\" = \"%3\"")
                      .arg(table_name).arg(column_name).arg(value);
    bool res=false;
    if(query.exec(sql))
    {
        while(query.next())
        {
            res=true;
        }
    }
    return res;
}
QString DB::getError()const
{
    QSqlError error = query.lastError();
    qDebug() << "===== 错误详情 =====";
    qDebug() << "1. 错误码（数值）：" << error.nativeErrorCode();
    qDebug() << "2. 错误简述：" << error.text();
    qDebug() << "3. 数据库详细错误：" << error.databaseText();
    qDebug() << "4. 驱动详细错误：" << error.driverText();
    qDebug() << "5. 错误类型：" << error.type(); // 错误类型（枚举）
    return error.nativeErrorCode();
}
