#include "fileinfocsv.h"
#include "ui_fileinfocsv.h"
#include <QFile>
#include <QDebug>
#include <QString>

fileInfoCsv::fileInfoCsv(QString fileName, QWidget *parent) :
    QWidget(parent),
    ui(new Ui::fileInfoCsv)
{
    ui->setupUi(this);
    ui->labelFilename->setText("Error");
    m_filename = fileName;
    QFile file(m_filename);
    if(file.exists())
    {
        if(file.open(QFile::ReadOnly))
        {
            qDebug() << "file open";
            QByteArray fileRead = file.readAll();
            char style = '\n';
            for(int index=0;index<fileRead.count()-1;index++)
            {
                if( fileRead.at(index) == '\r')
                {
                     if(fileRead.at(index+1) == '\n')
                     {
                         qDebug() << "endline windows style";
                         style = '\n';
                         break;
                     }
                     else
                     {
                         qDebug() << "endline mac style";
                         style = '\r';
                         break;
                     }
                }

            }
            foreach(QByteArray bytesLine, fileRead.split(style))
            {
                if(style == '\r')
                    bytesLine.append('\r');
                if(bytesLine.count())
                {
                        //qDebug() << bytesLine.at(0);
                if(bytesLine.at(0) == '\r')
                    bytesLine = bytesLine.remove(0,1);
                }
                QStringList splitted;
                QString stringLine = QString(bytesLine);
                bool inAQuote = false;
                QString currentSplit;
                int i;
                for(i=0; i<stringLine.count();i++)
                {
                    if(stringLine.at(i) == '\"')
                    {
                        inAQuote = !inAQuote;
                    }
                    else if(stringLine.at(i) == ',')
                    {
                        if(!inAQuote)
                        {
                            splitted.append(currentSplit);
                            currentSplit.clear();
                        }
                        //Else ignore the comma in the quote
                    }
                    else
                    {
                        currentSplit.append(stringLine.at(i));
                    }
                }
                if(currentSplit.count()!=0)
                    splitted.append(currentSplit);
                m_dataLines.append(splitted);
                //m_dataLines.append(QString(bytesLine).split(QChar(',')));
            }
            ui->labelFilename->setText(fileName);
            ui->label_LinesCount->setText(QString::number(m_dataLines.count()));
            ui->checkBox->setChecked(true);
            file.close();
        }
        else
        {
            qDebug() << "file not opened";
        }
    }
    else
        qDebug() << "file not found";

}

bool sortList(const QStringList &s1, const QStringList &s2)
{
    return s1.at(0).toLower() < s2.at(0).toLower();
}

bool compLines(const QStringList a, const QStringList b)
{
    if(a.count() == b.count())
    {
        for(int i=0;i<a.count();i++)
        {
            //qDebug() << "compare cell :" << i;
            if(a.at(i) != b.at(i))
                return false;
        }
        return true;
    }
    else
        return false;
}

void fileInfoCsv::analyseFile()
{
    //Supprimer index
    if(m_dataLines.count()>0)
        m_dataLines.removeFirst();
    qDebug() << "=======================";
    qDebug() << "before=================" << m_dataLines.count();
    /*for(int i=0;i<20;i++)
    {
        qDebug() << "line " << i << " name: " << m_dataLines.at(i).first();
    }*/
    /*qDebug() << "=======================";
    qSort(m_dataLines.begin(), m_dataLines.end(), sortList);
    qDebug() << "sorted";
    for(int i=0;i<20;i++)
    {
        qDebug() << "line " << i << " name: " << m_dataLines.at(i).first();
    }*/
    for(int i=0;i<(m_dataLines.count()-1);i++)
    {
        //qDebug() << "comp Line " << i;
        if(compLines(m_dataLines.at(i), m_dataLines.at(i+1)))
        {
            m_dataLines.removeAt(i);
            i--;
        }
    }
    ui->label_LinesCountWithoutDoubles->setText(QString::number(m_dataLines.count()));
    /*qDebug() << "=double removed=================" << m_dataLines.count();
    for(int i=0;i<20;i++)
    {
        qDebug() << "line " << i << " name: " << m_dataLines.at(i).first();
    }
    qDebug() << "=================";*/
    QString currentBase;
    for(int i=0; i<m_dataLines.count() ;i++)
    {
        if(m_dataLines.at(i).count() >=14)
        {
            if(m_dataLines.at(i).count() >=15)
            {

            }
            if(m_dataLines.at(i).at(1) == "modified_base")
            {
                currentBase = m_dataLines.at(i).at(0);
            }
            else if((m_dataLines.at(i).at(1) == "Polymorphism") &&
                    (m_dataLines.at(i).at(6).count() != 0))
            {
                QChar morph;
                if(m_dataLines.at(i).at(6).count() == 6)
                {
                    morph = m_dataLines.at(i).at(6).at(5);
                }
                QStringList percentConv = m_dataLines.at(i).at(14).split('\%'); //1.4%
                float percent = -1;
                if(percentConv.count() >= 2)
                {
                    bool ok;
                    percent = percentConv.at(0).toFloat(&ok);
                    if(!ok)
                        percent = -1;
                    if(percentConv.count() == 3)
                    {
                        QStringList percentConv2 = percentConv.at(1).split(' ');
                        //qDebug() << "percentConv2 :" << percentConv2;
                        if(percentConv2.count() == 3 &&
                           percent != -1)
                        {
                            bool ok;
                            percent = (percent + percentConv2.at(2).toFloat(&ok))/2;
                            if(!ok)
                                percent = -1;
                        }
                    }
                }

                qDebug() << "found :" << currentBase << morph << m_dataLines.at(i).at(14) << percent;
                emit(newMutation(m_filename, currentBase+morph, percent, false));
            }
        }
    }



}

bool fileInfoCsv::valid()
{
    return (m_dataLines.count() != 0);
}

bool fileInfoCsv::checked()
{
    return ui->checkBox->isChecked();
}

fileInfoCsv::~fileInfoCsv()
{
    delete ui;
}

