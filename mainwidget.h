#ifndef MAINWIDGET_H
#define MAINWIDGET_H

#include <QWidget>

namespace Ui {
class mainWidget;
}

class mainWidget : public QWidget
{
    Q_OBJECT





public:
    typedef struct {
        QString name;
        QList<float> percent;
    } mutationInfo_t;

    typedef struct{
        QString fileName;
        bool success;
        //QList<float> percent;
    }patientInfo_t;
    explicit mainWidget(QWidget *parent = 0);
    ~mainWidget();

private slots:
    void on_buttonSelectFolder_clicked();

    void on_buttonAnalyse_clicked();
    void handleNewMutation(QString filename, QString positionMutation, float percentValue, bool success);

    void on_buttonExportCSV_clicked();

private:
    Ui::mainWidget *ui;
    //QStringList m_mutationNames;
    QList<mutationInfo_t*> m_mutations;
    QList<patientInfo_t*> m_patients;
    QString m_basePath;
    //bool mutationLowerThan(const mutationInfo_t *s1, const mutationInfo_t *s2);
};

#endif // MAINWIDGET_H
