#ifndef OPENADC_QT_H
#define OPENADC_QT_H

#include <QWidget>

namespace Ui {
class openadc_qt;
}

class openadc_qt : public QWidget
{
    Q_OBJECT
    
public:
    explicit openadc_qt(QWidget *parent = 0);
    ~openadc_qt();
    
private:
    Ui::openadc_qt *ui;
};

#endif // OPENADC_QT_H
