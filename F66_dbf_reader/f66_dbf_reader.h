#ifndef F66_DBF_READER_H
#define F66_DBF_READER_H

#include <QWidget>

class QString;
class QFile;
class QTextEdit;
class QLineEdit;
class QTableWidget;
class QVBoxLayout;

class F66_DBF_Reader : public QWidget
{
    Q_OBJECT

    QVBoxLayout     *m_plyt_table; // не смог сделать всё как мне хотелось без этого костыля
    QTableWidget    *m_ptwd_table;
    QTextEdit       *m_ptxe_status;
    QLineEdit       *m_pled_filename;

    bool checkFileName(const QString &filepath);
public:
    F66_DBF_Reader(QWidget *parent = nullptr);
    ~F66_DBF_Reader();

public slots:
    void slot_toF66_DBF_Old();
};
#endif // F66_DBF_READER_H
