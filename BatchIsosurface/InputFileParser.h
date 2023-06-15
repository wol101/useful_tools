#ifndef INPUTFILEPARSER_H
#define INPUTFILEPARSER_H

#include <QStringList>
#include <QString>
#include <QList>

class PipelineObject;

class InputFileParser
{
public:
    InputFileParser();
    ~InputFileParser();

    int ParseFile(QString &inputFileName);
    void SetDebugFlag(bool flag) { m_DebugFlag = flag; }

protected:

    QStringList m_InputLines;
    QList<PipelineObject *> m_PipelineObjects;

    bool m_DebugFlag;

};

#endif // INPUTFILEPARSER_H
