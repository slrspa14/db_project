#include "widget.h"
#include "ui_widget.h"
#include <QDesktopServices>
#include <QUrl>

// DB 경로
#define PATH_DATABASE "./danawa_db"

Widget::Widget(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::Widget)
{
    ui->setupUi(this);
    //git
    // 메인 버튼 왼쪽 정렬
    ui -> btn_topBrand->setStyleSheet("Text-align:left");
    ui->btn_topModel->setStyleSheet("Text-align:left");

    // 탭 길이조절
    ui -> tap_brand_model -> setStyleSheet("QTabBar::tab {height: 25px; width: 225px;}");
    ui -> tapMonth -> setStyleSheet("QTabBar::tab {height: 25px; width: 225px;}");
    ui -> tapBrand -> setStyleSheet("QTabBar::tab {height: 25px; width: 225px;}");
    ui -> tapModel -> setStyleSheet("QTabBar::tab {height: 25px; width: 225px;}");

    QSqlDatabase db = QSqlDatabase::addDatabase("QSQLITE"); //db연결
    db.setDatabaseName(PATH_DATABASE);
    db.open();//열고
    if( !db.open() )
    {
        qDebug() << db.lastError();
    }
    QMovie * Movie = new QMovie("C:/Users/aiot/Desktop/danawa_AD.gif");
    ui -> label_6 -> setMovie(Movie);
    Movie -> setScaledSize(QSize(361,91));
    Movie -> start();
    brand_top3_domestic();
    model_top();

    // QPixmap good("C:/Users/aiot/Desktop/d165967c-5881-485b-82a5-a53a55bec706.png"); //변수명 및 경로지정
    // ui -> good -> setPixmap(good); //이미지 출력
    // ui -> good -> setScaledContents(true);//라벨 크기에 맞게
}

void Widget::find_brand() //브랜드 TOP3 기간조회
{
    int startMonth, endMonth, mon_sell, alltotal;
    QSqlQuery qry_model, qry;
    QString model;
    QStringList insertMonth;
    QStringList month = {"January", "February", "March", "April", "May", "June", "July", "August", "September", "October", "November", "December"};
    double market_share;
    // 테이블 초기화
    int row = 0;
    ui -> tableWidget_3 -> clearContents();
    ui -> tableWidget_3 -> setRowCount(0);

    startMonth = ui->period_month1->currentText().toInt();
    endMonth = ui->period_month2->currentText().toInt();

    qDebug() << startMonth << endMonth;
    // 범위를 insertMonth의 요소로 넣기
    if(startMonth > endMonth)
    {
        QMessageBox::warning(this, "알림", "범위를 다시 설정해주세요.");
    }
    else
    {
        QString income_share = "SELECT sum(";//수입 합계 쿼리문
        QString showModel = "SELECT brand, ";
        for(int i = startMonth-1; i < endMonth; i++)
        {
            showModel.append(month[i]);
            income_share.append(month[i]);

            // 마지막으로 뽑히는 거엔 + 되면 안되니께
            if(i != endMonth-1)
            {
                showModel.append("+");
                income_share.append("+");
            }
        }

        if(ui -> tapBrand -> currentIndex() == 1)   //수입
        {
            showModel.append(" FROM month_total WHERE domestic_income = 1 ORDER BY 2 DESC LIMIT 3");
            income_share.append(") as month_total FROM allCar WHERE domestic_income = 1");//점유율
            qry.exec(income_share);
            while(qry.next())
            {
                alltotal = qry.value("month_total").toInt(); //가져온 수입합계 변수에 저장
            }
        }
        else    // 국산
        {

            showModel.append(" FROM month_total WHERE domestic_income = 0 ORDER BY 2 DESC LIMIT 3");
            income_share.append(")as month_total FROM allCar WHERE domestic_income = 0");//점유율
            // qDebug() << income_share;
            qry.exec(income_share);
            while(qry.next())
            {
                alltotal = qry.value("month_total").toInt(); //가져온 합계 변수에 저장
            }
        }
        // qDebug() << showModel;
        qry_model.exec(showModel);
        while(qry_model.next())
        {
            model = qry_model.value(0).toString();
            mon_sell = qry_model.value(1).toInt();
            market_share = round((double(mon_sell)/alltotal)*100*10)/10;

            ui -> tableWidget_3 -> insertRow(row);
            ui -> tableWidget_3 -> setItem(row, 0, new QTableWidgetItem(model));        //브랜드명
            ui -> tableWidget_3 -> setItem(row, 1, new QTableWidgetItem(QString::number(mon_sell)));   //판매량
            ui -> tableWidget_3 -> setItem(row, 2, new QTableWidgetItem(tr("%1%").arg(market_share,0,'f',1))); //점유율
            row++;
        }
    }
}

void Widget::on_period_find_clicked() //모델 TOP10 기간조회
{
    int startMonth, endMonth;
    QSqlQuery qry_model;
    QString model, model_sell;
    QStringList insertMonth;
    QStringList month = {"January", "February", "March", "April", "May", "June", "July", "August", "September", "October", "November", "December"};

    // 테이블 초기화
    int row = 0;
    ui -> tableWidget_2 -> clearContents();
    ui -> tableWidget_2 -> setRowCount(0);

    startMonth = ui->period_month1->currentText().toInt();
    endMonth = ui->period_month2->currentText().toInt();

    qDebug() << startMonth << endMonth;
    // 범위를 insertMonth의 요소로 넣기
    if(startMonth > endMonth)
    {
        QMessageBox::warning(this, "알림", "범위를 다시 설정해주세요.");
    }
    else
    {
        QString showModel = "SELECT Model, ";
        for(int i = startMonth-1; i < endMonth; i++)
        {
            showModel.append(month[i]);

            // 마지막으로 뽑히는 거엔 + 되면 안되니께
            if(i != endMonth-1)
            {
                showModel.append("+");
            }
        }
        if(ui -> tapModel -> currentIndex() == 1)   //수입
        {
            showModel.append(" FROM allCar WHERE domestic_income = 1 ORDER BY 2 DESC LIMIT 10");
        }
        else    // 국산
        {
            showModel.append(" FROM allCar WHERE domestic_income = 0 ORDER BY 2 DESC LIMIT 10");
        }

        qDebug() << showModel;

        qry_model.exec(showModel);
        while(qry_model.next())
        {
            model = qry_model.value(0).toString();
            model_sell = qry_model.value(1).toString();

            ui -> tableWidget_2 -> insertRow(row);
            ui -> tableWidget_2 -> setItem(row, 0, new QTableWidgetItem(model));        //브랜드명
            ui -> tableWidget_2 -> setItem(row, 1, new QTableWidgetItem(model_sell));   //판매량
            row++;
        }
    }
}


void Widget::tapMonth_Index()
{
    if(ui -> tapMonth -> currentIndex() == 1)
    {
        find_brand();
        on_period_find_clicked();
    }
    else
    {
        brand_top3_domestic();
        model_top();
    }
}

void Widget::brand_top3_domestic() //브랜드 TOP3
{
    //국산 수입 점유율 따로 구해서 넣기
    // 브랜드, 판매량 쿼리문
    // 국산, 수입 점유율 쿼리문
    //브랜드 -> 판매량 -> 점유율
    QStringList month = QStringList() << "January" << "February" << "March" << "April" << "May" << "June" << "July" << "August" <<
                        "September" << "October" << "November" << "December";
    int month_choice = ui -> month_combo -> currentIndex();
    QString brand, month_sales;

    QSqlQuery qry;
    int mon_sell, alltotal;
    double market_share; // 점유율 퍼센트용

    if(ui -> tapBrand -> currentIndex() == 1) // 수입차탭일때
    {
        QString income_share = "SELECT sum("+month[month_choice]+") as month_total FROM allCar WHERE domestic_income = 1";//수입 합계 쿼리문
        qry.exec(income_share);
        while(qry.next())
        {
            alltotal = qry.value("month_total").toInt(); //가져온 수입합계 변수에 저장
        }

        int row = 0;
        QString MONTHLY_BRAND = "SELECT brand,"+ month[month_choice] +" FROM month_total WHERE domestic_income = 1 ORDER BY 2 DESC LIMIT 3;"; // 브랜드별 국산, 수입 top3
        qry.exec(MONTHLY_BRAND);
        while(qry.next())
        {
            ui -> tableWidget_3 -> removeRow(row);//초기화용

            brand = qry.value(0).toString();
            mon_sell = qry.value(1).toInt();

            market_share = round((double(mon_sell)/alltotal)*100*10)/10;

            ui -> tableWidget_3 -> insertRow(row);
            ui -> tableWidget_3 -> setItem(row, 0, new QTableWidgetItem(brand));//브랜드명
            ui -> tableWidget_3 -> setItem(row, 1, new QTableWidgetItem(QString::number(mon_sell)));//판매량
            ui -> tableWidget_3 -> setItem(row, 2, new QTableWidgetItem(tr("%1%").arg(market_share,0,'f',1))); //점유율

            row++;
        }
    }
    else
    {
        QString domestic_share = "SELECT sum("+month[month_choice]+") as month_total FROM allCar WHERE domestic_income = 0";//국산 합계 쿼리문
        qry.exec(domestic_share);
        while(qry.next())
        {
            alltotal = qry.value("month_total").toInt(); //가져온 국산합계 변수에 저장
        }

        int row = 0;
        QString MONTHLY_BRAND = "SELECT brand, "+month[month_choice]+" FROM month_total WHERE domestic_income = 0 ORDER BY 2 DESC LIMIT 3;";
        qry.exec(MONTHLY_BRAND);
        while(qry.next())
        {
            ui -> tableWidget_3 -> removeRow(row);//초기화용
            brand = qry.value(0).toString();
            mon_sell = qry.value(1).toInt();
            market_share = round((double(mon_sell)/alltotal)*100*10)/10;

            ui -> tableWidget_3 -> insertRow(row);
            ui -> tableWidget_3 -> setItem(row, 0, new QTableWidgetItem(brand));//브랜드명
            ui -> tableWidget_3 -> setItem(row, 1, new QTableWidgetItem(QString::number(mon_sell)));//판매량
            ui -> tableWidget_3 -> setItem(row, 2, new QTableWidgetItem(tr("%1%").arg(market_share,0,'f',1))); //점유율
            row++;
        }
    }
}
void Widget::model_top() // 모델 top10
{
    ui->tableWidget_2->clearContents();//초기화
    ui->tableWidget_2->setRowCount(0);
    int row = 0;

    int total, model_sell, previous, abs, month_choice;     // 전월대비, 판매량, 전월판매량, 절대값, 콤보박스에서 선택한 달
    QSqlQuery qry_model;
    QString model, model_top;                               // 브랜드명, 쿼리문
    QStringList month = {"January", "February", "March", "April", "May", "June", "July", "August", "September", "October", "November", "December"};

    // 콤보박스 값 int로 받음
    month_choice = ui -> month_combo -> currentText().toInt();

    // 탭 구분(국산/수입 쿼리문만 다르고 뽑는 건 똑같음)
    if(ui -> tapModel -> currentIndex() == 1) //수입
    {
        // 만약 콤보박스로 선택한 달이 1월이면 month_choice가 1인데, month[1]의 요소는 February임. 그래서 -1 해갖고 month[0] 요소인 January 갖고옴
        if(month_choice == 1)
        {
            model_top = "SELECT Model, " + month[month_choice-1] +" FROM allCar WHERE domestic_income = 1 ORDER BY 2 DESC LIMIT 10;";
        }
        else
        {
            model_top = "SELECT Model, "+month[month_choice -2]+","+ month[month_choice-1] +" FROM allCar WHERE domestic_income = 1 ORDER BY 3 DESC LIMIT 10;";
        }
    }
    else //국산
    {
        if(month_choice == 1)
        {
            model_top = "SELECT Model, " + month[month_choice-1] +" FROM allCar WHERE domestic_income = 0 ORDER BY 2 DESC LIMIT 10;";
        }
        else
        {
            model_top = "SELECT Model, "+month[month_choice -2]+","+ month[month_choice-1] +" FROM allCar WHERE domestic_income = 0 ORDER BY 3 DESC LIMIT 10;";
        }
    }
    // 출력
    qry_model.exec(model_top);
    while(qry_model.next())
    {
        // 모델명
        model = qry_model.value(0).toString();
        ui -> tableWidget_2 -> insertRow(row);
        ui -> tableWidget_2 -> setItem(row, 0, new QTableWidgetItem(model));//브랜드명

        // 판매량
        if(month_choice == 1)       // 1월일 때
        {
            model_sell = qry_model.value(1).toInt();
            total = 0;

            ui -> tableWidget_2 -> setItem(row, 1, new QTableWidgetItem(QString::number(model_sell)));//판매량
        }
        else
        {
            previous = qry_model.value(1).toInt();
            model_sell = qry_model.value(2).toInt();
            total = model_sell - previous;

            ui -> tableWidget_2 -> setItem(row, 1, new QTableWidgetItem(QString::number(model_sell)));//판매량
        }

        // 전월대비
        if(total < 0)
        {
            abs = qAbs(total);

            QString insert_value = "▼ " + QString::number(abs);
            ui->tableWidget_2->setItem(row, 2, new QTableWidgetItem(insert_value));
            ui->tableWidget_2->item(row, 2)->setForeground(Qt::blue);
            row++;
        }
        else if(total == 0)
        {
            QString insert_value = "-";
            ui->tableWidget_2->setItem(row, 2, new QTableWidgetItem(insert_value));
            row++;
        }
        else
        {
            QString insert_value = "▲ " + QString::number(total);
            ui->tableWidget_2->setItem(row, 2, new QTableWidgetItem(insert_value));
            ui->tableWidget_2->item(row, 2)->setForeground(Qt::red);
            row++;
        }
    }
}

void Widget::on_tapBrand_currentChanged(int index) //브랜드 국산 수입 선택 탭
{
    if(ui -> tapMonth -> currentIndex() == 1) //기간조회탭이면
    {
        find_brand();
        on_period_find_clicked();
    }
    else
    {
        //월별조회
        QStringList month = QStringList() << "January" << "February" << "March" << "April" << "May" << "June" << "July" << "August" <<
                            "September" << "October" << "November" << "December";
        int month_choice = ui -> month_combo -> currentIndex();
        QString brand;

        QSqlQuery qry;
        int mon_sell, alltotal;
        double market_share; // 점유율 퍼센트용

        if(ui -> tapBrand -> currentIndex() == 1) // 수입차탭일때
        {
            QString income_share = "SELECT sum("+month[month_choice]+") as month_total FROM allCar WHERE domestic_income = 1";//수입 합계 쿼리문
            qry.exec(income_share);
            while(qry.next())
            {
                alltotal = qry.value("month_total").toInt(); //가져온 수입합계 변수에 저장
            }

            int row = 0;
            // QString MONTHLY_BRAND = "SELECT BrandName,"+ month[month_choice] +" FROM month_total WHERE domestic_income = 1 ORDER BY 2 DESC LIMIT 3;"; // 브랜드별 국산, 수입 top3
            QString MONTHLY_BRAND = "SELECT brand,"+month[month_choice]+" FROM month_total WHERE domestic_income = 1 ORDER BY 2 DESC LIMIT 3;";
            // qDebug() << MONTHLY_BRAND;
            qry.exec(MONTHLY_BRAND);
            while(qry.next())
            {
                ui -> tableWidget_3 -> removeRow(row);//초기화용

                brand = qry.value(0).toString();
                mon_sell = qry.value(1).toInt();

                market_share = round((double(mon_sell)/alltotal)*100*10)/10;

                ui -> tableWidget_3 -> insertRow(row);
                ui -> tableWidget_3 -> setItem(row, 0, new QTableWidgetItem(brand));//브랜드명
                ui -> tableWidget_3 -> setItem(row, 1, new QTableWidgetItem(QString::number(mon_sell)));//판매량
                ui -> tableWidget_3 -> setItem(row, 2, new QTableWidgetItem(tr("%1%").arg(market_share,0,'f',1))); //점유율

                row++;
            }
        }
        else
        {
            QString domestic_share = "SELECT sum("+month[month_choice]+") as month_total FROM allCar WHERE domestic_income = 0";//국산 합계 쿼리문
            qry.exec(domestic_share);
            while(qry.next())
            {
                alltotal = qry.value("month_total").toInt(); //가져온 국산합계 변수에 저장
            }

            int row = 0;
            QString MONTHLY_BRAND = "SELECT brand, "+month[month_choice]+" FROM month_total WHERE domestic_income = 0 ORDER BY 2 DESC LIMIT 3;";
            qry.exec(MONTHLY_BRAND);
            while(qry.next())
            {
                ui -> tableWidget_3 -> removeRow(row);//초기화용
                brand = qry.value(0).toString();
                mon_sell = qry.value(1).toInt();
                market_share = round((double(mon_sell)/alltotal)*100*10)/10;

                ui -> tableWidget_3 -> insertRow(row);
                ui -> tableWidget_3 -> setItem(row, 0, new QTableWidgetItem(brand));//브랜드명
                ui -> tableWidget_3 -> setItem(row, 1, new QTableWidgetItem(QString::number(mon_sell)));//판매량
                ui -> tableWidget_3 -> setItem(row, 2, new QTableWidgetItem(tr("%1%").arg(market_share,0,'f',1))); //점유율
                row++;
            }
        }
    }

}

void Widget::on_tapModel_currentChanged(int index)// 모델 국산 수입 선택 탭
{

    if(ui -> tapMonth -> currentIndex() == 1)
    {
        find_brand();
        on_period_find_clicked();
    }
    else
    {
        ui->tableWidget_2->clearContents();
        ui->tableWidget_2->setRowCount(0);
        int row = 0;

        int total, model_sell, previous, abs, month_choice;     // 전월대비, 판매량, 전월판매량, 절대값, 콤보박스에서 선택한 달
        QSqlQuery qry_model;
        QString model, model_top;                               // 브랜드명, 쿼리문
        QStringList month = {"January", "February", "March", "April", "May", "June", "July", "August", "September", "October", "November", "December"};

        // 콤보박스 값 int로 받음
        month_choice = ui -> month_combo -> currentText().toInt();

        // 탭 구분(국산/수입 쿼리문만 다르고 뽑는 건 똑같음)
        if(ui -> tapModel -> currentIndex() == 1) //수입
        {
            // 만약 콤보박스로 선택한 달이 1월이면 month_choice가 1인데, month[1]의 요소는 February임. 그래서 -1 해갖고 month[0] 요소인 January 갖고옴
            if(month_choice == 1)
            {
                model_top = "SELECT Model, " + month[month_choice-1] +" FROM allCar WHERE domestic_income = 1 ORDER BY 2 DESC LIMIT 10;";
            }
            else
            {
                model_top = "SELECT Model, "+month[month_choice -2]+","+ month[month_choice-1] +" FROM allCar WHERE domestic_income = 1 ORDER BY 3 DESC LIMIT 10;";
            }
        }
        else //국산
        {
            if(month_choice == 1)
            {
                model_top = "SELECT Model, " + month[month_choice-1] +" FROM allCar WHERE domestic_income = 0 ORDER BY 2 DESC LIMIT 10;";
            }
            else
            {
                model_top = "SELECT Model, "+month[month_choice -2]+","+ month[month_choice-1] +" FROM allCar WHERE domestic_income = 0 ORDER BY 3 DESC LIMIT 10;";
            }
        }

        // 출력
        qry_model.exec(model_top);
        while(qry_model.next())
        {
            // 모델명
            model = qry_model.value(0).toString();
            ui -> tableWidget_2 -> insertRow(row);
            ui -> tableWidget_2 -> setItem(row, 0, new QTableWidgetItem(model));//브랜드명

            // 판매량
            if(month_choice == 1)       // 1월일 때
            {
                model_sell = qry_model.value(1).toInt();
                total = 0;

                ui -> tableWidget_2 -> setItem(row, 1, new QTableWidgetItem(QString::number(model_sell)));//판매량
            }
            else
            {
                previous = qry_model.value(1).toInt();
                model_sell = qry_model.value(2).toInt();
                total = model_sell - previous;

                ui -> tableWidget_2 -> setItem(row, 1, new QTableWidgetItem(QString::number(model_sell)));//판매량
            }

            // 전월대비
            if(total < 0)
            {
                abs = qAbs(total);

                QString insert_value = "▼ " + QString::number(abs);
                ui->tableWidget_2->setItem(row, 2, new QTableWidgetItem(insert_value));
                ui->tableWidget_2->item(row, 2)->setForeground(Qt::blue);
                row++;
            }
            else if(total == 0)
            {
                QString insert_value = "-";
                ui->tableWidget_2->setItem(row, 2, new QTableWidgetItem(insert_value));
                row++;
            }
            else
            {
                QString insert_value = "▲ " + QString::number(total);
                ui->tableWidget_2->setItem(row, 2, new QTableWidgetItem(insert_value));
                ui->tableWidget_2->item(row, 2)->setForeground(Qt::red);
                row++;
            }
        }
    }

}

Widget::~Widget() //소멸
{
    delete ui;
}

void Widget::model_type()//차종별 //스택위젯 추가로 인한 인덱스값 변경
{
    ui -> stackedWidget_3 -> setCurrentIndex(1);
    ui -> stackedWidget_4 -> setCurrentIndex(3);

    QPushButton * temp = qobject_cast<QPushButton*>(sender());  // QPushButton에 대한 포인터
    QString model_type = temp ->objectName();

    // 클릭된 버튼의 자동차 유형을 나타내는 문자열
    if (model_type == "light_car")
        car = "경차";
    else if(model_type == "small_car")
        car = "소형";
    else if(model_type == "semimidium_car")
        car = "준중형";
    else if(model_type == "midium_Car")
        car = "중형";
    else if(model_type == "semilarge_car")
        car = "준대형";
    else if(model_type == "large_Car")
        car = "대형";
    else if(model_type == "sports_car")
        car = "스포츠카";
    else if(model_type == "small_suv")
        car = "소형SUV";
    else if(model_type == "midium_suv")
        car = "중형SUV";
    else if(model_type == "large_suv")
        car = "대형SUV";
    else if(model_type == "RVMPV_btn")
        car = "RV";
    else
        car = "상용";

    CarType_Month();
}

void Widget::CarType_Month() //차종 월별

{
    int combo_Month, total, allTotal;
    double share;
    int row = 0;
    QString model, light_car;
    QSqlQuery qry;
    QStringList monthList = {"January", "February", "March", "April", "May", "June", "July", "August", "September", "October", "November", "December"};

    ui->tableWidget_10->setRowCount(0);

    combo_Month = ui->month_combo_2->currentText().toInt();

    //차종별끼리의 합
    QString income_share = "SELECT sum("+monthList[combo_Month-1]+") as month_total FROM allCar WHERE size = '"+ car +"'"; //수입 합계 쿼리문
    qry.exec(income_share);
    while(qry.next())
    {
        allTotal = qry.value("month_total").toInt();    //가져온 수입합계 변수에 저장
    }

    if(car == "RV")
    {
        light_car = "SELECT Model, "+monthList[combo_Month-1]+" FROM allCar WHERE size LIKE '"+ car +"%' AND "+monthList[combo_Month-1]+" != 0 ORDER BY 2 DESC";
    }
    else
    {
        light_car = "SELECT Model, "+monthList[combo_Month-1]+" FROM allCar WHERE size = '"+ car +"' AND "+monthList[combo_Month-1]+" != 0 ORDER BY 2 DESC";
    }
    qry.exec(light_car);
    while(qry.next())
    {
        model = qry.value(0).toString();
        total = qry.value(1).toInt();
        share = round((double(total)/allTotal)*100*10)/10;

        ui->tableWidget_10->insertRow(row);
        ui->tableWidget_10->setItem(row, 0, new QTableWidgetItem(model));
        ui->tableWidget_10->setItem(row, 1, new QTableWidgetItem(QString::number(total)));
        ui->tableWidget_10->setItem(row, 2, new QTableWidgetItem(tr("%1%").arg(share,0,'f',1)));
        row++;
    }
}
void Widget::period_type() //차종별 기간조회
{
    int startMonth, endMonth, mon_sell, alltotal;
    QSqlQuery qry_model, qry;
    QString model;
    QStringList insertMonth;
    QStringList month = {"January", "February", "March", "April", "May", "June", "July", "August", "September", "October", "November", "December"};
    double market_share;

    int row = 0;
    // ui -> stackedWidget_3 -> setCurrentIndex(1);
    // ui -> stackedWidget_4 -> setCurrentIndex(3);
    // 테이블 초기화
    ui -> tableWidget_10 -> clearContents();
    ui -> tableWidget_10 -> setRowCount(0);

    startMonth = ui->period_month1_2->currentText().toInt();
    endMonth = ui->period_month2_2->currentText().toInt();

    qDebug() << startMonth << endMonth;
    // 범위를 insertMonth의 요소로 넣기
    if(startMonth > endMonth)
    {
        QMessageBox::warning(this, "알림", "범위를 다시 설정해주세요.");
    }
    else
    {
        QString income_share = "SELECT sum(";//합계 쿼리문
        QString showModel = "SELECT Model, ";
        for(int i = startMonth-1; i < endMonth; i++)
        {
            showModel.append(month[i]);
            income_share.append(month[i]);

            // 마지막으로 뽑히는 거엔 + 되면 안되니께
            if(i != endMonth-1)
            {
                showModel.append("+");
                income_share.append("+");
            }
        }

        showModel.append(" as zero FROM allCar WHERE size = '"+ car +"' AND zero !=0 ORDER BY 2 DESC");
        income_share.append(") as month_total FROM allCar WHERE size = '"+ car +"'");//점유율합계

        qDebug() << showModel;
        qDebug() << income_share;
        qry.exec(income_share);
        while(qry.next())
        {
            alltotal = qry.value("month_total").toInt(); //가져온 합계 변수에 저장
        }
        qry_model.exec(showModel);
        while(qry_model.next())
        {
            model = qry_model.value(0).toString();
            mon_sell = qry_model.value(1).toInt();
            market_share = round((double(mon_sell)/alltotal)*100*10)/10;

            ui -> tableWidget_10 -> insertRow(row);
            ui -> tableWidget_10 -> setItem(row, 0, new QTableWidgetItem(model));        //브랜드명
            ui -> tableWidget_10 -> setItem(row, 1, new QTableWidgetItem(QString::number(mon_sell)));   //판매량
            ui -> tableWidget_10 -> setItem(row, 2, new QTableWidgetItem(tr("%1%").arg(market_share,0,'f',1))); //점유율
            row++;
        }
    }
}


void Widget::on_type_pre_clicked()//차종별 스택위젯
{
    ui -> stackedWidget_2 -> setCurrentIndex(0);
}
void Widget::on_type_next_clicked()//차종별 스택위젯
{
    ui -> stackedWidget_2 -> setCurrentIndex(1);
}
void Widget::on_mainTap_all_clicked()//전체탭

{
    ui -> stackedWidget_4 -> setCurrentIndex(0);
    ui -> stackedWidget_3 -> setCurrentIndex(0);
}

void Widget:: click_test()
{
    // if(ui -> light_car -> setChecked(true))
    // ui -> light_car -> setDown(true);
}


void Widget::on_pushButton_clicked()
{
    QDesktopServices::openUrl(QUrl("https://mauto.danawa.com/newcar/?Work=record"));
}

