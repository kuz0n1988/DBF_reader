#include "dbfdescriptor.h"

DBFDescriptor::DBFDescriptor(const char *descriptor)
{
   /* По http://www.independent-software.com/dbase-dbf-dbt-file-format.html
    * 0-10     0x00	11	string	Field name (padded with NULL-bytes) +
    * 11       0x0b	1	char	Field type                          +
    * 12-15    0x0c	4	uint32  Field data address in memory        -
    * 16       0x10	1	byte	Field length                        +
    * 17       0x11	1	byte	Field decimal count                 ?
    * 18-19    0x12	2           Reserved for dBASE IIIPlus/LAN
    * 20       0x14	1	byte    Work area ID                        -
    * 21-22    0x15	2           Reserved for dBASE IIIPlus/LAN
    * 23       0x17	1	byte    SET FIELDS flag
    * 24-31    0x18	8           Reserved                            +
    */

    if(descriptor != nullptr)
    {
        // ===== byte [0-10] Название заголовка
        for(int i = 0; i < 11; i++)
            m_name[i] = descriptor[i];

        // ===== byte [11] Тип поля
        m_type = descriptor[11];

        // ===== byte [12-15] Адрес поля в памяти (не нужен)
        memcpy(&m_address_in_memory, &descriptor[12], 4);

        // ===== byte [16] Длина поля в байтах
        m_length = descriptor[16];

        // Остальные поля в файлах пустые. Понять их назначение
        // уже невозможно, да и не очень то и нужно.
        m_field_decimal_count = descriptor[17];
        m_workarea_id = descriptor[20];
        m_field_flag = descriptor[23];
    }
}

DBFDescriptor::DBFDescriptor(){}

std::string DBFDescriptor::getDescriptorInfo()
{
    std::string info = "\nColumn descriptor:";
    info.append("\nName:         " + getName());

    info.append("\nType:          ");
    switch (getType())
    {
    case 'C': info.append("Character string");      break;
    case 'D': info.append("Date (YYYYMMDD)");       break;
    case 'F': info.append("Float");                 break;
    case 'N': info.append("Numeric (Float)");        break;
    case 'L': info.append("Logical (bool)");        break;
    default:  info.append("Type not support: "); info += getType();
    }

    info.append("\nLength:        " + std::to_string(getLength()) + " bytes");

    info.append("\nDecimal count: " + std::to_string(m_field_decimal_count));
    info.append("\nWork area ID:  " + std::to_string(m_workarea_id));
    info.append("\nField flag:    " + std::to_string(m_field_flag));

    return info;
}

std::string DBFDescriptor::getName() const
{    return std::string (m_name);   }

/*  C	Character	A string of characters, padded with spaces if shorter than the field length
    D	Date		Date stored as string in YYYYMMDD format
    F	Float		Floating point number, stored as string, padded with spaces if shorter than the field length
    N	Numeric		Floating point number, stored as string, padded with spaces if shorter than the field length
    L	Logical		A boolean value, stored as one of YyNnTtFf. May be set to ? if not initialized
 * */
char DBFDescriptor::getType() const
{    return m_type;     }

uint8_t DBFDescriptor::getLength() const
{    return m_length;   }
