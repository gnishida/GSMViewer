/********************************************************************************
** Form generated from reading UI file 'ControlWidget.ui'
**
** Created: Tue Dec 17 19:10:56 2013
**      by: Qt User Interface Compiler version 4.8.1
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_CONTROLWIDGET_H
#define UI_CONTROLWIDGET_H

#include <QtCore/QVariant>
#include <QtGui/QAction>
#include <QtGui/QApplication>
#include <QtGui/QButtonGroup>
#include <QtGui/QDockWidget>
#include <QtGui/QGroupBox>
#include <QtGui/QHeaderView>
#include <QtGui/QLabel>
#include <QtGui/QLineEdit>
#include <QtGui/QPushButton>
#include <QtGui/QWidget>

QT_BEGIN_NAMESPACE

class Ui_ControlWidget
{
public:
    QWidget *dockWidgetContents;
    QPushButton *pushButtonSimplify;
    QLineEdit *lineEditSimplifyThreshold;
    QLabel *label_4;
    QGroupBox *groupBox;
    QLabel *label;
    QLineEdit *lineEditType;
    QLabel *label_2;
    QLineEdit *lineEditLanes;
    QLabel *label_3;
    QLineEdit *lineEditOneWay;

    void setupUi(QDockWidget *ControlWidget)
    {
        if (ControlWidget->objectName().isEmpty())
            ControlWidget->setObjectName(QString::fromUtf8("ControlWidget"));
        ControlWidget->resize(192, 563);
        ControlWidget->setMinimumSize(QSize(192, 240));
        ControlWidget->setStyleSheet(QString::fromUtf8("background-color: rgb(181, 181, 181);"));
        dockWidgetContents = new QWidget();
        dockWidgetContents->setObjectName(QString::fromUtf8("dockWidgetContents"));
        pushButtonSimplify = new QPushButton(dockWidgetContents);
        pushButtonSimplify->setObjectName(QString::fromUtf8("pushButtonSimplify"));
        pushButtonSimplify->setGeometry(QRect(20, 480, 151, 31));
        lineEditSimplifyThreshold = new QLineEdit(dockWidgetContents);
        lineEditSimplifyThreshold->setObjectName(QString::fromUtf8("lineEditSimplifyThreshold"));
        lineEditSimplifyThreshold->setGeometry(QRect(100, 450, 71, 20));
        label_4 = new QLabel(dockWidgetContents);
        label_4->setObjectName(QString::fromUtf8("label_4"));
        label_4->setGeometry(QRect(20, 450, 46, 21));
        groupBox = new QGroupBox(dockWidgetContents);
        groupBox->setObjectName(QString::fromUtf8("groupBox"));
        groupBox->setGeometry(QRect(10, 270, 171, 111));
        label = new QLabel(groupBox);
        label->setObjectName(QString::fromUtf8("label"));
        label->setGeometry(QRect(10, 20, 31, 16));
        lineEditType = new QLineEdit(groupBox);
        lineEditType->setObjectName(QString::fromUtf8("lineEditType"));
        lineEditType->setGeometry(QRect(80, 20, 71, 20));
        label_2 = new QLabel(groupBox);
        label_2->setObjectName(QString::fromUtf8("label_2"));
        label_2->setGeometry(QRect(10, 50, 51, 16));
        lineEditLanes = new QLineEdit(groupBox);
        lineEditLanes->setObjectName(QString::fromUtf8("lineEditLanes"));
        lineEditLanes->setGeometry(QRect(80, 50, 71, 20));
        label_3 = new QLabel(groupBox);
        label_3->setObjectName(QString::fromUtf8("label_3"));
        label_3->setGeometry(QRect(10, 80, 51, 16));
        lineEditOneWay = new QLineEdit(groupBox);
        lineEditOneWay->setObjectName(QString::fromUtf8("lineEditOneWay"));
        lineEditOneWay->setGeometry(QRect(80, 80, 71, 20));
        ControlWidget->setWidget(dockWidgetContents);

        retranslateUi(ControlWidget);

        QMetaObject::connectSlotsByName(ControlWidget);
    } // setupUi

    void retranslateUi(QDockWidget *ControlWidget)
    {
        pushButtonSimplify->setText(QApplication::translate("ControlWidget", "Simplify", 0, QApplication::UnicodeUTF8));
        label_4->setText(QApplication::translate("ControlWidget", "Threshold", 0, QApplication::UnicodeUTF8));
        groupBox->setTitle(QApplication::translate("ControlWidget", "Edge", 0, QApplication::UnicodeUTF8));
        label->setText(QApplication::translate("ControlWidget", "Type", 0, QApplication::UnicodeUTF8));
        label_2->setText(QApplication::translate("ControlWidget", "Lanes", 0, QApplication::UnicodeUTF8));
        label_3->setText(QApplication::translate("ControlWidget", "One Way", 0, QApplication::UnicodeUTF8));
        Q_UNUSED(ControlWidget);
    } // retranslateUi

};

namespace Ui {
    class ControlWidget: public Ui_ControlWidget {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_CONTROLWIDGET_H
