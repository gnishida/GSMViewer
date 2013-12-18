#include "RoadGraphRenderer.h"
#include <QtOpenGL>

RoadGraphRenderer::RoadGraphRenderer() {
}

//void RoadGraphRenderer::render(std::vector<Renderable>& renderables) {
void RoadGraphRenderer::render(Renderable* renderable) {
	if (renderable->type == GL_LINES || renderable->type == GL_LINE_STRIP) {
		glLineWidth(renderable->size);
	}

	if (renderable->type == GL_POINTS) {
		glPointSize(renderable->size);
	}

	glBegin(renderable->type);
	for (int j = 0; j < renderable->vertices.size(); ++j) {
		glColor3f(renderable->vertices[j].color[0], renderable->vertices[j].color[1], renderable->vertices[j].color[2]);
		glNormal3f(renderable->vertices[j].normal[0], renderable->vertices[j].normal[1], renderable->vertices[j].normal[2]);
		glVertex3f(renderable->vertices[j].location[0], renderable->vertices[j].location[1], renderable->vertices[j].location[2]);
	}
	glEnd();
}

