#include <QtCore/QCoreApplication>
#include <QStringList>
#include <QString>
#include <QList>
#include <QTimer>

#include <iostream>

#include "InputFileParser.h"
#include "PipelineObject.h"

int main(int argc, char *argv[])
{
    QCoreApplication app(argc, argv);

    // (1) We *could* write a whole application
    // here if it doesn't need event handling.

    QStringList arguments = QCoreApplication::arguments ();
    QString inputFile;
    bool debugFlag = false;

    QString usage =
            "Usage: BatchIsosurface -i inputFile\n"
            "Options:\n"
            "-i --input inputFile containing list of instructions\n"
            "-d --debug turn on debugging\n";

    for (int i = 1; i < arguments.size(); i++)
    {
        if (arguments.at(i) == "-i" || arguments.at(i) == "--input")
        {
            i++;
            if (i < arguments.size()) inputFile = arguments.at(i);
            continue;
        }

        if (arguments.at(i) == "-d" || arguments.at(i) == "--debug")
        {
            debugFlag = true;
            continue;
        }

        std::cerr << (const char *)usage.toUtf8();
        return 1;

    }

    if (inputFile.size() == 0)
    {
        std::cerr << (const char *)usage.toUtf8();
        return 1;
    }

    InputFileParser inputParser;
    inputParser.SetDebugFlag(debugFlag);
    if (inputParser.ParseFile(inputFile)) return 1;

    // (2) Set up something that will trigger events
    // e.g. QTimer

    // this example just sends a quite signal to the queue
    QTimer::singleShot(0, &app, SLOT(quit()));

    return app.exec();
}



