#ifndef DBFDESCRIPTOR_H
#define DBFDESCRIPTOR_H

#include <cstdint>
#include <string>

class DBFDescriptor
{
private:
    // самое важное
    uint8_t     m_length;   // max: 254 (0xFE)
    char        m_name [11];
    char        m_type;

    // он есть, но он нахер не нужен
    uint32_t    m_address_in_memory;

    // а вот эти поля в файлах пустые.
    // Зато они есть в спецификации к формату DBF
    uint8_t     m_field_decimal_count;
    uint8_t     m_workarea_id;
    uint8_t     m_field_flag;

public:
    DBFDescriptor(const char *descriptor); // 32 байта
    DBFDescriptor();

    std::string getDescriptorInfo();
    std::string getName() const ;
    char        getType() const ;
    uint8_t     getLength() const;
};

#endif // DBFDESCRIPTOR_H
