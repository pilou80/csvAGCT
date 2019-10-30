#include "fileinfocsv.h"
#include "ui_fileinfocsv.h"
#include <QFile>
#include <QDebug>
#include <QString>

#define colonneAminoAcidChangeIndex 8
#define colonneNameIndex 0

fileInfoCsv::fileInfoCsv(QString fileName, QWidget *parent) :
    QWidget(parent),
    ui(new Ui::fileInfoCsv)
{
    ui->setupUi(this);
    ui->labelFilename->setText("Error");
    m_filename = fileName;
    QFile file(m_filename);
    ui->checkBox->setChecked(true);
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
                    else if(stringLine.at(i) == ';')
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
            }

            ui->labelFilename->setText(fileName);
            ui->label_LinesCount->setText(QString::number(m_dataLines.count()));

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
    bool succes = false;
    if(m_dataLines.count())
    {
        if(m_dataLines.at(0).count()>=17)
        {
            if(m_dataLines.at(0).at(16) == "succes\r")
               succes = true;
        }
        else
        {
            qDebug() << "no succes line !";
        }
    }


        //qDebug() << "line first:" << m_dataLines.at(0);
    //Supprimer index
    if(m_dataLines.count()>0)
        m_dataLines.removeFirst();
    qDebug() << "analysis " << this->m_filename;
    //qDebug() << "before=================" << m_dataLines.count();
    /*for(int i=0;i<20;i++)
    {
        qDebug() << "line " << i << " name: " << m_dataLines.at(i).first();
    }*/

    ui->label_LinesCountWithoutDoubles->setText(QString::number(m_dataLines.count()));

    QString currentBase;
    /*for(int i=0; i<m_dataLines.count() ;i++)
    {

        if(m_dataLines.at(i).count() >=17)
        {
            if(m_dataLines.at(i).at(1) == "modified_base")
            {
                currentBase = m_dataLines.at(i).at(0);
            }
            else if((m_dataLines.at(i).at(1) == "Polymorphism") &&
                    (m_dataLines.at(i).at(6).count() != 0))
            {
                QChar morph;
                if(m_dataLines.at(i).at(8).count() == 6)
                {
                    morph = m_dataLines.at(i).at(8).at(5);
                }
                QStringList percentConv = m_dataLines.at(i).at(15).split('\%'); //1.4%
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

                //qDebug() << "found :" << currentBase << morph << m_dataLines.at(i).at(14) << percent;
                emit(newMutation(m_filename, currentBase+morph, percent, succes));
            }
            else
                qDebug() << "not found line " << i;
        }
        else
            qDebug() << "Error m_dataLines.at(i).count() == " << m_dataLines.at(i).count();
    }*/

    for(int i=0; i<m_dataLines.count() ;i++)
    {
        if(m_dataLines.at(i).count() >=17)
        {
            //Il faut trouver la modified_base correspondante à ce polymorphisme
            if(m_dataLines.at(i).at(colonneAminoAcidChangeIndex).count() == 6) //6 => ex "M -> V"
            {
                currentBase = m_dataLines.at(i).at(colonneAminoAcidChangeIndex).at(0); //On récupere le M qu'il faut aller chercher dans modified_base
                QChar morph = m_dataLines.at(i).at(colonneAminoAcidChangeIndex).at(5);
                QStringList percentConv = m_dataLines.at(i).at(15).split('\%'); //1.4%
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

                for(int rewindCount=i-1;
                        rewindCount>=0;
                        rewindCount--)
                {
                    if(m_dataLines.at(rewindCount).at(colonneNameIndex).count() == 0)
                    {
                        qDebug() << "Problème => Fichier " << m_filename << "  ligne " << i << " colonne Name vide!";
                        break;
                    }
                    else if(m_dataLines.at(rewindCount).at(colonneNameIndex).count() >= 2 && //On vérifie qu'il y à potentielment un chiffre après la lettre
                            m_dataLines.at(rewindCount).at(colonneNameIndex).at(0) == currentBase)
                    {
                        if(m_dataLines.at(rewindCount).at(colonneNameIndex).at(1).isDigit())
                        {
                            currentBase = m_dataLines.at(rewindCount).at(colonneNameIndex);
                            emit(newMutation(m_filename, currentBase+morph, percent, succes));
                            break;//exit the for rewindCount
                        }
                    }
                }

            }
        }
        else
            qDebug() << "Error m_dataLines.at(" << i <<  ").count() == " << m_dataLines.at(i).count();
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

