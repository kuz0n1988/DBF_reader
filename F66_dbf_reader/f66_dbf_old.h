#ifndef F66_DBF_OLD_H
#define F66_DBF_OLD_H

#include <cstdint>
#include <string>

#include <QList>
#include <QString>

#include "dbftitle.h"       // класс для заголовка
#include "dbfdescriptor.h"  // класс для дескриптора (описание колонки)

/* DBF-структура файла (вроде по пятому стандарту)
 *
 * // =====Заголовок файла=====
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
 * byte [32-n]: Массив дескрипторов полей
 * byte [n+1]:  Конец дескриптора полей 0x0D (13)
 *
 * Каждая новая запись начинается с: 0x20 - если запись активна (32 он же пробел)
 *                                   0x2A - если запись удалена (42, как иронично)
 * Конец заголовка файла:            0x0D (13)
 * Конец вообще всего файла:         0x1A (26)
 *
 * // =====Массив дескрипторов полей=====
 * По википедии:
 * byte [n+0–n+10]:     Имя поля в ASCII 11 байт
 * byte [n+11]:         Тип поля. (C, D, F, L, M, или N)
 * byte [n+12–n+15]:	RESERVED (0)
 * byte [n+16]:         Длина поля в двоичном формате (максимум 254 (0xFE)).
 * byte [n+17]:         Десятичное число полей в двоичном формате (хуита какая-то, в Ф66 нуль стоит)
 * byte [n+18–n+19]:	Идентификатор рабочей зоны 2 байта (чего?)
 * byte [n+20]:         Пример (чеггоооо? Но я так понимаю, что bool)
 * byte [n+21–n+30]:	RESERVED (0)
 * byte [n+31]:         1 байт	Флаг производственного многомерного поля (bool, но мне он нахер не нужен)
 *
 * По http://www.independent-software.com/dbase-dbf-dbt-file-format.html
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
 *
 * // =====Типы полей=====
 * С - char[] в формате ASCII
 * D - дата 8 цифр YYYYMMDD
 * F - float/double
 * L - bool (Y,y,T,t - true; N,n,F,f - false; ? - неинициализирован(NaN))
 * M - памятка (10 цифр)
 * N - число (int или long)
 * */

class F66_DBF_Old
{
private:
    DBFTitle                m_title;
    QList<DBFDescriptor>    m_descriptors;
    QList<std::string>      m_table;
    const std::string       m_filepath;

public:
    F66_DBF_Old(const QString &filepath);

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
    std::string             getElementFromFile(const unsigned int &row, const unsigned int &col);   // Мой код не в состоянии переварить файл
    // в котором 121'464 и 31 столбец. Возможно, потому что я наплодил слишком много сущностей
    // а возможно, потому что в этом грёбаном массиве больше 3млн значений, а значит я натыкаюсь
    // на переполнение переменной типа int
    // попробую вытягивать данные адресно непосредственно из файла


};

#endif // F66_DBF_OLD_H
