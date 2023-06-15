#include "Utilities.h"

#include <QString>

Utilities::Utilities()
{

}

bool Utilities::toBool(const QString &string)
{
    bool ok = false;
    double v = string.toDouble(&ok);
    if (ok)
    {
        if (v == 0) return false;
        return true;
    }
    if (string.compare("false", Qt::CaseInsensitive) || string.isEmpty()) return false;
    return true;
}

