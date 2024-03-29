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
#include "qmljscontextcrumblepath.h"

#include <QMouseEvent>
#include <QDebug>

namespace QmlJSInspector {
namespace Internal {

ContextCrumblePath::ContextCrumblePath(QWidget *parent)
    : CrumblePath(parent)
    , m_isEmpty(true)
{
    updateContextPath(QStringList(), QList<int>());
}

void ContextCrumblePath::updateContextPath(const QStringList &path,
                                           const QList<int> &debugIds)
{
    Q_ASSERT(path.count() == debugIds.count());

    CrumblePath::clear();

    m_isEmpty = path.isEmpty();
    if (m_isEmpty) {
        pushElement(tr("[no context]"), -2);
    } else {
        for (int i = 0; i < path.count(); i++)
            pushElement(path[i], debugIds[i]);
    }
}

void ContextCrumblePath::addChildren(const QStringList &childrenNames,
                                     const QList<int> &childrenDebugIds)
{
    Q_ASSERT(childrenNames.count() == childrenDebugIds.count());
    for (int i = 0; i < childrenNames.count(); i++)
        addChild(childrenNames[i], childrenDebugIds[i]);

    //Sort them alphabetically
    if (childrenDebugIds.count())
        sortChildren();
}

void ContextCrumblePath::clear()
{
    updateContextPath(QStringList(), QList<int>());
}

bool ContextCrumblePath::isEmpty() const
{
    return m_isEmpty;
}

int ContextCrumblePath::debugIdForIndex(int index) const
{
    return CrumblePath::dataForIndex(index).toInt();
}

} // namespace Internal
} // namespace QmlJSInspector
