#pragma once

#include "AbstractArea.h"
#include <QVector2D>
#include <vector>

class BBox : public AbstractArea {
public:
	QVector2D minPt;
	QVector2D maxPt;

public:
	BBox();
	~BBox();

	inline BBox(const BBox &ref) {	
		minPt = ref.minPt;
		maxPt = ref.maxPt;
	}

	inline BBox& operator=(const BBox &ref) {
		minPt = ref.minPt;
		maxPt = ref.maxPt;				
		return (*this);
	}

	inline void reset() {
		minPt.setX(FLT_MAX);
		minPt.setY(FLT_MAX);
		maxPt.setX(-FLT_MAX);
		maxPt.setY(-FLT_MAX);
	}

	void combineWithBBox(const BBox& other);
	void addPoint(const QVector2D& newPt);

	inline bool overlapsWithBBoxXY(const BBox& other) {
		return  
			( (this->minPt.x() <= other.maxPt.x()) && (this->maxPt.x() >= other.minPt.x()) ) &&
			( (this->minPt.y() <= other.maxPt.y()) && (this->maxPt.y() >= other.minPt.y()) );					
	}

	inline QVector2D midPt() const {
		return 0.5 * (minPt + maxPt);
	}

	inline float dx() const {
		return maxPt.x() - minPt.x();
	}

	inline float dy() const {
		return maxPt.y() - minPt.y();
	}

	inline void translate(float x, float y) {
		minPt.setX(minPt.x() + x);
		minPt.setY(minPt.y() + y);
		maxPt.setX(maxPt.x() + x);
		maxPt.setY(maxPt.y() + y);
	}

	bool hitTest(const QVector2D& pt) const {
		if (pt.x() < minPt.x() - dx() * 0.1f) return false;
		if (pt.y() < minPt.y() - dy() * 0.1f) return false;
		if (pt.x() > maxPt.x() + dx() * 0.1f) return false;
		if (pt.y() > maxPt.y() + dy() * 0.1f) return false;

		return true;
	}

	bool hitTestDistortionPoint(const QVector2D& pt) const {
		if (fabs(pt.x() - midPt().x()) < dx() * 0.1f && fabs(pt.y() - minPt.y()) < dy() * 0.1f) return true;
		else return false;
	}

	bool hitTestResizingPoint(const QVector2D& pt) const {
		if (fabs(pt.x() - maxPt.x()) < dx() * 0.1f && fabs(pt.y() - minPt.y()) < dy() * 0.1f) return true;
		else return false;
	}

	std::vector<QVector2D> polyline() const {
		std::vector<QVector2D> ret;

		ret.push_back(minPt);
		ret.push_back(QVector2D(maxPt.x(), minPt.y()));
		ret.push_back(maxPt);
		ret.push_back(QVector2D(minPt.x(), maxPt.y()));

		return ret;
	}

	virtual QVector2D distortionPt() const {
		return QVector2D(midPt().x(), minPt.y());
	}

	inline void recalculate(const std::vector<QVector2D>& vertices) {
		minPt.setX(FLT_MAX);
		minPt.setY(FLT_MAX);
		maxPt.setX(-FLT_MAX);
		maxPt.setY(-FLT_MAX);

		for (size_t i = 0; i < vertices.size(); ++i) {
			if (vertices[i].x() < minPt.x()) { minPt.setX(vertices[i].x()); }
			if (vertices[i].y() < minPt.y()) { minPt.setY(vertices[i].y()); }

			if (vertices[i].x() > maxPt.x()) { maxPt.setX(vertices[i].x()); }
			if (vertices[i].y() > maxPt.y()) { maxPt.setY(vertices[i].y()); }
		}
	}

	bool contains(const QVector2D &pt) const;
};

