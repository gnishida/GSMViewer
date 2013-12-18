#include "ControlWidget.h"
#include "MainWindow.h"
#include "GraphUtil.h"

ControlWidget::ControlWidget(MainWindow* mainWin) : QDockWidget("Control Widget", (QWidget*)mainWin) {
	this->mainWin = mainWin;
	ui.setupUi(this);

	// set up the UI
	ui.lineEditSimplifyThreshold->setText("10");

	// register the event handlers
	connect(ui.pushButtonSimplify, SIGNAL(clicked()), this, SLOT(simplify()));

	hide();
}

void ControlWidget::setRoadVertex(RoadVertex* selectedVertex) {

}

void ControlWidget::setRoadEdge(RoadEdge* selectedEdge) {
	QString type("");
	QString numLanes("");
	QString oneWay("");

	if (selectedEdge != NULL) {
		type.setNum(selectedEdge->type);
		numLanes.setNum(selectedEdge->lanes);
		if (selectedEdge->oneWay) {
			oneWay = "Yes";
		} else {
			oneWay = "No";
		}
	}

	ui.lineEditType->setText(type);
	ui.lineEditLanes->setText(numLanes);
	ui.lineEditOneWay->setText(oneWay);
}

/**
 * Event handler for button [simplify]
 */
void ControlWidget::simplify() {
	float threshold = ui.lineEditSimplifyThreshold->text().toFloat();

	GraphUtil::simplify(mainWin->glWidget->editor->roads, threshold);
	GraphUtil::clean(mainWin->glWidget->editor->roads);
	mainWin->glWidget->updateGL();
}
