#ifndef PARSEXML_H
#define PARSEXML_H

#include <QMainWindow>
#include <QList>
#include <QStringList>
#include <QDomElement>
#include <QTableWidget>
#include <QComboBox>

namespace Ui {
class ParseXML;
}

class ParseXML : public QMainWindow
{
    Q_OBJECT

public:
    explicit ParseXML(QWidget *parent = 0);
    ~ParseXML();

    void ParseProvincesElement(const QDomElement &rootElement);
    void ParseCityElement(const QDomElement &element);
    void ParseDistrictElement(const QDomElement &element);

    void WriteFile(const QString &fileName, const QStringList &list);
    void WriteFile(const QString &fileName, const QList<QStringList> &list);

    void ReadFile();

    void LoadComboBox(QComboBox *comboBox, const QStringList &list);

private slots:
    void on_btn_choose_file_clicked();

    void on_btn_load_clicked();

    void currentIndexChanged_comboBox_1(const QString &text);

    void currentIndexChanged_comboBox_2(const QString &text);
private:
    Ui::ParseXML *ui;

    QStringList provincesList;
    QStringList cityList;
    QStringList districtList;

    QStringList stringList_provinces_city_district;
    QList<QStringList> list_provinces_city_district;

    QString provinceName;
    QStringList provinceList;

//    TableModel *model;
//    TableView *view;
};

#endif // PARSEXML_H
