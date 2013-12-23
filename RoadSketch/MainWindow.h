#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QtGui/QMainWindow>
#include "ui_MainWindow.h"
#include "GLWidget.h"

class MainWindow : public QMainWindow {
Q_OBJECT

public:
	Ui::MainWindowClass ui;
	GLWidget* glWidget;

public:
	MainWindow(QWidget *parent = 0, Qt::WFlags flags = 0);
	~MainWindow();

public slots:
	void onNew();
	void onOpen();
	void onSave();
};

#endif // MAINWINDOW_H
