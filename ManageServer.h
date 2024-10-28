#pragma once
#include <QTcpServer>
#include <QTcpSocket>
#include <QJsonDocument>
#include <QJsonObject>
#include <QSqlDatabase>
#include <QSqlQuery>
#include <QtWidgets/QMainWindow>
#include "ui_ManageServer.h"

class ManageServer : public QMainWindow
{
	Q_OBJECT

public:
	ManageServer(QWidget* parent = nullptr);
	~ManageServer();

private slots:
	void newConnection();
	void readData();
	void handleRegister(QTcpSocket* socket, const QString& username, const QString& encryptedPassword, const QString& email, const QString& phone);
	void handleLogin(QTcpSocket* socket, const QString& username, const QString& encryptedPassword, const QString& role);
	void handleUpdatePromotionCard(QTcpSocket* socket, int cardId);
	void handleUpdateCommodityCard(QTcpSocket* socket);
	void loadLogs();
private:
	QTcpServer* server;  
	QTimer* updateTimer;
private:
	Ui::ManageServerClass ui;
};

