#ifndef DBFTITLE_H
#define DBFTITLE_H

#include <cstdint>
#include <string>

// Хранит данные заголовка DBF-файла
class DBFTitle // информация из 32-байтного заголовка файла
{
private:
    char        m_version;          // хранится в виде однобайтного значения.
    uint8_t     m_lastmodified_y;   // год хранится двумя цифрами. Учитывай!
    uint8_t     m_lastmodified_m;
    uint8_t     m_lastmodified_d;
    uint32_t    m_rows_count;
    uint8_t     m_cols_count;
    uint16_t    m_title_size;
    uint16_t    m_row_size;

    // скорее всего не понадобятся:
    bool m_incomplete_transaction;
    bool m_crypting_flag;
    bool m_productionmdx;
    char m_language_id;

public:
    DBFTitle(const char *title);    // массив 32 байта
    DBFTitle();

    std::string getTitleInfo()          const;
    std::string getVersionInfo()        const;
    std::string getLastModifiedDate()   const;
    uint32_t    getRowsCount()          const;
    uint8_t     getColumnsCount()       const;  // максимум 255
    uint16_t    getTitleSize()          const;
    uint16_t    getRowSize()            const;
    bool        getMemoFlag()           const;
};

#endif // DBFTITLE_H
