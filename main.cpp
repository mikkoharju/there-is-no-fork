
#include <QQmlEngine>
#include <QQmlComponent>
#include <QQmlContext>
#include <QGuiApplication>

#include <QDebug>
#include <unistd.h>
#include <sys/wait.h>

#include <pthread.h>


class App : public QGuiApplication {
    Q_OBJECT
public:
    App() : QGuiApplication(argc, argv) {}
    static int argc;
    static char **argv;
    Q_INVOKABLE int exec() { return QGuiApplication::exec(); }
};

int App::argc = 0;
char **App::argv = 0;

int tst_staticData()
{
    QQmlEngine *e = new QQmlEngine;
    QQmlComponent *staticData = new QQmlComponent(e,e);
    staticData->setData("import QtQml 2.0\nQtObject{ Component.onCompleted: console.log(\"yay\") }", QUrl());
    qDebug() << staticData->errorString();
    QObject *o1 = staticData->create();
    o1->setParent(e);
    delete e;
    return 0;
}

int tst_fromFile(const QString &qmlfile = QString())
{
    QQmlEngine *e = new QQmlEngine;
    {
        QQmlComponent *fromFile = new QQmlComponent(e, e);
        fromFile->loadUrl(QUrl::fromLocalFile(qmlfile.isEmpty() ? "fork.qml" : qmlfile));
        qDebug() << fromFile->errorString();
        QObject *o2 = fromFile->create();
        o2->setParent(e);
    }
    delete e;
    return 0;
}

int tst_appInQml()
{
    QQmlEngine *e = new QQmlEngine;
    qmlRegisterType<App>("app", 1, 0, "GuiApplication");

    QQmlComponent staticData(e);
    staticData.setData("import QtQml 2.0          \n"
                       "import app 1.0            \n"
                       "GuiApplication { id: a    \n"
                       "  Component.onCompleted: {\n"
                       "    console.log(\"yay\")  \n"
                       "    exec()                \n"
                       "  }                       \n"
                       "property var t: Timer {   \n"
                       "  interval: 2000          \n"
                       "  running: true           \n"
                       "  onTriggered: {          \n"
                       "     console.log(\"aya\");\n" 
                       "     a.quit()             \n"
                       "}}}", QUrl());
    qDebug() << staticData.errorString();
    QObject *o1 = staticData.create();
    return 0;
}

int tst_fork()
{
    QQmlEngine *e = new QQmlEngine;
    QList<pid_t> v;
    for (int i=0; i<5; i++) {
        pid_t pid = fork();
        if (!pid) {
            QGuiApplication a(App::argc, App::argv);
            QObject::connect(e, SIGNAL(quit()), &a, SLOT(quit()));
            QObject::connect(e, SIGNAL(quit()), e, SLOT(deleteLater()));

            e->rootContext()->setContextProperty("pid", getpid());

            QQmlComponent fromFile2(e);
            fromFile2.loadUrl(QUrl::fromLocalFile("Gui.qml"));
            qDebug() << fromFile2.errorString();
            QObject *o3 = fromFile2.create(); o3->setParent(e);

            return a.exec();
        } else {
            v.push_back(pid);
        }
    }
    while (!v.isEmpty()) {
        int status;
        pid_t childpid = wait(&status);
        qDebug() << "child" << childpid << "exited with status" << status;
        v.removeAll(childpid);
    }
    return EXIT_SUCCESS;
}

int main(int argc, char *argv[])
{
    App::argc = argc;
    App::argv = argv;

    if (argc > 1) {
        int tc = atoi(argv[1]);
        QString localfile;
        if (argc > 2)
            localfile = QString::fromUtf8(argv[2]);

        switch(tc) {
        case 0: return tst_fromFile(localfile);
        case 1: return tst_staticData();
        case 2: return tst_appInQml();
        case 3: return tst_fork();
        default:
            break;
        }
    }
    return EXIT_SUCCESS;
}

#include "main.moc"
