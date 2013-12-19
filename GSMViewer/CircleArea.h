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
};

