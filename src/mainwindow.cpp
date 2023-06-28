// Inclure les fichiers d'en-tête nécessaires
#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "filecontentdialog.h"

// Définir des constantes pour une meilleure lisibilité et maintenabilité
#define TRUE 1
#define FALSE 0

/*
Les constantes REGISTRY_KEY sont utilisées dans ce code pour représenter les clés de registre (ou les noms de paramètres de configuration)
associées aux différents paramètres de l'application. Ces constantes sont utilisées lors de la lecture et de l'écriture des paramètres à
partir d'un objet QSettings, qui permet de stocker et de gérer les paramètres de l'application de manière persistante.
*/

#define REGISTRY_FILE "configs/settings.ini"

#define REGISTRY_KEY_EXISTS "YDG3X2M"
#define REGISTRY_KEY_RECOVERABLE "7N3MPX4"
#define REGISTRY_KEY_SERVER_HOSTNAME "Q2WU6O9" // Représente le hostname du serveur.
#define REGISTRY_KEY_SERVER_USERNAME "IRV31SZ" // Représente le nom d'utilisateur pour accéder au serveur.
#define REGISTRY_KEY_SERVER_DAILY "6H8D2K0" // Représente le chemin du répertoire de sauvegarde quotidienne sur le serveur.
#define REGISTRY_KEY_SERVER_BACKUP "TY78S4K" // Représente le chemin du répertoire de sauvegarde sur le serveur.
#define REGISTRY_KEY_CLIENT_BACKUP "7B4XM8Z" // Représente le chemin du répertoire de sauvegarde du client.
#define REGISTRY_KEY_CLIENT_KEY "6H01MW2" // Représente la clé SSH privée du client.

#define REGISTRY_KEY_COMMAND "69J71R8"
#define REGISTRY_KEY_ATTEMPTS "KTDJ7M9"

#define REGISTRY_DEFAULT_VALUE ""

#define EXIT_ANIMATION_LENGHT 500
#define EXIT_ANIMATION_CURVE QEasingCurve::InOutCirc

#define FAVORITE_TAB_INDEX 2
#define RESTART_PROGRAM close();\
    qApp->quit();\
    QCoreApplication::exit(0);\
    QProcess::startDetached(qApp->arguments()[0], qApp->arguments());

#define RESET_ATTEMPTS attempts = 11;\
settings.setValue(REGISTRY_KEY_ATTEMPTS, 11);

// Constructeur de la classe MainWindow
MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow),
    settings("settings.ini", QSettings::IniFormat, this),
    run(this),
    multi(this)
{
    // Initialisation de l'interface utilisateur
    ui->setupUi(this);

    // Définir la taille minimale de la fenêtre
    setMinimumSize(0, 0);

    // Traiter les événements en attente
    qApp->processEvents();

    // Centrer la fenêtre sur l'écran
    QRect rec;
    int height = rec.height() / 4;
    int width = rec.width() / 4;
    move(width, height);

    // Initialiser l'animation de fermeture de la fenêtre
    exitAnimation = new QPropertyAnimation(this, "windowOpacity");
    exitAnimation->setEasingCurve(EXIT_ANIMATION_CURVE);
    exitAnimation->setDuration(EXIT_ANIMATION_LENGHT);
    exitAnimation->setStartValue(qreal(1));
    exitAnimation->setEndValue(qreal(0));

    // Connecter le signal "finished" de l'animation à la méthode "quit" de l'application
    connect(exitAnimation, SIGNAL(finished()), qApp, SLOT(quit()));

    // Démarrer le thread multi
    multi.start();

    // Vérifier si la clé de registre existe
    checkRegistryKey();

    // Récupérer la valeur de la clé de registre pour la commande
    command = settings.value(REGISTRY_KEY_COMMAND, REGISTRY_DEFAULT_VALUE).toString();

    // Configurer et démarrer le thread run
    run.moveToThread(&multi);
    run.commandPtr = &command;
    run.internalTimer->stop();
    run.done = false;
    run.startRunning();

    // Démarrer le timer interne du thread run
    run.internalTimer->start();

    // Afficher les valeurs de configuration
    qDebug() << "Hostname du serveur : " + settings.value(REGISTRY_KEY_SERVER_HOSTNAME).toString();
    qDebug() << "Nom d'utilisateur pour accéder au serveur : " + settings.value(REGISTRY_KEY_SERVER_USERNAME).toString();
    qDebug() << "Chemin du répertoire de sauvegarde quotidienne sur le serveur : " + settings.value(REGISTRY_KEY_SERVER_DAILY).toString();
    qDebug() << "Chemin du répertoire de sauvegarde sur le serveur : " + settings.value(REGISTRY_KEY_CLIENT_BACKUP).toString();
    qDebug() << "Chemin du répertoire de sauvegarde du client : " + settings.value(REGISTRY_KEY_CLIENT_BACKUP).toString();
    qDebug() << "Clé SSH privée du client : " + settings.value(REGISTRY_KEY_CLIENT_KEY).toString();
}

// Destructeur de la classe MainWindow
MainWindow::~MainWindow()
{
    // Supprimer l'interface utilisateur
    delete ui;
}

// Gestionnaire d'événements lors de la fermeture de la fenêtre principale
void MainWindow::closeEvent(QCloseEvent *event)
{
    event->ignore();

    run.internalTimer->stop(); // Arrête le timer interne

    multi.exit(0); // Quitte le thread multi

    ui->centralWidget->hide(); // Cache le widget central

    // Initialise et démarre l'animation de fermeture
    exitAnimation->start(QPropertyAnimation::DeleteWhenStopped);

    event->accept();
}

// Vérifie si la clé de registre existe
void MainWindow::checkRegistryKey()
{
    if (settings.value(REGISTRY_KEY_EXISTS) != TRUE) {
        configureSoftware(); // Configure le logiciel si la clé de registre n'existe pas
    }
}

// Gestionnaire de clic sur le bouton de réinitialisation
void MainWindow::on_resetButton_clicked()
{
    int ok = QMessageBox::warning(this, "Warning",
"Do you really want to reset your data ? "
"You will still be able to recover most of it.",
QMessageBox::Yes, QMessageBox::No);

    if (ok == QMessageBox::No) {
        return;
    }

    settings.setValue(REGISTRY_KEY_EXISTS, FALSE); // Réinitialise la clé de registre

    QMessageBox::information(this, "Information",
                   "The software will now restart");

    qDebug() << "REGISTRY_KEY_EXISTS : " + settings.value(REGISTRY_KEY_EXISTS).toString();

    RESTART_PROGRAM // Redémarre le programme
}

// Gestionnaire de clic sur le bouton de réinitialisation complète
void MainWindow::on_hResetButton_clicked()
{
    int ok = QMessageBox::warning(this, "Warning",
"Do you really want to reset your data ? "
"You won't be able to recover any of it.",
QMessageBox::Yes, QMessageBox::No);

    if (ok == QMessageBox::No) {
        return;
    }

    settings.clear(); // Efface toutes les données de configuration
    QMessageBox::information(this, "Information",
                   "The software will now restart");

    RESTART_PROGRAM // Redémarre le programme
}

// Configure le logiciel en fonction des valeurs de configuration
void MainWindow::configureSoftware()
{
    int ok = QMessageBox::question(this, "Welcome",
"It seems to be the first time you run this program. "
"You must configure it first. Continue ?",
QMessageBox::Yes, QMessageBox::Abort);

    if (ok == QMessageBox::Abort) {
        close();
        return;
    }

    int recover(0);

    if (settings.value(REGISTRY_KEY_RECOVERABLE) == TRUE) {
        recover = QMessageBox::question(this, "Recover",
"A previous installation was detected. "
"Do you want to try to recover data from it ?",
QMessageBox::Yes, QMessageBox::No);
    }

    if (recover == QMessageBox::Yes) {
        configInstance = new configDialog(
        (settings.value(REGISTRY_KEY_SERVER_USERNAME).toString()),
        (settings.value(REGISTRY_KEY_SERVER_HOSTNAME).toString()),
        (settings.value(REGISTRY_KEY_SERVER_DAILY).toString()),
        (settings.value(REGISTRY_KEY_SERVER_BACKUP).toString()),
        (settings.value(REGISTRY_KEY_CLIENT_BACKUP).toString()),
        (settings.value(REGISTRY_KEY_CLIENT_KEY).toString()),
                    this);
        configInstance->show();
    }
    else {
        configInstance = new configDialog(this);
        configInstance->show();
    }

    configInstance->setModal(true);

    run.done = true;

    connect(configInstance, SIGNAL(rejected()), this, SLOT(close()));
    connect(configInstance, SIGNAL(accepted()), this, SLOT(connectionTest()));

}

// Teste la connexion et met à jour les paramètres de configuration
void MainWindow::connectionTest()
{


    settings.setValue(REGISTRY_KEY_EXISTS, TRUE);
    settings.setValue(REGISTRY_KEY_RECOVERABLE, TRUE);

    settings.setValue(REGISTRY_KEY_SERVER_HOSTNAME, (configInstance->serverHost->text()));

    settings.setValue(REGISTRY_KEY_SERVER_USERNAME, (configInstance->serverUser->text()));

    settings.setValue(REGISTRY_KEY_SERVER_DAILY,    (configInstance->serverDaily->text()));

    settings.setValue(REGISTRY_KEY_SERVER_BACKUP,   (configInstance->serverBackup->text()));

    settings.setValue(REGISTRY_KEY_CLIENT_BACKUP,   (configInstance->clientBackup->text()));

    settings.setValue(REGISTRY_KEY_CLIENT_KEY, (configInstance->clientKey->text()));

    QMessageBox::information(this, "Success", "Data successfully saved.");
    sync commandString(sync::linuxos);

    QString osName = getOsName();
    if (osName == "Linux") {
        commandString.Platform = (sync::linuxos);
    } else if (osName == "MacOSX") {
        commandString.Platform = (sync::macosx);
    } else {
        QMessageBox::critical(this, "ERROR", "Your OS is not supported. \n"
                                                         "This software only supports "
                                                         "LinuxOS and MacOSX. \nIf you think "
                                                         "that there is an issue, please report it on github.");
        qApp->quit();
    }

    commandString.setServerUser(configInstance->serverUser->text());
    commandString.setServerHostname(configInstance->serverHost->text());
    commandString.setServerDaily(configInstance->serverDaily->text());

    commandString.setClientDir(configInstance->clientBackup->text());
    commandString.setClientKey(configInstance->clientKey->text());

    command = commandString.generateCommand();
    settings.setValue(REGISTRY_KEY_COMMAND, command);
    run.done = false;

}

// Modifie la commande de synchronisation en fonction des données de configuration
void MainWindow::modifyBackup () {
    sync commandString(sync::linuxos);
    QString osName = getOsName();

    if (osName == "Linux") {
        commandString.Platform = (sync::linuxos);
    } else if (osName == "MacOSX") {
        commandString.Platform = (sync::macosx);
    } else {
        QMessageBox::critical(this, "ERROR", "Your OS is not supported. \n"
                                                         "This software only supports "
                                                         "LinuxOS and MacOSX. \nIf you think "
                                                         "that there is an issue, please report it on github.");

        qApp->quit();
    }

    QString serverHost = settings.value(REGISTRY_KEY_SERVER_HOSTNAME).toString();
    QString serverUser = settings.value(REGISTRY_KEY_SERVER_USERNAME).toString();
    QString serverDaily = settings.value(REGISTRY_KEY_SERVER_DAILY).toString();

    QString clientBackup = settings.value(REGISTRY_KEY_CLIENT_BACKUP).toString();
    QString clientKey = settings.value(REGISTRY_KEY_CLIENT_KEY).toString();

    commandString.setServerHostname(serverHost);
    commandString.setServerUser(serverUser);
    commandString.setServerDaily(serverDaily);

    commandString.setClientDir(clientBackup);
    commandString.setClientKey(clientKey);

    command = commandString.generateCommand();
    settings.setValue(REGISTRY_KEY_COMMAND, command);
}

// Récupère une nouvelle chaîne de caractères pour une propriété donnée
QString MainWindow::getNewString (QString property, bool &ok)
{
    QString newString = QInputDialog::getText(this,
                        "Change " + property, "Enter new " + property, QLineEdit::Normal, QString(), &ok);
    return newString;
}

// Gestionnaire de clic sur le bouton "Changer l'hôte du serveur"
void MainWindow::on_CServerHostButton_clicked()
{
    bool ok;
    QString newValue = getNewString("server hostname", ok);
    if (ok)
        settings.setValue(REGISTRY_KEY_SERVER_HOSTNAME, newValue);
    modifyBackup();
    qDebug() << "Hostname du serveur : " + settings.value(REGISTRY_KEY_SERVER_HOSTNAME).toString();

}

// Gestionnaire de clic sur le bouton "Changer l'utilisateur du serveur"
void MainWindow::on_CServerUserButton_clicked()
{
    bool ok;
    QString newValue = getNewString("server username", ok);
    if (ok)
        settings.setValue(REGISTRY_KEY_SERVER_USERNAME, newValue);
    modifyBackup();

    qDebug() << "Nom d'utilisateur pour accéder au serveur : " + settings.value(REGISTRY_KEY_SERVER_USERNAME).toString();
}

// Gestionnaire de clic sur le bouton "Changer le répertoire de sauvegarde quotidien du serveur"
void MainWindow::on_CServerDailyButton_clicked()
{
    bool ok;
    QString newValue = getNewString("Server Daily Backup Directory Path", ok);
    if (ok)
        settings.setValue(REGISTRY_KEY_SERVER_DAILY, newValue);
    modifyBackup();
    qDebug() << "Chemin du répertoire de sauvegarde quotidienne sur le serveur : " + settings.value(REGISTRY_KEY_SERVER_DAILY).toString();
}

// Gestionnaire de clic sur le bouton "Changer le répertoire de sauvegarde du serveur"
void MainWindow::on_CServerBackupButton_clicked()
{
    bool ok;
    QString newValue = getNewString("Server Backup Directory Path", ok); // Récupérer le nouveau chemin du répertoire de sauvegarde du client
    if (ok)
        settings.setValue(REGISTRY_KEY_SERVER_BACKUP, newValue);
    qDebug() << "Chemin du répertoire de sauvegarde sur le serveur : " + settings.value(REGISTRY_KEY_CLIENT_BACKUP).toString();
}

// Gestionnaire de clic sur le bouton "Changer le répertoire de sauvegarde du client"
void MainWindow::on_CClientBackupButton_clicked()
{
    bool ok;
    QString newValue = getNewString("Client Backup Directory Path", ok);
    if (ok)
        settings.setValue(REGISTRY_KEY_CLIENT_BACKUP, newValue);
    modifyBackup(); // Modifier le Backup
}

void MainWindow::on_CClientKeyButton_clicked()
{
    bool ok;
    QString newValue = getNewString("Client Private SSH Key", ok); // Récupérer la nouvelle clé SSH privée du client
    if (ok)
        settings.setValue(REGISTRY_KEY_CLIENT_KEY, newValue);
    modifyBackup(); // Modifier le Backup
    qDebug() << "Clé SSH privée du client : " + settings.value(REGISTRY_KEY_CLIENT_KEY).toString();
}

// Gestionnaire de clic sur le bouton "Search"
void MainWindow::on_CSearchButton_clicked()
{
    // Initialiser la commande de synchronisation
    sync commandString(sync::linuxos);
    // Récupérer le nom du système d'exploitation
    QString osName = getOsName();
    // Récupérer les valeurs des paramètres du serveur
    QString serverHost = settings.value(REGISTRY_KEY_SERVER_HOSTNAME).toString().trimmed();
    QString serverUser = settings.value(REGISTRY_KEY_SERVER_USERNAME).toString().trimmed();
    QString serverBackup = settings.value(REGISTRY_KEY_SERVER_BACKUP).toString().trimmed();

    // Construire la commande SFTP
    QString fileManagerCommand = "";
    QString sftpCommand = "sftp://" + serverUser + "@" + serverHost + serverBackup;

    qDebug() << sftpCommand;

    int work = 0;
    showMinimized();

    // Exécuter la commande en fonction du système d'exploitation
    if (osName == "Linux") {
         fileManagerCommand = "nautilus " + sftpCommand + " &";
    } else if (osName == "MacOSX") {
       fileManagerCommand = "open /Applications/Cyberduck.app/Contents/MacOS/Cyberduck &";
    } else {
        QMessageBox::critical(this, "ERROR", "Your OS is not supported. \n"
                                                         "This software only supports "
                                                         "LinuxOS and MacOSX. \nIf you think "
                                                         "that there is an issue, please report it on github.");

        qApp->quit();
    }
    work = system (fileManagerCommand.toStdString().c_str());
    (void)work;
}

// Gestionnaire de clic sur le bouton "Sync"
void MainWindow::on_CSyncButton_clicked()
{
    // Initialiser la commande de synchronisation
    sync commandString(sync::linuxos);
    // Récupérer le nom du système d'exploitation
    QString osName = getOsName();

    // Exécuter la commande en fonction du système d'exploitation
    if (osName == "Linux") {
        commandString.Platform = (sync::linuxos);
    } else if (osName == "MacOSX") {
        commandString.Platform = (sync::macosx);
    } else {
        QMessageBox::critical(this, "ERROR", "Your OS is not supported. \n"
                                                         "This software only supports "
                                                         "LinuxOS and MacOSX. \nIf you think "
                                                         "that there is an issue, please report it on github.");

        qApp->quit();
    }

    // Récupérer les valeurs des paramètres du serveur et du client
    QString serverHost = settings.value(REGISTRY_KEY_SERVER_HOSTNAME).toString().trimmed();
    QString serverUser = settings.value(REGISTRY_KEY_SERVER_USERNAME).toString().trimmed();
    QString serverDaily = settings.value(REGISTRY_KEY_SERVER_DAILY).toString().trimmed();

    QString clientBackup = settings.value(REGISTRY_KEY_CLIENT_BACKUP).toString().trimmed();
    QString clientKey = settings.value(REGISTRY_KEY_CLIENT_KEY).toString().trimmed();

    // Configurer les paramètres de la commande de synchronisation
    commandString.setServerHostname(serverHost);
    commandString.setServerUser(serverUser);
    commandString.setServerDaily(serverDaily);

    commandString.setClientDir(clientBackup);
    commandString.setClientKey(clientKey);

    // Générer la commande de synchronisation
    command = commandString.generateCommand() + " &";

    // Exécuter la commande de synchronisation
    int work = system (command.toStdString().c_str());
    FileContentDialog dialog;
    dialog.exec();

    (void)work;
}

// Gestionnaire de clic sur le bouton "Log"
void MainWindow::on_CLogButton_clicked()
{
    QString osName = getOsName();
    int work = 0;
    QString logCommand = "";
    if (osName == "Linux") {
        logCommand = "mousepad /home/linus/Documents/BACKUP/PROJETINFO/rsync.log &";
    } else if (osName == "MacOSX") {
        logCommand = "open rsync.log &";
    } else {
        QMessageBox::critical(this, "ERROR", "Your OS is not supported. \n"
                                                         "This software only supports "
                                                         "LinuxOS and MacOSX. \nIf you think "
                                                         "that there is an issue, please report it on github.");

        qApp->quit();
    }
    work = system (logCommand.toStdString().c_str());
    // Exécuter la commande pour ouvrir le fichier de journalisation en fonction du système d'exploitation
    (void)work;
}

// Récupérer le nom du système d'exploitation en fonction des macros de préprocesseur
QString MainWindow::getOsName()
{
    #ifdef _WIN32
    return "Windows";
    #elif _WIN64
    return "Windows";
    #elif __APPLE__ || __MACH__
    return "MacOSX";
    #elif __linux__
    return "Linux";
    #else
    return "Other";
    #endif
}

// Gestionnaire de clic sur le bouton "changeBackupButton"
void MainWindow::on_changeBackupButton_clicked()
{
    QString fileName = QFileDialog::getOpenFileName(this, "Backup", QString(), QString());
    if (fileName.isEmpty()) return;
    settings.setValue(REGISTRY_KEY_CLIENT_BACKUP, fileName);
    modifyBackup();
}

// Gestionnaire de clic sur le bouton "changeSSHButton_clicked"
void MainWindow::on_changeSSHButton_clicked()
{
    QString fileName = QFileDialog::getOpenFileName(this, "SSH Key", QString(), QString());
    if (fileName.isEmpty()) return;
    settings.setValue(REGISTRY_KEY_CLIENT_KEY, fileName);
    modifyBackup();
}
