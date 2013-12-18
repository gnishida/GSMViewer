#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QtGui/QMainWindow>
#include "ui_MainWindow.h"
#include "ControlWidget.h"
#include "GLWidget.h"

class MainWindow : public QMainWindow {
Q_OBJECT

public:
	Ui::MainWindow ui;
	ControlWidget* controlWidget;
	GLWidget* glWidget;

public:
	MainWindow(QWidget *parent = 0, Qt::WFlags flags = 0);
	~MainWindow();

public slots:
	void newRoad();
	void openRoad();
	void saveRoad();
	void undo();
	void deleteEdge();
	void showControlWidget();
};

#endif // MAINWINDOW_H
