#include "settings-dialog.h"
#include <QFormLayout>
#include <QLineEdit>
#include <QSpinBox>
#include <QCheckBox>
#include <QDialogButtonBox>
#include <QMessageBox>
SettingsDialog::SettingsDialog(AppSettings&s,QWidget*p):QDialog(p),m(s){setWindowTitle("Sozlamalar");key=new QLineEdit(m.apiKey,this);key->setEchoMode(QLineEdit::Password);model=new QLineEdit(m.model,this);interval=new QSpinBox(this);interval->setRange(250,60000);interval->setValue(m.sampleIntervalMs);font=new QSpinBox(this);font->setRange(8,32);font->setValue(m.fontSize);periodic=new QCheckBox("Periodic sampling",this);periodic->setChecked(m.periodicSampling);auto*l=new QFormLayout(this);l->addRow("Gemini API key",key);l->addRow("Model",model);l->addRow("Interval (ms)",interval);l->addRow("Overlay font",font);l->addRow(periodic);auto*b=new QDialogButtonBox(QDialogButtonBox::Ok|QDialogButtonBox::Cancel,this);l->addWidget(b);connect(b,&QDialogButtonBox::accepted,this,&SettingsDialog::accept);connect(b,&QDialogButtonBox::rejected,this,&QDialog::reject);}
void SettingsDialog::accept(){m.apiKey=key->text();m.model=model->text();m.sampleIntervalMs=interval->value();m.fontSize=font->value();m.periodicSampling=periodic->isChecked();QString e;if(!m.valid(&e)){QMessageBox::warning(this,"Sozlamalar",e);return;}m.save();QDialog::accept();}
