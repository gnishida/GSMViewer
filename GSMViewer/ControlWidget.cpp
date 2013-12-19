#include "ControlWidget.h"
#include "MainWindow.h"
#include "GLWidget.h"
#include "RoadGraphEditor.h"
#include "GraphUtil.h"

ControlWidget::ControlWidget(MainWindow* mainWin) : QDockWidget("Control Widget", (QWidget*)mainWin) {
	this->mainWin = mainWin;

	// set up the UI
	ui.setupUi(this);
	ui.lineEditSimplifyThreshold->setText("10");
	ui.lineEditRemoveShortDeadendThreshold->setText("10");

	// register the event handlers
	connect(ui.pushButtonCopy, SIGNAL(clicked()), this, SLOT(copy()));
	connect(ui.pushButtonClean, SIGNAL(clicked()), this, SLOT(clean()));
	connect(ui.pushButtonSimplify, SIGNAL(clicked()), this, SLOT(simplify()));
	connect(ui.pushButtonRemoveShortDeadend, SIGNAL(clicked()), this, SLOT(removeShortDeadend()));

	hide();
}

void ControlWidget::setRoadVertex(RoadVertexDesc vertexDesc, RoadVertex* selectedVertex) {
	QString desc("");
	QString location("");
	QString neighbors("");

	if (selectedVertex != NULL) {
		desc.setNum(vertexDesc);

		location = QString("(%1, %2)").arg(selectedVertex->pt.x(), 0, 'f', 0).arg(selectedVertex->pt.y(), 0, 'f', 0);

		std::vector<RoadVertexDesc> n = GraphUtil::getNeighbors(mainWin->glWidget->editor->roads, vertexDesc);
		for (int i = 0; i < n.size(); i++) {
			QString str;
			str.setNum(n[i]);

			neighbors += str;
			if (i < n.size() - 1) neighbors += ",";
		}
	}

	ui.lineEditVertexDesc->setText(desc);
	ui.lineEditVertexPos->setText(location);
	ui.lineEditVertexNeighbors->setText(neighbors);
}

/**
 * Display the selected edge information.
 */
void ControlWidget::setRoadEdge(RoadEdge* selectedEdge) {
	QString type("");
	QString numLanes("");
	QString oneWay("");

	if (selectedEdge != NULL) {
		switch (selectedEdge->type) {
		case 3:
			type = "Highway";
			break;
		case 2:
			type = "Avenue";
			break;
		case 1:
			type = "Street";
			break;
		}

		numLanes.setNum(selectedEdge->lanes);

		if (selectedEdge->oneWay) {
			oneWay = "Yes";
		} else {
			oneWay = "No";
		}
	}

	ui.lineEditEdgeType->setText(type);
	ui.lineEditEdgeLanes->setText(numLanes);
	ui.lineEditEdgeOneWay->setText(oneWay);
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////
// Event handlers

/**
 * Event handler for button [Copy]
 */
void ControlWidget::copy() {
	GraphUtil::extractRoads(mainWin->glWidget->editor->roads, 6);

	mainWin->glWidget->updateGL();
}

/**
 * Event handler for button [Clean]
 */
void ControlWidget::clean() {
	GraphUtil::clean(mainWin->glWidget->editor->roads);

	mainWin->glWidget->updateGL();
}

/**
 * Event handler for button [Simplify]
 */
void ControlWidget::simplify() {
	float threshold = ui.lineEditSimplifyThreshold->text().toFloat();
	mainWin->glWidget->editor->simplify(threshold);

	mainWin->glWidget->updateGL();
}

/**
 * Event handler for button [Remove Short Dead-End]
 */
void ControlWidget::removeShortDeadend() {
	float threshold = ui.lineEditRemoveShortDeadendThreshold->text().toFloat();

	GraphUtil::removeShortDeadend(mainWin->glWidget->editor->roads, threshold);
	mainWin->glWidget->updateGL();
}