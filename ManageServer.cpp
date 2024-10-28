#include "ManageServer.h"
#include "ManageSql.h"
#include <QMessageBox>
#include <QStandardItemModel>
#include <QTimer>

ManageServer::ManageServer(QWidget* parent)
	: QMainWindow(parent), server(new QTcpServer(this)), updateTimer(new QTimer(this))
{

	ui.setupUi(this);

	connect(server, &QTcpServer::newConnection, this, &ManageServer::newConnection);

	// 监听9527端口
	if (!server->listen(QHostAddress::Any, 9527)) {
		QMessageBox::critical(this, "Server Error", "Unable to start the server: " + server->errorString());
		close();
	}
	else {
		qDebug() << "Server started on port" << server->serverPort();
	}

	connect(updateTimer, &QTimer::timeout, this, &ManageServer::loadLogs);
	updateTimer->start(1000); // 每1000毫秒（1秒）检查一次数据库
}

ManageServer::~ManageServer() {
	server->close();
}

// 创建新连接
void ManageServer::newConnection() {
	QTcpSocket* socket = server->nextPendingConnection();
	connect(socket, &QTcpSocket::readyRead, this, &ManageServer::readData);
}

// 读取数据
void ManageServer::readData() {
	QTcpSocket* socket = qobject_cast<QTcpSocket*>(sender());
	QByteArray data = socket->readAll();

	// 将JSON格式数据转化为String格式
	QJsonDocument doc = QJsonDocument::fromJson(data);
	QJsonObject json = doc.object();
	QString action = json["action"].toString();

	// 根据请求的不同，处理不同的动作
	if (action == "register") {
		QString username = json["username"].toString();
		QString encryptedPassword = json["password"].toString();
		QString email = json["email"].toString();
		QString phone = json["phone"].toString();
		handleRegister(socket, username, encryptedPassword, email, phone);
	}
	else if (action == "login") {
		QString username = json["username"].toString();
		QString encryptedPassword = json["password"].toString();
		QString role = json["role"].toString();
		handleLogin(socket, username, encryptedPassword, role);
	}
	else if (action == "getPromotionCardData") {
		handleUpdatePromotionCard(socket, json["cardId"].toInt());
	}
	else if (action == "getCommodityCardData") {
		handleUpdateCommodityCard(socket);
	}
}

void ManageServer::handleRegister(QTcpSocket* socket, const QString& username, const QString& encryptedPassword, const QString& email, const QString& phone) {
	QSqlQuery query;
	const QString& role = "user";
	// 检查用户名是否已存在
	query.prepare("SELECT COUNT(*) FROM users WHERE Username = :username");
	query.bindValue(":username", username);

	if (query.exec() && query.next() && query.value(0).toInt() > 0) {
		// 用户名已存在，注册失败
		QJsonObject responseJson;
		responseJson["status"] = "error";
		responseJson["message"] = "Username already exists.";
		QByteArray responseData = QJsonDocument(responseJson).toJson();
		socket->write(responseData);
	}
	else {
		// 用户名不存在，进行注册
		query.prepare("INSERT INTO users(Username, Password, Role, Email, PhoneNumber) VALUES(:username, :password, :role, :email, :phone);");
		query.bindValue(":username", username);
		query.bindValue(":password", encryptedPassword);
		query.bindValue(":role", role);
		query.bindValue(":email", email);
		query.bindValue(":phone", phone);

		if (query.exec()) {
			// 注册成功
			QJsonObject responseJson;
			responseJson["status"] = "success";
			QByteArray responseData = QJsonDocument(responseJson).toJson();
			socket->write(responseData);
		}
		else {
			// 数据库操作失败
			QJsonObject responseJson;
			responseJson["status"] = "error";
			responseJson["message"] = "Database error.";
			QByteArray responseData = QJsonDocument(responseJson).toJson();
			socket->write(responseData);
		}
	}

	socket->close();
}

void ManageServer::handleLogin(QTcpSocket* socket, const QString& username, const QString& encryptedPassword, const QString& role) {
	// 检查数据库中的账户、密码和角色
	QSqlQuery query;
	query.prepare("SELECT UserID, Password, Role FROM users WHERE Username = :username");
	query.bindValue(":username", username);

	if (!query.exec()) {
		qDebug() << "SQL query execution failed:" << query.lastError().text();
	}

	if (query.exec() && query.next()) {
		QString userID = query.value("UserID").toString();
		QString storedPassword = query.value("Password").toString();
		QString storedRole = query.value("Role").toString();
		qDebug() << "User found, stored password:" << storedPassword << ", stored role:" << storedRole;

		if (storedPassword == encryptedPassword && storedRole == role) {
			// 密码和角色匹配，登录成功
			QJsonObject responseJson;
			responseJson["status"] = "success";
			responseJson["userID"] = userID;
			responseJson["username"] = username;
			responseJson["role"] = role;
			QByteArray responseData = QJsonDocument(responseJson).toJson();
			socket->write(responseData);
		}
		else {
			// 密码或角色不匹配，登录失败
			QJsonObject responseJson;
			responseJson["status"] = "error";
			responseJson["message"] = "Incorrect password or role.";
			QByteArray responseData = QJsonDocument(responseJson).toJson();
			socket->write(responseData);
		}
	}
	else {
		qDebug() << "No user found with username:" << username;
		// 账户不存在，登录失败
		QJsonObject responseJson;
		responseJson["status"] = "error";
		responseJson["message"] = "Account not found.";
		QByteArray responseData = QJsonDocument(responseJson).toJson();
		socket->write(responseData);
	}

	socket->close();
}

void ManageServer::handleUpdatePromotionCard(QTcpSocket* socket, int cardId) {
	qDebug() << "处理卡片 ID:" << cardId;

	QSqlQuery query;
	query.prepare("SELECT CardTitle, CardPixmapPath, MainTitle, SubTitle FROM T_Card WHERE CardID = :cardId");
	query.bindValue(":cardId", cardId);

	QJsonObject responseObj;
	responseObj["cardId"] = cardId;

	if (query.exec() && query.next()) {
		responseObj["title"] = query.value("CardTitle").toString();
		responseObj["pixmap"] = query.value("CardPixmapPath").toString();
		responseObj["cardTitle"] = query.value("CardTitle").toString();
		responseObj["mainTitle"] = query.value("MainTitle").toString();
		responseObj["subTitle"] = query.value("SubTitle").toString();
		//qDebug() << "卡片 ID" << cardId << "的数据:" << responseObj;
	}
	else {
		//qDebug() << "查询卡片 ID" << cardId << "的数据失败:" << query.lastError().text();
		responseObj["error"] = "未找到指定卡片 ID 的数据。";
	}

	QJsonDocument responseDoc(responseObj);
	QByteArray responseData = responseDoc.toJson(QJsonDocument::Compact);
	//qDebug() << "发送卡片 ID" << cardId << "的数据:" << responseData;
	socket->write(responseData);
	socket->flush();

	// 可选：使用 sleep 确保数据已发送完毕
	// QThread::msleep(100);

	// 确保数据已写入
	//socket->waitForBytesWritten(3000); // 等待 3 秒
	//socket->disconnectFromHost();
}

void ManageServer::handleUpdateCommodityCard(QTcpSocket* socket) {
	QSqlQuery query("SELECT CardID, Title, SubTitle, CardPixmapPath, InteractiveTips, DetailedText, FloatPixmapPath FROM commodity_cards");

	QJsonArray cardArray;
	while (query.next()) {
		QJsonObject cardObj;
		cardObj["cardId"] = query.value("CardID").toInt();
		cardObj["title"] = query.value("Title").toString();
		cardObj["subTitle"] = query.value("SubTitle").toString();
		cardObj["cardImage"] = query.value("CardPixmapPath").toString();
		cardObj["interactiveTips"] = query.value("InteractiveTips").toString();
		cardObj["detailedText"] = query.value("DetailedText").toString();
		cardObj["floatPixmap"] = query.value("FloatPixmapPath").toString();
		cardArray.append(cardObj);
	}

	QJsonDocument responseDoc(cardArray);
	QByteArray responseData = responseDoc.toJson(QJsonDocument::Compact);
	qDebug() << "发送卡片数据:" << responseData;
	socket->write(responseData);
	socket->flush();

	// 断开连接
	socket->disconnectFromHost();
}

void ManageServer::loadLogs() {
	QSqlDatabase db = QSqlDatabase::database(); // 确保已有数据库连接
	QSqlQuery query(db);
	if (!query.exec("SELECT Username, Operation, OperationTime FROM logs")) {
		qDebug() << "Query error:" << query.lastError().text();
		return;
	}

	QStandardItemModel* model = new QStandardItemModel();
	model->setHorizontalHeaderLabels({ "用户名", "用户操作", "操作时间" });

	while (query.next()) {
		QList<QStandardItem*> rowItems;
		rowItems.append(new QStandardItem(query.value(0).toString())); // Username
		rowItems.append(new QStandardItem(query.value(1).toString())); // Operation
		rowItems.append(new QStandardItem(query.value(2).toString())); // OperationTime
		model->appendRow(rowItems);
	}

	ui.LogWidget->setModel(model); // 将模型设置到LogWidget
	 // 手动设置列宽
	ui.LogWidget->setColumnWidth(0, 150); // 设置第一列宽度为100
	ui.LogWidget->setColumnWidth(1, 500); // 设置第二列宽度为200
	ui.LogWidget->setColumnWidth(2, 300); // 设置第三列宽度为150
}