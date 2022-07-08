#include "f66_dbf_reader.h"

#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    F66_DBF_Reader w;
    w.show();
    return a.exec();
}
