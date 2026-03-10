#ifndef DB_H
#define DB_H
#include <QSqlDatabase>
#include <QSqlQuery>
#include <QVector>
class DB
{
public:
    static DB* getInstance();
    ~DB();
    bool execSql(const QString& sql);
    void selectSql(const QString& sql,int col,QVector<QVector<QString>>& res);
    bool checkColumnRepeat(const QString column_name,const QString table_name,const QString value);
    QString getError()const;


private:
    DB();
    DB(DB&)=delete;
    DB& operator=(const DB&)=delete;
    QSqlDatabase m_mysql;
    QSqlQuery query;
    static DB* m_db;
    QString m_sql;

};

#endif // DB_H
