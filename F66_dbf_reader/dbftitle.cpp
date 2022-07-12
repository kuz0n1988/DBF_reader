#include "dbftitle.h"

DBFTitle::DBFTitle(const char *title)
{
    if(title != nullptr)
    {
    /* Конструктор принимает (предполагается, что из потока) первые 32 бита
     * dbf-файла и раскладывает их по этой схеме:
     *
     * byte [0]:    0-2 bits - версия
     *              3   bit  - наличие БД для файла памятки DOS
     *              4-6 bit  - наличие таблицы SQL
     *              7   bit  - наличие любого файла-памятки
     * byte [1-3]:  Дата последнего обновления YYMMDD (3 байта)
     * byte [4-7]:  Количество записей в файле (32-разрядное число)
     * byte [8-9]:  Количество байтов в заголовке (16-разрядное число)
     * byte [10-11]:Количество байтов в одной записи (16-разрядное число)
     * byte [12-13]:RESERVED (0)
     * byte [14]:   Незавершенная транзакция (bool)
     * byte [15]:   Флаг шифрования (bool)
     * byte [16-27]:RESERVED (0)
     * byte [28]:   Наличие файла production.mdx (bool)
     * byte [29]:   Идентификатор языкового драйвера (в Ф66 - нули)
     * byte [30-31]:RESERVED (0)
     */

        // ===== byte[0] Первый байт содержит версию DBF-файла
        m_version = title[0];

        // ===== byte[1-3] Дата последнего обновления
        m_lastmodified_y = static_cast<uint8_t>(title[1]);   // год хранится двумя цифрами. Учитывай!
        m_lastmodified_m = static_cast<uint8_t>(title[2]);
        m_lastmodified_d = static_cast<uint8_t>(title[3]);

        // ===== byte [4-7]:  Количество записей в файле (32-разрядное число)
        memcpy(&m_rows_count, &title[4], 4);

        // ===== byte [8-9]:  Количество байтов в заголовке (16-разрядное число)
        memcpy(&m_title_size, &title[8], 2);

        // ===== byte [10-11]:Количество байтов в одной записи (16-разрядное число)
        memcpy(&m_row_size, &title[10], 2);

        // ===== byte [12-13]:RESERVED (0)
        // ----- пропускаем

        // ===== byte [14]:   Незавершенная транзакция (bool)
        // Понятия не имею, что это, но допустим
        m_incomplete_transaction = (title[14] != 0);

        // ===== byte [15]:   Флаг шифрования (bool)
        m_crypting_flag = (title[15] != 0);

        // ===== byte [16-27]:RESERVED (0)
        // ----- пропускаем

        // ===== byte [28]:   Наличие файла production.mdx (bool)
        m_productionmdx = (title[28] != 0);

        // ===== byte [29]:   Идентификатор языкового драйвера (в Ф66 - нули)
        m_language_id = title[29];

        // ===== byte [30-31]:RESERVED (0)
        // ----- пропускаем
    }
}

DBFTitle::DBFTitle(){}

std::string DBFTitle::getTitleInfo() const
{
    std::string temp_info = "\nDBF-file fifle info:";

    temp_info.append("\nFile version:  " + getVersionInfo());
    temp_info.append("\nLast modified: " + getLastModifiedDate());
    temp_info.append("\nRow size:      " + std::to_string(getRowSize()) + " bytes");
    temp_info.append("\nRows count:    " + std::to_string(getRowsCount()));
    temp_info.append("\nColumns count: " + std::to_string(getColumnsCount()));
    temp_info.append("\nTitle size:    " + std::to_string(getTitleSize()) + " bytes");

    return temp_info;
}

std::string DBFTitle::getVersionInfo() const
{
    switch (static_cast<unsigned char>(m_version))
    {
        case 0x02: return "FoxBase 1.0";
        case 0x03: return "FoxBase 2.x / dBASE III";
        case 0x83: return "FoxBase 2.x / dBASE III with memo file";
        case 0x30: return "Visual FoxPro";
        case 0x31: return "Visual FoxPro with auto increment";
        case 0x32: return "Visual FoxPro with varchar/varbinary";
        case 0x43: return "dBASE IV SQL Table, no memo file";
        case 0x63: return "dBASE IV SQL System, no memo file";
        case 0x8b: return "dBASE IV with memo file";
        case 0xcb: return "dBASE IV SQL Table with memo file";
        case 0xfb: return "FoxPro 2";
        case 0xf5: return "FoxPro 2 with memo file";
        default:   return "UNIDENTEFIED";
    }
}

std::string DBFTitle::getLastModifiedDate() const
{
    std::string date_t = "";

    // ===== день
    if(m_lastmodified_d < 10)
        date_t += "0";
    date_t += std::to_string(m_lastmodified_d) + ".";

    // ===== месяц
    if(m_lastmodified_m < 10)
        date_t += "0";
    date_t += std::to_string(m_lastmodified_m) + ".";

    // ===== год
    // формат появился на свет в 1983-м году
    // надеюсь, он умрёт до 2083-го.
    if(m_lastmodified_y > 83)
        date_t += "19";
    else
        date_t += "20";
    if(m_lastmodified_y < 10)
        date_t += "0";
    date_t += std::to_string(m_lastmodified_y);

    return date_t;
}

uint32_t    DBFTitle::getRowsCount() const
{    return m_rows_count;   }

uint8_t     DBFTitle::getColumnsCount() const
{
    // Количество столбцов не дано, но мы можем их вычислить
    // потому что знаем общую длину заголовка, размер титула (32 байта)
    // размер инфы в заголовке для одного столбца (32) и один закрывающий
    // байт 0x0D (m_title_size - 32 - 1) / 32;
    unsigned int temp = (m_title_size - 32 - 1) / 32;

    // столбцов не может быль больше 255
    if(temp > 255)
        throw "Pizdanite programmista po bashke. On che-to naputal!";

    return static_cast<uint8_t>(temp);
}

uint16_t    DBFTitle::getTitleSize() const
{    return m_title_size;   }
uint16_t    DBFTitle::getRowSize() const
{    return m_row_size;     }
