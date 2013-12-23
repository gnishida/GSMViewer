#include "MainWindow.h"
#include "GraphUtil.h"

MainWindow::MainWindow(QWidget *parent, Qt::WFlags flags) : QMainWindow(parent, flags) {
	ui.setupUi(this);

	connect(ui.actionNew, SIGNAL(triggered()), this, SLOT(onNew()));
	connect(ui.actionOpen, SIGNAL(triggered()), this, SLOT(onOpen()));
	connect(ui.actionSave, SIGNAL(triggered()), this, SLOT(onSave()));

	// setup the GL widget
	glWidget = new GLWidget(this);
	setCentralWidget(glWidget);
}

MainWindow::~MainWindow() {
}

void MainWindow::onNew() {
	glWidget->sketch->clear();
	glWidget->updateGL();
}

void MainWindow::onOpen() {
	QString filename = QFileDialog::getOpenFileName(this, tr("Open StreetMap file..."), "", tr("StreetMap Files (*.gsm)"));

	if (filename.isEmpty()) {
		printf("Unable to open file\n");
		return;
	}

	QApplication::setOverrideCursor(Qt::WaitCursor);
	GraphUtil::loadRoads(glWidget->sketch, filename);

	// search similar roads
	glWidget->roadDB->findSimilarRoads(glWidget->sketch, 1, glWidget->shadowRoads);

	glWidget->updateGL();
	QApplication::restoreOverrideCursor();
}

void MainWindow::onSave() {
	QString filename = QFileDialog::getSaveFileName(this, tr("Save StreetMap file..."), "", tr("StreetMap Files (*.gsm)"));

	if (filename.isEmpty()) {
		printf("Unable to open file\n");
		return;
	}

	QApplication::setOverrideCursor(Qt::WaitCursor);
	GraphUtil::saveRoads(glWidget->sketch, filename);
	QApplication::restoreOverrideCursor();
}