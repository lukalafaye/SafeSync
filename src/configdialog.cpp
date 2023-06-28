#include "configdialog.h"
#include "ui_configdialog.h"

// Constructeur de configDialog avec seulement parent comme argument
configDialog::configDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::configDialog)
{
    ui->setupUi(this);
    initPtr();
}

// Constructeur surchargé de configDialog avec des arguments supplémentaires pour les informations du serveur et du client
configDialog::configDialog(const QString &serverUser,
                           const QString &serverHost,
                           const QString &serverDaily,
                           const QString &serverBackup,
                           const QString &clientBackup,
                           const QString &clientKey,
                           QWidget *parent) :
    QDialog(parent),
    ui(new Ui::configDialog)
{
    ui->setupUi(this);

    // Initialiser les champs de texte avec les informations du serveur et du client
    ui->serverUserLine->setText(serverUser);
    ui->serverHostLine->setText(serverHost);
    ui->serverDailyLine->setText(serverDaily);
    ui->serverBackupLine->setText(serverBackup);

    ui->clientBackupLine->setText(clientBackup);
    ui->clientKeyLine->setText(clientKey);

    initPtr();
}

// Destructeur de configDialog
configDialog::~configDialog()
{
    delete ui;
}

// Fonction pour initialiser les pointeurs et l'ordre de tabulation des champs de texte
void configDialog::initPtr()
{
    serverUser = ui->serverUserLine;
    serverHost = ui->serverHostLine;
    serverDaily = ui->serverDailyLine;
    serverBackup = ui->serverBackupLine;

    clientBackup = ui->clientBackupLine;
    clientKey = ui->clientKeyLine;

    // Définir l'ordre de tabulation pour les champs de texte
    setTabOrder(ui->serverUserLine, ui->serverHostLine);
    setTabOrder(ui->serverHostLine, ui->serverDailyLine);
    setTabOrder(ui->serverDailyLine, ui->serverBackupLine);
    setTabOrder(ui->serverBackupLine, ui->clientBackupLine);
    setTabOrder(ui->clientBackupLine, ui->clientKeyLine);

    setModal(true);
}

// Fonction appelée lorsqu'on clique sur le bouton de recherche de dossier de sauvegarde client
void configDialog::on_searchBackupButton_clicked()
{
    QString folderName = QFileDialog::getExistingDirectory (this, "Open Client Backup Directory", QString(), QFileDialog::ShowDirsOnly);
    if (folderName.isEmpty()) return;
    ui->clientBackupLine->setText(folderName);
}

// Fonction appelée lorsqu'on clique sur le bouton de recherche de clé SSH publique
void configDialog::on_searchSSHButton_clicked()
{
    QString fileName = QFileDialog::getOpenFileName(this, "Select Public SSH key", QString(), QString());
    if (fileName.isEmpty()) return;
    ui->clientKeyLine->setText(fileName);
}
