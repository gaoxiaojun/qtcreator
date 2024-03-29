/**************************************************************************
**
** This file is part of Qt Creator
**
** Copyright (c) 2012 Nokia Corporation and/or its subsidiary(-ies).
**
** Contact: Nokia Corporation (qt-info@nokia.com)
**
**
** GNU Lesser General Public License Usage
**
** This file may be used under the terms of the GNU Lesser General Public
** License version 2.1 as published by the Free Software Foundation and
** appearing in the file LICENSE.LGPL included in the packaging of this file.
** Please review the following information to ensure the GNU Lesser General
** Public License version 2.1 requirements will be met:
** http://www.gnu.org/licenses/old-licenses/lgpl-2.1.html.
**
** In addition, as a special exception, Nokia gives you certain additional
** rights. These rights are described in the Nokia Qt LGPL Exception
** version 1.1, included in the file LGPL_EXCEPTION.txt in this package.
**
** Other Usage
**
** Alternatively, this file may be used in accordance with the terms and
** conditions contained in a signed written agreement between you and Nokia.
**
** If you have questions regarding the use of this file, please contact
** Nokia at qt-info@nokia.com.
**
**************************************************************************/

#ifndef COLORSCHEME_H
#define COLORSCHEME_H

#include "texteditor_global.h"
#include "texteditorconstants.h"

#include <QMap>
#include <QString>
#include <QColor>

QT_BEGIN_NAMESPACE
class QWidget;
QT_END_NAMESPACE

namespace TextEditor {

/*! Format for a particular piece of text (text/comment, etc). */
class TEXTEDITOR_EXPORT Format
{
public:
    Format();

    QColor foreground() const { return m_foreground; }
    void setForeground(const QColor &foreground);

    QColor background() const { return m_background; }
    void setBackground(const QColor &background);

    bool bold() const { return m_bold; }
    void setBold(bool bold);

    bool italic() const { return m_italic; }
    void setItalic(bool italic);

    bool equals(const Format &f) const;

    QString toString() const;
    bool fromString(const QString &str);

private:
    QColor m_foreground;
    QColor m_background;
    bool m_bold;
    bool m_italic;
};

inline bool operator==(const Format &f1, const Format &f2) { return f1.equals(f2); }
inline bool operator!=(const Format &f1, const Format &f2) { return !f1.equals(f2); }


/*! A color scheme combines a set of formats for different highlighting
    categories. It also provides saving and loading of the scheme to a file.
 */
class ColorScheme
{
public:
    ColorScheme();

    void setDisplayName(const QString &name)
    { m_displayName = name; }

    QString displayName() const
    { return m_displayName; }

    inline bool isEmpty() const
    { return m_formats.isEmpty(); }

    bool contains(TextStyle category) const;

    Format &formatFor(TextStyle category);
    Format formatFor(TextStyle category) const;

    void setFormatFor(TextStyle category, const Format &format);

    void clear();

    bool save(const QString &fileName, QWidget *parent) const;
    bool load(const QString &fileName);

    inline bool equals(const ColorScheme &cs) const
    {
        return m_formats == cs.m_formats
                && m_displayName == cs.m_displayName;
    }

    static QString readNameOfScheme(const QString &fileName);

private:
    QMap<TextStyle, Format> m_formats;
    QString m_displayName;
};

inline bool operator==(const ColorScheme &cs1, const ColorScheme &cs2) { return cs1.equals(cs2); }
inline bool operator!=(const ColorScheme &cs1, const ColorScheme &cs2) { return !cs1.equals(cs2); }

} // namespace TextEditor

#endif // COLORSCHEME_H
