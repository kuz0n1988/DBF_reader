#ifndef DBFMEMO_H
#define DBFMEMO_H

#include <cstdint>
#include <string>

class DBFMemo
{
private:
    static const std::size_t SIZE_TITLE;
    static const std::size_t SIZE_TITLE_USEFUL;

    std::string m_filepath;
    uint32_t    m_index_of_next_aviable;    // не понадобится
    uint16_t    m_size_block;
public:
    DBFMemo(const std::string &parent_filepath);
    ~DBFMemo();
    std::string getMemoData(const uint32_t &index);
    uint32_t    getIndexOfNextAviable();
    uint16_t    getSizeBlock();
};

#endif // DBFMEMO_H
