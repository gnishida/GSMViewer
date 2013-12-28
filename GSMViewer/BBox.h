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

	void translate(float x, float y);
	void resize(const QVector2D& pt);
	bool hitTest(const QVector2D& pt) const;

	bool hitTestResizingPoint(const QVector2D& pt) const {
		if (fabs(pt.x() - maxPt.x()) < dx() * 0.1f && fabs(pt.y() - minPt.y()) < dy() * 0.1f) return true;
		else return false;
	}

	std::vector<QVector2D> polyline() const;
	bool contains(const QVector2D &pt) const;
};

