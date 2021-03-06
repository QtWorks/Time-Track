#include "itemeditform.h"
#include "ui_itemeditform.h"
#include "mainform.h"

ItemEditForm::ItemEditForm(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::ItemEditForm)
{
    ui->setupUi(this);
    data = ((MainForm*)parentWidget())->getData();
    ui->name->installEventFilter(this);
}
void ItemEditForm::AddItem(){
    this->showNormal();
    task="add";
    ui->name->setText("");
}
QString ItemEditForm::AddValid(){
    QString error;
    QRegExp name_regrex("^[a-zA-Z0-9.-_\" )(/\\&-]+$");
    if(ui->name->text().length()<4)
        error = "Invalid Name: Minimum length 4";
    else if (!name_regrex.exactMatch(ui->name->text()))
       error = "Invalid Name: Special characters other than ( - _ . & \" ) are not allowed";
    else{
        QSqlQuery * qry = new QSqlQuery(data);
        QString name = ui->name->text();
        QString currentName;
        qry->prepare("SELECT name FROM itemlist where name = '"+name+"'");
        int nameCount =0;
        if(qry->exec()){
            while(qry->next()){
                nameCount++;
            }
        }

        if(nameCount>0){
            error = "Invalid name: Must be unique";
        }

       // qDebug()<<qry->lastError().text()<<"nameCount: "<<nameCount<<"currentName: "<<currentName<<"name: "<<name;
    }
   return error;
}

void ItemEditForm::EditItem(QString x){
    this->showNormal();
    task="edit";
    id = x;
    QSqlQuery * qry = new QSqlQuery(data);
    qry->prepare("SELECT name FROM itemlist WHERE id ='"+id+"'");
    if(qry->exec()){
        while(qry->next()){
            ui->name->setText(qry->value(0).toString());
        }
    }
}
QString ItemEditForm::EditValid(){
    QString error;
    QRegExp name_regrex("^[a-zA-Z0-9.-_\" )(/\\&-]+$");
    if(ui->name->text().length()<4)
        error = "Invalid Name: Minimum length 4";
    else if (!name_regrex.exactMatch(ui->name->text()))
       error = "Invalid Name: Special characters other than ( - _ . & \" ) are not allowed";
    else{
        QSqlQuery * qry = new QSqlQuery(data);
        QString name = ui->name->text();
        QString currentName;
        qry->prepare("SELECT name FROM itemlist where name = '"+name+"'");
        int nameCount =0;
        if(qry->exec()){
            while(qry->next()){
                nameCount++;
            }
        }
        qry->clear();
        qry->prepare("SELECT name FROM itemlist WHERE id = "+id+"");
        if(qry->exec()){
            while(qry->next()){
                currentName = qry->value(0).toString();
            }
        }
        if(name == currentName && nameCount > 1){
            error = "Invalid name: Must be unique";
        }else if(name != currentName && nameCount>0){
            error = "Invalid name: Must be unique";
        }

       // qDebug()<<qry->lastError().text()<<"nameCount: "<<nameCount<<"currentName: "<<currentName<<"name: "<<name;
    }
    return error;
}

ItemEditForm::~ItemEditForm()
{
    delete ui;
}
bool ItemEditForm::eventFilter(QObject* object,QEvent* event)
{
    if(object == ui->name && event->type() == QEvent::MouseButtonPress) {

        ui->name->setFocus();
        QRect rec = QApplication::desktop()->screenGeometry();
        if(rec.width() < 1400)
            QDesktopServices::openUrl(QUrl("file:///C:/Program Files/Common Files/Microsoft Shared/Ink/TabTip.exe"));
        return false;
    }
    return false;
}

QString ItemEditForm::getSuccessMsg() const
{
    return successMsg;
}
bool ItemEditForm::getSuccess() const
{
    return success;
}

void ItemEditForm::on_FinishButton_clicked()
{
    if(data.open()){
        if(AddValid() == "" && task=="add")
        {
            QSqlQuery * qry = new QSqlQuery(data);
            qry->prepare("insert into itemlist(name) "
                         " values('"+ui->name->text()+"')");


            if (qry->exec())
                success = true;
            else{
                success = false;
                successMsg = "Error Creating Item";
            }

            this->hide();
            emit finished();
        }
        if(EditValid() == "" && task=="edit")
        {
            QSqlQuery * qry = new QSqlQuery(data);
            qry->prepare("update itemlist set name='"+ui->name->text()+"'  where id = '"+id+"'");

            if (qry->exec())
                success = true;
            else{
                success = false;
                successMsg = "Error Editing Item";
            }

            qry->clear();
            qry->prepare("update shiftlist set itemname='"+ui->name->text()+"'  where itemid = '"+id+"'");
            if (qry->exec())
                success = true;
            else{
                success = false;
                successMsg = "Error Editing Item";
            }

            qry->clear();
            qry->prepare("SELECT id FROM projectlist");

            QList<QString> projects;
            if(qry->exec()){
                while(qry->next()){
                    projects<<qry->value(0).toString();
                }
            }
            for(int i = 0; i<projects.length();i++)
            {
                QString tableName = "project"+projects[i];
                qry->clear();
                qry->prepare("UPDATE "+tableName+" set name='"+ui->name->text()+"' where itemid = '"+id+"'");
                if (qry->exec())
                    success = true;
                else{
                    success = false;
                    successMsg = "Error Editing Item";
                }
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
void ItemEditForm::on_CancelButton_clicked()
{
    success=true;
    this->hide();
    emit finished();
}
