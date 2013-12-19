#pragma once

#include <QVector2D>

class AbstractArea {
public:
	AbstractArea();
	~AbstractArea();

	virtual bool contains(const QVector2D& pt) const = 0;
};

