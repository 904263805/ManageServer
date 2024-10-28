#include "managesql.h"
#include <QDebug>
#include <QSqlError>

ManageSql::ManageSql(QObject* parent) : QObject(parent)
{
	DatabaseInit();
}

void ManageSql::DatabaseInit()
{
	// 初始化数据库连接
	db = QSqlDatabase::addDatabase("QMYSQL");
	db.setHostName("127.0.0.1");
	db.setDatabaseName("mangeinfo");
	db.setUserName("root");
	db.setPassword("123456");
	// 打开数据库
	if (!db.open()) {
		qDebug() << "Failed to connect to database:" << db.lastError().text();
	}
	else {
		qDebug() << "Connected to database!";
	}
}

bool ManageSql::addUser(users info)
{
	QSqlQuery sql(db);
	QString strSql = "INSERT INTO users values(null,?,?,?,?,?)";
	sql.prepare(strSql);
	sql.addBindValue(info.Username);
	sql.addBindValue(info.Password);
	sql.addBindValue(info.Role);
	sql.addBindValue(info.Email);
	sql.addBindValue(info.PhoneNumber);
	return sql.exec();
}

bool ManageSql::delUser(int UserId)
{
	QSqlQuery sql(db);
	return sql.exec(QString("delete from users where UserId = %1;").arg(UserId));
}

bool ManageSql::delAllUser()
{
	QSqlQuery sql(db);
	return sql.exec("delete from users;");
}

bool ManageSql::updateusers(users info)
{
	QSqlQuery sql(db);
	QString strSql = "update users set UserName =?, Password =?, Role =?, Email =?, PhoneNumber =? where UserId =?";
	sql.prepare(strSql);
	sql.addBindValue(info.Username);
	sql.addBindValue(info.Password);
	sql.addBindValue(info.Role);
	sql.addBindValue(info.UserId);
	sql.addBindValue(info.Email);
	sql.addBindValue(info.PhoneNumber);
	return sql.exec();
}

QList<users> ManageSql::getAllUser()
{
	QList<users> l;
	QSqlQuery sql(db);
	sql.exec("select * from UserName");

	users info;

	return QList<users>();
}

int ManageSql::customerCnt()
{
	QSqlQuery sql(db);
	sql.exec("select count(UserId) from users;");
	uint Cnt = 0;
	while (sql.next()) {
		Cnt = sql.value(0).toUInt();
	}
	qDebug() << Cnt;
	return Cnt;
}
