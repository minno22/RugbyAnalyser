#include "querydb.h"

QueryDB::QueryDB()
{

}

void QueryDB::setQuery(QString qryString)
{
    this->qryString = qryString;
}

QSqlQuery QueryDB::executeQuery()
{
    QString serverName = "LOCALHOST\\SQLEXPRESS";
    QString dbName = "RugbyMatches";

    QSqlDatabase db = QSqlDatabase::addDatabase("QODBC");

    db.setConnectOptions();

    QString dsn = QString("DRIVER={SQL Server};SERVER=%1;DATABASE=%2;Trusted_Connection=Yes;").arg(serverName).arg(dbName);

    db.setDatabaseName(dsn);

    if(db.open())
    {
        qDebug() << "opened";

        QSqlQuery query;

        if (query.exec(this->qryString)){
            /*while (query.next()) {
                    QString name;
                    for (int i = 0; i < 7; i++){
                        name += query.value(i).toString() + "  ";
                    }
                    qDebug() << name ;
                }*/
            return query;
        }
        else{
            qDebug() << "error " << db.lastError().text();
        }

        db.close();
    }
    else{
        qDebug() << "error " << db.lastError().text();
    }
}

