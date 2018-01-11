#ifndef FILEINFOCSV_H
#define FILEINFOCSV_H

#include <QWidget>

namespace Ui {
class fileInfoCsv;
}

class fileInfoCsv : public QWidget
{
    Q_OBJECT

public:
    explicit fileInfoCsv(QString fileName, QWidget *parent = 0);
    ~fileInfoCsv();

    bool valid();
    bool checked();
    void analyseFile();

signals:
    void newMutation(QString filename, QString positionMutation, float percentValue, bool success);
private:
    Ui::fileInfoCsv *ui;
    QList<QStringList> m_dataLines;
    QString m_filename;
};

#endif // FILEINFOCSV_H
