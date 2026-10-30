#pragma once
#include <QDialog>
#include "config/appsettings.h"
class QLineEdit; class QSpinBox; class QCheckBox;
class SettingsDialog:public QDialog{Q_OBJECT public:SettingsDialog(AppSettings&,QWidget* p=nullptr);private slots:void accept()override;private:AppSettings&m;QLineEdit*key;QLineEdit*model;QSpinBox*interval;QSpinBox*font;QCheckBox*periodic;};
