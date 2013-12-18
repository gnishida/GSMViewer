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
	MainWindow* mainWin;
	Camera* camera;
	RoadGraphEditor* editor;
	RoadGraphRenderer* renderer;
	QPoint lastPos;

	// key status
	bool shiftPressed;
	bool controlPressed;
	bool altPressed;

public:
	GLWidget(MainWindow *parent);
	~GLWidget();

	void drawScene();

protected:
	void mousePressEvent(QMouseEvent *e);
	void mouseReleaseEvent(QMouseEvent *e);
	void mouseMoveEvent(QMouseEvent *e);
	void keyPressEvent(QKeyEvent* e);
	void keyReleaseEvent(QKeyEvent* e);
	void initializeGL();
	void resizeGL(int width, int height);
	void paintGL();

private:
	void mouseTo2D(int x, int y, QVector2D *result);
};

