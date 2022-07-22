#include "f66_dbf_old.h"

#include <iostream>
#include <fstream>
#include <string>
#include <stdlib.h>

#include <QDebug>

// Монструозный конструктор. Надо бы разбить на методы
F66_DBF_Old::F66_DBF_Old(const QString &filepath) :
    m_filepath(filepath.toStdString()),
    m_memo(nullptr)
{
    char *memblock;         // массив для блока данных
    char byte;              // переменная для проверочных байтов
    std::streampos size;    // размер считываемого блока

    // поток для файла
    std::ifstream file(m_filepath, std::ios::in | std::ios::binary);

    if(file.is_open())
    {
        // ===== Присваиваем значение объекту DBFTitle
        size = 32;                      // размер заголовка всегда 32
        memblock = new char [size];
        file.read(memblock, size);      // записываем данные в массив
        m_title = DBFTitle(memblock);   // создаём объект конструктором

        // ===== Формируем список из объектов DBFDescriptor
        for(uint8_t i = 0; i < getColumnsCount(); i++)
        {
            file.seekg(size + size * i);
            file.read(memblock, size);
            m_descriptors.append(DBFDescriptor(memblock));
        }
        delete [] memblock;

        // ===== При наличии MEMO-файла создаём соответствующий объект
        if(m_title.getMemoFlag())
        {
            m_memo = new DBFMemo(m_filepath);
            qDebug () << "MEMO-file exist!";
            qDebug () << "MEMO next aviable index: " << m_memo->getIndexOfNextAviable();
            qDebug () << "MEMO data-block size is: " << m_memo->getSizeBlock() << " bytes";
        }

        // Вот тут проверяем наличие закрывающего байта 0x0D для титульника.
        file.read(&byte, 1);
        if(byte != 0x0D)
            throw "DBF-file title is broken!";

        // Проверяем последний байт файла
        file.seekg(-1, file.end);
        file.read(&byte, 1);
        if(byte != 0x1A)
        {
            file.close();
            throw std::string("This is bullshit! ") + byte + std::string(" can't be a last byte of DBF-file!");
        }

        file.close();
    }
    else
        throw "Can't open file";
}

F66_DBF_Old::~F66_DBF_Old()
{
    if(m_memo != nullptr)
        delete m_memo;

    qDebug() << "DBF_Old - deleted";
}

// Геттеры для DBFTitle
DBFTitle F66_DBF_Old::getDBFTitle()
{    return m_title;    }

uint32_t F66_DBF_Old::getRowsCount()
{    return m_title.getRowsCount();     }

uint8_t F66_DBF_Old::getColumnsCount()
{    return m_title.getColumnsCount();  }

uint16_t F66_DBF_Old::getTitleSize()
{    return m_title.getTitleSize();     }

uint16_t F66_DBF_Old::getRowSize()
{    return m_title.getRowSize();       }

// Геттеры для DBFDescriptor
QList<DBFDescriptor> F66_DBF_Old::getDBFDescriptors()
{    return m_descriptors;  }

std::string F66_DBF_Old::getColumnName(const int &col)
{    return m_descriptors.at(col).getName();    }

char F66_DBF_Old::getColumnType(const int &col)
{    return m_descriptors.at(col).getType();    }

uint8_t F66_DBF_Old::getColumnLength(const int &col)
{    return m_descriptors.at(col).getLength();  }

// Геттер для всей таблицы (уж если приспичит)
QList<std::string> F66_DBF_Old::getTable()
{
    QList<std::string> table;
    for(uint32_t i = 0; i < getRowsCount(); i++)
        for(uint8_t j = 0; j < getColumnsCount(); j++)
            table.push_back(getElementFromFile(i, j));

    return table;
}

std::string F66_DBF_Old::getElement(const unsigned int &row, const unsigned int &col)
{
    return getElementFromFile(row, col);
}

std::string F66_DBF_Old::getElementFromFile(const uint32_t &row, const uint8_t &col)
{
    /* ===== Формула расчёта положения курсора =====
     * длина_титула + номер_ряда * длина_ряда + байт_удаления(1) + длина_предыдущих_ячеек_в_строке
     * */
    std::ifstream file(m_filepath, std::ios::in | std::ios::binary);
    std::streampos pos;                 // позиция курсора
    std::streampos size_before  = 1;    // длина предыдущих ячеек в строке (1 - для нулевой колонки)
    std::string result;                 // результат
    result.resize(getColumnLength(col));

    // проверка на адекватность введённого запроса
    if(row > getRowsCount() || col > getColumnsCount())
        throw std::string("Element " + std::to_string(row) + ", " +  std::to_string(col) + "is out of range");

    if(file.is_open())
    {
        // рассчёт длины предыдущих ячеек в строке
        for(uint8_t i = 0; i < col; i++)
            size_before += getColumnLength(i);

        // рассчёт позиции курсора
        pos = m_title.getTitleSize() + row * getRowSize() + size_before;

        // считываем данные по указанному адресу
        // молимся, чтобы не напороться на переполнение
        file.seekg(pos);
        file.read(&result[0], getColumnLength(col));

        /*
        if(getColumnType(col) == 'M')
            DBFMemo memo(m_filepath);
            */

        return result;
    }
    else
        throw "Can't open file";

    return "ZAGLUSHKA!";
}

bool F66_DBF_Old::rowDeleteStatus(const uint32_t &row)
{
    // проверка на адекватность введённого запроса
    if(row > getRowsCount())
        throw std::string("Row " + std::to_string(row) + "is out of range");

    // адрес = длина_титула + номер_ряда * длина_ряда
    std::streampos pos = getTitleSize() + row * getRowSize();
    std::ifstream file(m_filepath, std::ios::in | std::ios::binary);
    char byte;

    if(file.is_open())
    {
        file.seekg(pos);
        file.read(&byte, 1);
        file.close();
    }
    else
        throw "Can't open file";

    if      (byte == 0x20)
        return false;
    else if (byte == 0x2A)
        return true;
    else
        throw "Wrong data on position of row's deletion-byte.\nDBF-file can be broken, or programmist-dolboyob, or format does not support.";
}
