/**
 * @file mainwindow.cpp
 * @brief MainWindow GUI class
 *
 * @author Carsten Bruns (carst.bruns@gmx.de)
 */

#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "headerparser.h"
#include "newobjectdialog.h"
#include "configobjectdialog.h"
#include "acoparametersdialog.h"
#include <iostream>
#include <QProcessEnvironment>
#include <QMessageBox>
#include <QFileDialog>
#include <QInputDialog>
#include "consoleredirector.h"
#include "targetconfigdialog.h"
#include "developertools.h"
#include "datalogger.h"
#include "pinoverview.h"
#include "automaticcoreassigner.h"

using namespace std;

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow),
    dataLogger(NULL),
    dataLoggerSaved(true),
    bitfileGenerated(false),
    outputGenerator(NULL),
    otherModel(),
    expertMenu(NULL)
{
    ui->setupUi(this);
    new ConsoleRedirector(cout, ui->textBrowser, "Black");
    new ConsoleRedirector(cerr, ui->textBrowser, "Red");

    connect(ui->actionClose, SIGNAL(triggered()), this, SLOT(close()));
    connect(ui->actionNewObject, SIGNAL(triggered()), this, SLOT(newObject()));
    connect(ui->actionPasteModule, SIGNAL(triggered()), this, SLOT(pasteModule()));
    connect(ui->actionTarget, SIGNAL(triggered()), this, SLOT(targetConfig()));
    connect(ui->actionGenerate, SIGNAL(triggered()), this, SLOT(generate()));
    connect(ui->actionSynthesize, SIGNAL(triggered()), this, SLOT(synthesize()));
    connect(ui->actionSynthesizeOnly, SIGNAL(triggered()), this, SLOT(synthesizeOnly()));
    connect(ui->actionFlash, SIGNAL(triggered()), this, SLOT(flash()));
    connect(ui->actionStop, SIGNAL(triggered()), this, SLOT(stopTask()));

    connect(ui->actionNew, SIGNAL(triggered()), this, SLOT(newLogger()));
    connect(ui->actionOpen, SIGNAL(triggered()), this, SLOT(open()));
    connect(ui->actionSave, SIGNAL(triggered()), this, SLOT(save()));
    connect(ui->actionSaveAs, SIGNAL(triggered()), this, SLOT(saveAs()));

    connect(ui->actionPinOverview, SIGNAL(triggered()), this, SLOT(pinOverview()));
    connect(ui->actionDeveloperTools, SIGNAL(triggered()), this, SLOT(developerTools()));

    ui->listView->setModel(&otherModel);
    connect(ui->listView, SIGNAL(doubleClicked(QModelIndex)), this, SLOT(otherObjectConfig(QModelIndex)));
    ui->listView->setContextMenuPolicy(Qt::CustomContextMenu);
    connect(ui->listView, SIGNAL(customContextMenuRequested(QPoint)), this, SLOT(otherObjectMenu(QPoint)));
}

void MainWindow::show() {
    QMainWindow::show();

    datastreamView = new DatastreamView(ui->graphicsView);
    connect(datastreamView, SIGNAL(requestConfigDialog(CObject&)), this, SLOT(showConfigDialog(CObject&)));
    connect(datastreamView, SIGNAL(pasteModule(QPoint)), this, SLOT(pasteModule(QPoint)));
    connect(datastreamView, SIGNAL(copyObject(std::string)), this, SLOT(copyObject(std::string)));

    newLogger();

    string spmc_root = QProcessEnvironment::systemEnvironment().value("SPARTANMC_ROOT").toStdString();
    if (spmc_root.empty()) {
        cerr << tr("SpartanMC root not set. Please set the $SPARTANMC_ROOT environment variable.").toStdString() << endl;
    }
}

MainWindow::~MainWindow()
{
    delete datastreamView;
    delete ui;
}

void MainWindow::newObject() {
    NewObjectDialog dialog(this, NULL);
    if (dialog.exec() == QDialog::Accepted) {
        DataTypeStruct* dataType = dialog.getSelectedDataType();
        if (dataType != NULL) {
            dataLogger->newObject(dataType);
        }
     }
}

void MainWindow::newObject(DataTypeStruct* type, ConfigObjectDialog* confDialog) {
    NewObjectDialog dialog(this, type);
    if (dialog.exec() == QDialog::Accepted) {
        DataTypeStruct* dataType = dialog.getSelectedDataType();
        if (dataType != NULL) {
            confDialog->objectCreationFinished(dataLogger->newObject(dataType));
        } else {
            confDialog->objectCreationFinished("");
        }
     } else {
        confDialog->objectCreationFinished("");
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

void MainWindow::otherObjectCopy() {
    QModelIndex index = ui->listView->selectionModel()->selectedIndexes().first();

    string objectName = index.data(Qt::UserRole + 1).toString().toStdString();
    vector<CObject*> objects = dataLogger->getOtherObjects();
    vector<CObject*>::iterator i;
    for (i = objects.begin(); i != objects.end(); i++) {
        if ((*i)->getName().compare(objectName) == 0)
            break;
    }

    QString s;
    QXmlStreamWriter stream(&s);
    stream << **i;

    copyObject(s.toStdString());
}

void MainWindow::otherObjectMenu(QPoint pos) {
    if (!ui->listView->selectionModel()->selectedIndexes().isEmpty()) {
        QMenu *menu = new QMenu(ui->listView);
        QAction* action = new QAction(tr("Copy object"), this);
        connect(action, SIGNAL(triggered(bool)), this, SLOT(otherObjectCopy()));
        menu->addAction(action);
        menu->popup(ui->listView->mapToGlobal(pos));
    }
}

void MainWindow::showConfigDialog(CObject& object) {
    ConfigObjectDialog dialog(this, &object, dataLogger);
    QObject::connect(&dialog, SIGNAL(newObjectRequest(DataTypeStruct*,ConfigObjectDialog*)), this, SLOT(newObject(DataTypeStruct*, ConfigObjectDialog*)));
    if (dialog.exec() == ConfigObjectDialog::DeleteResult) {
        dataLogger->deleteObject(&object);
    }
    dataLoggerChanged();
}

void MainWindow::developerTools() {
    DeveloperTools dialog(this);
    dialog.exec();
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
    ui->actionSynthesizeOnly->setEnabled(false);
    ui->actionStop->setEnabled(true);

    clearErrorList();

    if (!dataLogger->isExpertMode()) {
        coreAssigner();
    }

    outputGenerator = new OutputGenerator(dataLogger, QFileInfo(dataLoggerPath.c_str()).dir().absolutePath().toStdString());
    connect(outputGenerator, SIGNAL(finished(bool, bool)), this, SLOT(outputGeneratorFinished(bool, bool)));
    connect(outputGenerator, SIGNAL(errorFound(std::string)), this, SLOT(addErrorToList(std::string)));
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

void MainWindow::synthesizeOnly() {
    if (outputGenerator == NULL) {
        if (newOutputGenerator()) {
            outputGenerator->synthesizeOnly();

            bitfileGenerated = true;
        }
    }
}

void MainWindow::flash() {
    if (outputGenerator == NULL) {
        if (!bitfileGenerated) {
            QMessageBox dialog(QMessageBox::Warning,
                               tr("Bitfile not up-to-date"),
                               tr("The Bitfile is not up-to-date.\nShould the datalogger be synthesized before flashing?"),
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

void MainWindow::outputGeneratorFinished(bool errorOccured, bool timingError) {
    outputGenerator->deleteLater();
    outputGenerator = NULL;

    ui->actionSynthesize->setEnabled(true);
    ui->actionGenerate->setEnabled(true);
    ui->actionFlash->setEnabled(true);
    ui->actionSynthesizeOnly->setEnabled(true);
    ui->actionStop->setEnabled(false);

    if (errorOccured) {
        cerr << tr("Error in task processing.").toStdString() << endl;
    } else {
        cout << tr("Task successfully completed.").toStdString() << endl;
    }
    if (timingError) {
        cerr << tr("The chosen clock frequency is too high. Please choose a lower one in \"File->Traget platform\".").toStdString() << endl;
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
                           tr("Datalogger not saved"),
                           tr("The Datalogger is not saved. Should it be saved before proceeding?"),
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
        clearErrorList();

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
        connect(dataLogger, SIGNAL(viasChanged()), this, SLOT(dataLoggerChanged()));
        connect(dataLogger, SIGNAL(datastreamModulesChanged()), this, SLOT(dataLoggerChanged()));
        connect(dataLogger, SIGNAL(otherModulesChanged()), this, SLOT(dataLoggerChanged()));

        connect(dataLogger, SIGNAL(expertModeChanged(bool)), this, SLOT(expertModeChanged(bool)));
        expertModeChanged(dataLogger->isExpertMode());

        datastreamView->setDataLogger(dataLogger);
        datastreamView->redrawModules();
        otherModel.setDataLogger(dataLogger);
    }
}

void MainWindow::open() {
    newLogger();

    QString fileName = QFileDialog::getOpenFileName(this,
                                                    tr("Open"),
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
            cout << tr("Loaded file %1!").arg(dataLoggerPath.c_str()).toStdString() << endl;

            if (dataLogger->getDefinitionsUpdated()) {
                QString changedText = tr("Some parameters of the following modules changed:")
                        + QString(dataLogger->getDefinitionsUpdatedModules().c_str()) + "\n\n"
                        + tr("Please check all parameters of these modules!");
                cout << changedText.toStdString() << endl;
                QMessageBox dialog(QMessageBox::Warning,
                                   tr("changed parameters"),
                                   changedText,
                                   QMessageBox::Ok);
                dialog.exec();
                dataLogger->setDefinitionsUpdated(false);
            }
        } else {
            cerr << tr("Error: could not open file.").toStdString() << endl;
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
        cout << tr("Datalogger saved in %1!").arg(dataLoggerPath.c_str()).toStdString() << endl;
    } else {
        cerr << tr("Error: could not open destination file.").toStdString() << endl;
    }
}

void MainWindow::saveAs() {
    QString fileName = QFileDialog::getSaveFileName(this,
                                                    tr("Save as"),
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

void MainWindow::pinOverview() {
    PinOverview dialog(this, dataLogger, dataLoggerPath);
    dialog.exec();
}

void MainWindow::copyObject(std::string objectDescription) {
    clipboardObjectDescription = objectDescription;
    ui->actionPasteModule->setEnabled(true);
    datastreamView->setPastePossible(true);
}

void MainWindow::pasteModule() {
    pasteModule(QPoint(0,0));
}

void MainWindow::pasteModule(QPoint pos) {
    QString s(clipboardObjectDescription.c_str());
    QXmlStreamReader object(s);
    object.readNextStartElement();

    bool dataStreamObject = false;
    DataTypeStruct* type = DataTypeStruct::getType(object.attributes().value("type").toString().toStdString());
    if (type->hasPrefix("device_") || type->hasPrefix("dm_") || type->hasPrefix("sink_")) {
        dataStreamObject = true;
    }
    QString name = dataLogger->findObjectName(dataStreamObject, type).c_str();

    bool ok;
    QInputDialog* inputDialog = new QInputDialog();
    inputDialog->setOptions(QInputDialog::NoButtons);

    name =  inputDialog->getText(NULL, tr("Paste module"), tr("Object name"), QLineEdit::Normal, name, &ok);

    if (ok && !name.isEmpty()) {
        dataLogger->addObject(name.toStdString(), dataStreamObject, object, pos);
    }
}

void MainWindow::clearErrorList() {
    ui->errorList->clear();
}

void MainWindow::addErrorToList(std::string message) {
    ui->errorList->addItem(message.c_str());
}

void MainWindow::coreAssigner() {
    AutomaticCoreAssigner* aco = dataLogger->getAutomaticCoreAssigner();
    connect(aco, SIGNAL(warningFound(std::string)), this, SLOT(addErrorToList(std::string)));
    aco->assignCores(dataLogger);
}

void MainWindow::coreAssignerParameters() {
    AcoParametersDialog acoDialog(dataLogger->getAutomaticCoreAssigner());
    acoDialog.exec();
}

void MainWindow::expertModeChanged(bool expertMode) {
    if (expertMode) {
        if (expertMenu == NULL) {
            expertMenu = ui->menuBar->addMenu(tr("Expert"));
            QAction* coreAssigner = expertMenu->addAction(tr("run CoreAssigner"));
            connect(coreAssigner, SIGNAL(triggered()), this, SLOT(coreAssigner()));
            QAction* coreAssignerParameters = expertMenu->addAction(tr("CoreAssigner parameters"));
            connect(coreAssignerParameters, SIGNAL(triggered()), this, SLOT(coreAssignerParameters()));
        }
    } else {
        if (expertMenu != NULL) {
            delete expertMenu;
            expertMenu = NULL;
        }
    }
}

void MainWindow::stopTask() {
    if (outputGenerator != NULL) {
        outputGenerator->stopTasks();
    }
}
