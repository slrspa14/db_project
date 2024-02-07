#ifndef WIDGET_H
#define WIDGET_H

#include <QWidget>
#include <QDebug>
#include <QSqlDatabase>
#include <QSqlError>
#include <QSqlRecord>
#include <QSqlQuery>
#include <QSqlTableModel>
#include <QTableView>
#include <QTableWidget>
#include <QMessageBox>
#include <QLineEdit>
#include <QFile>
#include <QStackedWidget>
#include <qpixmap.h>
#include <QString>
#include <QTranslator> //tr
#include <QMovie>

QT_BEGIN_NAMESPACE
namespace Ui
{
class Widget;//git_test
}
QT_END_NAMESPACE

class Widget : public QWidget
{
    Q_OBJECT

public:
    Widget(QWidget *parent = nullptr);
    ~Widget();


private slots:

    void brand_top3_domestic();
    void model_top();
    void on_tapBrand_currentChanged(int index);
    void on_tapModel_currentChanged(int index);
    void on_period_find_clicked();
    void find_brand();
    void model_type();
    void on_type_pre_clicked();
    void on_type_next_clicked();

    void on_mainTap_all_clicked();
    void tapMonth_Index();
    void CarType_Month();
    void period_type();
    void click_test();
    void on_pushButton_clicked();

private:
    Ui::Widget *ui;
    QSqlDatabase db;
    QSqlTableModel *model;
    QString car;
};
#endif // WIDGET_H
