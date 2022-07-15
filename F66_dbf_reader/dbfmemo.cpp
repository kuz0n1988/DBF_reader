#include "dbfmemo.h"

#include <unistd.h>

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
}
