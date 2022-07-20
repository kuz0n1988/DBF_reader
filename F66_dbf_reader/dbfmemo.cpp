#include "dbfmemo.h"

#include <unistd.h>
#include <iostream>
#include <fstream>
#include <cstring>

#include <QDebug>
#include <QString>

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
        char *temp_s = new char[8];
        file.read(temp_s, 8);

        for(int i = 0; i < 8; i++)
            qDebug() << i << "-element: " << temp_s[i];

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
        */
        memcpy(&m_index_of_next_aviable, &temp_s[0], 4);
        qDebug() << m_index_of_next_aviable;
        memcpy(&m_size_block, &temp_s[6], 2);
        qDebug() << m_size_block;

        delete [] temp_s;
    }
    else
        throw "Error! Can't open " + m_filepath;

    file.close();
}

// Геттеры
uint32_t DBFMemo::getIndexOfNextAviable()
{   return m_index_of_next_aviable; }

uint16_t DBFMemo::getSizeBlock()
{   return m_size_block;    }
