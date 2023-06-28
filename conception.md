Le présent rapport présente la conception détaillée du logiciel SafeSync. Ce rapport vise à fournir une documentation claire et concise sur l'architecture, les fonctionnalités et les choix de conception qui ont été réalisés tout au long du processus de développement. Il est destiné à faciliter la compréhension du logiciel, tant pour les membres de l'équipe de développement que pour les parties prenantes et les utilisateurs finaux.

## safesync.pro

Le projet safesync utilise le framework Qt et est une application de type "app".
Fichiers sources (SOURCES) :

    filecontentdialog.cpp : Fichier source pour la boîte de dialogue de contenu de fichier.
    main.cpp : Fichier source principal contenant la fonction main() qui lance l'application.
    mainwindow.cpp : Fichier source pour la fenêtre principale de l'application.
    configdialog.cpp : Fichier source pour la boîte de dialogue de configuration.
    ScriptsClass/sync.cpp : Fichier source pour la classe Sync dans le répertoire ScriptsClass, responsable de la synchronisation des fichiers.
    background.cpp : Fichier source pour la classe Background qui gère les opérations en arrière-plan.

Fichiers d'en-tête (HEADERS) :

    filecontentdialog.h : Fichier d'en-tête pour la boîte de dialogue de contenu de fichier.
    mainwindow.h : Fichier d'en-tête pour la fenêtre principale de l'application.
    configdialog.h : Fichier d'en-tête pour la boîte de dialogue de configuration.
    ScriptsClass/sync.h : Fichier d'en-tête pour la classe Sync dans le répertoire ScriptsClass, responsable de la synchronisation des fichiers.
    background.h : Fichier d'en-tête pour la classe Background qui gère les opérations en arrière-plan.
    ScriptsClass/commandstruct.h : Fichier d'en-tête contenant la structure de commande utilisée dans la classe Sync.

Formulaires (FORMS) :

    mainwindow.ui : Fichier de formulaire pour la fenêtre principale de l'application.
    configdialog.ui : Fichier de formulaire pour la boîte de dialogue de configuration.

Autres fichiers :

    Qt-AES-master/LICENSE : Fichier de licence provenant du projet Qt-AES utilisé dans le projet.
    resources.qrc : Fichier de ressources qui peut inclure des fichiers tels que des images ou des fichiers de traduction.

Cette structure de projet suit une organisation modulaire, où les fichiers sources et d'en-tête sont regroupés en fonction de leur fonctionnalité. 

## mainwindow.cpp

Cette classe est responsable de la gestion de la fenêtre principale de l'application. Voici une explication de chaque fonction :

```
MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow),
    settings("settings.ini", QSettings::IniFormat, this),
    run(this),
    multi(this)
{
    // Initialisation de l'interface utilisateur
    ui->setupUi(this);

    // ...

    // Démarrer le thread multi
    multi.start();

    // Vérifier si la clé de registre existe
    checkRegistryKey();

    // ...

    // Afficher les valeurs de configuration
    qDebug() << "Hostname du serveur : " + settings.value(REGISTRY_KEY_SERVER_HOSTNAME).toString();
    qDebug() << "Nom d'utilisateur pour accéder au serveur : " + settings.value(REGISTRY_KEY_SERVER_USERNAME).toString();
    qDebug() << "Chemin du répertoire de sauvegarde quotidienne sur le serveur : " + settings.value(REGISTRY_KEY_SERVER_DAILY).toString();
    qDebug() << "Chemin du répertoire de sauvegarde sur le serveur : " + settings.value(REGISTRY_KEY_CLIENT_BACKUP).toString();
    qDebug() << "Chemin du répertoire de sauvegarde du client : " + settings.value(REGISTRY_KEY_CLIENT_BACKUP).toString();
    qDebug() << "Clé SSH privée du client : " + settings.value(REGISTRY_KEY_CLIENT_KEY).toString();
}
```

La fonction MainWindow::MainWindow(QWidget *parent) est le constructeur de la classe MainWindow. Elle est appelée lorsqu'une instance de MainWindow est créée. La fonction initialise l'interface utilisateur, démarre un thread nommé multi, vérifie si une clé de registre existe à l'aide de la fonction checkRegistryKey(), et affiche les valeurs de configuration à l'aide de qDebug().

```
void MainWindow::closeEvent(QCloseEvent *event)
{
    event->ignore();

    run.internalTimer->stop(); // Arrête le timer interne

    multi.exit(0); // Quitte le thread multi

    ui->centralWidget->hide(); // Cache le widget central

    // Initialise et démarre l'animation de fermeture
    exitAnimation->start(QPropertyAnimation::DeleteWhenStopped);
}
```

La fonction MainWindow::closeEvent(QCloseEvent *event) est un gestionnaire d'événements qui est appelé lors de la fermeture de la fenêtre principale. Elle arrête un timer interne, quitte le thread multi, cache le widget central et démarre une animation de fermeture.

```
void MainWindow::checkRegistryKey()
{
    if (settings.value(REGISTRY_KEY_EXISTS) != TRUE) {
        configureSoftware(); // Configure le logiciel si la clé de registre n'existe pas
    }
}
```

La fonction MainWindow::checkRegistryKey() vérifie si une clé de registre spécifique (REGISTRY_KEY_EXISTS) existe dans les paramètres de l'application. Si la clé de registre n'existe pas, elle appelle la fonction configureSoftware() pour configurer le logiciel.

```
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
```

Cette fonction est appelée lorsque le bouton "resetButton" est cliqué. Elle affiche une boîte de dialogue de confirmation demandant à l'utilisateur s'il souhaite réinitialiser les données. Si l'utilisateur confirme, la valeur de la clé de registre "REGISTRY_KEY_EXISTS" est définie sur FALSE à l'aide de la fonction setValue de l'objet settings. Ensuite, une boîte de dialogue d'information est affichée, et la fonction qDebug() est utilisée pour afficher le contenu de la clé de registre "REGISTRY_KEY_EXISTS" dans la console de débogage. Enfin, la constante RESTART_PROGRAM est utilisée pour redémarrer le programme.

```
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
```

Cette fonction est similaire à la précédente, mais elle effectue une réinitialisation complète des données en appelant la fonction clear() de l'objet settings. Cela efface toutes les données de configuration enregistrées. Ensuite, une boîte de dialogue d'information est affichée, et la constante RESTART_PROGRAM est utilisée pour redémarrer le programme.

```
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
            (settings.value(REGISTRY_KEY_SERVER_HOSTNAME.

```

La fonction configureSoftware() est appelée pour configurer le logiciel en fonction des valeurs de configuration. Elle affiche une boîte de dialogue de confirmation pour vérifier si c'est la première fois que le programme est exécuté. Si l'utilisateur choisit d'annuler, la fenêtre principale se ferme. Sinon, si une installation précédente est détectée et récupérable, une boîte de dialogue est affichée pour demander si l'utilisateur souhaite récupérer les données. Selon la réponse de l'utilisateur, une instance de la classe configDialog est créée avec les valeurs de configuration appropriées et affichée. Ensuite, la fonction se connecte aux signaux rejected() et accepted() de la boîte de dialogue pour exécuter respectivement les slots close() et connectionTest().

La fonction connectionTest() est appelée lorsque l'utilisateur accepte la configuration dans la boîte de dialogue. Elle met à jour les paramètres de configuration avec les valeurs entrées par l'utilisateur dans la boîte de dialogue configDialog. Ensuite, elle affiche une boîte de dialogue d'information pour indiquer que les données ont été enregistrées avec succès. Elle crée une instance de la classe sync pour générer une commande de synchronisation en fonction du système d'exploitation. La commande de synchronisation est configurée avec les valeurs de configuration appropriées. Enfin, la fonction met à jour la variable command avec la commande générée et enregistre la commande dans les paramètres de configuration.

La fonction modifyBackup() est appelée pour modifier la commande de synchronisation en fonction des nouvelles valeurs de configuration. Elle crée une instance de la classe sync et configure la plateforme en fonction du système d'exploitation. Ensuite, elle récupère les valeurs des paramètres de configuration du serveur et du client, et met à jour la commande de synchronisation avec ces valeurs. Enfin, la fonction met à jour la variable command avec la nouvelle commande générée.

Les autres fonctions telles que getNewString(), getOsName(), on_CServerHostButton_clicked(), on_CServerUserButton_clicked(), etc., sont des gestionnaires de clic pour les boutons et effectuent des opérations similaires. Elles récupèrent les nouvelles valeurs des paramètres de configuration à partir de boîtes de dialogue d'entrée de texte, les enregistrent dans les paramètres de configuration et appellent la fonction modifyBackup() pour mettre à jour la commande de synchronisation.

En résumé, ce code configure le logiciel en fonction des valeurs de configuration fournies par l'utilisateur, génère une commande de synchronisation en fonction de ces valeurs et permet à l'utilisateur d'effectuer des actions telles que la synchronisation, la visualisation des journaux, etc.


## main.cpp

Le fichier main.cpp est le point d'entrée du programme. Il inclut le fichier d'en-tête "mainwindow.h" qui contient la déclaration de la classe MainWindow.

La fonction principale du projet, main(), est définie. Elle prend deux arguments : argc qui représente le nombre d'arguments passés en ligne de commande et argv qui est un tableau de pointeurs vers des chaînes de caractères contenant les arguments eux-mêmes.

Dans la fonction main(), une instance de la classe QApplication appelée a est créée. QApplication est la classe de base pour toutes les applications Qt. Elle gère les ressources de l'application et est nécessaire pour exécuter l'interface graphique.

Ensuite, une instance de la classe MainWindow appelée w est créée. MainWindow est la fenêtre principale de l'application. Cette fenêtre sera affichée à l'écran à l'aide de la méthode w.show().

Enfin, la fonction a.exec() est appelée pour exécuter la boucle principale de l'application. Cette boucle gère les événements et assure le bon fonctionnement de l'interface utilisateur. L'exécution de la boucle se poursuit tant que l'application est active. Lorsque l'utilisateur ferme la fenêtre principale, la boucle se termine et la fonction main() retourne, mettant ainsi fin au programme.

En résumé, le fichier main.cpp initialise l'application, crée et affiche la fenêtre principale, puis exécute la boucle principale de l'application jusqu'à ce que l'application se termine.

## filecontentdialog.cpp

La classe FileContentDialog et ses fonctions dans le fichier filecontentdialog.cpp sont utilisées pour afficher le contenu d'un fichier dans une boîte de dialogue. 

La fonction FileContentDialog::FileContentDialog(QWidget* parent) est le constructeur de la classe. Elle crée et configure les widgets nécessaires pour afficher le contenu du fichier dans la boîte de dialogue. Elle initialise également le timer pour les mises à jour périodiques du contenu.

La fonction FileContentDialog::~FileContentDialog() est le destructeur de la classe. Elle supprime le timer créé dynamiquement pour arrêter les mises à jour périodiques lorsque la boîte de dialogue est détruite.

La fonction FileContentDialog::updateFileContent() est appelée périodiquement par le timer pour mettre à jour le contenu du fichier affiché dans le QTextEdit. Elle lit le contenu du fichier spécifié, l'affiche dans le QTextEdit, et conserve la position de défilement vertical précédente pour éviter que la position de défilement ne change lors de la mise à jour du contenu. Si le fichier ne peut pas être ouvert, un message d'erreur est affiché dans la console de débogage.

Ces fonctions travaillent ensemble pour créer une boîte de dialogue qui affiche le contenu d'un fichier et le met à jour périodiquement. Cela peut être utile pour surveiller en temps réel les modifications d'un fichier ou afficher des journaux en direct.

## configdialog.cpp

```
#include "configdialog.h"

ConfigDialog::ConfigDialog(QWidget* parent)
    : QDialog(parent)
{
    // Crée les widgets et configure l'interface utilisateur
    layout = new QVBoxLayout(this);
    label = new QLabel("Configuration Options", this);
    button = new QPushButton("Save", this);
    layout->addWidget(label);
    layout->addWidget(button);
    setLayout(layout);
    setWindowTitle("Configuration");

    // Connecte le signal clicked() du bouton à la fonction saveConfig()
    connect(button, &QPushButton::clicked, this, &ConfigDialog::saveConfig);
}

ConfigDialog::~ConfigDialog()
{
    // Rien à faire ici car les widgets sont automatiquement supprimés lorsque le parent est détruit
}

void ConfigDialog::saveConfig()
{
    // Effectue les opérations de sauvegarde de la configuration
    // ...
    qDebug() << "Configuration saved!";
    accept(); // Ferme la boîte de dialogue en indiquant un résultat positif (accepté)
}

```

La fonction ConfigDialog::ConfigDialog(QWidget* parent) est le constructeur de la classe. Elle crée et configure les widgets nécessaires pour afficher les options de configuration dans la boîte de dialogue. Elle définit également le titre de la boîte de dialogue et connecte le signal clicked() du bouton à la fonction saveConfig().

La fonction ConfigDialog::~ConfigDialog() est le destructeur de la classe. Comme les widgets sont automatiquement supprimés lorsque le parent est détruit, il n'y a rien à faire ici.

La fonction ConfigDialog::saveConfig() est appelée lorsque le bouton "Save" est cliqué. Elle effectue les opérations nécessaires pour sauvegarder la configuration (qui ne sont pas détaillées ici) et affiche un message de débogage. Enfin, elle appelle la fonction accept() pour fermer la boîte de dialogue en indiquant un résultat positif (accepté).

Ces fonctions travaillent ensemble pour créer une boîte de dialogue de configuration qui permet à l'utilisateur de modifier les options et de sauvegarder les changements. Lorsque le bouton "Save" est cliqué, la configuration est sauvegardée et la boîte de dialogue est fermée.

## background.cpp

```
#include "background.h"

background::background(QWidget *mbparent, QObject *parent) : QObject(parent), temp(mbparent)
{
    // Crée un QTimer interne et le configure pour déclencher un signal toutes les 3000 millisecondes (3 secondes)
    internalTimer = new QTimer(parent);
    internalTimer->setInterval(3000);

    // Connecte le signal timeout() du QTimer à la slot verify() de la classe background
    connect(internalTimer, SIGNAL(timeout()), this, SLOT(verify()));
}

void background::startRunning()
{
    // Démarre le QTimer interne
    internalTimer->start();
}

void background::verify()
{
    // Vérifie l'heure actuelle
    int currentHour = QTime::currentTime().hour();

    // Si l'heure actuelle est 15 heures
    if (currentHour == 15)
    {
        // Vérifie si la tâche a déjà été effectuée
        if (done == false)
        {
            done = true;
            QString command = *commandPtr;
            qDebug() << "command: " << command;

            // Exécute une commande système donnée
            int work = system(command.toStdString().c_str());
            (void)work;
        }
    }
    else
    {
        // Réinitialise la variable done
        done = false;
        return;
    }
}
```

La fonction background::background(QWidget *mbparent, QObject *parent) est le constructeur de la classe. Elle crée un QTimer interne et le configure pour déclencher le signal timeout() toutes les 3000 millisecondes (3 secondes). Elle connecte ensuite ce signal à la slot verify() de la classe background.

La fonction background::startRunning() démarre le QTimer interne, ce qui déclenchera périodiquement le signal timeout().

La fonction background::verify() est la slot qui est exécutée lorsque le signal timeout() est émis par le QTimer interne. Elle vérifie l'heure actuelle en utilisant la fonction QTime::currentTime().hour(). Si l'heure actuelle est 15 heures et que la tâche n'a pas encore été effectuée (done == false), elle exécute une commande système spécifiée par la variable commandPtr. La variable done est ensuite mise à true pour indiquer que la tâche a été effectuée. Si l'heure actuelle n'est pas 15 heures, la variable done est réinitialisée à false.

Ces fonctions travaillent ensemble pour exécuter une tâche spécifique à une heure donnée (15 heures dans cet exemple) en utilisant un QTimer interne pour vérifier l'heure actuelle à intervalles réguliers. Lorsque l'heure correspond, la tâche est exécutée.


## sync.cpp

```
#include "sync.h"
#include "commandstruct.h"

sync::sync(sync::os platform, QObject *parent) : QObject(parent), Platform(platform)
{
    // Constructeur de la classe sync, prend une plateforme (os) en argument
}

void sync::setServerHostname(QString value)
{
    // Définit le nom d'hôte du serveur
    serverHostname = value;
    emit serverHostnameChanged();
}

void sync::setServerUser(QString value)
{
    // Définit l'utilisateur du serveur
    serverUser = value;
    emit serverUserChanged();
}

void sync::setServerDaily(QString value)
{
    // Définit le répertoire quotidien du serveur
    serverDaily = value;
    emit serverDailyChanged();
}

void sync::setClientDir(QString value)
{
    // Définit le répertoire client
    clientDir = value;
    emit clientDirChanged();
}

void sync::setClientKey(QString value)
{
    // Définit la clé client
    clientKey = value;
    emit clientKeyChanged();
}

QString sync::generateCommand()
{
    // Génère une commande en fonction de la plateforme (os)

    QString command;

    switch (Platform)
    {
        case linuxos: // même pour linux et mac
        case macosx:
            // Construit la commande à partir de commandStruct et des valeurs actuelles
            command = (commandStruct::first + clientKey +
                       commandStruct::third + clientDir +
                       commandStruct::fifth + serverUser +
                       commandStruct::seventh + serverHostname +
                       commandStruct::ninth + serverDaily +
                       commandStruct::eleventh + clientDir +
                       commandStruct::thirteenth);

            qDebug() << command;
            break;
    }

    return command;
}
```

La fonction sync::sync(sync::os platform, QObject *parent) est le constructeur de la classe sync. Elle prend une plateforme (os) en argument et initialise l'objet avec cette valeur.

Les fonctions sync::setServerHostname(QString value), sync::setServerUser(QString value), sync::setServerDaily(QString value), sync::setClientDir(QString value) et sync::setClientKey(QString value) sont des fonctions qui définissent respectivement le nom d'hôte du serveur, l'utilisateur du serveur, le répertoire quotidien du serveur, le répertoire client et la clé client. Chaque fonction émet un signal correspondant au changement de valeur de la propriété.

La fonction sync::generateCommand() génère une commande en fonction de la plateforme (os) actuelle. Dans le cas des plateformes Linux (linuxos) et macOS (macosx), elle construit une commande en utilisant des valeurs pré-définies à partir de commandStruct et les valeurs actuelles des propriétés. La commande générée est ensuite retournée.

Ces fonctions permettent de configurer les valeurs nécessaires à la génération d'une commande de synchronisation. La fonction generateCommand() génère ensuite la commande en fonction de la plateforme spécifiée et des valeurs actuelles, puis la retourne.

## mainwindow.ui 

Le fichier mainwindow.ui est un fichier de description d'interface utilisateur au format XML, utilisé généralement avec le framework Qt pour créer des interfaces graphiques. Il contient la structure et les propriétés des différents éléments de l'interface, tels que les fenêtres, les boutons, les boîtes de groupe, etc.

Ce fichier définit donc une fenêtre principale (MainWindow) contenant plusieurs éléments, tels que des boîtes de groupe (QGroupBox) pour les paramètres du serveur, les paramètres du client et les opérations. Les éléments sont organisés dans une grille (QGridLayout). Chaque boîte de groupe contient des widgets, tels que des boutons (QPushButton), qui sont utilisés pour interagir avec l'interface utilisateur.

Voici une description des différents boutons présents dans le code XML de mainwindow.ui :

- Change Server Hostname Button (CServerHostButton) : Ce bouton permet de changer le nom d'hôte du serveur.
- Change Server Username Button (CServerUserButton) : Ce bouton permet de changer le nom d'utilisateur du serveur.
- Change Server DailyBackup Path Button (CServerDailyButton) : Ce bouton permet de changer le chemin de sauvegarde quotidienne du serveur.
- Change Server Backup Path Button (CServerBackupButton) : Ce bouton permet de changer le chemin de sauvegarde du serveur.
- Change Client SSH Key Path Button (changeSSHButton) : Ce bouton permet de changer le chemin de la clé SSH du client.
- Change Client Backup Path Button (changeBackupButton) : Ce bouton permet de changer le chemin de sauvegarde du client.
- Log Button (CLogButton) : Ce bouton permet d'afficher les journaux.
- Hard Reset Button (hResetButton) : Ce bouton effectue une réinitialisation complète (hard reset).
- Soft Reset Button (resetButton) : Ce bouton effectue une réinitialisation douce (soft reset).
- Manual Sync Button (CSyncButton) : Ce bouton déclenche une synchronisation manuelle.
- Search Button (CSearchButton) : Ce bouton permet d'effectuer une recherche.

Ces boutons sont utilisés dans une interface graphique (MainWindow) qui comprend des groupes (GroupBox) pour les paramètres du serveur (serverSettingsGroup) et les paramètres du client (clientSettingsGroup). Il y a également un groupe pour les opérations (resetSettingsGroup) qui comprend les boutons de réinitialisation, de synchronisation et de journalisation.

## configdialog.ui

Le fichier configdialog.ui est un fichier XML qui décrit l'interface utilisateur de la boîte de dialogue de configuration. Voici une description des éléments principaux présents dans ce fichier :

- Le widget principal est un QDialog nommé "configDialog" avec un titre "Connect" et une géométrie de 380x463 pixels.

- Il utilise une feuille de style CSS pour définir l'apparence des boutons QPushButton et des groupes QGroupBox.

- Le widget contient deux groupes : "SERVER" et "CLIENT". Chaque groupe est un QGroupBox avec un titre et un agencement en QGridLayout.

- Dans le groupe "SERVER", il y a des QLabel pour afficher les étiquettes "Hostname" et "Username", et des QLineEdit pour entrer les valeurs correspondantes.

- Dans le groupe "CLIENT", il y a des QLabel pour afficher les étiquettes "Backup Path" et "SSH Key Path", et des QLineEdit pour entrer les valeurs correspondantes. Il y a également des QPushButton pour ouvrir une boîte de dialogue de recherche de fichiers.

- En bas de la boîte de dialogue, il y a un QDialogButtonBox nommé "buttonBox" avec des boutons "OK" et "Cancel".

- Les signaux accepted() et rejected() du buttonBox sont connectés aux slots accept() et reject() de la classe configDialog.

Ce fichier décrit l'interface utilisateur de la boîte de dialogue de configuration, permettant à l'utilisateur d'entrer les valeurs nécessaires pour la synchronisation.