#include "filecontentdialog.h"

FileContentDialog::FileContentDialog(QWidget* parent)
    : QDialog(parent)
{
    layout = new QVBoxLayout(this);
    textEdit = new QTextEdit(this);
    textEdit->setReadOnly(true);
    layout->addWidget(textEdit);

    setLayout(layout);
    setWindowTitle("File Content");

    setFixedSize(400, 300); // Set the desired width and height

    previousScrollValue = 0; // Initialize previousScrollValue

    updateFileContent();

    timer = new QTimer(this);
    timer->setInterval(1000); // Adjust the interval as needed

    connect(timer, &QTimer::timeout, this, &FileContentDialog::updateFileContent);

    timer->start();
}

FileContentDialog::~FileContentDialog()
{
    delete timer;
}

void FileContentDialog::updateFileContent()
{
    int scrollValue = textEdit->verticalScrollBar()->value(); // Save the current scroll position
    QFile file("/home/linus/Documents/BACKUP/PROJETINFO/rsync.log"); // Replace with the actual path to your file
    if (file.open(QIODevice::ReadOnly | QIODevice::Text))
    {
        QTextStream in(&file);
        QString fileContent = in.readAll();

        file.close();

        qDebug() << "File content read:" << fileContent;

        textEdit->setPlainText(fileContent);

        textEdit->verticalScrollBar()->setValue(scrollValue); // Restore the scroll position
    }
    else
    {
        qDebug() << "Failed to open file";
    }
}
