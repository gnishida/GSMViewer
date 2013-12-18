#include "MainWindow.h"

MainWindow::MainWindow(QWidget *parent, Qt::WFlags flags) : QMainWindow(parent, flags) {
	ui.setupUi(this);

	// setup the docking widgets
	controlWidget = new ControlWidget(this);

	// register the menu handlers
	connect(ui.actionNew, SIGNAL(triggered()), this, SLOT(newRoad()));
	connect(ui.actionOpen, SIGNAL(triggered()), this, SLOT(openRoad()));
	connect(ui.actionSave, SIGNAL(triggered()), this, SLOT(saveRoad()));
	connect(ui.actionExit, SIGNAL(triggered()), this, SLOT(close()));
	connect(ui.actionUndo, SIGNAL(triggered()), this, SLOT(undo()));
	connect(ui.actionDeleteEdge, SIGNAL(triggered()), this, SLOT(deleteEdge()));
	connect(ui.actionControlWidget, SIGNAL(triggered()), this, SLOT(showControlWidget()));

	// setup the GL widget
	glWidget = new GLWidget(this);
	setCentralWidget(glWidget);
}

MainWindow::~MainWindow() {
}

void MainWindow::newRoad() {
	glWidget->editor->clear();
	glWidget->updateGL();
}

void MainWindow::openRoad() {
	QString filename = QFileDialog::getOpenFileName(this, tr("Open StreetMap file..."), "", tr("StreetMap Files (*.gsm)"));

	if (filename.isEmpty()) {
		printf("Unable to open file\n");
		return;
	}

	QApplication::setOverrideCursor(Qt::WaitCursor);
	glWidget->editor->openRoad(filename.toAscii().constData());
	glWidget->updateGL();
	QApplication::restoreOverrideCursor();
}

void MainWindow::saveRoad() {
	QString filename = QFileDialog::getSaveFileName(this, tr("Save StreetMap file..."), "", tr("StreetMap Files (*.gsm)"));

	if (filename.isEmpty()) {
		printf("Unable to open file\n");
		return;
	}

	QApplication::setOverrideCursor(Qt::WaitCursor);
	glWidget->editor->saveRoad(filename.toAscii().constData());
	QApplication::restoreOverrideCursor();
}

void MainWindow::undo() {
	glWidget->editor->undo();
}

void MainWindow::deleteEdge() {
	glWidget->editor->deleteEdge();
}

void MainWindow::showControlWidget() {
	controlWidget->show();
	addDockWidget(Qt::LeftDockWidgetArea, controlWidget);
}
