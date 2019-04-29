#include "ParseXML.h"
#include "ui_ParseXML.h"

#include <QFileDialog>
#include <QMessageBox>
#include <QDomDocument>
#include <QDomNode>
#include <QFile>
#include <QTextStream>
#include <QDebug>
#include <QTableView>

ParseXML::ParseXML(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::ParseXML)
{
    ui->setupUi(this);

    connect(ui->comboBox_1,
            static_cast<void (QComboBox:: *)(const QString &)>(&QComboBox::currentIndexChanged),
            this,
            &ParseXML::currentIndexChanged_comboBox_1);
    connect(ui->comboBox_2,
            static_cast<void (QComboBox:: *)(const QString &)>(&QComboBox::currentIndexChanged),
            this,
            &ParseXML::currentIndexChanged_comboBox_2);

//    model = new TableModel();
//    view = new TableView(ui->comboBox_1);

    //connect(view, SIGNAL(currentChanged(int)), this, SLOT(currentChangedFromView(int)));

    //隐藏表头
//    view->horizontalHeader()->setVisible(false);
//    view->verticalHeader()->setVisible(false);
//    view->setAlternatingRowColors(true);
//    view->resizeColumnsToContents();
}

ParseXML::~ParseXML()
{
    delete ui;
}

void ParseXML::on_btn_choose_file_clicked()
{
    QString path = QFileDialog::getOpenFileName(this, QStringLiteral("选择文件"),
                                                QCoreApplication::applicationDirPath(),
                                                tr("XML files (*.xml)"));
    if (path.isEmpty())
    {
        return;
    }

    QFile file(path);
    if (!file.open(QIODevice::ReadOnly))
    {
        QMessageBox::critical(this, tr("Read File"), tr("Connot open file : \\n%1").arg(path));
        return;
    }

    QString errorMsg;
    int errorLine;
    int errorColumn;

    QDomDocument doc;
    if (!doc.setContent(&file, false, &errorMsg, &errorLine, &errorColumn))
    {
        QMessageBox::critical(this, tr("wrong"), tr("Parse error at line %1,column %2 : %3")
                              .arg(errorLine).arg(errorColumn).arg(errorMsg));
        return;
    }

    QDomElement rootElement = doc.documentElement();
    ParseProvincesElement(rootElement);

    file.close();

    //开始写文件
    WriteFile("provinces.txt", provinceList);
    WriteFile("provinces_city_district.txt", list_provinces_city_district);

}

void ParseXML::ParseProvincesElement(const QDomElement &rootElement)
{
    QDomNode provincesNode= rootElement.firstChild();//注意QDomNode::firstChild()和QDomNode::nextSibling()都是QDomNode成员函数，返回值都是QDomNode

    stringList_provinces_city_district.clear();
    provinceList.clear();

    while(!provincesNode.isNull())
    {
        if (provincesNode.toElement().tagName() == QStringLiteral("省"))
        {
            provinceName = provincesNode.toElement().attribute("name").trimmed();

            stringList_provinces_city_district << provinceName << "\n";
            provinceList << provinceName;
            ParseCityElement(provincesNode.toElement());
        }

        list_provinces_city_district << stringList_provinces_city_district;
        stringList_provinces_city_district.clear();

        provincesNode = provincesNode.nextSibling();
    }
}

void ParseXML::ParseCityElement(const QDomElement &element)
{
    QDomNode cityNode = element.firstChild();
    QString cityName_original;
    QString cityName_addcomma;

    while (!cityNode.isNull())
    {
        if (cityNode.toElement().tagName() == QStringLiteral("市"))
        {
            cityName_original = cityNode.toElement().attribute("name").trimmed();
            cityName_addcomma = "," + cityName_original;

//            stringList_provinces_city_district.removeLast();
//            stringList_provinces_city_district << "\n";
            stringList_provinces_city_district << cityName_original;

            //stringList_provinces_city << cityName_addcomma;
            //strList_city_district << cityName_original;

            ParseDistrictElement(cityNode.toElement());
        }
        cityNode = cityNode.nextSibling();
    }
}

void ParseXML::ParseDistrictElement(const QDomElement &element)
{
    QDomNode districtNode = element.firstChild();
    QString districtName_addcomma;

    while (!districtNode.isNull())
    {
        if (districtNode.toElement().tagName() == QStringLiteral("区"))
        {
            districtName_addcomma = "," + districtNode.toElement().text().trimmed();
            stringList_provinces_city_district << districtName_addcomma;
        }
        districtNode = districtNode.nextSibling();
    }
    stringList_provinces_city_district << "\n";
}

//重载函数
void ParseXML::WriteFile(const QString &fileName, const QStringList &list)
{
    Q_ASSERT(!fileName.isEmpty());

    QString path = QCoreApplication::applicationDirPath() + QStringLiteral("/XML数据/") + fileName;
    QFile file(path);

    if (!file.open(QIODevice::WriteOnly | QIODevice::Text))
    {
        return;
    }

    QTextStream out(&file);

    for (int i = 0; i < list.size(); i++)
    {
        out << list.at(i) << "\n";
    }
    file.close();
}

//重载函数
void ParseXML::WriteFile(const QString &fileName, const QList<QStringList> &list)
{
    Q_ASSERT(!fileName.isEmpty());

    QString path = QCoreApplication::applicationDirPath() + QStringLiteral("/XML数据/") + fileName;
    QFile file(path);
    QStringList strList;

    if (!file.open(QIODevice::WriteOnly | QIODevice::Text))
    {
        return;
    }

    QTextStream out(&file);

    for (int i = 0; i < list.size(); i++)
    {
        strList = list.at(i);
        for (int j = 0; j < strList.size(); j++)
        {
            out << strList.at(j);
        }
        out << "\n";
    }
    file.close();
}

void ParseXML::ReadFile()
{
    QString path = QCoreApplication::applicationDirPath() + QStringLiteral("/XML数据/") + QString("provinces.txt");
    QFile file(path);

    if (!file.open(QIODevice::ReadOnly))
    {
        return;
    }

    QTextStream in(&file);
    while (!in.atEnd())
    {
        ui->comboBox_1->addItem(in.readLine());
    }
    ui->comboBox_1->setCurrentIndex(0);

    //int row = ((provincesTableList.count() % 4) == 0) ? (provincesTableList.count()/4):((provincesTableList.count()/4) + 1);
    //model->List2Table(provincesTableList, 9, 4);

    file.close();

//    ui->comboBox_1->setModel(model);
//    ui->comboBox_1->setView(view);

    //注意到QTableView类还有一个成员方法：setColumnWidth()，显然是用来设置表格列宽的。但是，这里有个要求。
    //所有对setColumnWidth()的调用都要放在setModel()之后。如果在设置View类的Model之前就调用该方法来设置列宽，是不会起作用的。
//    view->setColumnWidth(0, 100);
//    view->setColumnWidth(1, 100);
//    view->setColumnWidth(2, 100);
//    view->setColumnWidth(3, 100);
//    view->horizontalHeader()->setStretchLastSection(true);
}

void ParseXML::on_btn_load_clicked()
{
    ReadFile();
}

//参数text即为选中的省的名称
void ParseXML::currentIndexChanged_comboBox_1(const QString &text)
{
    QString path = QCoreApplication::applicationDirPath() + QStringLiteral("/XML数据/") + QString("provinces_city_district.txt");
    QFile file(path);
    QString result;
    QStringList list;

    if (!file.open(QIODevice::ReadOnly))
    {
        return;
    }

    cityList.clear();

    QTextStream in(&file);

    while (!in.atEnd())
    {
        result = in.readLine();
        if (result == text)
        {
            while (!result.isEmpty())
            {
                result = in.readLine();
                list = result.split(",");
                if (!(list.at(0)).isEmpty())
                {
                    cityList.append(list.at(0));
                }
            }
            break;  //提前结束循环，防止不必要的循环
        }
    }

    file.close();

    LoadComboBox(ui->comboBox_2, cityList);
}

//参数text即为选中的市的名称
void ParseXML::currentIndexChanged_comboBox_2(const QString &text)
{
    QString province_name = ui->comboBox_1->currentText();

    QString path = QCoreApplication::applicationDirPath() + QStringLiteral("/XML数据/") + QString("provinces_city_district.txt");
    QFile file(path);
    QString result;
    QStringList list;

    if (!file.open(QIODevice::ReadOnly))
    {
        return;
    }

    districtList.clear();

    QTextStream in(&file);
    while (!in.atEnd())
    {
        result = in.readLine();
        if (result == province_name)
        {
            while (!result.isEmpty())
            {
                result = in.readLine();
                list = result.split(",");
                if (list.at(0) == text)
                {
                    for (int i = 1; i < list.size(); i++)
                    {
                        districtList.append(list.at(i));
                    }
                }
            }
            break;
        }
    }
    file.close();

    LoadComboBox(ui->comboBox_3, districtList);
}

void ParseXML::LoadComboBox(QComboBox *comboBox, const QStringList &list)
{
    comboBox->clear();
    for (int i = 0; i < list.size(); i++)
    {
        comboBox->addItem(list.at(i));
    }
    comboBox->setCurrentIndex(0);
}
