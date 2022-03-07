#include "record-button.hpp"

#include <QResizeEvent>

void RecordButton::resizeEvent(QResizeEvent *event)
{
	emit ResizeEvent();
	event->accept();
}
