#include "GLWidget.h"
#include "MainWindow.h"
#include "GraphUtil.h"
#include <gl/GLU.h>
#include <vector>

float GLWidget::MIN_Z = 150.0f;
float GLWidget::MAX_Z = 5760.0f;
//float GLWidget::MAX_Z = 11520.0f;
//float GLWidget::MAX_Z = 2880.0f;

GLWidget::GLWidget(MainWindow* mainWin) : QGLWidget(QGLFormat(QGL::SampleBuffers), (QWidget*)mainWin) {
	this->mainWin = mainWin;

	sketch = new Sketch();
	//roadDB = new RoadGraphDatabase("osm/example1.gsm");
	//roadDB = new RoadGraphDatabase("osm/3x3/paris.gsm");
	roadDB = new RoadGraphDatabase("osm/3x3_simplified/paris.gsm");
	//roadDB = new RoadGraphDatabase("osm/3x3_simplified/round.gsm");
	//roadDB = new RoadGraphDatabase("osm/3x3_simplified/sketch_match3.gsm");

	// set up the camera
	camera = new Camera();
	camera->setLookAt(0.0f, 0.0f, 0.0f);
	camera->setTranslation(0.0f, 0.0f, MIN_Z);

	// initialize the width and others
	sketch->setZ(MIN_Z);

	mode = MODE_DEFAULT;
}

GLWidget::~GLWidget() {
}

void GLWidget::drawScene() {
	// draw the road graph
	sketch->generateMesh();
	renderer->render(sketch->renderables);

	// draw the shadow roads
	for (int i = 0; i < shadowRoads.size(); i++) {
		shadowRoads[i]->generateMesh();
		renderer->render(shadowRoads[i]->roads->renderables);
	}
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Event handlers

void GLWidget::keyPressEvent(QKeyEvent *e) {
}

void GLWidget::keyReleaseEvent(QKeyEvent* e) {
}

void GLWidget::mousePressEvent(QMouseEvent *e) {
	this->setFocus();

	lastPos = e->pos();
	mouseTo2D(e->x(), e->y(), &last2DPos);
	if (e->buttons() & Qt::LeftButton) {

		sketch->startLine(last2DPos, camera->dz * 0.03f);
		/*
		RoadVertexDesc v1_desc;
		if (!GraphUtil::getVertex(sketch, last2DPos, camera->dz * 0.03f, v1_desc)) {
			RoadVertex* v1 = new RoadVertex(last2DPos);
			v1_desc = boost::add_vertex(sketch->graph);
			sketch->graph[v1_desc] = v1;
		}

		RoadVertex* v2 = new RoadVertex(last2DPos);
		RoadVertexDesc v2_desc = boost::add_vertex(sketch->graph);
		sketch->graph[v2_desc] = v2;

		sketch->currentVertex = v2_desc;
		sketch->currentEdge = GraphUtil::addEdge(sketch, v1_desc, v2_desc, 1, 1, false);
		*/

		mode = MODE_SKETCH;
	} else {
		mode = MODE_DEFAULT;
	}

	updateGL();
}

void GLWidget::mouseReleaseEvent(QMouseEvent *e) {
	float dx = (float)(e->x() - lastPos.x());
	float dy = (float)(e->y() - lastPos.y());
	
	lastPos = e->pos();

	if (mode == MODE_SKETCH) {
		sketch->finalizeLine(camera->dz * 0.01f, camera->dz * 0.03f);

		// search similar roads
		roadDB->findSimilarRoads(sketch, 1, shadowRoads);

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

	if (e->buttons() & Qt::LeftButton) {
		sketch->addPointToLine(pos);
		/*
		if ((pos - sketch->graph[sketch->currentVertex]->pt).length() > 3.0f) {
			sketch->graph[sketch->currentVertex]->pt = pos;
			sketch->graph[sketch->currentEdge]->polyLine.push_back(pos);
			sketch->setModified();
		}
		*/
	} else if (e->buttons() & Qt::MidButton) {   // Shift the camera
		camera->changeXYZTranslation(-dx * camera->dz * 0.001f, dy * camera->dz * 0.001f, 0);
	} else if (e->buttons() & Qt::RightButton) { // Zoom the camera
		setCursor(Qt::SizeVerCursor);

		camera->changeXYZTranslation(0, 0, -dy * camera->dz * 0.02f);
		if (camera->dz < MIN_Z) camera->dz = MIN_Z;
		if (camera->dz > MAX_Z) camera->dz = MAX_Z;

		// tell the Z coordinate to the road graph so that road graph updates rendering related variables.
		sketch->setZ(camera->dz);

		lastPos = e->pos();

		//mainWin->ui.statusBar->showMessage(QString("Z: %1").arg(camera->dz));
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
	gluProject(0, 0, 1.0f, modelview, projection, viewport, &origX, &origY, &origZ);

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

