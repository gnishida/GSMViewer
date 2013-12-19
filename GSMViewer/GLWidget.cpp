#include "GLWidget.h"
#include "MainWindow.h"
#include "GraphUtil.h"
#include <gl/GLU.h>
#include <vector>

float GLWidget::MIN_Z = 150.0f;
//float GLWidget::MAX_Z = 11520.0f;
float GLWidget::MAX_Z = 2880.0f;

GLWidget::GLWidget(MainWindow* mainWin) : QGLWidget(QGLFormat(QGL::SampleBuffers), (QWidget*)mainWin) {
	this->mainWin = mainWin;

	//roads = new RoadGraph();
	editor = new RoadGraphEditor();
	renderer = new RoadGraphRenderer();

	// set up the camera
	camera = new Camera();
	camera->setLookAt(0.0f, 0.0f, 0.0f);
	camera->setTranslation(0.0f, 0.0f, MAX_Z);

	// initialize the width and others
	editor->roads->setZ(MAX_Z);

	// initialize the key status
	shiftPressed = false;
	controlPressed = false;
	altPressed = false;

	//selecting = false;
	mode = MODE_DEFAULT;
}

GLWidget::~GLWidget() {
}

void GLWidget::drawScene() {
	// draw the road graph
	editor->roads->generateMesh();
	renderer->render(editor->roads->renderable);

	// define the height for other items
	float height = (float)((int)(camera->dz * 0.012f)) * 0.1f * 1.5f;

	// draw the select area
	if (editor->bbox != NULL) {
		renderer->renderBBox(*editor->bbox, height);
	}

	// draw the selected vertex
	if (editor->selectedVertex != NULL) {
		renderer->renderPoint(editor->selectedVertex->pt, height);
	}

	// draw the selected edge
	if (editor->selectedEdge != NULL) {
		renderer->renderPolyline(editor->selectedEdge->polyLine, height);
	}
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Event handlers

void GLWidget::keyPressEvent(QKeyEvent *e) {
	mainWin->onMenuEdit();

	shiftPressed = false;
	controlPressed = false;
	altPressed = false;

	switch (e->key()) {
	case Qt::Key_Shift:
		shiftPressed = true;
		break;
	case Qt::Key_Control:
		controlPressed = true;
		break;
	case Qt::Key_Alt:
		altPressed = true;
		break;
	/*
	case Qt::Key_Delete:
		if (editor->deleteEdge()) {
			updateGL();
		}
		break;
	case Qt::Key_D:
		if (controlPressed) {
			editor->undo();
		}
	*/
	}
}

void GLWidget::keyReleaseEvent(QKeyEvent* e) {
	if (e->isAutoRepeat()) {
		e->ignore();
		return;
	}

	switch (e->key()) {
	case Qt::Key_Shift:
		shiftPressed = false;
		break;
	case Qt::Key_Control:
		controlPressed = false;
		break;
	case Qt::Key_Alt:
		altPressed = false;
	}
}

void GLWidget::mousePressEvent(QMouseEvent *e) {
	this->setFocus();

	lastPos = e->pos();
	mouseTo2D(e->x(), e->y(), &last2DPos);

	if (e->buttons() & Qt::LeftButton) {
		//mainWin->ui.statusBar->showMessage(QString("clicked (%1, %2)").arg(pos.x()).arg(pos.y()));

		if (hitTest(editor->bbox, last2DPos)) {
			mode = MODE_MOVING_SELECTION;
		} else {
			// if the vertex is close to the point, the vertex is selected
			if (editor->selectVertex(last2DPos)) {
				mode = MODE_VERTEX_SELECTED;
			} else if (editor->selectEdge(last2DPos)) {
				mode = MODE_EDGE_SELECTED;
			} else {
				// if neither a vertex nor a edge is selected, then the selection mode starts
				mode = MODE_RESIZING_SELECTION;

				editor->startSelection(last2DPos);
			}
		}

		mainWin->controlWidget->setRoadVertex(editor->selectedVertexDesc, editor->selectedVertex);
		mainWin->controlWidget->setRoadEdge(editor->selectedEdge);

		updateGL();
	}
}

void GLWidget::mouseReleaseEvent(QMouseEvent *e) {
	float dx = (float)(e->x() - lastPos.x());
	float dy = (float)(e->y() - lastPos.y());
	
	lastPos = e->pos();

	if (mode == MODE_VERTEX_SELECTED) {
		if (controlPressed) {
			float snap_threshold = camera->dz * 0.03f;
			editor->stopMovingSelectedVertex(snap_threshold);
		} else {
			editor->stopMovingSelectedVertex();
		}
	} else if (mode == MODE_RESIZING_SELECTION) {
		editor->endSelection();
		mode = MODE_DEFAULT;
	}

	e->ignore();

	setCursor(Qt::ArrowCursor);
	updateGL();
}

void GLWidget::mouseMoveEvent(QMouseEvent *e) {
	float dx = (float)(e->x() - lastPos.x());
	float dy = (float)(e->y() - lastPos.y());
	lastPos = e->pos();
	//float camElevation = camera->getCamElevation();

	QVector2D pos;
	mouseTo2D(e->x(), e->y(), &pos);
	float dx2D = pos.x() - last2DPos.x();
	float dy2D = pos.y() - last2DPos.y();
	last2DPos = pos;

	if (e->buttons() & Qt::LeftButton) {
		if (mode == MODE_MOVING_SELECTION) {
			editor->moveSelection(dx2D, dy2D);
		} else if (mode == MODE_VERTEX_SELECTED) {
			if (controlPressed) {
				float snap_threshold = camera->dz * 0.03f;

				// move the selected vertex
				editor->moveSelectedVertex(last2DPos, snap_threshold);
			} else {
				// move the selected vertex
				editor->moveSelectedVertex(last2DPos);
			}
		} else if (mode == MODE_RESIZING_SELECTION) {
			// update the selection box
			editor->updateSelection(last2DPos);
		}
	} else if (e->buttons() & Qt::MidButton) {   // Shift the camera
		camera->changeXYZTranslation(-dx * camera->dz * 0.001f, dy * camera->dz * 0.001f, 0);
	} else if (e->buttons() & Qt::RightButton) { // Zoom the camera
		setCursor(Qt::SizeVerCursor);

		camera->changeXYZTranslation(0, 0, -dy * camera->dz * 0.02f);
		if (camera->dz < MIN_Z) camera->dz = MIN_Z;
		if (camera->dz > MAX_Z) camera->dz = MAX_Z;

		// tell the Z coordinate to the road graph so that road graph updates rendering related variables.
		editor->roads->setZ(camera->dz);

		lastPos = e->pos();

		mainWin->ui.statusBar->showMessage(QString("Z: %1").arg(camera->dz));
	}

	last2DPos = pos;

	updateGL();
}

void GLWidget::initializeGL() {
	qglClearColor(QColor(233, 229, 220));
	glClearDepth(1.0f);

	glTexEnvf( GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE );
	glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);

	glHint(GL_POINT_SMOOTH_HINT, GL_NICEST);
	glHint(GL_LINE_SMOOTH_HINT, GL_NICEST);
	glHint(GL_POLYGON_SMOOTH_HINT, GL_NICEST);

	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LEQUAL);

	//---- lighting ----
	glDisable(GL_LIGHTING);
	glEnable(GL_LIGHT0);

	static GLfloat lightPosition[4] = { 0.0f, 0.0f, 100.0f, 0.0f };
	static GLfloat lightAmbient[4] = { 0.0f, 0.0f, 0.0f, 1.0f };
	static GLfloat lightDiffuse[4] = { 0.75f, 0.75f, 0.5f, 1.0f };

	glEnable(GL_COLOR_MATERIAL);	
	glColorMaterial(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE);	
	glLightfv(GL_LIGHT0, GL_POSITION, lightPosition);
	glLightfv(GL_LIGHT0, GL_AMBIENT, lightAmbient);
	glLightfv(GL_LIGHT0, GL_DIFFUSE, lightDiffuse);
	static GLfloat materialSpecular[4] = { 0.0f, 0.0f, 0.0f, 1.0f };
	static GLfloat materialEmission[4] = { 0.0f, 0.0f, 0.0f, 1.0f };	
	glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT, materialSpecular);
	glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, materialEmission);
}

void GLWidget::resizeGL(int width, int height) {
	height = height ? height : 1;

	glViewport(0, 0, (GLint)width, (GLint)height);

	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(60.0f, (GLfloat)width/(GLfloat)height, 5.0f, 90000.0f);

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	camera->applyCamTransform();
}

void GLWidget::paintGL() {
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LESS);
	glDisable(GL_TEXTURE_2D);

	glMatrixMode(GL_MODELVIEW);
	camera->applyCamTransform();	

	drawScene();	
}

/**
 * Convert the screen space coordinate (x, y) to the model space coordinate.
 */
void GLWidget::mouseTo2D(int x,int y, QVector2D *result) {
	GLint viewport[4];
	GLdouble modelview[16];
	GLdouble projection[16];

	// retrieve the matrices
	glGetDoublev(GL_MODELVIEW_MATRIX, modelview);
	glGetDoublev(GL_PROJECTION_MATRIX, projection);
	glGetIntegerv(GL_VIEWPORT, viewport);

	// retrieve the projected z-buffer of the origin
	GLdouble origX, origY, origZ;
	gluProject(0, 0, 0, modelview, projection, viewport, &origX, &origY, &origZ);

	// set up the projected point
	GLfloat winX = (float)x;
	GLfloat winY = (float)viewport[3] - (float)y;
	GLfloat winZ = origZ;
	
	// unproject the image plane coordinate to the model space
	GLdouble posX, posY, posZ;
	gluUnProject(winX, winY, winZ, modelview, projection, viewport, &posX, &posY, &posZ);

	result->setX(posX);
	result->setY(posY);
}

bool GLWidget::hitTest(BBox* bbox, const QVector2D& pt) {
	if (bbox == NULL) return false;

	if (!bbox->contains(pt)) return false;

	float dx = bbox->maxPt.x() - bbox->minPt.x();
	float dy = bbox->maxPt.y() - bbox->minPt.y();

	if (pt.x() < bbox->minPt.x() + dx * 0.1f) return true;
	if (pt.x() > bbox->maxPt.x() - dx * 0.1f) return true;
	if (pt.y() < bbox->minPt.y() + dy * 0.1f) return true;
	if (pt.y() > bbox->maxPt.y() - dy * 0.1f) return true;

	return false;
}
