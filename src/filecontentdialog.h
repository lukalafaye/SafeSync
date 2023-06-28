#ifndef FILECONTENTDIALOG_H
#define FILECONTENTDIALOG_H

#include <QDialog>
#include <QVBoxLayout>
#include <QTextEdit>
#include <QScrollBar>
#include <QTimer>
#include <QFile>
#include <QTextStream>
#include <QDebug>

class FileContentDialog : public QDialog
{
    Q_OBJECT

public:
    explicit FileContentDialog(QWidget* parent = nullptr);
    ~FileContentDialog();

private slots:
    void updateFileContent();

private:
    QVBoxLayout* layout;
    QTextEdit* textEdit;
    QTimer* timer;
    int previousScrollValue;
};

#endif // FILECONTENTDIALOG_H
