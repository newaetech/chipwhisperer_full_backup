#include "openadc_qt.h"
#include "ui_openadc_qt.h"

openadc_qt::openadc_qt(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::openadc_qt)
{
    ui->setupUi(this);
}

openadc_qt::~openadc_qt()
{
    delete ui;
}
