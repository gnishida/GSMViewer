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
	ui.horizontalSliderInterpolationRatio->setMinimum(0);
	ui.horizontalSliderInterpolationRatio->setMaximum(20);
	ui.horizontalSliderInterpolationRatio->setSingleStep(1);

	// register the event handlers
	connect(ui.pushButtonClean, SIGNAL(clicked()), this, SLOT(clean()));
	connect(ui.pushButtonSimplify, SIGNAL(clicked()), this, SLOT(simplify()));
	connect(ui.pushButtonReduce, SIGNAL(clicked()), this, SLOT(reduce()));
	connect(ui.pushButtonRemoveShortDeadend, SIGNAL(clicked()), this, SLOT(removeShortDeadend()));
	connect(ui.pushButtonExtractAvenues, SIGNAL(clicked()), this, SLOT(extractAvenues()));
	connect(ui.pushButtonConnectRoads, SIGNAL(clicked()), this, SLOT(connectRoads()));
	connect(ui.pushButtonInterpolation, SIGNAL(clicked()), this, SLOT(interpolation()));
	connect(ui.horizontalSliderInterpolationRatio, SIGNAL(valueChanged(int)), this, SLOT(interpolationRatio(int)));
	connect(ui.pushButtonFinalizeInterpolation, SIGNAL(clicked()), this, SLOT(finalizeInterpolation()));

	hide();
}

void ControlWidget::setRoadVertex(RoadVertexDesc vertexDesc, RoadVertexPtr selectedVertex) {
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
void ControlWidget::setRoadEdge(RoadEdgePtr selectedEdge) {
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
 * Event handler for button [Reduce]
 */
void ControlWidget::reduce() {
	mainWin->glWidget->editor->reduce();

	mainWin->glWidget->updateGL();
}

/**
 * Event handler for button [Remove Short Dead-End]
 */
void ControlWidget::removeShortDeadend() {
	float threshold = ui.lineEditRemoveShortDeadendThreshold->text().toFloat();
	mainWin->glWidget->editor->removeShortDeadend(threshold);

	mainWin->glWidget->updateGL();
}

/**
 * Event handler for button [Extract Avenues]
 */
void ControlWidget::extractAvenues() {
	GraphUtil::extractRoads(mainWin->glWidget->editor->roads, 6);

	mainWin->glWidget->updateGL();
}

/**
 * Event handler for button [Connect Roads]
 */
void ControlWidget::connectRoads() {
	mainWin->glWidget->editor->connectRoads();

	mainWin->glWidget->updateGL();
}

/**
 * Event handler for button [Interpolation]
 */
void ControlWidget::interpolation() {
	mainWin->glWidget->editor->interpolate();

	mainWin->glWidget->updateGL();
}

void ControlWidget::interpolationRatio(int ratio) {
	mainWin->glWidget->editor->showInterpolatedRoads(ratio);

	mainWin->glWidget->updateGL();
}

void ControlWidget::finalizeInterpolation() {
	int ratio = ui.horizontalSliderInterpolationRatio->value();
	mainWin->glWidget->editor->finalizeInterpolation(ratio);

	mainWin->glWidget->updateGL();
}
