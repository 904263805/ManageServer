#ifndef MANAGESQL_H
#define MANAGESQL_H

#include <QSqlDatabase>
#include <QObject>
#include <QtSql>

// 用户信息结构体
struct users
{
	int UserId;             // 用户ID
	QString Username;       // 用户名
	QString Password;       // 用户密码
	QString Role;           // 用户权限
	QString Email;          // 用户邮箱
	QString PhoneNumber;    // 用户手机号
};

class ManageSql : public QObject
{
	Q_OBJECT
public:
	explicit ManageSql(QObject* parent = nullptr);

	// 初始化数据库
	void DatabaseInit();
	// 添加用户
	bool addUser(users info);
	// 删除用户
	bool delUser(int UserId);
	// 删除所有用户信息
	bool delAllUser();
	// 修改用户信息
	bool updateusers(users info);
	// 获取所有用户
	QList<users> getAllUser();
	// 获取普通用户权限的用户数
	int customerCnt();



signals:

private:
	QSqlDatabase db;
};

#endif // MANAGESQL_H