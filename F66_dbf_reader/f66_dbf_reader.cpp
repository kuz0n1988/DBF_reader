#include "f66_dbf_reader.h"
#include "f66_dbf_old.h"

// Рамочки для гуи
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QGroupBox>
#include <QFrame>

// Элементы главного окна
#include <QLineEdit>    // адресная строка
#include <QTextEdit>    // логи
#include <QTableWidget> // таблица с БД
#include <QPushButton>  // кнопка "открыть"

// Для работы с файлами + эксперименты всякие
#include <QFile>        // тупо файл
#include <QFileInfo>    // чтобы не парсить адресную строку
#include <QTextCodec>   // для CP866

// Это чтобы всё не развалилось
#include <QString>
#include <QList>

// STL-библиотеки
#include <iostream>
#include <fstream>
#include <string>
#include <Windows.h>

#include <QDebug>

bool F66_DBF_Reader::checkFileName(const QString &filepath)
{
    // Проверяем введённый адрес на "адекватность"
    QFileInfo fi(filepath);
    QString filename = fi.fileName();

    if(!filename.contains(".DBF") && !filename.contains(".dbf"))
       return false;
    return true;
}

F66_DBF_Reader::F66_DBF_Reader(QWidget *parent)
    : QWidget(parent)
{
    const QString name = "F66 DBF-reader";

    resize(800, 600);
    setObjectName(name);
    setWindowTitle(name);

    // ========================================
    // Инициализируем элементы главного окна
    // ========================================

    // Основное поле вывода таблички с БД
    m_ptwd_table    = new QTableWidget(3, 3, this);
    m_ptwd_table->setMinimumSize(600, 450);
    m_ptwd_table->setFrameStyle(QFrame::Box | QFrame::Raised);
    m_ptwd_table->setLineWidth(5);

    // Адресная строка
    m_pled_filename  = new QLineEdit(this);
    m_pled_filename->setText("C:\\F_066\\DAN\\KOT16.DBF");
    m_pled_filename->setMinimumWidth(200);

    // Кнопка "открыть"
    QPushButton *pbtn_openfile  = new QPushButton("Open", this);
    connect(pbtn_openfile, SIGNAL(clicked()), this, SLOT(slot_toF66_DBF_Old()));

    // Логи
    m_ptxe_status  = new QTextEdit("Логи", this);
    m_ptxe_status->setMinimumSize(250, 200);
    m_ptxe_status->setReadOnly(true);

    // ========================================
    // Раскладываем всё добро по слоям и рамкам
    // ========================================

    // Рабочая область
    QGroupBox *pgbx_table   = new QGroupBox("Database table", this);
               m_plyt_table = new QVBoxLayout();
    pgbx_table->setLayout(m_plyt_table);
    m_plyt_table->addWidget(m_ptwd_table);

    // Панель управления со своим слоем
    QGroupBox   *pgbx_controls  = new QGroupBox("Controls", this);
    QVBoxLayout *plyt_controls  = new QVBoxLayout();
    pgbx_controls->setLayout(plyt_controls);
    plyt_controls->addWidget(m_pled_filename);
    plyt_controls->addWidget(pbtn_openfile);
    plyt_controls->addWidget(m_ptxe_status, 4);

    // Главный слой
    QHBoxLayout *plyt_main  = new QHBoxLayout(this);
    plyt_main->addWidget(pgbx_table, 4);
    plyt_main->addWidget(pgbx_controls, 0);
}

F66_DBF_Reader::~F66_DBF_Reader()
{
}

void F66_DBF_Reader::slot_toF66_DBF_Old()
{
    // Проверяем введённый адрес на "адекватность"
    if(!checkFileName(m_pled_filename->text()))
    {
       m_ptxe_status->append("ERROR! " + m_pled_filename->text() + " does not contains *.DBF");
       return;
    }

    m_ptxe_status->setPlainText("Trying to open " + m_pled_filename->text() + " in raw-format...");
    try
    {
        F66_DBF_Old dbf_file (m_pled_filename->text());

        // ----- Уничтожаем старую таблицу и строим на её месте новую
        delete m_ptwd_table;
        m_ptwd_table = new QTableWidget(dbf_file.getRowsCount(), dbf_file.getColumnsCount());
        m_ptwd_table->setParent(this);
        m_plyt_table->addWidget(m_ptwd_table);
        // ----- Построили заготовку для таблицы и сделали её активной

        // ----- Делаем шапочку
        QStringList col_names;
        for(unsigned int x = 0; x < dbf_file.getColumnsCount(); x++)
            col_names.append(QString::fromStdString(dbf_file.getColumnName(x)));
        m_ptwd_table->setHorizontalHeaderLabels(col_names);
        // ----- Сделали шапочку

        m_ptxe_status->append(QString::fromStdString(dbf_file.getDBFTitle().getTitleInfo()));

        // Фигачим в окошко с логами данные о колонках
        for(int i = 0; i < dbf_file.getColumnsCount(); i++)
        {
            DBFDescriptor dscrp = dbf_file.getDBFDescriptors().at(i);
            m_ptxe_status->append("\nColumn descriptor idx: " + QString::number(i));
            m_ptxe_status->append(QString::fromStdString(dscrp.getDescriptorInfo()));
        }

        // Без этого будем смотреть на козяблики
        QTextCodec  *codec866 = QTextCodec::codecForName( "CP866" );
        QByteArray temp;
        std::string stmp;

        for(uint32_t i = 0; i < dbf_file.getRowsCount(); i++)
        {
            for(uint8_t j = 0; j < dbf_file.getColumnsCount(); j++)
            {
                stmp = dbf_file.getElement(i, j);
                temp = QByteArray(stmp.c_str());
                m_ptwd_table->setItem(i, j, new QTableWidgetItem(codec866->toUnicode(temp) ));
            }
        }
        m_ptxe_status->append("Succes!");
    }
    catch (char const *err)
    {   m_ptxe_status->append(QString::fromStdString(err));    }
    catch (const std::string &str)
    {   m_ptxe_status->append(QString::fromStdString(str));    }
}
