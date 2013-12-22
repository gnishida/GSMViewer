#pragma once

#include "AbstractArea.h"
#include <QVector2D>

class CircleArea : public AbstractArea {
private:
	QVector2D center;
	float radius;

public:
	CircleArea(const QVector2D& center, float radius);
	~CircleArea();

	bool contains(const QVector2D& pt) const;
	QVector2D midPt() const;
	float dx() const;
	float dy() const;
	void translate(float x, float y);
	bool hitTest(const QVector2D& pt) const;
	bool hitTestDistortionPoint(const QVector2D& pt) const;
	bool hitTestResizingPoint(const QVector2D& pt) const;
	std::vector<QVector2D> polyline() const;
	QVector2D distortionPt() const;
};

