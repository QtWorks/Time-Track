#include "projecteditform.h"
#include "ui_projecteditform.h"
#include "mainform.h"

ProjectEditForm::ProjectEditForm(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::ProjectEditForm)
{
    ui->setupUi(this);
    data = ((MainForm*)parentWidget())->getData();
    clicked=false;

    ui->Hours->installEventFilter(this);
    ui->Quantity->installEventFilter(this);
    ui->Item->installEventFilter(this);
    ui->name->installEventFilter(this);

}

ProjectEditForm::~ProjectEditForm()
{
    delete ui;
}
bool ProjectEditForm::eventFilter(QObject* object,QEvent* event)
{
    if(object == ui->name && event->type() == QEvent::MouseButtonPress) {

        ui->name->setFocus();
        QRect rec = QApplication::desktop()->screenGeometry();
        if(rec.width() < 1400)
            QDesktopServices::openUrl(QUrl("file:///C:/Program Files/Common Files/Microsoft Shared/Ink/TabTip.exe"));
        return false;
    }
    else if(object == ui->Item && event->type() == QEvent::MouseButtonPress) {

        ui->Item->setFocus();
        QRect rec = QApplication::desktop()->screenGeometry();
        if(rec.width() < 1400)
            QDesktopServices::openUrl(QUrl("file:///C:/Program Files/Common Files/Microsoft Shared/Ink/TabTip.exe"));
        return false;
    }
    else if(object == ui->Quantity && event->type() == QEvent::MouseButtonPress) {

        ui->Quantity->setFocus();
        QRect rec = QApplication::desktop()->screenGeometry();
        if(rec.width() < 1400)
            QDesktopServices::openUrl(QUrl("file:///C:/Program Files/Common Files/Microsoft Shared/Ink/TabTip.exe"));
        return false;
    }else if(object == ui->Hours && event->type() == QEvent::MouseButtonPress) {

        ui->Hours->setFocus();
        QRect rec = QApplication::desktop()->screenGeometry();
        if(rec.width() < 1400)
            QDesktopServices::openUrl(QUrl("file:///C:/Program Files/Common Files/Microsoft Shared/Ink/TabTip.exe"));
        return false;
    }
    return false;
}
void ProjectEditForm::DateInitialize(){
    ui->bidDate->setDate(QDate::currentDate());
}
void ProjectEditForm::SectionInitialize(){
    ui->Sections->setRowCount(0);
    ui->Sections->setColumnCount(4);
    ui->Sections->setHorizontalHeaderItem(0,new QTableWidgetItem("Item"));
    ui->Sections->setHorizontalHeaderItem(1,new QTableWidgetItem("Quantity"));
    ui->Sections->setHorizontalHeaderItem(2,new QTableWidgetItem("Unit"));
    ui->Sections->setHorizontalHeaderItem(3,new QTableWidgetItem("Est. Hours"));
    ui->Sections->clearContents();
}
void ProjectEditForm::ItemInitialize(){
    QSqlQueryModel * model=new QSqlQueryModel();
    QSqlQuery* qry = new QSqlQuery(data);
    qry->prepare("SELECT name FROM itemlist WHERE id>0 ORDER BY name ASC");
    qry->exec();
    model->setQuery(*qry);
    ui->Item->setModel(model);
    QCompleter * comp = new QCompleter(this);
    comp->setWidget(ui->Item);
    comp->setCompletionMode(QCompleter::PopupCompletion);
    comp->setCaseSensitivity(Qt::CaseInsensitive);
    ui->Item->setCurrentText("");
}
void ProjectEditForm::DimensionInitialize(){
    QSqlQueryModel * model=new QSqlQueryModel();
    QSqlQuery* qry = new QSqlQuery(data);
    qry->prepare("SELECT name FROM dimensionlist ORDER BY name ASC");
    qry->exec();
    model->setQuery(*qry);
    ui->Dimension->setModel(model);
    QCompleter * comp = new QCompleter(this);
    comp->setWidget(ui->Dimension);
    comp->setCompletionMode(QCompleter::PopupCompletion);
    comp->setCaseSensitivity(Qt::CaseInsensitive);
    ui->Dimension->setCurrentText("");
}
void ProjectEditForm::updateProjectEdit(){
    ItemInitialize();
}

void ProjectEditForm::AddProject(){
    this->showNormal();
    task="add";
    DateInitialize();
    ItemInitialize();
    DimensionInitialize();
    SectionInitialize();
    ui->current->setChecked(true);

}
QString ProjectEditForm::AddValid(){
    QString error = "";
    QRegExp name_regrex("^[a-zA-Z0-9.-_\" )(/\\&-]+$");
    if(ui->name->text().length()<6)
        error = "Invalid Name: Minimum length 6";
    else if (!name_regrex.exactMatch(ui->name->text()))
       error = "Invalid Name: Special characters other than ( - _ . & \" ) are not allowed";
    else if(ui->Sections->rowCount()<1){
        error = "Invalid Project: Must have at least one item";
    }
    else{
        QSqlQuery * qry = new QSqlQuery(data);
        QString name = ui->name->text();
        QString currentName;
        qry->prepare("SELECT name FROM projectlist where name = '"+name+"'");
        int nameCount =0;
        if(qry->exec()){
            while(qry->next()){
                nameCount++;
            }
        }

        if(nameCount>0){
            error = "Invalid name: Must be unique";
        }

        //qDebug()<<qry->lastError().text()<<"nameCount: "<<nameCount<<"currentName: "<<currentName<<"name: "<<name;
    }

    return error;
}

void ProjectEditForm::EditProject(QString x){
    this->showNormal();
    task="edit";
    id = x;
    ItemInitialize();
    DimensionInitialize();
    SectionInitialize();

    QSqlQuery * qry = new QSqlQuery(data);
    qry->prepare("SELECT name,current,date FROM projectlist WHERE id='"+id+"'");
    if(qry->exec()){
        while(qry->next()){
            ui->name->setText(qry->value(0).toString());
            if(qry->value(1).toString()=="1")
                ui->current->setChecked(true);
            else
                ui->current->setChecked(false);
            ui->bidDate->setDate(QDate(qry->value(2).toString().split("/")[2].toInt(),qry->value(2).toString().split("/")[0].toInt(),qry->value(2).toString().split("/")[1].toInt()));

        }
    }

    qry->clear();
    qry->prepare("SELECT name,quantity,dimension,ehours FROM project"+id+"");


    if(qry->exec()){
        while(qry->next()){

            ui->Sections->setRowCount(ui->Sections->rowCount()+1);
            ui->Sections->setItem(ui->Sections->rowCount()-1,0,new QTableWidgetItem(qry->value(0).toString()));
            ui->Sections->setItem(ui->Sections->rowCount()-1,1,new QTableWidgetItem(qry->value(1).toString()));
            ui->Sections->setItem(ui->Sections->rowCount()-1,2,new QTableWidgetItem(qry->value(2).toString()));
            ui->Sections->setItem(ui->Sections->rowCount()-1,3,new QTableWidgetItem(qry->value(3).toString()));
        }
    }
    ui->Sections->resizeRowsToContents();
}
QString ProjectEditForm::EditValid(){
    QString error = "";
    QRegExp name_regrex("^[a-zA-Z0-9.-_\" )(/\\&-]+$");
    if(ui->name->text().length()<6)
        error = "Invalid Name: Minimum length 6";
    else if (!name_regrex.exactMatch(ui->name->text()))
       error = "Invalid Name: Special characters other than ( - _ . & \" ) are not allowed";
    else if(ui->Sections->rowCount()<1){
        error = "Invalid Project: Must have at least one item";
    }
    else{
        QSqlQuery * qry = new QSqlQuery(data);
        QString name = ui->name->text();
        QString currentName;
        qry->prepare("SELECT name FROM projectlist where name = '"+name+"'");
        int nameCount =0;
        if(qry->exec()){
            while(qry->next()){
                nameCount++;
            }
        }
        qry->prepare("SELECT name FROM projectlist where id = '"+id+"'");

        if(qry->exec()){
            while(qry->next()){
                currentName = qry->value(0).toString();
            }
        }
        if(currentName == name && nameCount>1)
            error = "Invalid name: Must be unique";
        else if(currentName != name && nameCount>0)
            error = "Invalid name: Must be unique";

        //qDebug()<<qry->lastError().text()<<"nameCount: "<<nameCount<<"currentName: "<<currentName<<"name: "<<name;
    }
    return error;
}

void ProjectEditForm::on_AddItem_clicked()
{
    int count = 0;
    for(int i = 0; i<ui->Sections->rowCount();i++){
        if(ui->Sections->item(i,0)->text() == ui->Item->currentText()){
            count++;
        }
    }
    if(count != 0){
        ui->error->setText("Invalid Item: Must be Unique");
    }
    else if(ui->Item->currentText()==""){
        ui->error->setText("Invalid Item: Select an Item");
    }else if( ui->Dimension->currentText()==""){
        ui->error->setText("Invalid Item: Select a Unit");
    }else if(ui->Quantity->value()==0){
        ui->error->setText("Invalid Item: Select a Quantity");
    }else if(ui->Hours->value()==0){
        ui->error->setText("Invalid Item: Select a Estimate for Hours");
    }else{
        ui->Sections->setRowCount(ui->Sections->rowCount()+1);
        ui->Sections->setItem(ui->Sections->rowCount()-1,0,new QTableWidgetItem(ui->Item->currentText()));
        ui->Sections->setItem(ui->Sections->rowCount()-1,1,new QTableWidgetItem(QString::number(ui->Quantity->value())));
        ui->Sections->setItem(ui->Sections->rowCount()-1,2,new QTableWidgetItem(ui->Dimension->currentText()));
        ui->Sections->setItem(ui->Sections->rowCount()-1,3,new QTableWidgetItem(QString::number(ui->Hours->value())));

        ui->Sections->resizeRowsToContents();
        ui->Item->setEditText("");
        ui->error->setText("");
    }



}
void ProjectEditForm::on_EditItem_clicked()
{
    if(ui->Item->currentText()==""){
        ui->error->setText("Invalid Item: Select an Item");
    }else if( ui->Dimension->currentText()==""){
        ui->error->setText("Invalid Item: Select a Unit");
    }else if(ui->Quantity->value()==0){
        ui->error->setText("Invalid Item: Select a Quantity");
    }else if(ui->Hours->value()==0){
        ui->error->setText("Invalid Item: Select a Estimate for Hours");
    }else if(clicked){
        ui->Sections->setItem(selectedRow,0,new QTableWidgetItem(ui->Item->currentText()));
        ui->Sections->setItem(selectedRow,1,new QTableWidgetItem(QString::number(ui->Quantity->value())));
        ui->Sections->setItem(selectedRow,2,new QTableWidgetItem(ui->Dimension->currentText()));
        ui->Sections->setItem(selectedRow,3,new QTableWidgetItem(QString::number(ui->Hours->value())));

        ui->Sections->resizeRowsToContents();
        ui->Item->setEditText("");
        ui->Quantity->setValue(0);
        ui->Dimension->setEditText("");
        ui->Hours->setValue(0);
        clicked = false;
    }
}
void ProjectEditForm::on_DeleteItem_clicked()
{
    if (clicked)
    {
        ui->Sections->removeRow(selectedRow);
        clicked = false;
    }
}
void ProjectEditForm::on_Sections_cellClicked(int row, int column)
{
    selectedRow=row;
    clicked = true;
    column++;
    QString item = ui->Sections->item(row,0)->text();
    QString quantity = ui->Sections->item(row,1)->text();
    QString dimension = ui->Sections->item(row,2)->text();
    QString hours = ui->Sections->item(row,3)->text();

    int index = ui->Item->findText(item);
    if ( index != -1 ) { // -1 for not found
       ui->Item->setCurrentIndex(index);
    }

    ui->Quantity->setValue(quantity.toInt());

    index = ui->Dimension->findText(dimension);
    if ( index != -1 ) { // -1 for not found
       ui->Dimension->setCurrentIndex(index);
    }

    ui->Hours->setValue(hours.toInt());
}

void ProjectEditForm::on_FinishButton_clicked()
{
    if(data.open()){
        if(AddValid() == "" && task=="add")
        {
            QSqlQuery * qry = new QSqlQuery(data);
            qry->prepare("INSERT into projectlist(name,current,date)  values('"+ui->name->text()+"','1',"
                                    "'"+QString::number(ui->bidDate->date().month())+"/"+QString::number(ui->bidDate->date().day())+"/"+QString::number(ui->bidDate->date().year())+"')");
            if (qry->exec())
                success = true;
            else{
                success = false;
                successMsg = "Error Creating Project(1)";
                this->hide();
                emit finished();
                return;
            }



            qry->clear();
            QString id;
            qry->prepare("SELECT id FROM projectlist WHERE id=(select max(id) FROM projectlist)");
            if(qry->exec())
            {
                while(qry->next())
                {
                    id = qry->value(0).toString();
                }
                success=true;
            }
            else{
                success = false;
                successMsg = "Error Creating Project(2)";
                this->hide();
                emit finished();
                return;
            }
            qry->clear();
            qry->prepare("CREATE TABLE Project"+id+" (id int PRIMARY KEY AUTO_INCREMENT, itemid int, name varchar(45),quantity varchar(45), dimension varchar(45), ehours varchar(45))");
            qry->exec();

            QString itemId;
            for(int i = 0; i<ui->Sections->rowCount();i++){
                qry->clear();
                qry->prepare("SELECT id FROM itemlist WHERE name='"+ui->Sections->item(i,0)->text()+"'");
                if(qry->exec()){
                    while(qry->next())
                        itemId = qry->value(0).toString();
                    success = true;
                }
                else{
                    success = false;
                    successMsg = "Error Creating Project(3)";
                    this->hide();
                    emit finished();
                    return;
                }
                qry->prepare("INSERT into Project"+id+"(itemid,name,quantity,dimension,ehours)  values('"+itemId+"','"+ui->Sections->item(i,0)->text()+"'"
                             ",'"+ui->Sections->item(i,1)->text()+"','"+ui->Sections->item(i,2)->text()+"','"+ui->Sections->item(i,3)->text()+"')");
                if (qry->exec())
                    success = true;
                else{
                    success = false;
                    successMsg = "Error Creating Project(4)";
                    this->hide();
                    emit finished();
                    return;
                }
            }

            this->hide();
            emit finished();
        }
        if(EditValid()=="" && task=="edit")
        {
            QSqlQuery * qry = new QSqlQuery(data);
            QString c = "1";
            if(!ui->current->isChecked())
                c = "0";
            qry->prepare("UPDATE projectlist SET name='"+ui->name->text()+"', current='"+c+"',"
                                    "date='"+QString::number(ui->bidDate->date().month())+"/"+QString::number(ui->bidDate->date().day())+"/"+QString::number(ui->bidDate->date().year())+"' where id='"+id+"'");
            if (qry->exec())
                success = true;
            else{
                //qDebug()<<qry->lastError().text();
                //qDebug()<<qry->lastQuery();
                success = false;
                successMsg = "Error Editing Project(1)";
                this->hide();
                emit finished();
                return;
            }
            qry->clear();
            qry->prepare("TRUNCATE TABLE Project"+id);
            qry->exec();
            QString itemId;
            for(int i = 0; i<ui->Sections->rowCount();i++){
                qry->clear();
                qry->prepare("SELECT id FROM itemlist WHERE name='"+ui->Sections->item(i,0)->text()+"'");
                if(qry->exec()){
                    while(qry->next())
                        itemId = qry->value(0).toString();
                    success = true;
                }
                else{
                    success = false;
                    successMsg = "Error Editing Project(2)";
                    this->hide();
                    emit finished();
                    return;
                }
                qry->prepare("INSERT into Project"+id+"(itemid,name,quantity,dimension,ehours)  values('"+itemId+"','"+ui->Sections->item(i,0)->text()+"'"
                             ",'"+ui->Sections->item(i,1)->text()+"','"+ui->Sections->item(i,2)->text()+"','"+ui->Sections->item(i,3)->text()+"')");
                if (qry->exec())
                    success = true;
                else{
                    success = false;
                    successMsg = "Error Editing Project(3)";
                    this->hide();
                    emit finished();
                    return;
                }
            }




            qry->clear();
            qry->prepare("update shiftlist set projectname='"+ui->name->text()+"'  where projectid = '"+id+"'");
            if (qry->exec())
                success = true;
            else{
                success = false;
                successMsg = "Error Editing Project(4)";
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
        ui->error->setText("Disconnected From Database. Verify Connection and Try Again");
    }
}
void ProjectEditForm::on_CancelButton_clicked()
{
    success=true;
    this->hide();
    emit finished();
}

bool ProjectEditForm::getSuccess() const
{
    return success;
}
QString ProjectEditForm::getSuccessMsg() const
{
    return successMsg;
}


