#pragma once

#include "Camera.h"
#include "RoadGraphEditor.h"
#include "RoadGraph.h"
#include "RoadGraphRenderer.h"
#include <QGLWidget>
#include <QString>

class MainWindow;

class GLWidget : public QGLWidget {
public:
	static float MIN_Z;
	static float MAX_Z;

public:
	MainWindow* mainWin;
	Camera* camera;
	RoadGraphEditor* editor;
	RoadGraphRenderer* renderer;
	QPoint lastPos;
	QVector2D last2DPos;
	int city_id;

	bool showArea;

	// key status
	bool shiftPressed;
	bool controlPressed;
	bool altPressed;
	bool keyXPressed;

public:
	GLWidget(MainWindow *parent);
	~GLWidget();

	void drawScene();
	void showStatusMessage();

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
	bool hitTest(const AbstractArea& area, const QVector2D& pt);
	bool hitTestDistortionPoint(const AbstractArea& area, const QVector2D& pt);
	bool hitTestResizingPoint(const AbstractArea& area, const QVector2D& pt);
};

