#include "mainwidget.h"
#include "ui_mainwidget.h"
#include <QDir>
#include "fileinfocsv.h"
#include <QFileDialog>
#include <QDebug>
#include <QSettings>

bool mutationLowerThan(const mainWidget::mutationInfo_t *s1, const mainWidget::mutationInfo_t *s2);

mainWidget::mainWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::mainWidget)
{
    ui->setupUi(this);
    ui->tabWidget->setCurrentIndex(0);

    mutationInfo_t test1, test2;
    test1.name = "A140B";
    test2.name = "A141C";

    qDebug() << test1.name << "<" << test2.name << "=>" << mutationLowerThan(&test1, &test2);
}

mainWidget::~mainWidget()
{
    delete ui;
}

void mainWidget::on_buttonSelectFolder_clicked()
{
    QSettings settings("pilouSoft", "csvAGCT");
    m_basePath = settings.value("basePath", QString("/")).toString();
    QStringList fileNames = QFileDialog::getOpenFileNames(this,
        tr("Select csv files"), m_basePath, tr("Comma-separated values (*.csv)"));
    if(fileNames.count()>0)
    {
        qDebug() << "first Filename :" << fileNames.first();
        QFileInfo fileInfo(fileNames.first());
        settings.setValue("basePath", fileInfo.absolutePath());
        qDebug() << "new last Path:" << fileInfo.absolutePath();

    }
    foreach(QString filename, fileNames)
    {
        qDebug() << filename;
        fileInfoCsv *info = new fileInfoCsv(filename, this);
        if(!info->valid())
            delete info;
        else
        {
            ui->verticalLayoutFilelist->addWidget(info);
            info->show();
            connect(info, SIGNAL(newMutation(QString,QString,float,bool)), this, SLOT(handleNewMutation(QString,QString,float,bool)));
        }
    }
}

void mainWidget::on_buttonAnalyse_clicked()
{
    foreach(fileInfoCsv *info, findChildren<fileInfoCsv *>())
    {
        if(info->checked())
            info->analyseFile();
    }
    ui->tabWidget->setCurrentIndex(1);
}

void mainWidget::handleNewMutation(QString filename, QString positionMutation, float percentValue, bool success)
{
    //Search the index of the mutation
    int indexPositionMutation;
    for(indexPositionMutation=0;
        indexPositionMutation<m_mutations.count();
        indexPositionMutation++)
    {
        if(m_mutations.at(indexPositionMutation)->name == positionMutation)
            break;
    }

    //If the mutation is not found create it and fill with 0 percents
    if(indexPositionMutation==m_mutations.count())
    {
        mutationInfo_t *newMutation = new mutationInfo_t;
        newMutation->name = positionMutation;
        for(int i=0; i<m_patients.count(); i++)
            newMutation->percent.append(0.0);
        m_mutations.append(newMutation);
    }

    //Search for the patient
    bool found;
    /*foreach(patientInfo_t *patient, m_patients)
    {
        if(addMutationOnPatient)
        {
            if(patient->fileName == filename)
            {
                patient->percent.append(percentValue);
                found = true;
                continue;
            }
            else
                patient->percent.append(0.0);
        }
        else if(patient->fileName == filename)
        {
            patient->percent.replace(indexPositionMutation, percentValue);
            found = true;
            continue;
        }
    }*/
    int indexPositionPatient;
    for(indexPositionPatient=0;
        indexPositionPatient<m_patients.count();
        indexPositionPatient++)
    {
        if(m_patients.at(indexPositionPatient)->fileName == filename)
        {
            found = true;
            break;
        }
    }
    if(!found)
    {
        patientInfo_t* patient = new patientInfo_t;
        m_patients.append(patient);
        patient->fileName=filename;
        patient->success = success;
        for(int i=0; i<m_mutations.count(); i++)
        {
            m_mutations.at(i)->percent.append(0.0);
        }
    }

    m_mutations.at(indexPositionMutation)->percent.replace(indexPositionPatient, percentValue);

    qDebug() << m_patients.count() << m_mutations.count();
    ui->label_mutationCount->setText(QString::number(m_mutations.count()));
}

bool mutationLowerThan(const mainWidget::mutationInfo_t *s1, const mainWidget::mutationInfo_t *s2)
{
    QRegExp rx("(\\D+)(\\d+)(\\D+)");
    int position1, position2;
    QString base1, base2;
    QString mutation1, mutation2;

    if (rx.indexIn(s1->name) != -1) {
        base1 = rx.cap(1);
        position1 = rx.cap(2).toInt();
        mutation1 = rx.cap(3);
        //qDebug() << "plop1" << base1 << position1 << mutation1;
    }
    if (rx.indexIn(s2->name) != -1) {
        base2 = rx.cap(1);
        position2 = rx.cap(2).toInt();
        mutation2 = rx.cap(3);
        //qDebug() << "plop2" << base2 << position2 << mutation2;
    }
    if(position1 == position2)
    {
        if(base1 == base2)
        {
            return mutation1 < mutation2;
        }
        else
            return base1 < base2;
    }
    else
        return position1 < position2;
}

void mainWidget::on_buttonExportCSV_clicked()
{
    QFile fileExport(m_basePath+"/export.csv");

    QString debugLine;
    for(int i=0;i<30;i++)
        debugLine += m_mutations.at(i)->name + QString("|");
    qDebug() << "before" << debugLine;
    qSort(m_mutations.begin(), m_mutations.end(), mutationLowerThan);
    debugLine.clear();
    for(int i=0;i<30;i++)
        debugLine += m_mutations.at(i)->name + QString("|");
    qDebug() << "before" << debugLine;

    if(fileExport.open(QFile::WriteOnly))
    {
        QString line;
        line += "ID";
        foreach(mutationInfo_t *mutation, m_mutations)
        {
            if(line.count() != 0)
                line += ";";
            line += mutation->name;
        }
        line.remove(line.count()-1);
        line += 'outcome\n';
        fileExport.write(line.toLocal8Bit());

        QLocale french(QLocale::French);

        for(int indexPositionPatient=0;
            indexPositionPatient<m_patients.count();
            indexPositionPatient++)
        {
            line.clear();
            line.append(m_patients.at(indexPositionPatient)->fileName);
            for(int indexPositionMutation=0;
                indexPositionMutation<m_mutations.count();
                indexPositionMutation++)
            {
                if(line.count() != 0)
                    line += ";";
                line.append(french.toString(m_mutations.at(indexPositionMutation)->percent.at(indexPositionPatient)));
            }
            if(m_patients.at(indexPositionPatient)->success)
                line += ";1";
            else
                line += ";0";
            line += '\n';
            fileExport.write(line.toLocal8Bit());
        }
        fileExport.close();
    }
}
