#pragma once

#include <QPushButton>

class OBSBasicControls;

class RecordButton : public QPushButton {
	Q_OBJECT

public:
	inline RecordButton(QWidget *parent = nullptr) : QPushButton(parent) {}
	inline RecordButton(const QString &text, QWidget *parent = nullptr)
		: QPushButton(text, parent)
	{
	}

	virtual void resizeEvent(QResizeEvent *event) override;

signals:
	void ResizeEvent();
};
