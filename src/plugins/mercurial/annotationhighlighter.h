/**************************************************************************
**
** This file is part of Qt Creator
**
** Copyright (c) 2009 Brian McGillion
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

#ifndef ANNOTATIONHIGHLIGHTER_H
#define ANNOTATIONHIGHLIGHTER_H

#include <vcsbase/baseannotationhighlighter.h>
#include <QRegExp>

namespace Mercurial {
namespace Internal {

class MercurialAnnotationHighlighter : public VcsBase::BaseAnnotationHighlighter
{
public:
    explicit MercurialAnnotationHighlighter(const ChangeNumbers &changeNumbers, const QColor &bg,
                                            QTextDocument *document = 0);

private:
    virtual QString changeNumber(const QString &block) const;
    mutable QRegExp changeset;
};

} //namespace Internal
}// namespace Mercurial
#endif // ANNOTATIONHIGHLIGHTER_H
