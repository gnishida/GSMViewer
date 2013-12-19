#include "CircleArea.h"

CircleArea::CircleArea(const QVector2D& center, float radius) {
	this->center = center;
	this->radius = radius;
}

CircleArea::~CircleArea() {
}

bool CircleArea::contains(const QVector2D& pt) const {
	if ((pt - center).length() < radius) return true;
	else return false;
}
