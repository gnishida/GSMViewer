#include "RoadGraphRenderer.h"
#include <QtOpenGL>

RoadGraphRenderer::RoadGraphRenderer() {
}

//void RoadGraphRenderer::render(std::vector<Renderable>& renderables) {
void RoadGraphRenderer::render(Renderable* renderable) {
	if (renderable->type == GL_LINE_STIPPLE) {
		glEnable(GL_LINE_STIPPLE);
		glLineStipple(1 , 0xF0F0);
		glLineWidth(renderable->size);
	} else {
		glDisable(GL_LINE_STIPPLE);
	}

	if (renderable->type == GL_LINES || renderable->type == GL_LINE_STRIP) {
		glLineWidth(renderable->size);
	}

	if (renderable->type == GL_POINTS) {
		glPointSize(renderable->size);
	}

	if (renderable->type != GL_LINE_STIPPLE) {
		glBegin(renderable->type);
	} else {
		glBegin(GL_LINE_STRIP);
	}
	for (int j = 0; j < renderable->vertices.size(); ++j) {
		glColor3f(renderable->vertices[j].color[0], renderable->vertices[j].color[1], renderable->vertices[j].color[2]);
		glNormal3f(renderable->vertices[j].normal[0], renderable->vertices[j].normal[1], renderable->vertices[j].normal[2]);
		glVertex3f(renderable->vertices[j].location[0], renderable->vertices[j].location[1], renderable->vertices[j].location[2]);
	}
	glEnd();
}

void RoadGraphRenderer::renderArea(const AbstractArea& area, float height) {
	std::vector<Renderable> renderables;
	renderables.push_back(Renderable(GL_LINE_STIPPLE, 3.0f));
	renderables.push_back(Renderable(GL_POINTS, 10.0f));

	Vertex v;

	v.color[0] = 0.0f;
	v.color[1] = 0.0f;
	v.color[2] = 1.0f;
	v.normal[0] = 0.0f;
	v.normal[1] = 0.0f;
	v.normal[2] = 1.0f;

	for (int i = 0; i < area.polyline().size(); i++) {
		v.location[0] = area.polyline()[i].x();
		v.location[1] = area.polyline()[i].y();
		v.location[2] = height;
		renderables[0].vertices.push_back(v);
		renderables[1].vertices.push_back(v);
	}

	v.location[0] = area.polyline()[0].x();
	v.location[1] = area.polyline()[0].y();
	v.location[2] = height;
	renderables[0].vertices.push_back(v);

	// distortion control point
	v.color[0] = 1.0f;
	v.color[1] = 0.0f;
	v.color[2] = 0.0f;
	v.location[0] = area.distortionPt().x();
	v.location[1] = area.distortionPt().y();
	v.location[2] = height;
	renderables[1].vertices.push_back(v);

	for (int i = 0; i < renderables.size(); i++) {
		render(&renderables[i]);
	}
}

void RoadGraphRenderer::renderPoint(const QVector2D& pt, float height) {
	Renderable renderable(GL_POINTS, 10.0f);

	Vertex v;

	v.location[0] = pt.x();
	v.location[1] = pt.y();
	v.location[2] = height;
	v.color[0] = 0.0f;
	v.color[1] = 0.0f;
	v.color[2] = 1.0f;
	v.normal[0] = 0.0f;
	v.normal[1] = 0.0f;
	v.normal[2] = 1.0f;

	renderable.vertices.push_back(v);

	render(&renderable);
}

void RoadGraphRenderer::renderPolyline(std::vector<QVector2D>& polyline, float height) {
	std::vector<Renderable> renderables;
	renderables.push_back(Renderable(GL_LINE_STRIP, 3.0f));
	renderables.push_back(Renderable(GL_POINTS, 10.0f));
	
	Vertex v;

	// add lines
	for (int i = 0; i < polyline.size(); i++) {
		v.location[0] = polyline[i].x();
		v.location[1] = polyline[i].y();
		v.location[2] = height;
		v.color[0] = 0.0f;
		v.color[1] = 0.0f;
		v.color[2] = 1.0f;
		v.normal[0] = 0.0f;
		v.normal[1] = 0.0f;
		v.normal[2] = 1.0f;

		renderables[0].vertices.push_back(v);
		renderables[1].vertices.push_back(v);
	}

	for (int i = 0; i < renderables.size(); i++) {
		render(&renderables[i]);
	}
}

