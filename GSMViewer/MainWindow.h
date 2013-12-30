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
	QLabel* statusLebelL;
	QLabel* statusLabelR;

public:
	MainWindow(QWidget *parent = 0, Qt::WFlags flags = 0);
	~MainWindow();

	void setStatusMessage(QString message1, QString message2);

protected:
	void keyPressEvent(QKeyEvent* e);
	void keyReleaseEvent(QKeyEvent* e);

public slots:
	void onMenuMode();
	void onMenuEdit();
	void onNew();
	void onOpen();
	void onOpenToAdd();
	void onSave();
	void onModeBasic();
	void onModeLayer();
	void onModeSketch();
	void onSelectAll();
	void onUndo();
	void onCut();
	void onCopy();
	void onPaste();
	void onDeleteEdge();
	void onVoronoi();
	void onVoronoiCut();
	void onShowArea();
	void onShowControlWidget();
};

#endif // MAINWINDOW_H
