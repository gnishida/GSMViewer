#pragma once

#include <QVector2D>
#include <vector>

class AbstractArea {
public:
	static enum {
		RESIZING_TOP_LEFT = 0,
		RESIZING_TOP_RIGHT,
		RESIZING_BOTTOM_LEFT,
		RESIZING_BOTTOM_RIGHT
	};

public:
	AbstractArea();
	~AbstractArea();

	virtual bool contains(const QVector2D& pt) const = 0;
	virtual QVector2D midPt() const = 0;
	virtual float dx() const = 0;
	virtual float dy() const = 0;
	virtual void translate(float x, float y) = 0;
	virtual void resize(const QVector2D& pt, int type) = 0;
	virtual bool hitTest(const QVector2D& pt) const = 0;
	virtual bool hitTestDistortionPoint(const QVector2D& pt) const = 0;
	virtual bool hitTestResizingPoint(const QVector2D& pt) const = 0;
	virtual std::vector<QVector2D> polyline() const = 0;
	virtual QVector2D distortionPt() const = 0;
};


