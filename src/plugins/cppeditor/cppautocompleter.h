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

#ifndef CPPAUTOCOMPLETER_H
#define CPPAUTOCOMPLETER_H

#include <texteditor/autocompleter.h>
#include <Token.h>

namespace CppEditor {
namespace Internal {

class CppAutoCompleter : public TextEditor::AutoCompleter
{
public:
    CppAutoCompleter();
    virtual ~CppAutoCompleter();

    virtual bool contextAllowsAutoParentheses(const QTextCursor &cursor,
                                              const QString &textToInsert = QString()) const;
    virtual bool contextAllowsElectricCharacters(const QTextCursor &cursor) const;
    virtual bool isInComment(const QTextCursor &cursor) const;
    virtual QString insertMatchingBrace(const QTextCursor &cursor,
                                        const QString &text,
                                        QChar la,
                                        int *skippedChars) const;
    virtual QString insertParagraphSeparator(const QTextCursor &cursor) const;

private:
    bool isInCommentHelper(const QTextCursor &cursor, CPlusPlus::Token *retToken = 0) const;
    const CPlusPlus::Token tokenAtPosition(const QList<CPlusPlus::Token> &tokens, const unsigned pos) const;
};

} // Internal
} // CppEditor

#endif // CPPAUTOCOMPLETER_H
