#include "src/mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    fileName = " ";
    ui->pointGroupLabel->setText(" ");
    ui->molarMassLabel->setText(" ");
    ui->chemicalFormulaLabel->setText(" ");
    mol = nullptr;
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::on_toolButtonFile_clicked()
{
    QString fileName = QFileDialog::getOpenFileName(this, tr("Open File"), "~", tr("*.xyz *.out"));
    mol = new Molecule;
    mol->read_from_xyz(fileName.toStdString());

    this->ui->molarMassLabel->setText(QString::number(static_cast<double>(mol->get_mass()))+" dal");

    std::vector<std::string> elements;
    for(unsigned int i=0; i<mol->get_n_atom(); i++)
    {
        elements.push_back(mol->get_atom(i).get_type());
    }

    QString formula = "";
    if(std::count(elements.begin(),elements.end(),"C") > 0)
    {
        formula += "C";
        formula += "<sub>";
        formula += QString::number(std::count(elements.begin(),elements.end(),"C"));
        formula += "</sub>";
        elements.erase(std::remove(elements.begin(),elements.end(),"C"),elements.end());
    }
    if(std::count(elements.begin(),elements.end(),"H") > 0)
    {
        formula += "H";
        formula += "<sub>";
        formula += QString::number(std::count(elements.begin(),elements.end(),"H"));
        formula += "</sub>";
        elements.erase(std::remove(elements.begin(),elements.end(),"H"),elements.end());
    }
    if(std::count(elements.begin(),elements.end(),"N") > 0)
    {
        formula += "N";
        formula += "<sub>";
        formula += QString::number(std::count(elements.begin(),elements.end(),"N"));
        formula += "</sub>";
        elements.erase(std::remove(elements.begin(),elements.end(),"N"),elements.end());
    }
    if(std::count(elements.begin(),elements.end(),"O") > 0)
    {
        formula += "O";
        formula += "<sub>";
        formula += QString::number(std::count(elements.begin(),elements.end(),"O"));
        formula += "</sub>";
        elements.erase(std::remove(elements.begin(),elements.end(),"O"),elements.end());
    }
    if(std::count(elements.begin(),elements.end(),"P") > 0)
    {
        formula += "P";
        formula += "<sub>";
        formula += QString::number(std::count(elements.begin(),elements.end(),"P"));
        formula += "</sub>";
        elements.erase(std::remove(elements.begin(),elements.end(),"P"),elements.end());
    }
    if(std::count(elements.begin(),elements.end(),"S") > 0)
    {
        formula += "S";
        formula += "<sub>";
        formula += QString::number(std::count(elements.begin(),elements.end(),"S"));
        formula += "</sub>";
        elements.erase(std::remove(elements.begin(),elements.end(),"S"),elements.end());
    }

    while(elements.size()>0)
    {
        formula += QString::fromStdString(elements[0]);
        formula += "<sub>";
        formula += QString::number(std::count(elements.begin(),elements.end(),elements[0]));
        formula += "</sub>";
        elements.erase(std::remove(elements.begin(),elements.end(),elements[0]),elements.end());
    }

    this->ui->chemicalFormulaLabel->setText(formula);


    this->ui->glWidget->draw_molecule(mol);
}


void MainWindow::on_toolButtonSymmetry_clicked()
{
   if(mol != nullptr)
   {
       mol->find_point_group();
       QString pointGroup(QString::fromStdString(mol->get_point_group()));
       QString pointGroupToPrint = QString(pointGroup[0]);
       pointGroupToPrint += "<sub>" + pointGroup.remove(0,1) + "</sub>";
       this->ui->pointGroupLabel->setText(pointGroupToPrint);
   }
}

void MainWindow::on_toolButtonColor_clicked()
{
    QColor color = QColorDialog::getColor();
    double r,g,b;
    color.getRgbF(&r,&g,&b);
    this->ui->glWidget->set_background_color(r,g,b);
    update();
}
