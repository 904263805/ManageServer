/********************************************************************************
** Form generated from reading UI file 'ManageServer.ui'
**
** Created by: Qt User Interface Compiler version 5.12.12
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_MANAGESERVER_H
#define UI_MANAGESERVER_H

#include <QtCore/QVariant>
#include <QtWidgets/QApplication>
#include <QtWidgets/QHeaderView>
#include <QtWidgets/QMainWindow>
#include <QtWidgets/QTableView>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_ManageServerClass
{
public:
    QWidget *centralWidget;
    QTableView *LogWidget;

    void setupUi(QMainWindow *ManageServerClass)
    {
        if (ManageServerClass->objectName().isEmpty())
            ManageServerClass->setObjectName(QString::fromUtf8("ManageServerClass"));
        ManageServerClass->resize(602, 430);
        centralWidget = new QWidget(ManageServerClass);
        centralWidget->setObjectName(QString::fromUtf8("centralWidget"));
        LogWidget = new QTableView(centralWidget);
        LogWidget->setObjectName(QString::fromUtf8("LogWidget"));
        LogWidget->setGeometry(QRect(0, 0, 611, 431));
        ManageServerClass->setCentralWidget(centralWidget);

        retranslateUi(ManageServerClass);

        QMetaObject::connectSlotsByName(ManageServerClass);
    } // setupUi

    void retranslateUi(QMainWindow *ManageServerClass)
    {
        ManageServerClass->setWindowTitle(QApplication::translate("ManageServerClass", "\346\234\215\345\212\241\345\231\250\346\227\245\345\277\227", nullptr));
    } // retranslateUi

};

namespace Ui {
    class ManageServerClass: public Ui_ManageServerClass {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_MANAGESERVER_H
