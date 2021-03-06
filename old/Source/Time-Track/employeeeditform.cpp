#include "employeeeditform.h"
#include "ui_employeeeditform.h"
#include "mainform.h"

EmployeeEditForm::EmployeeEditForm(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::EmployeeEditForm)
{
    ui->setupUi(this);
    data = ((MainForm*)parentWidget())->getData();
    ui->admin->setChecked(false);
    ui->name->installEventFilter(this);
    ui->pin->installEventFilter(this);


}


// Initializer for when a employee is being created
void EmployeeEditForm::AddEmployee(){
    this->showNormal();
    task="add";

    this->CrewInitialize();
    ui->name->setText("");
    ui->crew->setCurrentText("");
    ui->pin->setText("");
    ui->admin->setChecked(false);
    ui->active->setChecked(false);
    ui->current->setChecked(true);
}

// Initializer for when a employee is being edited
void EmployeeEditForm::EditEmployee(QString x){
    this->showNormal();
    task = "edit";
    id=x;
    QSqlQuery * qry = new QSqlQuery(data);
    QString name,pin,admin,current,active,crew;
    qry->prepare("SELECT name,crew,pin,adminstatus,current,active FROM employeelist WHERE id ='"+id+"'");
    if(qry->exec()){
        while(qry->next()){
            name = qry->value(0).toString();
            crew = qry->value(1).toString();
            pin = qry->value(2).toString();
            admin = qry->value(3).toString();
            current = qry->value(4).toString();
            active = qry->value(5).toString();
        }
    }

    this->CrewInitialize();

    ui->name->setText(name);
    ui->crew->setCurrentText(crew);
    ui->pin->setText(pin);
    if(admin =="1")
        ui->admin->setChecked(true);
    else
        ui->admin->setChecked(false);
    if(current =="1")
        ui->current->setChecked(true);
    else
        ui->current->setChecked(false);

    if(active =="1")
        ui->active->setChecked(true);
    else
        ui->active->setChecked(false);
    ui->name->installEventFilter(this);
    ui->pin->installEventFilter(this);
}

EmployeeEditForm::~EmployeeEditForm()
{
    delete ui;
}

void EmployeeEditForm::CrewInitialize(){
    QSqlQueryModel * modal = new QSqlQueryModel();
    QSqlQuery* qry = new QSqlQuery(data);
    qry->prepare("select DISTINCT crew from employeelist ORDER BY name ASC");
    qry->exec();
    modal->setQuery(*qry);
    ui->crew->setModel(modal);
    QCompleter * comp = new QCompleter(this);
    comp->setWidget(ui->crew);
    comp->setCompletionMode(QCompleter::PopupCompletion);
    comp->setCaseSensitivity(Qt::CaseInsensitive);
}

bool EmployeeEditForm::eventFilter(QObject* object,QEvent* event)
{
    if(object == ui->name && event->type() == QEvent::MouseButtonPress) {

        ui->name->setFocus();
        QRect rec = QApplication::desktop()->screenGeometry();
        if(rec.width() < 1400)
            QDesktopServices::openUrl(QUrl("file:///C:/Program Files/Common Files/Microsoft Shared/Ink/TabTip.exe"));
        return false;
    }
    else if(object == ui->pin && event->type() == QEvent::MouseButtonPress) {

        ui->pin->setFocus();
        QRect rec = QApplication::desktop()->screenGeometry();
        if(rec.width() < 1400)
            QDesktopServices::openUrl(QUrl("file:///C:/Program Files/Common Files/Microsoft Shared/Ink/TabTip.exe"));
        return false;
    }
    return false;
}
// Generates random unique pin
int EmployeeEditForm::generateRandom(){
    int x = (qrand()%999+100) *1000;
    int y = qrand()%999+100;
    x=x+y;
//    std::random_device rd;
//    std::mt19937 gen(rd());
//    std::uniform_real_distribution<> distribution(100000, 999999);
//    int x = (int)distribution(gen);
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
void EmployeeEditForm::on_GenerateButton_clicked()
{
    ui->pin->setText(QString::number(generateRandom()));
}

// Getters for the values of the validation methods
QString EmployeeEditForm::getSuccessMsg() const
{
    return successMsg;
}
bool EmployeeEditForm::getSuccess() const
{
    return success;
}
// Validation for when a employeee is created
QString EmployeeEditForm::AddValid(){
    QString error;
    QRegExp name_regrex("^[a-zA-Z, ]+$");
    QRegExp pin_regrex("^[0-9]+$");

    if(ui->name->text().length()<5)
        error = "Invalid Name: Minimum length 5";
    else if (!name_regrex.exactMatch(ui->name->text()))
       error = "Invalid Name: Must be Letters only";
    else if (ui->crew->currentText() != "" && !name_regrex.exactMatch(ui->crew->currentText()))
       error = "Invalid Crew: Must be Letters only";
    else if(ui->pin->text().length() < 4)
       error = "Invalid Pin: Minimum length 4";
    else if(!pin_regrex.exactMatch(ui->pin->text()))
           error = "Invalid Pin: Must be Numbers only";
    return error;
}
// Validation for when a employee is edited
QString EmployeeEditForm::EditValid(){
    QString error = "";
    QRegExp name_regrex("^[a-zA-Z, ]+$");
    QRegExp pin_regrex("^[0-9]+$");
    if(ui->name->text().length()<5)
        error = "Invalid Name: Minimum length 5";
    else if (!name_regrex.exactMatch(ui->name->text()))
       error = "Invalid Name: Must be Letters only";
    else if (ui->crew->currentText() != "" && !name_regrex.exactMatch(ui->crew->currentText()))
       error = "Invalid Crew: Must be Letters only";
    else if(ui->pin->text().length() < 4)
       error = "Invalid Pin: Minimum length 4";
    else if(!pin_regrex.exactMatch(ui->pin->text()))
           error = "Invalid Pin: Must be Numbers only";
    else{
        QSqlQuery * qry = new QSqlQuery(data);
        QString pin = ui->pin->text();
        QString currentPin;
        qry->prepare("SELECT pin FROM employeelist WHERE pin = "+pin+"");
        int pinCount =0;
        if(qry->exec()){
            while(qry->next()){
                pinCount++;
            }
        }
        qry->clear();
        qry->prepare("SELECT pin FROM employeelist WHERE id = "+id+"");
        if(qry->exec()){
            while(qry->next()){
                currentPin = qry->value(0).toString();
            }
        }
        if(pin == currentPin && pinCount > 1){
            error = "Invalid pin: Must be unique";
        }else if(pin !=currentPin && pinCount>0){
            error = "Invalid pin: Must be unique";
        }

       // qDebug()<<qry->lastError().text();
    }
    return error;
}
// Self evident
void EmployeeEditForm::on_FinishButton_clicked()
{
    if(data.open()){
        if(AddValid() == "" && task=="add")
        {
            QSqlQuery * qry = new QSqlQuery(data);
            QString admin = QString::number(int(ui->admin->isChecked()));
            QString active = QString::number(int(ui->active->isChecked()));
            QString current = QString::number(int(ui->current->isChecked()));
            qry->prepare("insert into employeelist(name,crew,pin,adminstatus,shiftcount,active,current) "
                         " values('"+ui->name->text()+"','"+ui->crew->currentText()+"','"+ui->pin->text()+"','"+admin+"','1','"+active+"','"+current+"')");
            if (qry->exec())
                success = true;
            else{
                success = false;
                successMsg = "Error Creating Employee";
            }

            this->hide();
            emit finished();
        }
        if(EditValid() == "" && task=="edit")
        {
            QSqlQuery * qry = new QSqlQuery(data);
            QString admin = QString::number(int(ui->admin->isChecked()));
            QString active = QString::number(int(ui->active->isChecked()));
            QString current = QString::number(int(ui->current->isChecked()));
            qry->prepare("update employeelist set name='"+ui->name->text()+"',crew='"+ui->crew->currentText()+"', pin='"+ui->pin->text()+"', adminstatus='"+admin+"', current='"+current+"', active='"+active+"'  where id = '"+id+"'");

            if (qry->exec())
                success = true;
            else{
                success = false;
                successMsg = "Error Editing Employee";
            }

            qry->clear();
            qry->prepare("update shiftlist set employeename='"+ui->name->text()+"'  where employeeid = '"+id+"'");
            if (qry->exec())
                success = true;
            else{
                success = false;
                successMsg = "Error Editing Employee";
            }
            this->hide();
            emit finished();
        }
        else if(task=="edit"){
            ui->error->setText(EditValid());
        }
        else{
            ui->error->setText(AddValid());
        }

    }
    else{
        ui->error->setText("Disconnected From Database\nVerify Connection and Try Again");
    }

}
void EmployeeEditForm::on_CancelButton_clicked()
{
    success=true;
    this->hide();
    emit finished();

}
void EmployeeEditForm::on_pin_returnPressed()
{
    on_FinishButton_clicked();
}


