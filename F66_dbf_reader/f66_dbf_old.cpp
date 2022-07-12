#include "f66_dbf_old.h"

#include <iostream>
#include <fstream>
#include <string>
#include <cstdint>
#include <stdlib.h>

#include <QDebug>

// Монструозный конструктор. Надо бы разбить на методы
F66_DBF_Old::F66_DBF_Old(const QString &filepath) : m_filepath(filepath.toStdString())
{
    char *memblock;         // массив для блока данных
    char byte;              // переменная для проверочных байтов
    std::streampos size;    // размер считываемого блока

    uint32_t rows;          // кол-во строк (для удобства работы с массивом данных)
    uint8_t  columns;       // кол-во колонок (для удобства работы с массивом данных)

    // поток для файла
    std::ifstream file(m_filepath, std::ios::in | std::ios::binary);

    if(file.is_open())
    {
        // ===== Присваиваем значение объекту DBFTitle
        size = 32;                      // размер заголовка всегда 32
        memblock = new char [size];
        file.read(memblock, size);      // записываем данные в массив
        m_title = DBFTitle(memblock);   // создаём объект конструктором

        columns = m_title.getColumnsCount();
        rows    = m_title.getRowsCount();

        // ===== Формируем список из объектов DBFDescriptor
        for(uint8_t i = 0; i < columns; i++)
        {
            // вычисляем положение дескриптора и перетаскиваем курсор
            // файлового потока на нужный байт
            file.seekg(size + size * i);

            // записываем нужный участок файла в массив
            // size не меняем, поскольку длина дескриптора тоже 32
            file.read(memblock, size);

            m_descriptors.append(DBFDescriptor(memblock)); // создаём объект + добавляем в массив
        }
        delete [] memblock; // размер строки скорее всего отличается

        // Вот тут проверяем наличие закрывающего байта 0x0D.
        // Если его нет, значит либо файл битый, либо я дебил
        file.read(&byte, 1);
        if(byte != 0x0D)
            throw "DBF-file is broken!";

        // ===== Формируем таблицу элементов (но по сути одномерный массив)
/*        for(uint32_t i = 0; i < rows; i++)
            for(uint8_t j = 0; j < columns; j++)            
                m_table.push_back(getElementFromFile(i, j));*/

        /* Предыдущие три строчки кода делают почти всё тоже самое
         * (кроме проверки на флаг удаления)
         *
        std::string      cell;  // это непосредственно для изъятия данных
        uint16_t         j_pos; // для отслеживания позиции текущего блока
        size_t           num;   // размер ячейки в байтах (для упрощения кода)

        size = m_title.getRowSize();
        memblock = new char [size];

        // ----- Начинаем с первой строки
        for(uint32_t i = 0; i < rows; i++)
        {
            file.read(memblock, size);

            // ===== Проверка наличия флага "удаления"
            // Каждая запись начинается с 1-байтового флага "удаления":
            // - если запись активна: 0x20 - выполняем цикл
            // - если запись удалена: 0x2A - нахер пропускаем всё это дерьмо
            // - если неведомая херня - выкидываем ошибку


            if      (memblock[0] == 0x2A)
                break;
            else if (memblock[0] != 0x20)
            {
                delete [] memblock;
                file.close();
                throw std::string("This is bullshit! The row can't begin from character ") + memblock[0];
            }
            else
            {
                j_pos = 1; // Нулевой байт, это флаг, по этому пропускаем его

                // ----- Теперь перебираем каждую ячейку
                for(uint8_t j = 0; j < columns; j++)
                {
                    num = getColumnLength(j);
                    cell.resize (num);  // без этого всё ломается
                    memcpy(&cell[0], &memblock[j_pos], num);
                    m_table.push_back(cell);
                    j_pos += num;
                }
            }
        }

        delete [] memblock;
*/

        // Проверяем значение последнего байта.
        // Последний байт должен быть 0x1A. Если это не так
        // значит либо я дебил, либо (скорее всего) - файл битый
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
    //std::string temp = m_table.at(row * m_title.getColumnsCount() + col);
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

        return std::string(result);
    }
    else
        throw "Can't open file";

    return "ZAGLUSHKA!";
}
