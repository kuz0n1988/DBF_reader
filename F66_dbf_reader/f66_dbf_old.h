#ifndef F66_DBF_OLD_H
#define F66_DBF_OLD_H

#include <cstdint>
#include <string>

#include <QList>
#include <QString>

#include "dbftitle.h"
#include "dbfdescriptor.h"
#include "dbfmemo.h"

class F66_DBF_Old
{
private:    // члены
    DBFTitle                m_title;
    QList<DBFDescriptor>    m_descriptors;
    const std::string       m_filepath;
    DBFMemo                *m_memo;

private:    // методы
    std::string             getElementFromFile(const uint32_t &row, const uint8_t &col);
    std::string             getMEMOData(const std::string& idx);

public:
    F66_DBF_Old(const QString &filepath);
    ~F66_DBF_Old();

    // Геттеры для m_title
    DBFTitle                getDBFTitle      ();
    uint32_t                getRowsCount();
    uint8_t                 getColumnsCount();  // максимум 255
    uint16_t                getTitleSize();
    uint16_t                getRowSize();

    // Геттеры для m_descriptors
    QList<DBFDescriptor>    getDBFDescriptors();
    std::string             getColumnName   (const int &col);
    char                    getColumnType   (const int &col);
    uint8_t                 getColumnLength (const int &col);

    QList<std::string>      getTable          ();    // Таблица формируется по схеме row * i + column
    std::string             getElement        (const unsigned int &row, const unsigned int &col);
    bool                    rowDeleteStatus(const uint32_t &row);

};

#endif // F66_DBF_OLD_H
