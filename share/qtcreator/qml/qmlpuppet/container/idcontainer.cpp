/**************************************************************************
**
** This file is part of Qt Creator
**
** Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies).
**
** Contact: Nokia Corporation (info@qt.nokia.com)
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
** Nokia at info@qt.nokia.com.
**
**************************************************************************/

#include "idcontainer.h"

namespace QmlDesigner {

IdContainer::IdContainer()
    : m_instanceId(-1)
{
}

IdContainer::IdContainer(qint32 instanceId, const QString &id)
    : m_instanceId(instanceId),
    m_id(id)
{
}

qint32 IdContainer::instanceId() const
{
    return m_instanceId;
}

QString IdContainer::id() const
{
    return m_id;
}

QDataStream &operator<<(QDataStream &out, const IdContainer &container)
{
    out << container.instanceId();
    out << container.id();

    return out;
}

QDataStream &operator>>(QDataStream &in, IdContainer &container)
{
    in >> container.m_instanceId;
    in >> container.m_id;

    return in;
}
} // namespace QmlDesigner