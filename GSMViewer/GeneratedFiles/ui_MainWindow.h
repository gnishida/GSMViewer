/********************************************************************************
** Form generated from reading UI file 'MainWindow.ui'
**
** Created by: Qt User Interface Compiler version 5.6.0
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_MAINWINDOW_H
#define UI_MAINWINDOW_H

#include <QtCore/QVariant>
#include <QtWidgets/QAction>
#include <QtWidgets/QApplication>
#include <QtWidgets/QButtonGroup>
#include <QtWidgets/QHeaderView>
#include <QtWidgets/QMainWindow>
#include <QtWidgets/QMenu>
#include <QtWidgets/QMenuBar>
#include <QtWidgets/QStatusBar>
#include <QtWidgets/QToolBar>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_MainWindow
{
public:
    QAction *actionNew;
    QAction *actionOpen;
    QAction *actionExit;
    QAction *actionControlWidget;
    QAction *actionDeleteEdge;
    QAction *actionSave;
    QAction *actionUndo;
    QAction *actionSelectAll;
    QAction *actionCut;
    QAction *actionCopy;
    QAction *actionPaste;
    QAction *actionOpenToAdd;
    QAction *actionVoronoi;
    QAction *actionVoronoiCut;
    QAction *actionModeBasic;
    QAction *actionModeLayer;
    QAction *actionModeSketch;
    QAction *actionShowArea;
    QWidget *centralWidget;
    QMenuBar *menuBar;
    QMenu *menuFile;
    QMenu *menuWindow;
    QMenu *menuEdit;
    QMenu *menuSelect;
    QToolBar *mainToolBar;
    QStatusBar *statusBar;

    void setupUi(QMainWindow *MainWindow)
    {
        if (MainWindow->objectName().isEmpty())
            MainWindow->setObjectName(QStringLiteral("MainWindow"));
        MainWindow->resize(1006, 800);
        MainWindow->setMinimumSize(QSize(1000, 800));
        actionNew = new QAction(MainWindow);
        actionNew->setObjectName(QStringLiteral("actionNew"));
        QIcon icon;
        icon.addFile(QStringLiteral(":/MainWindow/Resources/New_24x24.png"), QSize(), QIcon::Normal, QIcon::Off);
        actionNew->setIcon(icon);
        actionOpen = new QAction(MainWindow);
        actionOpen->setObjectName(QStringLiteral("actionOpen"));
        QIcon icon1;
        icon1.addFile(QStringLiteral(":/MainWindow/Resources/Open_24x24.png"), QSize(), QIcon::Normal, QIcon::Off);
        actionOpen->setIcon(icon1);
        actionExit = new QAction(MainWindow);
        actionExit->setObjectName(QStringLiteral("actionExit"));
        actionControlWidget = new QAction(MainWindow);
        actionControlWidget->setObjectName(QStringLiteral("actionControlWidget"));
        actionDeleteEdge = new QAction(MainWindow);
        actionDeleteEdge->setObjectName(QStringLiteral("actionDeleteEdge"));
        QIcon icon2;
        icon2.addFile(QStringLiteral(":/MainWindow/Resources/Delete_24x24.png"), QSize(), QIcon::Normal, QIcon::Off);
        actionDeleteEdge->setIcon(icon2);
        actionSave = new QAction(MainWindow);
        actionSave->setObjectName(QStringLiteral("actionSave"));
        QIcon icon3;
        icon3.addFile(QStringLiteral(":/MainWindow/Resources/Save_24x24.png"), QSize(), QIcon::Normal, QIcon::Off);
        actionSave->setIcon(icon3);
        actionUndo = new QAction(MainWindow);
        actionUndo->setObjectName(QStringLiteral("actionUndo"));
        QIcon icon4;
        icon4.addFile(QStringLiteral(":/MainWindow/Resources/Undo_24x24.png"), QSize(), QIcon::Normal, QIcon::Off);
        actionUndo->setIcon(icon4);
        actionSelectAll = new QAction(MainWindow);
        actionSelectAll->setObjectName(QStringLiteral("actionSelectAll"));
        actionCut = new QAction(MainWindow);
        actionCut->setObjectName(QStringLiteral("actionCut"));
        QIcon icon5;
        icon5.addFile(QStringLiteral(":/MainWindow/Resources/Cut_24x24.png"), QSize(), QIcon::Normal, QIcon::Off);
        actionCut->setIcon(icon5);
        actionCopy = new QAction(MainWindow);
        actionCopy->setObjectName(QStringLiteral("actionCopy"));
        QIcon icon6;
        icon6.addFile(QStringLiteral(":/MainWindow/Resources/Copy_24x24.png"), QSize(), QIcon::Normal, QIcon::Off);
        actionCopy->setIcon(icon6);
        actionPaste = new QAction(MainWindow);
        actionPaste->setObjectName(QStringLiteral("actionPaste"));
        QIcon icon7;
        icon7.addFile(QStringLiteral(":/MainWindow/Resources/Paste_24x24.png"), QSize(), QIcon::Normal, QIcon::Off);
        actionPaste->setIcon(icon7);
        actionOpenToAdd = new QAction(MainWindow);
        actionOpenToAdd->setObjectName(QStringLiteral("actionOpenToAdd"));
        actionVoronoi = new QAction(MainWindow);
        actionVoronoi->setObjectName(QStringLiteral("actionVoronoi"));
        actionVoronoiCut = new QAction(MainWindow);
        actionVoronoiCut->setObjectName(QStringLiteral("actionVoronoiCut"));
        actionModeBasic = new QAction(MainWindow);
        actionModeBasic->setObjectName(QStringLiteral("actionModeBasic"));
        actionModeBasic->setCheckable(true);
        actionModeLayer = new QAction(MainWindow);
        actionModeLayer->setObjectName(QStringLiteral("actionModeLayer"));
        actionModeLayer->setCheckable(true);
        actionModeSketch = new QAction(MainWindow);
        actionModeSketch->setObjectName(QStringLiteral("actionModeSketch"));
        actionModeSketch->setCheckable(true);
        actionShowArea = new QAction(MainWindow);
        actionShowArea->setObjectName(QStringLiteral("actionShowArea"));
        centralWidget = new QWidget(MainWindow);
        centralWidget->setObjectName(QStringLiteral("centralWidget"));
        MainWindow->setCentralWidget(centralWidget);
        menuBar = new QMenuBar(MainWindow);
        menuBar->setObjectName(QStringLiteral("menuBar"));
        menuBar->setGeometry(QRect(0, 0, 1006, 21));
        menuFile = new QMenu(menuBar);
        menuFile->setObjectName(QStringLiteral("menuFile"));
        menuWindow = new QMenu(menuBar);
        menuWindow->setObjectName(QStringLiteral("menuWindow"));
        menuEdit = new QMenu(menuBar);
        menuEdit->setObjectName(QStringLiteral("menuEdit"));
        menuSelect = new QMenu(menuBar);
        menuSelect->setObjectName(QStringLiteral("menuSelect"));
        MainWindow->setMenuBar(menuBar);
        mainToolBar = new QToolBar(MainWindow);
        mainToolBar->setObjectName(QStringLiteral("mainToolBar"));
        MainWindow->addToolBar(Qt::TopToolBarArea, mainToolBar);
        statusBar = new QStatusBar(MainWindow);
        statusBar->setObjectName(QStringLiteral("statusBar"));
        MainWindow->setStatusBar(statusBar);

        menuBar->addAction(menuFile->menuAction());
        menuBar->addAction(menuSelect->menuAction());
        menuBar->addAction(menuEdit->menuAction());
        menuBar->addAction(menuWindow->menuAction());
        menuFile->addAction(actionNew);
        menuFile->addAction(actionOpen);
        menuFile->addSeparator();
        menuFile->addAction(actionSave);
        menuFile->addSeparator();
        menuFile->addAction(actionExit);
        menuWindow->addAction(actionControlWidget);
        menuEdit->addAction(actionUndo);
        menuEdit->addSeparator();
        menuEdit->addAction(actionDeleteEdge);
        menuSelect->addAction(actionSelectAll);

        retranslateUi(MainWindow);

        QMetaObject::connectSlotsByName(MainWindow);
    } // setupUi

    void retranslateUi(QMainWindow *MainWindow)
    {
        MainWindow->setWindowTitle(QApplication::translate("MainWindow", "GSM Viewer", 0));
        actionNew->setText(QApplication::translate("MainWindow", "New", 0));
        actionNew->setShortcut(QApplication::translate("MainWindow", "Ctrl+N", 0));
        actionOpen->setText(QApplication::translate("MainWindow", "Open", 0));
        actionOpen->setShortcut(QApplication::translate("MainWindow", "Ctrl+O", 0));
        actionExit->setText(QApplication::translate("MainWindow", "Exit", 0));
        actionControlWidget->setText(QApplication::translate("MainWindow", "Control Widget", 0));
        actionDeleteEdge->setText(QApplication::translate("MainWindow", "Delete Edge", 0));
        actionDeleteEdge->setShortcut(QApplication::translate("MainWindow", "Del", 0));
        actionSave->setText(QApplication::translate("MainWindow", "Save", 0));
        actionSave->setShortcut(QApplication::translate("MainWindow", "Ctrl+S", 0));
        actionUndo->setText(QApplication::translate("MainWindow", "Undo", 0));
#ifndef QT_NO_WHATSTHIS
        actionUndo->setWhatsThis(QString());
#endif // QT_NO_WHATSTHIS
        actionUndo->setShortcut(QApplication::translate("MainWindow", "Ctrl+Z", 0));
        actionSelectAll->setText(QApplication::translate("MainWindow", "Select All", 0));
        actionSelectAll->setShortcut(QApplication::translate("MainWindow", "Ctrl+A", 0));
        actionCut->setText(QApplication::translate("MainWindow", "Cut", 0));
        actionCut->setShortcut(QApplication::translate("MainWindow", "Ctrl+X", 0));
        actionCopy->setText(QApplication::translate("MainWindow", "Copy", 0));
        actionCopy->setShortcut(QApplication::translate("MainWindow", "Ctrl+C", 0));
        actionPaste->setText(QApplication::translate("MainWindow", "Paste", 0));
        actionPaste->setShortcut(QApplication::translate("MainWindow", "Ctrl+V", 0));
        actionOpenToAdd->setText(QApplication::translate("MainWindow", "Open To Add", 0));
        actionVoronoi->setText(QApplication::translate("MainWindow", "Voronoi", 0));
        actionVoronoiCut->setText(QApplication::translate("MainWindow", "Voronoi Cut", 0));
        actionModeBasic->setText(QApplication::translate("MainWindow", "Basic", 0));
        actionModeLayer->setText(QApplication::translate("MainWindow", "Graph", 0));
        actionModeSketch->setText(QApplication::translate("MainWindow", "Sketch", 0));
        actionShowArea->setText(QApplication::translate("MainWindow", "Show Area", 0));
        menuFile->setTitle(QApplication::translate("MainWindow", "File", 0));
        menuWindow->setTitle(QApplication::translate("MainWindow", "Window", 0));
        menuEdit->setTitle(QApplication::translate("MainWindow", "Edit", 0));
        menuSelect->setTitle(QApplication::translate("MainWindow", "Select", 0));
    } // retranslateUi

};

namespace Ui {
    class MainWindow: public Ui_MainWindow {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_MAINWINDOW_H
