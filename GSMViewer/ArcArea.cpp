#include "ArcArea.h"

ArcArea::ArcArea(const QVector2D& leftPt, const QVector2D& rightPt, float radius, float arc_len) {
	this->leftPt = leftPt;
	this->rightPt = rightPt;
	this->radius = radius;
	this->arc_len = arc_len;
}

ArcArea::~ArcArea() {
}

bool ArcArea::contains(const QVector2D& pt) const {
	// compute the center of the arc
	QVector2D center = (leftPt + rightPt) / 2.0f;
	center.setX(center.x() - radius);

	// compute the width
	float width = rightPt.x() - leftPt.x();

	// compute the radian
	float rad = arc_len / radius;

	if ((pt - center).length() < radius - width / 2.0f) return false;
	if ((pt - center).length() > radius + width / 2.0f) return false;

	float angle = atan2f((pt - center).y(), (pt - center).x());
	if (angle < -rad / 2.0f || angle > rad / 2.0f) return false;

	return true;
}

QVector2D ArcArea::midPt() const {
	return (leftPt + rightPt) / 2.0f;
}

float ArcArea::dx() const {
	return rightPt.x() - leftPt.x();
}

float ArcArea::dy() const {
	return arc_len;
}

void ArcArea::translate(float x, float y) {
	leftPt.setX(leftPt.x() + x);
	leftPt.setY(leftPt.y() + y);
	rightPt.setX(rightPt.x() + x);
	rightPt.setY(rightPt.y() + y);
}

bool ArcArea::hitTest(const QVector2D& pt) const {
	// compute the center of the arc
	QVector2D center = (leftPt + rightPt) / 2.0f;
	center.setX(center.x() - radius);

	// compute the width
	float width = rightPt.x() - leftPt.x();

	// compute the radian
	float rad = arc_len / radius;

	if ((pt - center).length() < radius - width / 2.0f * 1.1f) return false;
	if ((pt - center).length() > radius + width / 2.0f * 1.1f) return false;

	float angle = atan2f((pt - center).y(), (pt - center).x());
	if (angle < -rad / 2.0f * 1.1f || angle > rad / 2.0f * 1.1f) return false;

	return true;
}

bool ArcArea::hitTestDistortionPoint(const QVector2D& pt) const {
	// compute the radian
	float rad = arc_len / radius;

	if (fabs(pt.x() - distortionPt().x()) < dx() * 0.1f && fabs(pt.y() - distortionPt().y()) < dy() * 0.1f) return true;
	else return false;
}

bool ArcArea::hitTestResizingPoint(const QVector2D& pt) const {
	// compute the radian
	float rad = arc_len / radius;

	// compute the width
	float width = rightPt.x() - leftPt.x();

	QVector2D resizingPt = (leftPt + rightPt) / 2.0f;
	resizingPt.setX(resizingPt.x() - radius);
	resizingPt.setX(resizingPt.x() + cosf(-rad / 2.0f) * (radius + width / 2.0f));
	resizingPt.setY(resizingPt.y() + sinf(-rad / 2.0f) * (radius + width / 2.0f));

	if (fabs(pt.x() - resizingPt.x()) < dx() * 0.1f && fabs(pt.y() - resizingPt.y()) < dy() * 0.1f) return true;
	else return false;
}

std::vector<QVector2D> ArcArea::polyline() const {
	std::vector<QVector2D> ret;

	// compute the center of the arc
	QVector2D center = (leftPt + rightPt) / 2.0f;
	center.setX(center.x() - radius);

	// compute the width
	float width = rightPt.x() - leftPt.x();

	// compute the radian
	float rad = arc_len / radius;

	int n = 8;
	for (int i = 0; i <= n; i++) {
		float theta = rad * (float)i / (float)n;
		ret.push_back(QVector2D(center.x() + cosf(rad / 2.0f - theta) * (radius - width / 2.0f), center.y() + sinf(rad / 2.0f - theta) * (radius - width / 2.0f)));
	}
	for (int i = 0; i <= n; i++) {
		float theta = rad * (float)i / (float)n;
		ret.push_back(QVector2D(center.x() + cosf(-rad / 2.0f + theta) * (radius + width / 2.0f), center.y() + sinf(-rad / 2.0f + theta) * (radius + width / 2.0f)));
	}

	return ret;
}

QVector2D ArcArea::distortionPt() const {
	// compute the radian
	float rad = arc_len / radius;

	QVector2D distortionPt = (leftPt + rightPt) / 2.0f;
	distortionPt.setX(distortionPt.x() - radius);
	distortionPt.setX(distortionPt.x() + cosf(-rad / 2.0f) * radius);
	distortionPt.setY(distortionPt.y() + sinf(-rad / 2.0f) * radius);

	return distortionPt;
}

QVector2D ArcArea::deform(const QVector2D& pt) const {
	// compute the center of the arc
	QVector2D center = (leftPt + rightPt) / 2.0f;
	center.setX(center.x() - radius);

	// compute the width
	float width = rightPt.x() - leftPt.x();

	// compute the radian
	float rad = arc_len / radius;

	int n = 8;

	// locate the cell in which the point resides.
	int u = (pt.x() - leftPt.x()) * (float)n / width;
	int v = (pt.y() - leftPt.y() + arc_len / 2.0f) * (float)n / arc_len;
	if (u < 0) u = 0;
	if (u >= n) u = n;
	if (v < 0) v = 0;
	if (v >= n) v = n;

	float theta = rad * ((float)v + 0.5f) / (float)n - rad / 2.0f;
	QVector2D c0(leftPt.x(), leftPt.y() - arc_len / 2.0f);
	c0.setX(c0.x() + ((float)u + 0.5f) / (float)n * width);
	c0.setY(c0.y() + ((float)v + 0.5f) / (float)n * arc_len);

	float r1 = radius + ((float)u + 0.5f) / (float)n * width - width / 2.0f;
	QVector2D c1(center.x() + cosf(theta) * r1, center.y() + sinf(theta) * r1);

	QVector2D pt2(pt.x() - c0.x(), pt.y() - c0.y());
	QVector2D pt3;
	pt3.setX(cosf(theta) * pt2.x() - sinf(theta) * pt2.y());
	pt3.setY(sinf(theta) * pt2.x() + cosf(theta) * pt2.y());
	pt3.setX(pt3.x() + c1.x());
	pt3.setY(pt3.y() + c1.y());

	return pt3;
}
