#include "mainform.h"
#include "ui_mainform.h"
#include <QFileDialog>
#include <QMessageBox>
#include <QFile>
#include <QStandardPaths>
MainForm::MainForm(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::MainForm)
{
    ui->setupUi(this);
    connectionForm = new ConnectionForm(this);
    QObject::connect(connectionForm,SIGNAL(finished()),this,SLOT(start()));

    QFile file(QStandardPaths::locate(QStandardPaths::DocumentsLocation,QString(),QStandardPaths::LocateDirectory)+"Time-Track/Connection.txt");
    if(!file.open(QIODevice::ReadOnly)) {
        file.close();
        connectionForm->show();
    }else{
        file.close();
        start();
    }

    ui->ShiftEmployeeCombo->installEventFilter(this);
    ui->ShiftProjectCombo->installEventFilter(this);
    ui->ShiftItemCombo->installEventFilter(this);
}

MainForm::~MainForm()
{
    delete ui;
}
bool MainForm::eventFilter(QObject* object,QEvent* event)
{
    if(object == ui->ShiftEmployeeCombo && event->type() == QEvent::MouseButtonPress) {

        ui->ShiftEmployeeCombo->setFocus();
        QRect rec = QApplication::desktop()->screenGeometry();
        if(rec.width() < 1400)
            QDesktopServices::openUrl(QUrl("file:///C:/Program Files/Common Files/Microsoft Shared/Ink/TabTip.exe"));
        return false;
    }
    else if(object == ui->ShiftProjectCombo && event->type() == QEvent::MouseButtonPress) {

        ui->ShiftProjectCombo->setFocus();
        QRect rec = QApplication::desktop()->screenGeometry();
        if(rec.width() < 1400)
            QDesktopServices::openUrl(QUrl("file:///C:/Program Files/Common Files/Microsoft Shared/Ink/TabTip.exe"));
        return false;
    }
    else if(object == ui->ShiftItemCombo && event->type() == QEvent::MouseButtonPress) {

        ui->ShiftItemCombo->setFocus();
        QRect rec = QApplication::desktop()->screenGeometry();
        if(rec.width() < 1400)
            QDesktopServices::openUrl(QUrl("file:///C:/Program Files/Common Files/Microsoft Shared/Ink/TabTip.exe"));
        return false;
    }
    return false;
}
void MainForm::start(){

    QFile file(QStandardPaths::locate(QStandardPaths::DocumentsLocation,QString(),QStandardPaths::LocateDirectory)+"Time-Track/Connection.txt");
    file.open(QIODevice::ReadOnly);
    QTextStream in(&file);
    QString text = in.readAll();

    //qDebug()<<file.isOpen()<<text.split(" ").length();
    if(file.isOpen() && text.split(" ").length() == 5){

        QString databasename,port,username,pass;

        //qDebug()<<text<<text.split(" ").length();
        ip =text.split(" ")[4];
        databasename = text.split(" ")[0];
        port = text.split(" ")[1];
        username = text.split(" ")[2];
        pass = text.split(" ")[3];

        //qDebug()<<ip<<databasename<<port<<username;
        file.close();

        Connect(databasename,port,username,pass,ip);

        ui->error->setText("");
        if (ip.split(".")[3] =="10")
            this->showFullScreen();
        else
            this->showMaximized();
        //qDebug()<<"start"<<ip;
        clockoutForm = new ClockoutForm(this);
        clockoutForm->hide();
        shifteditform = new ShiftEditForm(this);
        shifteditform->hide();
        exportForm = new ExportForm(this);
        employeeeditform = new EmployeeEditForm(this);
        employeeeditform->hide();
        itemeditform = new ItemEditForm(this);
        itemeditform->hide();
        projecteditform = new ProjectEditForm(this);
        projecteditform->hide();

        loginInitialize();
        isConnected();
        setIcons();

    }
    else
    {
        this->hide();
        QObject::connect(connectionForm,SIGNAL(finished()),this,SLOT(start()));
        connectionForm->show();
        connectionForm->setError("Invalid Connection");
    }
}

/* These are the database connections that this database uses.
 * The first is called setup. It's purpose is to store the
 * location of data which is the database that can be placed
 * anywhere on your computer or server and holds the actual
 * information about employees and such
*/

bool MainForm::Connect(QString database,QString port,QString username,QString password,QString ip){

    data = QSqlDatabase::addDatabase("QMYSQL");
    data.setPort(port.toInt());
    data.setDatabaseName(database);
    data.setPassword(password);
    data.setUserName(username);
    data.setHostName(ip);
    ui->ConnectionLabel->setText(data.lastError().text());
    return data.open();

}

/* These classes connect the different forms through
 * signals and slots
*/

void MainForm::establishConnections(){
    QObject::connect(clockoutForm,SIGNAL(finished()),this,SLOT(reenter()));

    QObject::connect(projecteditform,SIGNAL(finished()),this,SLOT(refreshFromProjectEdit()));
    QObject::connect(projecteditform,SIGNAL(finished()),this,SLOT(displayProjectSuccess()));
    QObject::connect(itemeditform,SIGNAL(finished()),this,SLOT(refreshFromItemEdit()));
    QObject::connect(itemeditform,SIGNAL(finished()),this,SLOT(displayItemSuccess()));
    QObject::connect(employeeeditform,SIGNAL(finished()),this,SLOT(refreshFromEmployeeEdit()));
    QObject::connect(employeeeditform,SIGNAL(finished()),this,SLOT(displayEmployeeSuccess()));
    QObject::connect(shifteditform,SIGNAL(finished()),this,SLOT(refreshShiftTab()));
    QObject::connect(shifteditform,SIGNAL(finished()),this,SLOT(displayShiftSuccess()));
    QObject::connect(exportForm,SIGNAL(excel()),this,SLOT(exportToExcel()));

    QObject::connect(ui->ProjectView->model(),SIGNAL(dataChanged(QModelIndex,QModelIndex,QVector<int>)),this, SLOT(refreshProjectStuff()));
    QObject::connect(ui->ItemView->model(),SIGNAL(dataChanged(QModelIndex,QModelIndex,QVector<int>)),this, SLOT(refreshItemStuff()));
    QObject::connect(ui->EmployeeView->model(),SIGNAL(dataChanged(QModelIndex,QModelIndex,QVector<int>)),this, SLOT(refreshEmployeeStuff()));
}
void MainForm::reenter(){

    showtheThings();
    basicInitialize();
    loginInitialize();
}

/* Initialization of the different pages of mainform*/

void MainForm::setIcons(){
    QPixmap pixmap("../Icons/checked.png");
    QIcon ButtonIcon(pixmap);
    ui->basicPageClockIn->setIcon(ButtonIcon);
    ui->basicPageClockIn->setIconSize(QSize(200,200));

    pixmap = * new QPixmap("../Icons/unchecked.png");
    ButtonIcon =  * new QIcon(pixmap);
    ui->basicPageClockOut->setIcon(ButtonIcon);
    ui->basicPageClockOut->setIconSize(QSize(200,200));

    pixmap = * new QPixmap("../Icons/bars.png");
    ButtonIcon =  * new QIcon(pixmap);
    ui->basicPageAdvanced->setIcon(ButtonIcon);
    ui->basicPageAdvanced->setIconSize(QSize(200,200));

    pixmap = * new QPixmap("../Icons/power-button.png");
    ButtonIcon =  * new QIcon(pixmap);
    ui->basicPagePower->setIcon(ButtonIcon);
    ui->basicPagePower->setIconSize(QSize(200,200));


    pixmap = * new QPixmap("../Icons/connected.png");
    ButtonIcon =  * new QIcon(pixmap);
    ui->basicPageConnect->setIcon(ButtonIcon);
    ui->basicPageConnect->setIconSize(QSize(200,200));

    pixmap = * new QPixmap("../Icons/go-back-arrow.png");
    ButtonIcon =  * new QIcon(pixmap);
    ui->basicPageFinish->setIcon(ButtonIcon);
    ui->basicPageFinish->setIconSize(QSize(200,200));

//    pixmap = * new QPixmap("../Icons/connect.png");
//    ButtonIcon =  * new QIcon(pixmap);
//    ui->DataBaseConnect->setIcon(ButtonIcon);
//    ui->DataBaseConnect->setIconSize(QSize(50,50));
//    pixmap = * new QPixmap("../Icons/disconnect.png");
//    ButtonIcon =  * new QIcon(pixmap);
//    ui->DataBaseDisconnect->setIcon(ButtonIcon);
//    ui->DataBaseDisconnect->setIconSize(QSize(50,50));


    pixmap = * new QPixmap("../Icons/EmployeeAdd.png");
    ButtonIcon =  * new QIcon(pixmap);
    ui->EmployeeAdd->setIcon(ButtonIcon);
    ui->EmployeeAdd->setIconSize(QSize(34,50));

    pixmap = * new QPixmap("../Icons/EmployeeEdit.png");
    ButtonIcon =  * new QIcon(pixmap);
    ui->EmployeeEdit->setIcon(ButtonIcon);
    ui->EmployeeEdit->setIconSize(QSize(34,50));


    pixmap = * new QPixmap("../Icons/EmployeeDelete.png");
    ButtonIcon =  * new QIcon(pixmap);
    ui->EmployeeDelete->setIcon(ButtonIcon);
    ui->EmployeeDelete->setIconSize(QSize(34,50));




    pixmap = * new QPixmap("../Icons/ProjectAdd.png");
    ButtonIcon =  * new QIcon(pixmap);
    ui->ProjectAdd->setIcon(ButtonIcon);
    ui->ProjectAdd->setIconSize(QSize(34,50));

    pixmap = * new QPixmap("../Icons/ProjectEdit.png");
    ButtonIcon =  * new QIcon(pixmap);
    ui->ProjectArchive->setIcon(ButtonIcon);
    ui->ProjectArchive->setIconSize(QSize(34,50));

    pixmap = * new QPixmap("../Icons/ProjectDelete.png");
    ButtonIcon =  * new QIcon(pixmap);
    ui->ProjectDelete->setIcon(ButtonIcon);
    ui->ProjectDelete->setIconSize(QSize(34,50));



    pixmap = * new QPixmap("../Icons/SubProjectAdd.png");
    ButtonIcon =  * new QIcon(pixmap);
    ui->ItemAdd->setIcon(ButtonIcon);
    ui->ItemAdd->setIconSize(QSize(34,50));
    pixmap = * new QPixmap("../Icons/SubProjectEdit.png");
    ButtonIcon =  * new QIcon(pixmap);
    ui->ItemEdit->setIcon(ButtonIcon);
    ui->ItemEdit->setIconSize(QSize(34,50));
    pixmap = * new QPixmap("../Icons/SubProjectDelete.png");
    ButtonIcon =  * new QIcon(pixmap);
    ui->ItemDelete->setIcon(ButtonIcon);
    ui->ItemDelete->setIconSize(QSize(34,50));




    pixmap = * new QPixmap("../Icons/ShiftAdd.png");
    ButtonIcon =  * new QIcon(pixmap);
    ui->ShiftAdd->setIcon(ButtonIcon);
    ui->ShiftAdd->setIconSize(QSize(34,50));

    pixmap = * new QPixmap("../Icons/ShiftEdit.png");
    ButtonIcon =  * new QIcon(pixmap);
    ui->ShiftEdit->setIcon(ButtonIcon);
    ui->ShiftEdit->setIconSize(QSize(34,50));

    pixmap = * new QPixmap("../Icons/ShiftDelete.png");
    ButtonIcon =  * new QIcon(pixmap);
    ui->ShiftDelete->setIcon(ButtonIcon);
    ui->ShiftDelete->setIconSize(QSize(34,50));



    pixmap = * new QPixmap("../Icons/min.png");
    ButtonIcon =  * new QIcon(pixmap);
    ui->SettingsMax->setIcon(ButtonIcon);
    ui->SettingsMax->setIconSize(QSize(50,50));

    pixmap = * new QPixmap("../Icons/max.png");
    ButtonIcon =  * new QIcon(pixmap);
    ui->SettingsFull->setIcon(ButtonIcon);
    ui->SettingsFull->setIconSize(QSize(50,50));

    pixmap = * new QPixmap("../Icons/export.png");
    ButtonIcon =  * new QIcon(pixmap);
    ui->SettingsExport->setIcon(ButtonIcon);
    ui->SettingsExport->setIconSize(QSize(50,50));

    pixmap = * new QPixmap("../Icons/connected2.png");
    ButtonIcon =  * new QIcon(pixmap);
    ui->SettingsConnections->setIcon(ButtonIcon);
    ui->SettingsConnections->setIconSize(QSize(50,50));

    pixmap = * new QPixmap("../Icons/print.png");
    ButtonIcon =  * new QIcon(pixmap);
    ui->SettingsPrint->setIcon(ButtonIcon);
    ui->SettingsPrint->setIconSize(QSize(50,50));

    pixmap = * new QPixmap("../Icons/Settings.png");
    ButtonIcon =  * new QIcon(pixmap);
    ui->SettingsAll->setIcon(ButtonIcon);
    ui->SettingsAll->setIconSize(QSize(50,50));




}
void MainForm::loginInitialize(){

    ui->mainStack->setCurrentIndex(0);
    admin=false;
    ui->passEdit->setAlignment(Qt::AlignCenter);
    ui->passLabel->setAlignment(Qt::AlignCenter);

    isConnected();
    ui->mainFinish->hide();
}
void MainForm::isConnected(){
    ui->passEdit->setText("");
    ui->passLabel->setText("");

    if(data.open())
    {
        ui->passEdit->show();
        ui->passEdit->show();
        ui->loginNumPad->show();
        ui->basicPageConnect->hide();
        ui->ConnectionLabel->hide();
    }
    else
    {
        this->hide();
        QObject::connect(connectionForm,SIGNAL(finished()),this,SLOT(start()));
        connectionForm->show();
        connectionForm->setError("Invalid Connection");

    }
}
void MainForm::basicInitialize()
{
    ui->mainStack->setCurrentIndex(1);
    if(admin==true){
        ui->basicPagePower->show();
        ui->basicPageAdvanced->show();
    }
    else{
        ui->basicPagePower->hide();
        ui->basicPageAdvanced->hide();
    }

    QSqlQuery qry(data);
    QString active;

    qry.prepare("SELECT active FROM employeelist WHERE id = '"+id+"'");
    if(qry.exec())
    {
        while(qry.next())
        {
            active = qry.value(0).toString();
        }
    }
    qDebug()<<"HEY"<<active;
    if(active == "1")
    {
       ui->basicPageClockIn->hide();
       ui->basicPageClockOut->show();
    }
    else if(active== "0")
    {
        ui->basicPageClockIn->show();
        ui->basicPageClockOut->hide();
    }
    ui->mainFinish->hide();
}
void MainForm::advInitialize(){
    ui->mainStack->setCurrentIndex(2);
    ui->MainTabs->tabBar()->hide();
    ui->mainFinish->show();

    establishConnections();
}

void MainForm::on_Login0_clicked()
{


    ui->passEdit->setText(ui->passEdit->text()+"0");

}
void MainForm::on_Login1_clicked()
{
    ui->passEdit->setText(ui->passEdit->text()+"1");
}
void MainForm::on_Login2_clicked()
{
    ui->passEdit->setText(ui->passEdit->text()+"2");
}
void MainForm::on_Login3_clicked()
{
    ui->passEdit->setText(ui->passEdit->text()+"3");
}
void MainForm::on_Login4_clicked()
{
    ui->passEdit->setText(ui->passEdit->text()+"4");
}
void MainForm::on_Login5_clicked()
{
    ui->passEdit->setText(ui->passEdit->text()+"5");
}
void MainForm::on_Login6_clicked()
{
    ui->passEdit->setText(ui->passEdit->text()+"6");
}
void MainForm::on_Login7_clicked()
{
    ui->passEdit->setText(ui->passEdit->text()+"7");
}
void MainForm::on_Login8_clicked()
{
    ui->passEdit->setText(ui->passEdit->text()+"8");
}
void MainForm::on_Login9_clicked()
{
    ui->passEdit->setText(ui->passEdit->text()+"9");
}
void MainForm::on_LoginBack_clicked()
{
    ui->passEdit->setText("");
}
void MainForm::on_LoginGo_clicked()
{
    on_passEdit_returnPressed();
}

/* Deal with the signals and slots of the mainForm that
 * change what menu you are at
*/

void MainForm::hidetheThings(){
    ui->basicPageAdvanced->hide();
    ui->basicPageClockIn->hide();
    ui->basicPageClockOut->hide();
    ui->mainFinish->hide();

}
void MainForm::showtheThings(){

    ui->basicPageAdvanced->show();
    ui->basicPageClockIn->show();
    ui->basicPageClockOut->show();
    ui->mainFinish->show();
}
void MainForm::on_HeaderTabs_currentChanged(int index)
{
    if(index ==0){
        ui->MainTabs->setCurrentIndex(0);
        refreshEmployeeTab();
    }
    else if(index ==1){
        refreshProjectTab();
        ui->MainTabs->setCurrentIndex(1);
    }
    else if(index ==2){
        refreshItemTab();
        ui->MainTabs->setCurrentIndex(2);
    }
    else if(index ==3){
        refreshShiftTab();
        ui->MainTabs->setCurrentIndex(3);
    }
}
void MainForm::on_basicPageClockIn_clicked()
{
    if(data.open()){
        QSqlQuery qry1(data),qry2(data),qry3(data),qry4(data);
        QString employeename, employeeid, timein,datein;
        QDateTime x = QDateTime::currentDateTime();
        QDateTime z = x;

        employeeid = id;
        timein = z.toString("HH:mm:ss");
        datein = z.toString("yyyy-MM-dd");
        qry1.prepare("SELECT shiftcount,name FROM employeelist WHERE id = '"+id+"'");


        if(qry1.exec())
        {
            while(qry1.next())
            {
                employeename = qry1.value(1).toString();
            }
        }

        QString shiftid;
        qry1.prepare("select MAX(shiftid) As maxshiftid from shiftlist");
        if(qry1.exec()){
            while(qry1.next()){
                shiftid=qry1.value(0).toString();}}
        //qDebug()<<"The max shiftid is shown to be" +shiftid;
        int id1 = shiftid.toInt();
        id1++;
        shiftid = QString::number(id1);


        // Inserting clockin time and shiftnumber into pin.
        qry4.prepare("insert into shiftlist (employeeid,employeename,timein,datein,shiftid) values('"+employeeid+"','"+employeename+"','"+timein+"','"+datein+"','"+shiftid+"')");
        if (qry4.exec())
        {
            // Setting active.
            qry2.prepare("update employeelist set active=1 where id = '"+id+"'");
            qry2.exec();
            qry3.prepare("update employeelist set shiftcount = '"+shiftid+"' where id = '"+id+"'");
            qry3.exec();
        }

        ui->basicPageClockIn->hide();
        ui->basicPageClockOut->show();
        ui->error->setText("");
        loginInitialize();
    }else{
        ui->error->setText("Disconnected From Database. Verify Connection");
    }



}
void MainForm::on_basicPageClockOut_clicked()
{
    if(data.open()){

        clockoutForm = new ClockoutForm(this);
        establishConnections();
        if (ip.split(".")[3] =="10")
            clockoutForm->showFullScreen();
        else
            clockoutForm->showMaximized();


        clockoutForm->ClockoutInitialize(id);
        ui->error->setText("");

    }else{
        ui->error->setText("Disconnected From Database. Verify Connection");
    }

}
void MainForm::on_basicPageConnect_clicked()
{
    this->hide();
    if (connectionForm->getIp().split(".")[3] =="10")
        connectionForm->showFullScreen();
    else
        connectionForm->showNormal();

    QObject::connect(connectionForm,SIGNAL(finished()),this,SLOT(start()));
}
void MainForm::on_basicPageAdvanced_clicked()
{
    if(data.open()){
        advInitialize();
        EmployeeTab();
        ShiftTab();
        ProjectTab();
        ItemTab();
        SettingsTab();
        ui->MainTabs->setCurrentIndex(0);
        ui->HeaderTabs->setCurrentIndex(0);
        ui->error->setText("");
    }else{
        ui->error->setText("Disconnected From Database. Verify Connection");
    }
}
void MainForm::on_basicPagePower_clicked()
{
    QCoreApplication::quit();
}
void MainForm::on_basicPageFinish_clicked()
{
    loginInitialize();
}
void MainForm::on_mainFinish_clicked()
{
    loginInitialize();

}

/* Formatting of qdatetimes in order to only have times
 * that are in quarters
*/

QDateTime MainForm::format_datetimes(QDateTime z)
{
    int minTime = z.time().minute();
    int hourTime =z.time().hour();
    int dayOfMonth = z.date().daysInMonth();
    int dayOfYear = z.date().dayOfYear();
    if(minTime<7&&minTime>=0)
    {
        QTime x(z.time().hour(),0,0);
        QDateTime n(z.date(),x);
        z=n;
    }
    else if(minTime>=7&&minTime<23)
    {
        QTime x(z.time().hour(),15,0);
        QDate y(z.date());
        QDateTime n(y,x);
        z=n;
    }
    else if(minTime>=23&&minTime<37)
    {
        QTime x(z.time().hour(),30,0);
        QDateTime n(z.date(),x);
        z=n;
    }
    else if(minTime>=37&&minTime<53)
    {
        QTime x(z.time().hour(),45,0);
        QDateTime n(z.date(),x);
        z=n;
    }
    else if(minTime>=53 && hourTime==23 && z.date().day()==dayOfMonth&& z.date().daysInYear()==dayOfYear)
    {
        z.date().addYears(1);
        z.date().addDays(1);
        z.date().addMonths(1);
        QTime x(z.time().hour()+1,0,0);
        QDateTime n(z.date(),x);
        z=n;
    }
    else if(minTime>=53 && hourTime==23 && z.date().day()==dayOfMonth)
    {
        z.date().addDays(1);
        z.date().addMonths(1);
        QTime x(z.time().hour()+1,0,0);
        QDateTime n(z.date(),x);
        z=n;
    }
    else if(minTime>=53 && hourTime==23)
    {
        z.date().addDays(1);
        QTime x(z.time().hour()+1,0,0);
        QDateTime n(z.date(),x);
        z=n;
    }
    else if(minTime>=53)
    {
        QTime x(z.time().hour()+1,0,0);
        QDateTime n(z.date(),x);
        z=n;
    }
    return z;

}

//Login Section!

void MainForm::on_passEdit_returnPressed()
{
    if(data.open()){

        pin= ui->passEdit->text();
        QSqlQuery qry1(data),qry2(data);
        qry1.prepare("SELECT current FROM employeelist where pin = '"+pin+"'");
        if (qry1.exec())
        {
            int count=0;
            QString current = "0";
            while(qry1.next())
            {
                current = qry1.value(0).toString();

                count++;
            }
            if(count == 1 && current=="1")
            {
                qry2.prepare("SELECT adminstatus,id FROM employeelist WHERE pin = '"+pin+"'");
                if(qry2.exec()){
                    while(qry2.next()){
                        QString x= qry2.value(0).toString();
                        if(x=="1")
                            admin=true;
                        else
                            admin=false;
                        id = qry2.value(1).toString();
                    }
                }
                basicInitialize();
            }
            else if(count < 1){
                if(ui->passEdit->text()!="")
                    ui->passLabel->setText("Invalid");
            }
            else if(current == "0"){
                ui->passLabel->setText("No Longer Employed");
            }
            else
                ui->passLabel->setText("");
        }
    }
    if(!data.open()){

        isConnected();
        ui->error->setText("Disconnected From Database");
    }
}




// Employee Section!

/* Initialization and refreshing of the 'employeetab' in
 * 'sections'  along with the model for 'employeeView'.
*/
void MainForm::EmployeeTab()
{
    employeemodel=EmployeeModel();
    employeefiltermodel = EmployeeFilterModel();
    ui->EmployeeView->setModel(employeefiltermodel);
     ui->EmployeeView->horizontalHeader()->setSectionResizeMode(QHeaderView::ResizeToContents);

    establishConnections();
    ui->EmployeeView->setSortingEnabled(true);
    ui->EmployeeId->setChecked(false);
    ui->EmployeeView->hideColumn(0);
    ui->EmployeeName->setChecked(true);
    ui->EmployeeView->showColumn(1);
    ui->EmployeeCrew->setChecked(true);
    ui->EmployeeView->showColumn(2);
    ui->EmployeePin->setChecked(true);
    ui->EmployeeView->showColumn(3);
    ui->EmployeeAdminStatus->setChecked(true);
    ui->EmployeeView->showColumn(4);
    ui->EmployeeShiftCount->setChecked(false);
    ui->EmployeeView->hideColumn(5);
    ui->EmployeeActive->setChecked(true);
    ui->EmployeeView->hideColumn(6);
    ui->EmployeeCurrent->setChecked(false);
    ui->EmployeeView->hideColumn(7);
    ui->CurrentRadio->setChecked(true);
    refreshEmployeeTab();

}
void MainForm::refreshEmployeeStuff(){

    shifteditform->EmployeeInitialize();
    refreshShiftEmployee();

}
void MainForm::refreshEmployeeTab(){
    employeemodel=EmployeeModel();
    employeefiltermodel = EmployeeFilterModel();
    ui->EmployeeView->setModel(employeefiltermodel);
    if(data.open()){
        ui->error->setText("");

        establishConnections();

        if(ui->EmployeeId->isChecked())
            ui->EmployeeView->showColumn(0);
        else
            ui->EmployeeView->hideColumn(0);        
        if(ui->EmployeeName->isChecked())
            ui->EmployeeView->showColumn(1);
        else
            ui->EmployeeView->hideColumn(1);
        if(ui->EmployeeCrew->isChecked())
            ui->EmployeeView->showColumn(2);
        else
            ui->EmployeeView->hideColumn(2);
        if(ui->EmployeePin->isChecked())
            ui->EmployeeView->showColumn(3);
        else
            ui->EmployeeView->hideColumn(3);

        if(ui->EmployeeAdminStatus->isChecked())
            ui->EmployeeView->showColumn(4);
        else
            ui->EmployeeView->hideColumn(4);

        if(ui->EmployeeShiftCount->isChecked())
            ui->EmployeeView->showColumn(5);
        else
            ui->EmployeeView->hideColumn(5);

        if(ui->EmployeeActive->isChecked())
            ui->EmployeeView->showColumn(6);
        else
            ui->EmployeeView->hideColumn(6);

        if(ui->EmployeeCurrent->isChecked())
            ui->EmployeeView->showColumn(7);
        else
            ui->EmployeeView->hideColumn(7);
        if(ui->AllRadio->isChecked()){
            QSqlQueryModel * x = EmployeeModel();
            for(int i=0; i< x->rowCount(); i++)
            {
                ui->EmployeeView->showRow(i);
            }
        }
        if(ui->CurrentRadio->isChecked()){
            QSqlQueryModel * x = EmployeeModel();
            for(int i=0; i< x->rowCount(); i++)
            {
                int current = x->record(i).value(7).toInt();
                if(current == 1)
                   ui->EmployeeView->showRow(i);
                else
                    ui->EmployeeView->hideRow(i);
            }
        }
        if(ui->PastRadio->isChecked()){
            QSqlQueryModel * x = EmployeeModel();
            for(int i=0; i< x->rowCount(); i++)
            {
                int current = x->record(i).value(7).toInt();
                if(current == 1)
                   ui->EmployeeView->hideRow(i);
                else
                    ui->EmployeeView->showRow(i);
            }
        }
    }
    else{
        ui->error->setText("Disconnected From Database. Verify Connection");
    }
}
QSqlQueryModel * MainForm::EmployeeModel(){
//    QSqlTableModel * model = new QSqlTableModel(0,data);
//    model->setTable("employeelist");
//    model->select();
//    model->setEditStrategy(QSqlTableModel::OnFieldChange);
    QString current = "%";
    if(ui->CurrentRadio->isChecked()){
        current = "1";
    }else if( ui->PastRadio->isChecked()){
        current = "0";
    }
    QSqlQueryModel * model = new QSqlQueryModel();
    QSqlQuery * qry = new QSqlQuery(data);
    qry->prepare("SELECT * FROM employeelist WHERE current LIKE '"+current+"'  ORDER BY name,adminstatus,pin");
    qry->exec();
    model->setQuery(*qry);
    model->setHeaderData(0,Qt::Horizontal,tr("Id"));
    model->setHeaderData(1,Qt::Horizontal,tr("Name"));
    model->setHeaderData(2,Qt::Horizontal,tr("Crew"));
    model->setHeaderData(3,Qt::Horizontal,tr("Pin"));
    model->setHeaderData(4,Qt::Horizontal,tr("Adminstatus"));
    model->setHeaderData(5,Qt::Horizontal,tr("Shiftcount"));
    model->setHeaderData(6,Qt::Horizontal,tr("Clocked In"));
    model->setHeaderData(7,Qt::Horizontal,tr("Employed"));

    //qDebug()<<"EMPLOYEE MODEL: "<<qry->lastError().text()<<qry->executedQuery();
    return model;

}
QSortFilterProxyModel * MainForm::EmployeeFilterModel(){
    QSortFilterProxyModel * m = new QSortFilterProxyModel(this);
    m->setDynamicSortFilter(true);
    m->setSourceModel(employeemodel);
    return m;
}
void MainForm::displayEmployeeSuccess(){
    QMessageBox::StandardButton reply;
    if(!employeeeditform->getSuccess())
        reply = QMessageBox::information(this, "Time-Track",employeeeditform->getSuccessMsg(),QMessageBox::Ok);
}
void MainForm::refreshFromEmployeeEdit(){
    refreshEmployeeTab();
    refreshShiftEmployee();
    shifteditform->updateShiftEdit();

}

/* Option menu 1 for EmployeeTab*/

void MainForm::on_EmployeeAdd_clicked()
{

    if(data.open()){
        employeeeditform = new EmployeeEditForm(this);
        establishConnections();
        employeeeditform->AddEmployee();
        ui->error->setText("");
    }
    else{
        ui->error->setText("Disconnected From Database. Verify Connection and Try Again");
    }
}
int MainForm::generateRandom(){
    int x = rand()%9000+1000;
    QSqlQuery * qry = new QSqlQuery(data);
    qry->prepare("select pin from employeelist");
    if(qry->exec())
    {
        while(qry->next())
        {
            if(qry->value(0).toInt()==x)
                return generateRandom();
        }
    }
    return x;
}
void MainForm::on_EmployeeEdit_clicked()
{

    if(data.open()){
        employeeeditform = new EmployeeEditForm(this);
        establishConnections();
        QModelIndexList list = ui->EmployeeView->selectionModel()->selection().indexes();
        if(list != QModelIndexList())
        {
            QString id = employeemodel->record(employeefiltermodel->mapToSource(list.at(0)).row()).value(0).toString();
            employeeeditform->EditEmployee(id);
        }
        ui->error->setText("");
    }
    else{
        ui->error->setText("Disconnected From Database. Verify Connection and Try Again");
    }

}
void MainForm::on_EmployeeDelete_clicked()
{
    if(data.open()){
        QSqlQuery * qry = new QSqlQuery(data);
        QModelIndexList list = ui->EmployeeView->selectionModel()->selection().indexes();
        if(list != QModelIndexList())
        {
            QMessageBox::StandardButton reply;
            reply = QMessageBox::question(this, "Time-Track",   "Are you sure you want to perminantly delete "+
                                          employeemodel->record(employeefiltermodel->mapToSource(list.at(0)).row()).value(1).toString()
                                          + "'s records?", QMessageBox::Yes|QMessageBox::No);
            if (reply == QMessageBox::Yes) {

                for(int i=0; i< list.count(); i++)
                {
                    QString id = employeemodel->record(employeefiltermodel->mapToSource(list.at(i)).row()).value(0).toString();
                    qry->clear();
                    qry->prepare("DELETE from employeelist where id='"+id+"'");
                    qry->exec();
                }
                refreshEmployeeTab();
                refreshShiftEmployee();
                ui->MainTabs->setCurrentIndex(0);
                shifteditform->updateShiftEdit();
            }
        }
        ui->error->setText("");
    }
    else{
        ui->error->setText("Disconnected From Database. Verify Connection and Try Again");
    }

}


/* Option menu 2 for EmployeeTab*/

void MainForm::on_EmployeeName_clicked()
{
    refreshEmployeeTab();
}
void MainForm::on_EmployeeCrew_clicked()
{
    refreshEmployeeTab();
}
void MainForm::on_EmployeePin_clicked()
{
    refreshEmployeeTab();
}
void MainForm::on_EmployeeAdminStatus_clicked()
{
    refreshEmployeeTab();
}
void MainForm::on_EmployeeShiftCount_clicked()
{
    refreshEmployeeTab();
}
void MainForm::on_EmployeeActive_clicked()
{
    refreshEmployeeTab();
}
void MainForm::on_EmployeeId_clicked()
{
    refreshEmployeeTab();
}
void MainForm::on_EmployeeCurrent_clicked()
{
    refreshEmployeeTab();
}

/* Option menu 3 for EmployeeTab*/

void MainForm::on_AllRadio_toggled(bool checked)
{
    refreshEmployeeTab();
}
void MainForm::on_CurrentRadio_toggled(bool checked)
{    
    refreshEmployeeTab();
}
void MainForm::on_PastRadio_toggled(bool checked)
{
    refreshEmployeeTab();
}




// Project Section!

/* Initialization and refreshing of the projecttab in
 * 'sections' along wiht the themodel. This section is
 * particular in the way it has two tables. That is the
 * for the three models for the 2nd table. 'ProjectItemModelFirst'
 * is for just after initialization the the others are for
 * general use after that special case.
*/
void MainForm::ProjectTab(){


    ui->ProjectView->setSortingEnabled(true);
    ui->ProjectView->horizontalHeader()->setSectionResizeMode(QHeaderView::ResizeToContents);
    ui->ProjectView->hideColumn(0);
    ui->ProjectView->hideColumn(2);
    ui->ProjectName->setChecked(true);
    ui->ProjectCurrentRadio->setChecked(true);
    ui->ProjectDate->setChecked(true);

    ui->ProjectItemId->setChecked(false);
    ui->ProjectItemName->setChecked(true);    
    ui->ProjectItemDimension->setChecked(true);
    ui->ProjectItemQuantity->setChecked(true);
    ui->ProjectItemEstHours->setChecked(true);
    ui->ProjectItemEstHourUnit->setChecked(true);
    ui->ProjectItemActHours->setChecked(true);
    ui->ProjectItemActHourUnit->setChecked(true);
    ui->ProjectItemDifference->setChecked(true);



    ui->ShiftEmployeeCombo->setSizeAdjustPolicy(QComboBox::AdjustToContents);

    projectfiltermodel = ProjectFilterModel();
    projectmodel =  ProjectModel();
    ui->ProjectView->setModel(projectfiltermodel);

    establishConnections();


}
void MainForm::on_ProjectView_clicked(const QModelIndex &index)
{
    refreshProjectItemTab();
    refreshProjectItemTable();
}
void MainForm::refreshProjectStuff(){
    refreshShiftProject();
    shifteditform->ProjectInitialize();
    clockoutForm->ProjectInitialize();
}
void MainForm::refreshProjectTab(){
    projectmodel = ProjectModel();
    projectfiltermodel = ProjectFilterModel();
    ui->ProjectView->setModel(projectfiltermodel);
    if(data.open()){
        ui->error->setText("");

        establishConnections();        

        if(ui->ProjectId->isChecked())
            ui->ProjectView->showColumn(0);
        else
            ui->ProjectView->hideColumn(0);

        if(ui->ProjectName->isChecked())
            ui->ProjectView->showColumn(1);
        else
            ui->ProjectView->hideColumn(1);

        if(ui->ProjectCurrent->isChecked())
            ui->ProjectView->showColumn(2);
        else
            ui->ProjectView->hideColumn(2);
        if(ui->ProjectDate->isChecked())
            ui->ProjectView->showColumn(3);
        else
            ui->ProjectView->hideColumn(3);
        for(int i=1; i< projectfiltermodel->rowCount(); i++)
        {
            ui->ProjectView->showRow(i);
        }
        if(ui->ProjectAllRadio->isChecked()){
            QSqlQueryModel * x = ProjectModel();
            for(int i=0; i< x->rowCount(); i++)
            {
                ui->ProjectView->showRow(i);
            }
        }
        if(ui->ProjectCurrentRadio->isChecked()){
            QSqlQueryModel * x = ProjectModel();
            for(int i=0; i< x->rowCount(); i++)
            {
                int current = x->record(i).value(2).toInt();
                if(current == 1)
                   ui->ProjectView->showRow(i);
                else
                    ui->ProjectView->hideRow(i);
            }
        }
        if(ui->ProjectPastRadio->isChecked()){
            QSqlQueryModel * x = ProjectModel();
            for(int i=0; i< x->rowCount(); i++)
            {
                int current = x->record(i).value(2).toInt();
                if(current == 1)
                   ui->ProjectView->hideRow(i);
                else
                    ui->ProjectView->showRow(i);
            }
        }

    }
    else{
        ui->error->setText("Disconnected From Database. Verify Connection");
    }




    }
void MainForm::refreshProjectItemTab(){
    //qDebug()<<"refresh item";

    if(ui->ProjectItemId->isChecked())
        ui->ProjectItemWidget->showColumn(1);
    else
        ui->ProjectItemWidget->hideColumn(1);
    if(ui->ProjectItemName->isChecked())
        ui->ProjectItemWidget->showColumn(2);
    else
        ui->ProjectItemWidget->hideColumn(2);
    if(ui->ProjectItemQuantity->isChecked())
        ui->ProjectItemWidget->showColumn(3);
    else
        ui->ProjectItemWidget->hideColumn(3);
    if(ui->ProjectItemDimension->isChecked())
        ui->ProjectItemWidget->showColumn(4);
    else
        ui->ProjectItemWidget->hideColumn(4);
    if(ui->ProjectItemEstHours->isChecked())
        ui->ProjectItemWidget->showColumn(5);
    else
        ui->ProjectItemWidget->hideColumn(5);
    if(ui->ProjectItemEstHourUnit->isChecked())
        ui->ProjectItemWidget->showColumn(6);
    else
        ui->ProjectItemWidget->hideColumn(6);
    if(ui->ProjectItemActHours->isChecked())
        ui->ProjectItemWidget->showColumn(7);
    else
        ui->ProjectItemWidget->hideColumn(7);
    if(ui->ProjectItemActHourUnit->isChecked())
        ui->ProjectItemWidget->showColumn(8);
    else
        ui->ProjectItemWidget->hideColumn(8);
    if(ui->ProjectItemDifference->isChecked())
        ui->ProjectItemWidget->showColumn(9);
    else
        ui->ProjectItemWidget->hideColumn(9);
}
QSqlQueryModel * MainForm::ProjectModel(){
//    QSqlTableModel * model = new QSqlTableModel(0,data);
//    model->setTable("projectlist");
//    model->setEditStrategy(QSqlTableModel::OnFieldChange);
//    model->select();
    QString current = "%";
    if(ui->ProjectCurrentRadio->isChecked()){
        current = "1";
    }else if( ui->ProjectPastRadio->isChecked()){
        current = "0";
    }
    QSqlQueryModel * model = new QSqlQueryModel();
    QSqlQuery * qry = new QSqlQuery(data);
    qry->prepare("SELECT * FROM projectlist WHERE current LIKE '"+current+"'  ORDER BY name,date ");
    qry->exec();
    model->setQuery(*qry);
    model->setHeaderData(0,Qt::Horizontal,tr("Id"));
    model->setHeaderData(1,Qt::Horizontal,tr("Name"));
    model->setHeaderData(2,Qt::Horizontal,tr("Current"));
    model->setHeaderData(3,Qt::Horizontal,tr("Date"));

    //qDebug()<<"PROJECT MODEL: "<<qry->lastError().text()<<qry->executedQuery();
    return model;

}
QSortFilterProxyModel * MainForm::ProjectFilterModel(){
    QSortFilterProxyModel * m = new QSortFilterProxyModel(this);
    m->setDynamicSortFilter(true);
    m->setSourceModel(ProjectModel());
    return m;
}
void MainForm::refreshProjectItemTable(){
    QModelIndexList  list =  ui->ProjectView->selectionModel()->selection().indexes();
    qDebug()<<list.length();

    ui->ProjectItemWidget->horizontalHeader()->setSectionResizeMode(QHeaderView::ResizeToContents);
    QModelIndex index = list.at(0);
    QString id = projectmodel->record(projectfiltermodel->mapToSource(index).row()).value(0).toString();
    ui->ProjectItemWidget->clear();
    ui->ProjectItemWidget->clearContents();
    ui->ProjectItemWidget->setRowCount(0);
    ui->ProjectItemWidget->setColumnCount(10);
    ui->ProjectItemWidget->setHorizontalHeaderItem(0,new QTableWidgetItem("Id"));
    ui->ProjectItemWidget->setHorizontalHeaderItem(1,new QTableWidgetItem("itemId"));
    ui->ProjectItemWidget->setHorizontalHeaderItem(2,new QTableWidgetItem("Name"));
    ui->ProjectItemWidget->setHorizontalHeaderItem(3,new QTableWidgetItem("Quantity"));
    ui->ProjectItemWidget->setHorizontalHeaderItem(4,new QTableWidgetItem("Unit"));
    ui->ProjectItemWidget->setHorizontalHeaderItem(5,new QTableWidgetItem("Est. Hours"));
    ui->ProjectItemWidget->setHorizontalHeaderItem(6,new QTableWidgetItem("Est. Hours/Unit"));
    ui->ProjectItemWidget->setHorizontalHeaderItem(7,new QTableWidgetItem("Act. Hours"));
    ui->ProjectItemWidget->setHorizontalHeaderItem(8,new QTableWidgetItem("Act. Hours/Unit"));
    ui->ProjectItemWidget->setHorizontalHeaderItem(9,new QTableWidgetItem("Difference(%)"));
    ui->ProjectItemWidget->hideColumn(0);
    ui->ProjectItemWidget->hideColumn(1);





    QSqlQuery * qry = new QSqlQuery(data);
    qry->prepare("SELECT id,itemid,name,quantity,dimension,ehours FROM Project"+id+"");
    if(qry->exec()){
        while(qry->next()){
            ui->ProjectItemWidget->setRowCount(ui->ProjectItemWidget->rowCount()+1);
            ui->ProjectItemWidget->setItem(ui->ProjectItemWidget->rowCount()-1,0,new QTableWidgetItem(qry->value(0).toString()));
            ui->ProjectItemWidget->setItem(ui->ProjectItemWidget->rowCount()-1,1,new QTableWidgetItem(qry->value(1).toString()));
            ui->ProjectItemWidget->setItem(ui->ProjectItemWidget->rowCount()-1,2,new QTableWidgetItem(qry->value(2).toString()));
            ui->ProjectItemWidget->setItem(ui->ProjectItemWidget->rowCount()-1,3,new QTableWidgetItem(qry->value(3).toString()));
            ui->ProjectItemWidget->setItem(ui->ProjectItemWidget->rowCount()-1,4,new QTableWidgetItem(qry->value(4).toString()));
            ui->ProjectItemWidget->setItem(ui->ProjectItemWidget->rowCount()-1,5,new QTableWidgetItem(qry->value(5).toString()));
        }
    }

    for(int i= 0; i<ui->ProjectItemWidget->rowCount();i++){
        //Est. Hours/Unit
        QString e = QString::number(((double)ui->ProjectItemWidget->item(i,5)->text().toInt())/ui->ProjectItemWidget->item(i,3)->text().toInt(),'f',3);
        ui->ProjectItemWidget->setItem(i,6,new QTableWidgetItem(e+" HR/"+ui->ProjectItemWidget->item(i,4)->text()));

        qry->clear();
        int h =0,m=0;
        qry->prepare("SELECT time FROM shiftlist WHERE projectid='"+id+"' AND itemid='"+ui->ProjectItemWidget->item(i,1)->text()+"'");
        if(qry->exec()){
            while(qry->next()){
               if(qry->value(0).toString() != ""){
                   QString time = qry->value(0).toString();
                   h += time.split(":")[0].toInt();
                   m += time.split(":")[1].toInt();
               }

            }
        }
        h+=m/60;
        m = m%60;
        double ahours = h + (((double)m)/60);
        //qDebug()<<qry->lastQuery();
        ui->ProjectItemWidget->setItem(i,7,new QTableWidgetItem(QString::number(ahours)));



        QString a = QString::number(((double)ui->ProjectItemWidget->item(i,7)->text().toDouble())/ui->ProjectItemWidget->item(i,3)->text().toDouble(),'f',3);
        ui->ProjectItemWidget->setItem(i,8,new QTableWidgetItem(a+" HR/"+ui->ProjectItemWidget->item(i,4)->text()));

        QString p = QString::number(((double)(100*ui->ProjectItemWidget->item(i,7)->text().toDouble()))/ui->ProjectItemWidget->item(i,5)->text().toDouble(),'f',0);
        if( ui->ProjectItemWidget->item(i,5)->text().toInt() == 0){
            ui->ProjectItemWidget->setItem(i,9,new QTableWidgetItem(p));
        }else{
            ui->ProjectItemWidget->setItem(i,9,new QTableWidgetItem(p+"%"));
        }

    }
    ui->ProjectItemWidget->setRowCount(ui->ProjectItemWidget->rowCount()+2);
    ui->ProjectItemWidget->setItem(ui->ProjectItemWidget->rowCount()-1,2,new QTableWidgetItem("Total:"));
    double etotal=0.0, atotal=0.0;
    for(int i= 0; i<ui->ProjectItemWidget->rowCount()-2;i++){
        etotal+=ui->ProjectItemWidget->item(i,5)->text().toDouble();
        atotal+=ui->ProjectItemWidget->item(i,7)->text().toDouble();
    }
    ui->ProjectItemWidget->setItem(ui->ProjectItemWidget->rowCount()-1,5,new QTableWidgetItem(QString::number(etotal)));
    ui->ProjectItemWidget->setItem(ui->ProjectItemWidget->rowCount()-1,7,new QTableWidgetItem(QString::number(atotal)));
    QString t = QString::number(((100*ui->ProjectItemWidget->item(ui->ProjectItemWidget->rowCount()-1,7)->text().toDouble()))/ui->ProjectItemWidget->item(ui->ProjectItemWidget->rowCount()-1,5)->text().toDouble(),'f',0);
    if( ui->ProjectItemWidget->item(ui->ProjectItemWidget->rowCount()-1,5)->text().toInt() == 0){
        ui->ProjectItemWidget->setItem(ui->ProjectItemWidget->rowCount()-1,9,new QTableWidgetItem(t));
    }else{
        ui->ProjectItemWidget->setItem(ui->ProjectItemWidget->rowCount()-1,9,new QTableWidgetItem(t+"%"));
    }

}

void MainForm::displayProjectSuccess(){
    QMessageBox::StandardButton reply;
    if(!projecteditform->getSuccess())
        reply = QMessageBox::information(this, "Time-Track",projecteditform->getSuccessMsg(),QMessageBox::Ok);
}
void MainForm::refreshFromProjectEdit(){
    refreshProjectTab();
    refreshShiftProject();
    shifteditform->updateShiftEdit();

}
/* Option menu 1 for ProjectTab*/

void MainForm::on_ProjectAdd_clicked()
{
    if(data.open()){
        projecteditform = new ProjectEditForm(this);
        establishConnections();
        projecteditform->AddProject();
        ui->error->setText("");
    }
    else{
        ui->error->setText("Disconnected From Database. Verify Connection and Try Again");
    }
}
void MainForm::on_ProjectDelete_clicked()
{
    if(data.open()){
        QSqlQuery * qry = new QSqlQuery(data);
        QModelIndexList list = ui->ProjectView->selectionModel()->selection().indexes();
        if(list != QModelIndexList())
        {
            QMessageBox::StandardButton reply;
            reply = QMessageBox::question(this, "Time-Track", "Are you sure you want to delete the " +
                                          projectmodel->record(projectfiltermodel->mapToSource(list.at(0)).row()).value(1).toString()
                                          + "'s records?", QMessageBox::Yes|QMessageBox::No);
            if (reply == QMessageBox::Yes) {
                for(int i=0; i< list.count(); i++)
                {
                    QModelIndex index =list.at(i);
                    int idInt = projectmodel->record(projectfiltermodel->mapToSource(index).row()).value(0).toInt();
                    QString id = "Project"+QString::number(idInt);

                    qry->clear();
                    qry->prepare("DROP TABLE "+id+"");
                    qry->exec();
                    qry->clear();
                    qry->prepare("DELETE from projectlist where id='"+QString::number(idInt)+"'");
                    qry->exec();
                }
            }
            refreshProjectTab();
            refreshShiftProject();
            ui->MainTabs->setCurrentIndex(1);
            shifteditform->updateShiftEdit();
        }
        ui->error->setText("");
    }
    else{
        ui->error->setText("Disconnected From Database. Verify Connection and Try Again");
    }
}
void MainForm::on_ProjectArchive_clicked()
{
    if(data.open()){
        projecteditform = new ProjectEditForm(this);
        establishConnections();
        QModelIndexList list = ui->ProjectView->selectionModel()->selection().indexes();
        if(list != QModelIndexList())
        {
            QString id = projectmodel->record(projectfiltermodel->mapToSource(list.at(0)).row()).value(0).toString();
            projecteditform->EditProject(id);
        }
        ui->error->setText("");
    }
    else{
        ui->error->setText("Disconnected From Database. Verify Connection and Try Again");
    }
}

/* Option menu 2 for ProjectTab*/

void MainForm::on_ProjectName_clicked()
{
    refreshProjectTab();
}
void MainForm::on_ProjectId_clicked()
{
    refreshProjectTab();
}
void MainForm::on_ProjectCurrent_clicked()
{
    refreshProjectTab();
}
void MainForm::on_ProjectDate_clicked()
{
    refreshProjectTab();
}

/* Option menu 3 for ProjectTab*/

void MainForm::on_ProjectAllRadio_toggled(bool checked)
{
    refreshProjectTab();
}
void MainForm::on_ProjectCurrentRadio_toggled(bool checked)
{
    refreshProjectTab();
}
void MainForm::on_ProjectPastRadio_toggled(bool checked)
{
    refreshProjectTab();
}

/* Option menu 4 for ProjectTab*/

void MainForm::on_ProjectItemName_clicked()
{
    refreshProjectItemTab();
}
void MainForm::on_ProjectItemId_clicked()
{
    refreshProjectItemTab();
}
void MainForm::on_ProjectItemQuantity_clicked()
{
    refreshProjectItemTab();
}
void MainForm::on_ProjectItemDimension_clicked()
{
    refreshProjectItemTab();
}
void MainForm::on_ProjectItemEstHours_clicked()
{
    refreshProjectItemTab();
}
void MainForm::on_ProjectItemEstHourUnit_clicked()
{
    refreshProjectItemTab();
}
void MainForm::on_ProjectItemActHours_clicked()
{
    refreshProjectItemTab();
}
void MainForm::on_ProjectItemActHourUnit_clicked()
{
    refreshProjectItemTab();
}
void MainForm::on_ProjectItemDifference_clicked()
{
    refreshProjectItemTab();
}




// Item Section!

/* Initialization,model,and refreshing for the
 * 'itemtab' in 'sections'
 */

void MainForm::ItemTab(){
    itemmodel= ItemModel();
    itemfiltermodel = ItemFilterModel();
    ui->ItemView->setModel(itemfiltermodel);

    establishConnections();
    ui->ItemView->horizontalHeader()->setSectionResizeMode(QHeaderView::ResizeToContents);
    ui->ItemView->hideColumn(1);
    ui->ItemView->setSortingEnabled(true);
    ui->ItemName->setChecked(true);
    ui->ItemId->setChecked(false);
    ui->ItemCategory->setChecked(false);
    ui->ItemSub->setChecked(false);
    ui->ItemDimension->setChecked(false);
    ui->ItemCategory->hide();
    ui->ItemSub->hide();
    ui->ItemDimension->hide();

}
QSqlQueryModel * MainForm::ItemModel(){



    QSqlQueryModel * model = new QSqlQueryModel();
    QSqlQuery * qry = new QSqlQuery(data);
    qry->prepare("SELECT * FROM itemlist ORDER BY name ");
    qry->exec();
    model->setQuery(*qry);




    model->setHeaderData(0,Qt::Horizontal,tr("Id"));
    model->setHeaderData(1,Qt::Horizontal,tr("Name"));
    model->setHeaderData(2,Qt::Horizontal,tr("Category"));
    model->setHeaderData(3,Qt::Horizontal,tr("Sub-Catergory"));
    model->setHeaderData(4,Qt::Horizontal,tr("Dimension"));
    return model;
}
QSortFilterProxyModel * MainForm::ItemFilterModel(){
    QSortFilterProxyModel * m = new QSortFilterProxyModel(this);
    m->setDynamicSortFilter(true);
    m->setSourceModel(itemmodel);
    return m;
}
void MainForm::refreshItemTab(){
    itemmodel= ItemModel();
    itemfiltermodel = ItemFilterModel();
    ui->ItemView->setModel(itemfiltermodel);
    if(data.open()){
        ui->error->setText("");
        establishConnections();
        if(ui->ItemCategory->isChecked())
            ui->ItemView->showColumn(2);
        else
            ui->ItemView->hideColumn(2);
        if(ui->ItemSub->isChecked())
            ui->ItemView->showColumn(3);
        else
            ui->ItemView->hideColumn(3);
        if(ui->ItemDimension->isChecked())
            ui->ItemView->showColumn(4);
        else
            ui->ItemView->hideColumn(4);
        if(ui->ItemId->isChecked())
            ui->ItemView->showColumn(0);
        else
            ui->ItemView->hideColumn(0);
        if(ui->ItemName->isChecked())
            ui->ItemView->showColumn(1);
        else
            ui->ItemView->hideColumn(1);
    }
    else{
        ui->error->setText("Disconnected From Database. Verify Connection");
    }
}
void MainForm::refreshItemStuff(){
    refreshShiftItem();
    shifteditform->ItemInitialize();
    clockoutForm->ItemInitialize();
}
void MainForm::displayItemSuccess(){
    QMessageBox::StandardButton reply;
    if(!itemeditform->getSuccess())
        reply = QMessageBox::information(this, "Time-Track",itemeditform->getSuccessMsg(),QMessageBox::Ok);
}
void MainForm::refreshFromItemEdit(){
    refreshItemTab();
    refreshShiftItem();
    projecteditform->updateProjectEdit();

}
/* Option menu 1 for ItemTab*/

void MainForm::on_ItemAdd_clicked()
{
    if(data.open()){
        itemeditform = new ItemEditForm(this);
        establishConnections();
        itemeditform->AddItem();
        ui->error->setText("");
    }
    else{
        ui->error->setText("Disconnected From Database. Verify Connection and Try Again");
    }
}
void MainForm::on_ItemEdit_clicked()
{
    if(data.open()){
        itemeditform = new ItemEditForm(this);
        establishConnections();
        QModelIndexList list = ui->ItemView->selectionModel()->selection().indexes();
        if(list != QModelIndexList())
        {
            QString id = itemmodel->record(itemfiltermodel->mapToSource(list.at(0)).row()).value(0).toString();
            itemeditform->EditItem(id);
        }
        ui->error->setText("");
    }
    else{
        ui->error->setText("Disconnected From Database. Verify Connection and Try Again");
    }
}
void MainForm::on_ItemDelete_clicked()
{
    if(data.open()){
        QSqlQuery * qry = new QSqlQuery(data);
        QModelIndexList list = ui->ItemView->selectionModel()->selection().indexes();
        if(list != QModelIndexList())
        {
            QMessageBox::StandardButton reply;
            reply = QMessageBox::question(this, "Time-Track",   "Are you sure you want to permanantly delete the " +
                                          itemmodel->record(itemfiltermodel->mapToSource(list.at(0)).row()).value(1).toString()
                                          + " item?", QMessageBox::Yes|QMessageBox::No);
            if (reply == QMessageBox::Yes) {
                for(int i=0; i< list.count(); i++)
                {
                    QString id = itemmodel->record(itemfiltermodel->mapToSource(list.at(i)).row()).value(0).toString();
                    qry->clear();
                    qry->prepare("DELETE from itemlist where id='"+id+"'");
                    qry->exec();
                }
                refreshItemTab();
                refreshShiftItem();
                ui->MainTabs->setCurrentIndex(2);
            }
        }
        ui->error->setText("");
    }
    else{
        ui->error->setText("Disconnected From Database. Verify Connection and Try Again");
    }
}

/* Option menu 2 for ItemTab*/

void MainForm::on_ItemName_clicked()
{
    refreshItemTab();
}
void MainForm::on_ItemId_clicked()
{
    refreshItemTab();
}
void MainForm::on_ItemCategory_clicked()
{
    refreshItemTab();
}
void MainForm::on_ItemSub_clicked()
{
    refreshItemTab();
}
void MainForm::on_ItemDimension_clicked()
{
    refreshItemTab();
}




// Shift Section!

/* Initialization,three combobox refreshers,model,
 * and main refresher for the 'shiftTab' within 'sections'
*/

void MainForm::ShiftTab(){
    shiftmodel = ShiftModel();
    shiftfiltermodel=ShiftFilterModel();
    ui->ShiftView->setModel(shiftfiltermodel);

    int day = QDate::currentDate().dayOfWeek();
    ui->ShiftDate1->setDate(QDate::currentDate().addDays(-day));
    ui->ShiftDate2->setDate(QDate::currentDate().addDays(6-day));

    refreshShiftProject();
    refreshShiftItem();
    refreshShiftEmployee();



    ui->ShiftView->hideColumn(0);
    ui->ShiftView->hideColumn(1);
    ui->ShiftView->hideColumn(2);
    ui->ShiftView->hideColumn(3);
    ui->ShiftView->hideColumn(13);

    ui->ShiftView->horizontalHeader()->setSectionResizeMode(QHeaderView::ResizeToContents);
    ui->ShiftView->horizontalHeader()->setStretchLastSection(true);
    ui->ShiftEmployeeBox->setChecked(false);
    ui->ShiftProjectBox->setChecked(false);
    ui->ShiftItemBox->setChecked(false);

    ui->ShiftEmployeeCombo->setEnabled(false);
    ui->ShiftProjectCombo->setEnabled(false);
    ui->ShiftItemCombo->setEnabled(false);
    ui->ShiftView->setSortingEnabled(true);

}
void MainForm::refreshShiftEmployee(){
    QSqlQueryModel * a = new QSqlQueryModel();
    QSqlQuery * A = new QSqlQuery(data);
    A->prepare("Select name from employeelist ORDER BY name ASC");
    A->exec();
    a->setQuery(*A);
    ui->ShiftEmployeeCombo->setModel(a);
    ui->ShiftEmployeeCombo->setSizeAdjustPolicy(QComboBox::AdjustToContents);

    QCompleter * comp = new QCompleter(this);
    comp->setWidget(ui->ShiftEmployeeCombo);
    comp->setCompletionMode(QCompleter::PopupCompletion);
    comp->setCaseSensitivity(Qt::CaseInsensitive);
    ui->ShiftEmployeeCombo->setCurrentText("");
}
void MainForm::refreshShiftProject(){
    QSqlQueryModel * b = new QSqlQueryModel();
    QSqlQuery * B = new QSqlQuery(data);
    B->prepare("Select name from projectlist ORDER BY name ASC");
    B->exec();
    b->setQuery(*B);
    ui->ShiftProjectCombo->setModel(b);
    ui->ShiftProjectCombo->setSizeAdjustPolicy(QComboBox::AdjustToContents);

    QCompleter * comp = new QCompleter(this);
    comp->setWidget(ui->ShiftProjectCombo);
    comp->setCompletionMode(QCompleter::PopupCompletion);
    comp->setCaseSensitivity(Qt::CaseInsensitive);
    ui->ShiftProjectCombo->setCurrentText("");
}
void MainForm::refreshShiftItem(){
    QSqlQueryModel * c = new QSqlQueryModel();
    QSqlQuery * C = new QSqlQuery(data);

    if(ui->ShiftProjectBox->isChecked())
    {
        QString id;
        C->prepare("SELECT id FROM projectlist WHERE name='"+ui->ShiftProjectCombo->currentText()+"'");
        if( C->exec()){
            while(C->next()){
                id = C->value(0).toString();}}
        QString x = "project"+id;
        C->clear();
        C->prepare("SELECT name from project"+id+" ORDER BY name ASC");

    }
    else{
        C->prepare("Select name from itemlist ORDER BY name ASC");
    }
    C->exec();
    //qDebug()<<C->lastError().text();
    c->setQuery(*C);
    ui->ShiftItemCombo->setModel(c);
    ui->ShiftItemCombo->setSizeAdjustPolicy(QComboBox::AdjustToContents);

    QCompleter * comp = new QCompleter(this);
    comp->setWidget(ui->ShiftItemCombo);
    comp->setCompletionMode(QCompleter::PopupCompletion);
    comp->setCaseSensitivity(Qt::CaseInsensitive);
    ui->ShiftItemCombo->setCurrentText("");
}
QSqlQueryModel * MainForm::ShiftModel(){
    //QSqlTableModel * model = new QSqlTableModel(0,data);
    //model->setTable("shiftlist");
    QSqlQueryModel * model = new QSqlQueryModel();
    QSqlQuery * qry = new QSqlQuery(data);
    QString employee,project,item;
    QString d1 = ui->ShiftDate1->date().toString("yyyy-M-dd");
    QString d2 = ui->ShiftDate2->date().toString("yyyy-M-dd");
    if (ui->ShiftEmployeeBox->isChecked()){
        employee = ui->ShiftEmployeeCombo->currentText();
    }else{
        employee = "%";
    }
    if (ui->ShiftProjectBox->isChecked()){
        project = ui->ShiftProjectCombo->currentText();
    }else{
        project = "%";
    }
    if (ui->ShiftItemBox->isChecked()){
        item = ui->ShiftItemCombo->currentText();
    }else{
        item = "%";
    }
    QString q = "SELECT * FROM shiftlist WHERE datein >='"+d1+"' AND datein <='"+d2+"'";
    if(ui->ShiftEmployeeBox->isChecked())
        q = q+" AND employeename='"+employee+"'";
    if(ui->ShiftProjectBox->isChecked())
        q = q+" AND projectname='"+project+"'";
    if(ui->ShiftItemBox->isChecked())
        q = q+" AND itemname='"+item+"'";
    q = q+ " ORDER BY datein,timein,employeename";
    qry->prepare(q);
    qry->exec();
    model->setQuery(*qry);

//    model->setEditStrategy(QSqlTableModel::OnFieldChange);
//    model->select();
    model->setHeaderData(0,Qt::Horizontal,tr("Id"));
    model->setHeaderData(1,Qt::Horizontal,tr("Employee Id"));
    model->setHeaderData(2,Qt::Horizontal,tr("Project Id"));
    model->setHeaderData(3,Qt::Horizontal,tr("Item Id"));
    model->setHeaderData(4,Qt::Horizontal,tr("Employee Name"));
    model->setHeaderData(5,Qt::Horizontal,tr("Project Name"));
    model->setHeaderData(6,Qt::Horizontal,tr("Item Name"));
    model->setHeaderData(7,Qt::Horizontal,tr("Time In"));
    model->setHeaderData(8,Qt::Horizontal,tr("Time Out"));
    model->setHeaderData(9,Qt::Horizontal,tr("Date In"));
    model->setHeaderData(10,Qt::Horizontal,tr("Date out"));
    model->setHeaderData(11,Qt::Horizontal,tr("Lunch"));
    model->setHeaderData(12,Qt::Horizontal,tr("Time"));
    model->setHeaderData(14,Qt::Horizontal,tr("Description"));
    //qDebug()<<"SHIFT MODEL: "<<qry->lastError().text()<<qry->executedQuery();
    return model;

}
QSortFilterProxyModel * MainForm::ShiftFilterModel(){
    QSortFilterProxyModel * m = new QSortFilterProxyModel(this);
    m->setDynamicSortFilter(true);
    m->setSourceModel(shiftmodel);
    return m;
}
void MainForm::refreshShiftTab(){

    shiftmodel = ShiftModel();
    shiftfiltermodel = ShiftFilterModel();
    ui->ShiftView->setModel(shiftfiltermodel);
    if(data.open()){
        ui->error->setText("");
        int h = 0,m = 0;
        for(int i=0; i< shiftfiltermodel->rowCount(); i++)
        {
            QString time = shiftfiltermodel->data(shiftfiltermodel->index(i,12),Qt::DisplayRole).toString();
            if(!ui->ShiftView->isRowHidden(i)&&time!="")
            {
                QString hours = time.split(":")[0];
                QString minutes = time.split(":")[1];
                h += hours.toInt();
                m += minutes.toInt();
            }
        }
        h += m/60;
        m = m%60;

        QString hours = QString::number(h);
        QString minutes = QString::number(m);
        if (minutes == "0")
            minutes ="00";
        ui->ShiftTotalTime->setText(hours+":"+minutes);

    }
    else{
        ui->error->setText("Disconnected From Database. Verify Connection");
    }
}
void MainForm::displayShiftSuccess(){
    QMessageBox::StandardButton reply;
    if(!shifteditform->getSuccess())
        reply = QMessageBox::information(this, "Time-Track","Shift edit failed!",QMessageBox::Ok);
}

/* Option menu 1 for shiftTab*/

void MainForm::on_ShiftDate1_dateChanged(const QDate &date)
{
    refreshShiftTab();
}
void MainForm::on_ShiftDate2_dateChanged(const QDate &date)
{
    refreshShiftTab();
}

/* Option menu 2 for shiftTab*/

void MainForm::on_ShiftEmployeeBox_clicked()
{
    refreshShiftTab();
    if(ui->ShiftEmployeeBox->isChecked())
        ui->ShiftEmployeeCombo->setEnabled(true);
    else
        ui->ShiftEmployeeCombo->setEnabled(false);
}
void MainForm::on_ShiftProjectBox_clicked()
{
    refreshShiftTab();
    if(ui->ShiftProjectBox->isChecked())
        ui->ShiftProjectCombo->setEnabled(true);
    else
        ui->ShiftProjectCombo->setEnabled(false);
    refreshShiftItem();
}
void MainForm::on_ShiftItemBox_clicked()
{
    refreshShiftTab();
    if(ui->ShiftItemBox->isChecked())
        ui->ShiftItemCombo->setEnabled(true);
    else
        ui->ShiftItemCombo->setEnabled(false);
}
void MainForm::on_ShiftEmployeeCombo_currentTextChanged(const QString &arg1)
{
    refreshShiftTab();
}
void MainForm::on_ShiftProjectCombo_currentTextChanged(const QString &arg1)
{
    refreshShiftTab();
    refreshShiftItem();
}
void MainForm::on_ShiftItemCombo_currentTextChanged(const QString &arg1)
{
    refreshShiftTab();
}

/* Option menu 3 for shiftTab*/

void MainForm::on_ShiftAdd_clicked()
{
    if(data.open()){
        shifteditform = new ShiftEditForm(this);
        establishConnections();
        shifteditform->AddShift();
        ui->error->setText("");
    }
    else{
        ui->error->setText("Disconnected From Database. Verify Connection and Try Again");
    }

}
void MainForm::on_ShiftEdit_clicked()
{
    if(data.open()){
        shifteditform = new ShiftEditForm(this);
        establishConnections();
        QModelIndexList list = ui->ShiftView->selectionModel()->selection().indexes();
        if(list!= QModelIndexList())
        {
            QModelIndex index =list.at(0);
            QSqlRecord rec = shiftmodel->record(shiftfiltermodel->mapToSource(index).row());
            if(rec.value(2).toString()=="0")
                shifteditform->EditWorkingShift(rec.value(13).toString(),rec.value(0).toString());
            else
                shifteditform->EditFinishedShift(rec.value(13).toString());
        }
        ui->error->setText("");
    }
    else{
        ui->error->setText("Disconnected From Database. Verify Connection and Try Again");
    }
}
void MainForm::on_ShiftDelete_clicked()
{
    if(data.open()){
        QSqlQuery * qry = new QSqlQuery(data);
        QModelIndexList list = ui->ShiftView->selectionModel()->selection().indexes();
        if(list != QModelIndexList())
        {
            QMessageBox::StandardButton reply;
            reply = QMessageBox::question(this, "Time-Track",   "Are you sure you want to permanantly delete " +
                                          shiftmodel->record(shiftfiltermodel->mapToSource(list.at(0)).row()).value(4).toString()
                                          + "'s shift?", QMessageBox::Yes|QMessageBox::No);
            if (reply == QMessageBox::Yes) {

                for(int i=0; i< list.count(); i++)
                {
                    QString id = shiftmodel->record(shiftfiltermodel->mapToSource(list.at(i)).row()).value(13).toString();
                    qry->clear();
                    qry->prepare("DELETE from shiftlist where shiftid='"+id+"'");
                    qry->exec();
                    if(shiftmodel->record(shiftfiltermodel->mapToSource(list.at(i)).row()).value(2).toString()=="0")
                    {
                        qry->clear();
                        qry->prepare("update employeelist set active='0' where name='"+shiftmodel->record(shiftfiltermodel->mapToSource(list.at(i)).row()).value(4).toString()+"'");
                        qry->exec();
                    }
                }
            }
            refreshShiftTab();
        }
        ui->error->setText("");
    }
    else{
        ui->error->setText("Disconnected From Database. Verify Connection and Try Again");
    }
}





//Settings Section!

/* Quickly made this section so that I could maximize
 * and fullscreen the program easily plan on making
 * this cleaner later.*/

void MainForm::SettingsTab(){

    QSqlQueryModel * x = ProjectModel();
    currentProject = x->record(1).value(1).toString();

    ui->groupBox_19->hide();
    ui->groupBox_20->hide();

}
void MainForm::on_SettingsMax_clicked()
{
    this->setWindowState(Qt::WindowMaximized);
}
void MainForm::on_SettingsFull_clicked()
{

    this->setWindowState(Qt::WindowFullScreen);
}
void MainForm::on_SettingsExport_clicked()
{    
    if(data.open()){
        exportForm =  new ExportForm(this);
        establishConnections();
        exportForm->show();
        ui->error->setText("");
    }
    else{
        ui->error->setText("Disconnected From Database. Verify Connection and Try Again");
    }

}
void MainForm::exportToExcel(){

    QString FileToExport = exportForm->Location;
    QString TabletoExport = exportForm->Table;
    QString Name = exportForm->Name;
    QDate To = exportForm->To;
    QDate From = exportForm->From;
    QXlsx::Document doc;

    QXlsx::Format title;
    title.setFontBold(true);
    title.setFontSize(20);

    QXlsx::Format subtitle;
    subtitle.setFontBold(true);
    subtitle.setFontSize(15);

    QXlsx::Format label;
    label.setFontBold(true);
    label.setFontSize(13);

    if(TabletoExport == "Employees")
    {
        QSqlQueryModel * model = (QSqlQueryModel *)ui->EmployeeView->model();
        QXlsx::Format title;
        title.setFontBold(true);
        title.setFontSize(20);
        doc.setRowFormat(1,1,title);

        doc.write(1,1,"AACI");
        if(ui->AllRadio->isChecked())
            doc.write(2,1,"Employee(s): All");
        else if(ui->CurrentRadio->isChecked())
            doc.write(2,1,"Employee(s): Current");
        else if(ui->PastRadio->isChecked())
            doc.write(2,1,"Employee(s): Past");

        QXlsx::Format label;
        label.setFontBold(true);
        label.setFontSize(15);
        doc.setRowFormat(4,4,label);
        //qDebug()<<"title";

        doc.write(4,1,"Name");
        doc.write(4,2,"Pin");
        doc.write(4,3,"Admin Status");

//        for(int i = 0; i < model->rowCount(); i++){
//            for(int j = 0; j < model->columnCount(); j++){
//                if(!ui->EmployeeView->isRowHidden(i))
//                {
//                    if(j == 1 || j == 2 || j == 3)
//                        doc.write(i+5,j, model->record(i).value(j).toString());
//                }
//            }
//        }
        //qDebug()<<"write";
    }
    if(TabletoExport == "Projects")
    {
        QSqlQueryModel * model = (QSqlQueryModel *)ui->ProjectView->model();
        QXlsx::Format title;
        title.setFontBold(true);
        title.setFontSize(20);

        doc.setRowFormat(1,1,title);

        doc.write(1,1,"AACI");
        if(ui->ProjectAllRadio->isChecked())
            doc.write(2,1,"Project(s): All");
        else if(ui->ProjectCurrentRadio->isChecked())
            doc.write(2,1,"Project(s): Current");
        else if(ui->ProjectPastRadio->isChecked())
            doc.write(2,1,"Project(s): Past");


        QXlsx::Format label;
        label.setFontBold(true);
        label.setFontSize(15);
        doc.setRowFormat(4,4,label);


        doc.write(4,1,"Name");
        doc.write(4,2,"Date");

        for(int i = 0; i < model->rowCount(); i++){
            for(int j = 0; j < model->columnCount(); j++){
                if(!ui->ProjectView->isRowHidden(i))
                {
                    if(j == 1)
                        doc.write(i+5,j, model->record(i).value(j).toString());
                    else if(j == 3)
                        doc.write(i+5,j-1, model->record(i).value(j).toString());
                }
            }
        }
    }
    if(TabletoExport == "Items")
    {
        QSqlQueryModel * model = (QSqlQueryModel *)ui->ItemView->model();


        QXlsx::Format title;
        title.setFontBold(true);
        title.setFontSize(20);

        doc.setRowFormat(1,1,title);

        doc.write(1,1,"AACI");

        doc.write(2,1,"Items(s): ALL");


        QXlsx::Format label;
        label.setFontBold(true);
        label.setFontSize(15);
        doc.setRowFormat(4,4,label);


        doc.write(4,1,"Name");
        doc.write(4,2,"Category");
        doc.write(4,3,"Sub-Catergory");
        doc.write(4,4,"Dimension");

        for(int i = 0; i < model->rowCount(); i++){
            for(int j = 0; j < model->columnCount(); j++){
                if(!ui->ShiftView->isRowHidden(i))
                {
                    if(j == 1 || j == 2 || j == 3 || j == 4)
                        doc.write(i+5,j, model->record(i).value(j).toString());
                }
            }
        }
    }
    if(TabletoExport == "Shift")
    {
        QSqlQuery * qry = new QSqlQuery(data);
        doc.addSheet(Name);
        doc.write(1,1,"AACI - Time Sheet");
        doc.write(2,1,"Employee: " + Name);
        doc.write(3,1,"Period: "+To.toString("yyyy/M/dd")+" - "+ From.toString("yyyy/M/dd"));
        doc.setRowFormat(1,1,title);
        doc.setRowFormat(2,3,label);
        qry->clear();
        QString q = "SELECT * FROM shiftlist WHERE datein >='"+To.toString("yyyy-M-dd")+"' AND datein <='"+From.toString("yyyy-M-dd")+"'"
                    "AND employeename='"+Name+"' ORDER BY datein,timein,employeename";

        qry->prepare(q);
        int rowLocation =0;

        if(qry->exec())
        {
            int i =0;
            int total = 0;
            QHash<QString, int> otHash;
            QHash<QString, int> hash;

            while(qry->next())
            {
                int tot = hash["Total"];
                int ot = otHash["Total"];
                int regval = hash[qry->value(5).toString()];
                int otval = otHash[qry->value(5).toString()];
                QString time = qry->value(12).toString();

                if(tot >= 2400){
                    otHash[qry->value(5).toString()] = TimeStringToMinutes(time)+otval;
                    ot +=TimeStringToMinutes(time);

                }else if(tot + TimeStringToMinutes(time) < 2400){
                    hash[qry->value(5).toString()] = TimeStringToMinutes(time)+regval;
                    tot +=TimeStringToMinutes(time);
                }else{
                    int timeLeft = 2400 - tot;
                    hash[qry->value(5).toString()] = timeLeft+regval;
                    otHash[qry->value(5).toString()] = TimeStringToMinutes(time) - timeLeft;
                    tot += timeLeft;
                    ot += TimeStringToMinutes(time) - timeLeft;
                }
                total = ot+tot;
                hash["Total"] = tot;
                otHash["Total"] = ot;
                i++;
            }

            doc.setRowFormat(5,5,subtitle);
            doc.setRowFormat(6,6,label);
            doc.write(5,1,"Summary");
            doc.write(6,1,"Project");
            doc.write(6,6,"Reg");
            doc.write(6,7,"OT");
            doc.write(6,8,"Time");
            rowLocation =7;
            QList<QString> allPlaces = hash.keys();
            allPlaces.append(otHash.keys());
            QSet<QString> allPlacesWithoutDups = QSet<QString>::fromList(allPlaces);
            QSetIterator<QString> allPlacesIter(allPlacesWithoutDups);
            while(allPlacesIter.hasNext()){
                QString place = allPlacesIter.next();
                if(place != "Total" && place != ""){
                    doc.write(rowLocation,1,place);
                    if(hash[place]!= 0)
                        doc.write(rowLocation,6,minutesToTimeString(hash[place]));
                    if(otHash[place]!= 0)
                        doc.write(rowLocation,7,minutesToTimeString(otHash[place]));
                    doc.write(rowLocation,8,minutesToTimeString(hash[place]+otHash[place]));
                    rowLocation++;
                }
            }
            doc.setRowFormat(rowLocation,rowLocation,label);
            doc.write(rowLocation,1,"Total");
            doc.write(rowLocation,6,minutesToTimeString(hash["Total"]));
            doc.write(rowLocation,7,minutesToTimeString(otHash["Total"]));
            doc.write(rowLocation,8,minutesToTimeString(total));
            rowLocation++;
        }else{
            doc.write(5,1,"No project selected in the project tab. Please do so, and try again.");
        }

        int colLength[] = {7,4,8,9,4,5,4};
        if(qry->exec()){
            int total = 0;
            rowLocation+=3;
            doc.setRowFormat(rowLocation,rowLocation,subtitle);
            doc.write(rowLocation,1,"Details");
            rowLocation++;
            doc.setRowFormat(rowLocation,rowLocation,label);
            doc.write(rowLocation,1,"Project");
            doc.write(rowLocation,2,"Item");
            doc.write(rowLocation,3,"Clock In");
            doc.write(rowLocation,4,"Clock Out");
            doc.write(rowLocation,5,"Date");
            doc.write(rowLocation,6,"Lunch");
            doc.write(rowLocation,7,"Reg");
            doc.write(rowLocation,8,"OT");
            rowLocation++;
            while(qry->next()){
                if(qry->value(5).toString().length() > colLength[0]){ colLength[0] = qry->value(5).toString().length();}
                doc.write(rowLocation,1, qry->value(5).toString());
                if(qry->value(6).toString().length() > colLength[1]){ colLength[1] = qry->value(6).toString().length();}
                doc.write(rowLocation,2, qry->value(6).toString());
                if(qry->value(7).toString().length() > colLength[2]){ colLength[2] = qry->value(7).toString().length();}
                doc.write(rowLocation,3, qry->value(7).toString());
                if(qry->value(8).toString().length() > colLength[3]){ colLength[3] = qry->value(8).toString().length();}
                doc.write(rowLocation,4, qry->value(8).toString());
                if(qry->value(9).toString().length() > colLength[4]){ colLength[4] = qry->value(9).toString().length();}
                doc.write(rowLocation,5, qry->value(9).toString());
                if(qry->value(11).toString().length() > colLength[5]){ colLength[5] = qry->value(11).toString().length();}
                doc.write(rowLocation,6, qry->value(11).toString());
                if(qry->value(12).toString().length() > colLength[6]){ colLength[6] = qry->value(12).toString().length();}
                int minutes = TimeStringToMinutes(qry->value(12).toString());
                if(total+minutes <=2400)
                    doc.write(rowLocation,7, qry->value(12).toString());
                else if(total > 2400)
                    doc.write(rowLocation,8, qry->value(12).toString());
                else{
                    doc.write(rowLocation,7, minutesToTimeString(2400-total));
                    doc.write(rowLocation,8, minutesToTimeString(minutes-2400+total));
                }
                total+=minutes;
                rowLocation++;
            }
        }
        doc.setColumnWidth(1,1,colLength[0]+2);
        doc.setColumnWidth(2,2,colLength[1]+2);
        doc.setColumnWidth(3,3,colLength[2]+2);
        doc.setColumnWidth(4,4,colLength[3]+2);
        doc.setColumnWidth(5,5,colLength[4]+2);
        doc.setColumnWidth(6,6,colLength[5]+2);
        doc.setColumnWidth(8,8,colLength[6]+2);



        doc.saveAs(FileToExport);

    }
    if(TabletoExport == "All Shifts"){

        QSqlQuery* qry=new QSqlQuery(data);
        QString x = "where current='1' ";
        qry->prepare("select DISTINCT employeename from shiftlist WHERE datein >='"+To.toString("yyyy-M-dd")+"' AND datein <='"+From.toString("yyyy-M-dd")+"'ORDER BY employeename ASC");
        QList<QString> Names;
        if(qry->exec())
        {
            while(qry->next()){
                Names<<qry->value(0).toString();
            }
        }
        //qDebug()<<Names<<"before loop";

        for(int i = 0;i< Names.length(); i++){
            Name = Names[i];
            qry->clear();
            doc.addSheet(Name);
            doc.write(1,1,"AACI - Time Sheet");
            doc.write(2,1,"Employee: " + Name);
            doc.write(3,1,"Period: "+To.toString("yyyy/M/dd")+" - "+ From.toString("yyyy/M/dd"));
            doc.setRowFormat(1,1,title);
            doc.setRowFormat(2,3,label);
            qry->clear();
            QString q = "SELECT * FROM shiftlist WHERE datein >='"+To.toString("yyyy-M-dd")+"' AND datein <='"+From.toString("yyyy-M-dd")+"'"
                        "AND employeename='"+Name+"' ORDER BY datein,timein,employeename";

            qry->prepare(q);
            int rowLocation =0;

            if(qry->exec())
            {
                int i =0;
                int total = 0;
                QHash<QString, int> otHash;
                QHash<QString, int> hash;

                while(qry->next())
                {
                    int tot = hash["Total"];
                    int ot = otHash["Total"];
                    int regval = hash[qry->value(5).toString()];
                    int otval = otHash[qry->value(5).toString()];
                    QString time = qry->value(12).toString();

                    if(tot >= 2400){
                        otHash[qry->value(5).toString()] = TimeStringToMinutes(time)+otval;
                        ot +=TimeStringToMinutes(time);

                    }else if(tot + TimeStringToMinutes(time) < 2400){
                        hash[qry->value(5).toString()] = TimeStringToMinutes(time)+regval;
                        tot +=TimeStringToMinutes(time);
                    }else{
                        int timeLeft = 2400 - tot;
                        hash[qry->value(5).toString()] = timeLeft+regval;
                        otHash[qry->value(5).toString()] = TimeStringToMinutes(time) - timeLeft;
                        tot += timeLeft;
                        ot += TimeStringToMinutes(time) - timeLeft;
                    }
                    total = ot+tot;
                    hash["Total"] = tot;
                    otHash["Total"] = ot;
                    i++;
                }

                doc.setRowFormat(5,5,subtitle);
                doc.setRowFormat(6,6,label);
                doc.write(5,1,"Summary");
                doc.write(6,1,"Project");
                doc.write(6,6,"Reg");
                doc.write(6,7,"OT");
                doc.write(6,8,"Time");
                rowLocation =7;
                QList<QString> allPlaces = hash.keys();
                allPlaces.append(otHash.keys());
                QSet<QString> allPlacesWithoutDups = QSet<QString>::fromList(allPlaces);
                QSetIterator<QString> allPlacesIter(allPlacesWithoutDups);
                while(allPlacesIter.hasNext()){
                    QString place = allPlacesIter.next();
                    if(place != "Total" && place != ""){
                        doc.write(rowLocation,1,place);
                        if(hash[place]!= 0)
                            doc.write(rowLocation,6,minutesToTimeString(hash[place]));
                        if(otHash[place]!= 0)
                            doc.write(rowLocation,7,minutesToTimeString(otHash[place]));
                        doc.write(rowLocation,8,minutesToTimeString(hash[place]+otHash[place]));
                        rowLocation++;
                    }
                }
                doc.setRowFormat(rowLocation,rowLocation,label);
                doc.write(rowLocation,1,"Total");
                doc.write(rowLocation,6,minutesToTimeString(hash["Total"]));
                doc.write(rowLocation,7,minutesToTimeString(otHash["Total"]));
                doc.write(rowLocation,8,minutesToTimeString(total));
                rowLocation++;
            }else{
                doc.write(5,1,"No project selected in the project tab. Please do so, and try again.");
            }

            int colLength[] = {7,4,8,9,4,5,4};
            if(qry->exec()){
                int total = 0;
                rowLocation+=3;
                doc.setRowFormat(rowLocation,rowLocation,subtitle);
                doc.write(rowLocation,1,"Details");
                rowLocation++;
                doc.setRowFormat(rowLocation,rowLocation,label);
                doc.write(rowLocation,1,"Project");
                doc.write(rowLocation,2,"Item");
                doc.write(rowLocation,3,"Clock In");
                doc.write(rowLocation,4,"Clock Out");
                doc.write(rowLocation,5,"Date");
                doc.write(rowLocation,6,"Lunch");
                doc.write(rowLocation,7,"Reg");
                doc.write(rowLocation,8,"OT");
                rowLocation++;
                while(qry->next()){
                    if(qry->value(5).toString().length() > colLength[0]){ colLength[0] = qry->value(5).toString().length();}
                    doc.write(rowLocation,1, qry->value(5).toString());
                    if(qry->value(6).toString().length() > colLength[1]){ colLength[1] = qry->value(6).toString().length();}
                    doc.write(rowLocation,2, qry->value(6).toString());
                    if(qry->value(7).toString().length() > colLength[2]){ colLength[2] = qry->value(7).toString().length();}
                    doc.write(rowLocation,3, qry->value(7).toString());
                    if(qry->value(8).toString().length() > colLength[3]){ colLength[3] = qry->value(8).toString().length();}
                    doc.write(rowLocation,4, qry->value(8).toString());
                    if(qry->value(9).toString().length() > colLength[4]){ colLength[4] = qry->value(9).toString().length();}
                    doc.write(rowLocation,5, qry->value(9).toString());
                    if(qry->value(11).toString().length() > colLength[5]){ colLength[5] = qry->value(11).toString().length();}
                    doc.write(rowLocation,6, qry->value(11).toString());
                    if(qry->value(12).toString().length() > colLength[6]){ colLength[6] = qry->value(12).toString().length();}
                    int minutes = TimeStringToMinutes(qry->value(12).toString());
                    if(total+minutes <=2400)
                        doc.write(rowLocation,7, qry->value(12).toString());
                    else if(total > 2400)
                        doc.write(rowLocation,8, qry->value(12).toString());
                    else{
                        doc.write(rowLocation,7, minutesToTimeString(2400-total));
                        doc.write(rowLocation,8, minutesToTimeString(minutes-2400+total));
                    }
                    total+=minutes;
                    rowLocation++;
                }
            }
            doc.setColumnWidth(1,1,colLength[0]+2);
            doc.setColumnWidth(2,2,colLength[1]+2);
            doc.setColumnWidth(3,3,colLength[2]+2);
            doc.setColumnWidth(4,4,colLength[3]+2);
            doc.setColumnWidth(5,5,colLength[4]+2);
            doc.setColumnWidth(6,6,colLength[5]+2);
            doc.setColumnWidth(8,8,colLength[6]+2);

        }
    }

    doc.saveAs(FileToExport);
}
QString MainForm::minutesToTimeString(int m){
    QString time = "";
    time += QString::number(qAbs(m/60));
    time += ":";
    if(qAbs(m%60) < 10)
        time += QString::number(qAbs(m%60))+"0";
    else
        time += QString::number(qAbs(m%60));
    if(m < 0)
        return "-" + time;
    return time;
}
int MainForm::TimeStringToMinutes(QString time){
    if(time == "")
        return 0;
    int min =0;
    min += time.split(":")[0].toInt()*60;
    min += time.split(":")[1].toInt()%60;
    return min;
}
void MainForm::on_SettingsPrint_clicked()
{

}
void MainForm::on_SettingsAll_clicked()
{

}
void MainForm::on_SettingsConnections_clicked()
{
    this->hide();

    if (connectionForm->getIp().split(".")[3] =="10")
        connectionForm->showFullScreen();
    else
        connectionForm->showNormal();
    QObject::connect(connectionForm,SIGNAL(finished()),this,SLOT(start()));

}




/* This is the method used for sending the data
 * database to sub-form.
 */

QSqlDatabase MainForm::getData() const
{
    return data;
}







