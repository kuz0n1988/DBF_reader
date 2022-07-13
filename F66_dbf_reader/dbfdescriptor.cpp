#include "dbfdescriptor.h"

DBFDescriptor::DBFDescriptor(const char *descriptor)
{
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
    std::string info = "";
    info.append("Name:\t" + getName());

    info.append("\nType:\t");
    switch (getType())
    {
    case 'C': info.append("Character string");          break;
    case 'D': info.append("Date (YYYYMMDD)");           break;
    case 'F': info.append("Float");                     break;
    case 'N': info.append("Numeric (Float)");           break;
    case 'L': info.append("Logical (bool)");            break;
    case 'T': info.append("DateTime (VoxPro type)");    break;
    case 'I': info.append("Integer (VoxPro type)");     break;
    case 'Y': info.append("Logical (VoxPro type)");     break;
    case 'M': info.append("MEMO (index of memoblock)"); break;
    default:  info.append("Type not support: "); info += getType();
    }

    info.append("\nLength:    \t" + std::to_string(getLength()) + " bytes");
    info.append("\nDecimal count:\t" + std::to_string(m_field_decimal_count));
    info.append("\nWork area ID:\t" + std::to_string(m_workarea_id));
    info.append("\nField flag:\t" + std::to_string(m_field_flag));

    return info;
}

std::string DBFDescriptor::getName() const
{    return std::string (m_name);   }

char DBFDescriptor::getType() const
{    return m_type;     }

uint8_t DBFDescriptor::getLength() const
{    return m_length;   }
