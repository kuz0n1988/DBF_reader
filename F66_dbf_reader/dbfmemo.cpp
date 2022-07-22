#include "dbfmemo.h"

#include <unistd.h>
#include <iostream>
#include <fstream>
#include <cstring>

#include <QDebug>
#include <QString>

const std::size_t DBFMemo::SIZE_TITLE        = 512;
const std::size_t DBFMemo::SIZE_TITLE_USEFUL = 8;

DBFMemo::DBFMemo(const std::string& parent_filepath)
{
    std::string dbt = parent_filepath.substr(0, parent_filepath.length()-3) + "DBT";
    std::string fpt = parent_filepath.substr(0, parent_filepath.length()-3) + "FPT";

    if      (access( fpt.c_str(), F_OK ) != -1)
        m_filepath = fpt;
    else if (access( dbt.c_str(), F_OK ) != -1)
        m_filepath = dbt;
    else
        throw "Error! There is no memofile (*.DBT or *.FPT)";

    qDebug () << QString::fromStdString(m_filepath);

    std::ifstream file(m_filepath, std::ios::in | std::ios::binary);
    if(file.is_open())
    {
        file.seekg(0);
        char  *temp_s = new char[SIZE_TITLE_USEFUL];
        file.read(temp_s, SIZE_TITLE_USEFUL);

        /* Последние четыре строчки я уже замахался по десятому разу переделывать
         * Код вроде правильный и должен работать адекватно. Во всяком случае в
         * соседнем dbftitle.cpp всё работает именно так.
         * Но здесь, эмпирическим путём мною было выяснено, что байты копируются
         * В ОБРАТНОМ ПОРЯДКЕ!!!
         * Т.Е. вместо того, чтобы записать в m_index_of_next_aviable
         * последовательность 00 01 f6 aa (128682) туда залетает АА F6 01 00 (2868248832)
         * С m_size_block ещё смешнее выходит. Вместо 00 40 (64) туда залетает 40 00 (16384)
         *
         * Я уже думал, что может у меня файл не с того места читается, или он читается
         * как-то задом наперёд, но перебрав каждый элемент массива обнаружил, что читается
         * он как раз таки как надо!!! Значит собака зарыта где-то в memcpy. Но где????

        memcpy(&m_index_of_next_aviable, &temp_s[0], 4);
        qDebug() << m_index_of_next_aviable;
        memcpy(&m_size_block, &temp_s[6], 2);
        qDebug() << m_size_block; */

        m_index_of_next_aviable =   (uchar(temp_s[0]) << 24) +
                                    (uchar(temp_s[1]) << 16) +
                                    (uchar(temp_s[2]) << 8)  +
                                    (uchar(temp_s[3]));
        m_size_block            =   (uchar(temp_s[6]) << 8)  +
                                    (uchar(temp_s[7]));

        delete [] temp_s;
        file.close();
    }
    else
        throw "Error! Can't open " + m_filepath;
}

DBFMemo::~DBFMemo(){}

std::string DBFMemo::getMemoData(const uint32_t &index)
{
    std::string result;
    std::streampos pos = m_size_block * index;

    if(index < m_index_of_next_aviable)
    {
        char *memblock = new char[8];
        std::ifstream file(m_filepath, std::ios::in | std::ios::binary);
        file.seekg(pos);
        file.read(memblock, 8);
        /*uint32_t memo_type =
                (uchar(memblock[0]) << 24) +
                (uchar(memblock[1]) << 16) +
                (uchar(memblock[2]) << 8)  +
                (uchar(memblock[3]));*/

        uint32_t memo_size =
                (uchar(memblock[4]) << 24) +
                (uchar(memblock[5]) << 16) +
                (uchar(memblock[6]) << 8)  +
                (uchar(memblock[7]));

        delete [] memblock;
        result.resize(memo_size);
        file.read(&result[0], memo_size);
        file.close();
    }
    else
        throw "MEMO-index is out of range! MAX idx must be < " + m_index_of_next_aviable;

    return result;
}

// Геттеры
uint32_t DBFMemo::getIndexOfNextAviable()
{   return m_index_of_next_aviable; }

uint16_t DBFMemo::getSizeBlock()
{   return m_size_block;    }

