#pragma once

#include "Camera.h"
#include "RoadGraphEditor.h"
#include "RoadGraph.h"
#include "RoadGraphRenderer.h"
#include "Sketch.h"
#include "RoadGraphDatabase.h"
#include "ShadowRoadGraph.h"
#include <QGLWidget>
#include <QString>

class MainWindow;

class GLWidget : public QGLWidget {
public:
	static enum { MODE_DEFAULT = 0, MODE_SKETCH };
	static float MIN_Z;
	static float MAX_Z;

public:
	MainWindow* mainWin;
	Camera* camera;
	Sketch* sketch;
	RoadGraphDatabase* roadDB;
	QList<ShadowRoadGraph*> shadowRoads;
	RoadGraphRenderer* renderer;
	QPoint lastPos;
	QVector2D last2DPos;
	int mode;

public:
	GLWidget(MainWindow *parent);
	~GLWidget();

	void drawScene();

public:
	void keyPressEvent(QKeyEvent* e);
	void keyReleaseEvent(QKeyEvent* e);

protected:
	void mousePressEvent(QMouseEvent *e);
	void mouseReleaseEvent(QMouseEvent *e);
	void mouseMoveEvent(QMouseEvent *e);
	void initializeGL();
	void resizeGL(int width, int height);
	void paintGL();

private:
	void mouseTo2D(int x, int y, QVector2D *result);
	void searchSimilariRoads(RoadGraph* roads);
};

