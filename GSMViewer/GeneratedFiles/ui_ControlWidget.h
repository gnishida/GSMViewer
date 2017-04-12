/********************************************************************************
** Form generated from reading UI file 'ControlWidget.ui'
**
** Created by: Qt User Interface Compiler version 5.6.0
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_CONTROLWIDGET_H
#define UI_CONTROLWIDGET_H

#include <QtCore/QVariant>
#include <QtWidgets/QAction>
#include <QtWidgets/QApplication>
#include <QtWidgets/QButtonGroup>
#include <QtWidgets/QCheckBox>
#include <QtWidgets/QDockWidget>
#include <QtWidgets/QGroupBox>
#include <QtWidgets/QHeaderView>
#include <QtWidgets/QLabel>
#include <QtWidgets/QLineEdit>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_ControlWidget
{
public:
    QWidget *dockWidgetContents;
    QGroupBox *groupBox;
    QLabel *label;
    QLineEdit *lineEditEdgeType;
    QLabel *label_2;
    QLineEdit *lineEditEdgeLanes;
    QLabel *label_3;
    QLineEdit *lineEditEdgeOneWay;
    QLineEdit *lineEditEdgeLink;
    QLabel *label_9;
    QLabel *label_10;
    QLineEdit *lineEditEdgeRoundabout;
    QGroupBox *groupBox_2;
    QLabel *label_4;
    QLineEdit *lineEditSimplifyThreshold;
    QPushButton *pushButtonSimplify;
    QGroupBox *groupBox_3;
    QPushButton *pushButtonRemoveShortDeadend;
    QLabel *label_5;
    QLineEdit *lineEditRemoveShortDeadendThreshold;
    QGroupBox *groupBox_4;
    QPushButton *pushButtonClean;
    QGroupBox *groupBox_5;
    QLabel *label_6;
    QLineEdit *lineEditVertexDesc;
    QLabel *label_7;
    QLineEdit *lineEditVertexPos;
    QLabel *label_8;
    QLineEdit *lineEditVertexNeighbors;
    QGroupBox *groupBox_6;
    QPushButton *pushButtonReduce;
    QCheckBox *checkBoxShowHighway;
    QCheckBox *checkBoxShowBoulevard;
    QCheckBox *checkBoxShowAvenue;
    QCheckBox *checkBoxShowLocalStreet;

    void setupUi(QDockWidget *ControlWidget)
    {
        if (ControlWidget->objectName().isEmpty())
            ControlWidget->setObjectName(QStringLiteral("ControlWidget"));
        ControlWidget->resize(192, 831);
        ControlWidget->setMinimumSize(QSize(192, 240));
        ControlWidget->setStyleSheet(QStringLiteral("background-color: rgb(181, 181, 181);"));
        dockWidgetContents = new QWidget();
        dockWidgetContents->setObjectName(QStringLiteral("dockWidgetContents"));
        groupBox = new QGroupBox(dockWidgetContents);
        groupBox->setObjectName(QStringLiteral("groupBox"));
        groupBox->setGeometry(QRect(10, 180, 171, 131));
        label = new QLabel(groupBox);
        label->setObjectName(QStringLiteral("label"));
        label->setGeometry(QRect(10, 20, 31, 16));
        lineEditEdgeType = new QLineEdit(groupBox);
        lineEditEdgeType->setObjectName(QStringLiteral("lineEditEdgeType"));
        lineEditEdgeType->setGeometry(QRect(80, 20, 81, 20));
        label_2 = new QLabel(groupBox);
        label_2->setObjectName(QStringLiteral("label_2"));
        label_2->setGeometry(QRect(10, 40, 51, 16));
        lineEditEdgeLanes = new QLineEdit(groupBox);
        lineEditEdgeLanes->setObjectName(QStringLiteral("lineEditEdgeLanes"));
        lineEditEdgeLanes->setGeometry(QRect(80, 40, 81, 20));
        label_3 = new QLabel(groupBox);
        label_3->setObjectName(QStringLiteral("label_3"));
        label_3->setGeometry(QRect(10, 60, 51, 16));
        lineEditEdgeOneWay = new QLineEdit(groupBox);
        lineEditEdgeOneWay->setObjectName(QStringLiteral("lineEditEdgeOneWay"));
        lineEditEdgeOneWay->setGeometry(QRect(80, 60, 81, 20));
        lineEditEdgeLink = new QLineEdit(groupBox);
        lineEditEdgeLink->setObjectName(QStringLiteral("lineEditEdgeLink"));
        lineEditEdgeLink->setGeometry(QRect(80, 80, 81, 20));
        label_9 = new QLabel(groupBox);
        label_9->setObjectName(QStringLiteral("label_9"));
        label_9->setGeometry(QRect(10, 80, 51, 16));
        label_10 = new QLabel(groupBox);
        label_10->setObjectName(QStringLiteral("label_10"));
        label_10->setGeometry(QRect(10, 100, 61, 16));
        lineEditEdgeRoundabout = new QLineEdit(groupBox);
        lineEditEdgeRoundabout->setObjectName(QStringLiteral("lineEditEdgeRoundabout"));
        lineEditEdgeRoundabout->setGeometry(QRect(80, 100, 81, 20));
        groupBox_2 = new QGroupBox(dockWidgetContents);
        groupBox_2->setObjectName(QStringLiteral("groupBox_2"));
        groupBox_2->setGeometry(QRect(10, 490, 171, 91));
        label_4 = new QLabel(groupBox_2);
        label_4->setObjectName(QStringLiteral("label_4"));
        label_4->setGeometry(QRect(20, 20, 46, 21));
        lineEditSimplifyThreshold = new QLineEdit(groupBox_2);
        lineEditSimplifyThreshold->setObjectName(QStringLiteral("lineEditSimplifyThreshold"));
        lineEditSimplifyThreshold->setGeometry(QRect(80, 20, 81, 20));
        pushButtonSimplify = new QPushButton(groupBox_2);
        pushButtonSimplify->setObjectName(QStringLiteral("pushButtonSimplify"));
        pushButtonSimplify->setGeometry(QRect(10, 50, 151, 31));
        groupBox_3 = new QGroupBox(dockWidgetContents);
        groupBox_3->setObjectName(QStringLiteral("groupBox_3"));
        groupBox_3->setGeometry(QRect(10, 590, 171, 91));
        pushButtonRemoveShortDeadend = new QPushButton(groupBox_3);
        pushButtonRemoveShortDeadend->setObjectName(QStringLiteral("pushButtonRemoveShortDeadend"));
        pushButtonRemoveShortDeadend->setGeometry(QRect(10, 50, 151, 31));
        label_5 = new QLabel(groupBox_3);
        label_5->setObjectName(QStringLiteral("label_5"));
        label_5->setGeometry(QRect(20, 20, 46, 21));
        lineEditRemoveShortDeadendThreshold = new QLineEdit(groupBox_3);
        lineEditRemoveShortDeadendThreshold->setObjectName(QStringLiteral("lineEditRemoveShortDeadendThreshold"));
        lineEditRemoveShortDeadendThreshold->setGeometry(QRect(80, 20, 81, 20));
        groupBox_4 = new QGroupBox(dockWidgetContents);
        groupBox_4->setObjectName(QStringLiteral("groupBox_4"));
        groupBox_4->setGeometry(QRect(10, 350, 171, 61));
        pushButtonClean = new QPushButton(groupBox_4);
        pushButtonClean->setObjectName(QStringLiteral("pushButtonClean"));
        pushButtonClean->setGeometry(QRect(10, 20, 151, 31));
        pushButtonClean->setStyleSheet(QStringLiteral(""));
        groupBox_5 = new QGroupBox(dockWidgetContents);
        groupBox_5->setObjectName(QStringLiteral("groupBox_5"));
        groupBox_5->setGeometry(QRect(10, 80, 171, 91));
        label_6 = new QLabel(groupBox_5);
        label_6->setObjectName(QStringLiteral("label_6"));
        label_6->setGeometry(QRect(10, 20, 31, 16));
        lineEditVertexDesc = new QLineEdit(groupBox_5);
        lineEditVertexDesc->setObjectName(QStringLiteral("lineEditVertexDesc"));
        lineEditVertexDesc->setGeometry(QRect(80, 20, 81, 20));
        label_7 = new QLabel(groupBox_5);
        label_7->setObjectName(QStringLiteral("label_7"));
        label_7->setGeometry(QRect(10, 40, 51, 16));
        lineEditVertexPos = new QLineEdit(groupBox_5);
        lineEditVertexPos->setObjectName(QStringLiteral("lineEditVertexPos"));
        lineEditVertexPos->setGeometry(QRect(80, 40, 81, 20));
        label_8 = new QLabel(groupBox_5);
        label_8->setObjectName(QStringLiteral("label_8"));
        label_8->setGeometry(QRect(10, 60, 51, 16));
        lineEditVertexNeighbors = new QLineEdit(groupBox_5);
        lineEditVertexNeighbors->setObjectName(QStringLiteral("lineEditVertexNeighbors"));
        lineEditVertexNeighbors->setGeometry(QRect(80, 60, 81, 20));
        groupBox_6 = new QGroupBox(dockWidgetContents);
        groupBox_6->setObjectName(QStringLiteral("groupBox_6"));
        groupBox_6->setGeometry(QRect(10, 420, 171, 61));
        pushButtonReduce = new QPushButton(groupBox_6);
        pushButtonReduce->setObjectName(QStringLiteral("pushButtonReduce"));
        pushButtonReduce->setGeometry(QRect(10, 20, 151, 31));
        checkBoxShowHighway = new QCheckBox(dockWidgetContents);
        checkBoxShowHighway->setObjectName(QStringLiteral("checkBoxShowHighway"));
        checkBoxShowHighway->setGeometry(QRect(10, 10, 70, 17));
        checkBoxShowBoulevard = new QCheckBox(dockWidgetContents);
        checkBoxShowBoulevard->setObjectName(QStringLiteral("checkBoxShowBoulevard"));
        checkBoxShowBoulevard->setGeometry(QRect(100, 10, 70, 17));
        checkBoxShowAvenue = new QCheckBox(dockWidgetContents);
        checkBoxShowAvenue->setObjectName(QStringLiteral("checkBoxShowAvenue"));
        checkBoxShowAvenue->setGeometry(QRect(10, 30, 70, 17));
        checkBoxShowLocalStreet = new QCheckBox(dockWidgetContents);
        checkBoxShowLocalStreet->setObjectName(QStringLiteral("checkBoxShowLocalStreet"));
        checkBoxShowLocalStreet->setGeometry(QRect(100, 30, 81, 17));
        ControlWidget->setWidget(dockWidgetContents);

        retranslateUi(ControlWidget);

        QMetaObject::connectSlotsByName(ControlWidget);
    } // setupUi

    void retranslateUi(QDockWidget *ControlWidget)
    {
        groupBox->setTitle(QApplication::translate("ControlWidget", "Edge", 0));
        label->setText(QApplication::translate("ControlWidget", "Type", 0));
        label_2->setText(QApplication::translate("ControlWidget", "Lanes", 0));
        label_3->setText(QApplication::translate("ControlWidget", "One Way", 0));
        label_9->setText(QApplication::translate("ControlWidget", "Link", 0));
        label_10->setText(QApplication::translate("ControlWidget", "Roundabout", 0));
        groupBox_2->setTitle(QApplication::translate("ControlWidget", "Simplify", 0));
        label_4->setText(QApplication::translate("ControlWidget", "Threshold", 0));
        pushButtonSimplify->setText(QApplication::translate("ControlWidget", "Simplify", 0));
        groupBox_3->setTitle(QApplication::translate("ControlWidget", "Remove Short Dead-End", 0));
        pushButtonRemoveShortDeadend->setText(QApplication::translate("ControlWidget", "Remove Short Dead-End", 0));
        label_5->setText(QApplication::translate("ControlWidget", "Threshold", 0));
        groupBox_4->setTitle(QApplication::translate("ControlWidget", "Clean", 0));
        pushButtonClean->setText(QApplication::translate("ControlWidget", "Clean", 0));
        groupBox_5->setTitle(QApplication::translate("ControlWidget", "Vertex", 0));
        label_6->setText(QApplication::translate("ControlWidget", "ID", 0));
        label_7->setText(QApplication::translate("ControlWidget", "Location", 0));
        label_8->setText(QApplication::translate("ControlWidget", "Neighbors", 0));
        groupBox_6->setTitle(QApplication::translate("ControlWidget", "Reduce", 0));
        pushButtonReduce->setText(QApplication::translate("ControlWidget", "Reduce", 0));
        checkBoxShowHighway->setText(QApplication::translate("ControlWidget", "Highways", 0));
        checkBoxShowBoulevard->setText(QApplication::translate("ControlWidget", "Boulevard", 0));
        checkBoxShowAvenue->setText(QApplication::translate("ControlWidget", "Avenues", 0));
        checkBoxShowLocalStreet->setText(QApplication::translate("ControlWidget", "Local Streets", 0));
        Q_UNUSED(ControlWidget);
    } // retranslateUi

};

namespace Ui {
    class ControlWidget: public Ui_ControlWidget {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_CONTROLWIDGET_H
