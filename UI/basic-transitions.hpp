#pragma once

#include <memory>

#include <QWidget>

class OBSBasic;

#include "ui_OBSBasicTransitions.h"

class OBSBasicTransitions : public QWidget {
	Q_OBJECT

	std::unique_ptr<Ui::OBSBasicTransitions> ui;

	bool enabled;

public:
	OBSBasicTransitions(OBSBasic *main);
	inline ~OBSBasicTransitions(){};

private slots:
	void on_transitions_currentIndexChanged(int idx);

	void TransitionsWidgetsEnabled(bool enable);

	void TransitionAdded(const QString &name);
	void TransitionChanged(const QString &transition, bool hasDuration,
			       bool hasProperties);
	void UnknownTransitionSet();
	void TransitionDurationChanged(int value);
	void TransitionRemoved(const QString &name);
	void TransitionRenamed(const QString &oldName, const QString &newName);

signals:
	void ChangeTransition(const QString &transition);
};
