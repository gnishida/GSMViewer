#include "MainWindow.h"

MainWindow::MainWindow(QWidget *parent, Qt::WFlags flags) : QMainWindow(parent, flags) {
	ui.setupUi(this);

	// setup the GL widget
	glWidget = new GLWidget(this);
	setCentralWidget(glWidget);
}

MainWindow::~MainWindow() {

}
