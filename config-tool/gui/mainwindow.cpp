#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "headerparser.h"
#include "newobjectdialog.h"
#include "configobjectdialog.h"
#include <iostream>
#include <QProcessEnvironment>
#include <QMessageBox>
#include <QFileDialog>
#include "consoleredirector.h"
#include "targetconfigdialog.h"
#include "datalogger.h"

using namespace std;

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow),
    dataLogger(NULL),
    dataLoggerSaved(true),
    bitfileGenerated(false),
    outputGenerator(NULL),
    otherModel()
{
    ui->setupUi(this);
    new ConsoleRedirector(cout, ui->textBrowser, "Black");
    new ConsoleRedirector(cerr, ui->textBrowser, "Red");

    connect(ui->actionClose, SIGNAL(triggered()), this, SLOT(close()));
    connect(ui->actionNewObject, SIGNAL(triggered()), this, SLOT(newObject()));
    connect(ui->actionTarget, SIGNAL(triggered()), this, SLOT(targetConfig()));
    connect(ui->actionGenerate, SIGNAL(triggered()), this, SLOT(generate()));
    connect(ui->actionSynthesize, SIGNAL(triggered()), this, SLOT(synthesize()));
    connect(ui->actionFlash, SIGNAL(triggered()), this, SLOT(flash()));

    connect(ui->actionNew, SIGNAL(triggered()), this, SLOT(newLogger()));
    connect(ui->actionOpen, SIGNAL(triggered()), this, SLOT(open()));
    connect(ui->actionSave, SIGNAL(triggered()), this, SLOT(save()));
    connect(ui->actionSaveAs, SIGNAL(triggered()), this, SLOT(saveAs()));

    ui->listView->setModel(&otherModel);
    connect(ui->listView, SIGNAL(doubleClicked(QModelIndex)), this, SLOT(otherObjectConfig(QModelIndex)));
}

void MainWindow::show() {
    QMainWindow::show();

    datastreamView = new DatastreamView(ui->graphicsView);
    connect(datastreamView, SIGNAL(requestConfigDialog(CObject&)), this, SLOT(showConfigDialog(CObject&)));

    newLogger();

    string spmc_root = QProcessEnvironment::systemEnvironment().value("SPARTANMC_ROOT").toStdString();
    if (spmc_root.empty()) {
        cerr << "SpartanMC root nicht gesetzt. Bitte setzen sie die $SPARTANMC_ROOT Umgebungsvariable." << endl;
    }
}

MainWindow::~MainWindow()
{
    delete datastreamView;
    delete ui;
}

void MainWindow::newObject() {
    NewObjectDialog dialog(this);
    if (dialog.exec() == QDialog::Accepted) {
        DataTypeStruct* dataType = dialog.getSelectedDataType();
        if (dataType != NULL) {
            dataLogger->newObject(dataType);
        }
     }
}

void MainWindow::otherObjectConfig(QModelIndex index) {
    string objectName = index.data(Qt::UserRole + 1).toString().toStdString();
    vector<CObject*> objects = dataLogger->getOtherObjects();
    vector<CObject*>::iterator i;
    for (i = objects.begin(); i != objects.end(); i++) {
        if ((*i)->getName().compare(objectName) == 0)
            break;
    }

    showConfigDialog(**i);
}

void MainWindow::showConfigDialog(CObject& object) {
    ConfigObjectDialog dialog(this, &object, dataLogger);
    if (dialog.exec() == ConfigObjectDialog::DeleteResult) {
        dataLogger->deleteObject(&object);
    }
    dataLoggerChanged();
}

void MainWindow::targetConfig() {
    TargetConfigDialog dialog(this, dataLogger);
    dialog.exec();
    dataLoggerChanged();
}

bool MainWindow::newOutputGenerator() {
    if (dataLoggerPath.empty()) {
        saveAs();
        if (dataLoggerPath.empty())
            return false;
    }

    ui->actionSynthesize->setEnabled(false);
    ui->actionGenerate->setEnabled(false);
    ui->actionFlash->setEnabled(false);

    outputGenerator = new OutputGenerator(dataLogger, QFileInfo(dataLoggerPath.c_str()).dir().absolutePath().toStdString());
    connect(outputGenerator, SIGNAL(finished(bool)), this, SLOT(outputGeneratorFinished(bool)));
    return true;
}

void MainWindow::generate() {
    if (outputGenerator == NULL) {
        if (newOutputGenerator())
            outputGenerator->generateConfigFiles();
    }
}

void MainWindow::synthesize() {
    if (outputGenerator == NULL) {
        if (newOutputGenerator()) {
            outputGenerator->synthesizeSystem();

            bitfileGenerated = true;
        }
    }
}

void MainWindow::flash() {
    if (outputGenerator == NULL) {
        if (!bitfileGenerated) {
            QMessageBox dialog(QMessageBox::Warning,
                               "Bitfile nicht aktuell",
                               "Das Bitfile ist nicht aktuell.\nSoll der Datenlogger vor dem Flashen synthetisiert werden?",
                               QMessageBox::Yes | QMessageBox::No | QMessageBox::Abort);
            dialog.setWindowIcon(QIcon::fromTheme("media-flash"));
            dialog.exec();
            if (dialog.result() == QMessageBox::Abort)
                return;
            if (!newOutputGenerator())
                return;
            if (dialog.result() == QMessageBox::Yes) {
                outputGenerator->synthesizeSystem();

                bitfileGenerated = true;
            }
        } else {
            if (!newOutputGenerator())
                return;
        }
        outputGenerator->flash();
    }
}

void MainWindow::outputGeneratorFinished(bool errorOccured) {
    outputGenerator->deleteLater();
    outputGenerator = NULL;

    ui->actionSynthesize->setEnabled(true);
    ui->actionGenerate->setEnabled(true);
    ui->actionFlash->setEnabled(true);

    if (errorOccured) {
        cerr << "Fehler bei Taskbearbeitung." << endl;
    } else {
        cout << "Task erfolgreich abgeschlossen." << endl;
    }
}

void MainWindow::dataLoggerChanged() {
    dataLoggerSaved = false;
    bitfileGenerated = false;
    refreshWindowTitle();
}

void MainWindow::refreshWindowTitle() {
    string title;
    if (!dataLoggerSaved) {
        title += "*";
    }
    if (!dataLoggerPath.empty()) {
        title += dataLoggerPath + " - ";
    }
    title += "fpga-log";
    setWindowTitle(title.c_str());
}

bool MainWindow::checkAndAskSave() {
    if (!dataLoggerSaved) {
        QMessageBox dialog(QMessageBox::Warning,
                           "Datenlogger nicht gespeichert",
                           "Der Datenlogger wurde nicht gespeichert.\nSoll der Datenlogger zuerst gespeichert werden?",
                           QMessageBox::Yes | QMessageBox::No | QMessageBox::Abort);
        dialog.setWindowIcon(QIcon::fromTheme("document-save"));
        dialog.exec();
        if (dialog.result() == QMessageBox::Abort)
            return false;
        if (dialog.result() == QMessageBox::Yes) {
            save();
        }
    }
    return true;
}

void MainWindow::newLogger() {
    if (checkAndAskSave()) {
        dataLoggerPath = "";
        dataLoggerSaved = true;
        bitfileGenerated = false;
        refreshWindowTitle();

        if (dataLogger != NULL) {
            datastreamView->setDataLogger(NULL);
            delete dataLogger;
        }

        dataLogger = new DataLogger();

        connect(dataLogger, SIGNAL(connectionsChanged()), this, SLOT(dataLoggerChanged()));
        connect(dataLogger, SIGNAL(datastreamModulesChanged()), this, SLOT(dataLoggerChanged()));
        connect(dataLogger, SIGNAL(otherModulesChanged()), this, SLOT(dataLoggerChanged()));

        datastreamView->setDataLogger(dataLogger);
        datastreamView->redrawModules();
        otherModel.setDataLogger(dataLogger);
    }
}

void MainWindow::open() {
    newLogger();

    QString fileName = QFileDialog::getOpenFileName(this,
                                                    QString::fromUtf8("Öffnen"),
                                                    QDir::currentPath() +
                                                    "/../projects/",
                                                    "fpga-log XML (*.xml)");
    if (!fileName.isEmpty()) {
        setDataLoggerPath(fileName.toStdString());

        QFile file(fileName);
        file.open(QIODevice::ReadOnly);

        if (file.isOpen()) {
            QXmlStreamReader reader(&file);
            reader >> *dataLogger;

            file.close();
            dataLoggerSaved = true;
            refreshWindowTitle();
            cout << "Datei " + dataLoggerPath + " geladen!" << endl;

            if (dataLogger->getDefinitionsUpdated()) {
                QString changedText = QString::fromUtf8("Einige Parameter der folgenden Module haben sich geändert:")
                        + QString(dataLogger->getDefinitionsUpdatedModules().c_str())
                        + QString::fromUtf8("\n\nBitte überprüfen sie daher alle Parameter dieser Module!");
                cout << changedText.toStdString() << endl;
                QMessageBox dialog(QMessageBox::Warning,
                                   QString::fromUtf8("geänderte Parameter"),
                                   changedText,
                                   QMessageBox::Ok);
                dialog.exec();
                dataLogger->setDefinitionsUpdated(false);
            }
        } else {
            cerr << "Fehler: Datei konnte nicht geöffnet werden." << endl;
        }
    }
}

void MainWindow::save() {
    if (dataLoggerPath.empty()) {
        saveAs();
        return;
    }

    QFile file(dataLoggerPath.c_str());
    file.open(QIODevice::WriteOnly);

    if (file.isOpen()) {
        QXmlStreamWriter writer(&file);
        writer.setAutoFormatting(true);

        writer.writeStartDocument();
        writer << *dataLogger;
        writer.writeEndDocument();

        file.close();

        dataLoggerSaved = true;
        refreshWindowTitle();
        cout << "Datenlogger unter " + dataLoggerPath + " gespeichert!" << endl;
    } else {
        cerr << "Fehler: Zieldatei konnte nicht geschrieben werden." << endl;
    }
}

void MainWindow::saveAs() {
    QString fileName = QFileDialog::getSaveFileName(this,
                                                    "Speichern unter",
                                                    QDir::currentPath() +
                                                    "/../projects/",
                                                    "fpga-log XML (*.xml)");
    if (!fileName.isEmpty()) {
        setDataLoggerPath(fileName.toStdString());
        save();
    }
}

void MainWindow::closeEvent(QCloseEvent* event) {
    if (checkAndAskSave())
        QMainWindow::closeEvent(event);
    else
        event->ignore();
}

void MainWindow::reparseLocalHeaders(string path) {
    HeaderParser hp = HeaderParser(false);
    hp.addFolder(path + "/firmware/include", true);
    hp.parseFiles();
}

void MainWindow::setDataLoggerPath(string newPath) {
    if (dataLoggerPath.compare(newPath) != 0) {
        DataType::removeLocalTypes();

        dataLoggerPath = newPath;

        if (!newPath.empty()) {
            newPath = newPath.substr(0, newPath.rfind('/'));
            reparseLocalHeaders(newPath);
        }
    }
}
