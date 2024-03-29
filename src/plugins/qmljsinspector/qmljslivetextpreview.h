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

#ifndef SCRIPTBINDINGREWRITER_H
#define SCRIPTBINDINGREWRITER_H

#include <QObject>
#include <QWeakPointer>
#include <texteditor/basetexteditor.h>

#include <qmldebug/baseenginedebugclient.h>
#include <qmljs/parser/qmljsastfwd_p.h>
#include <qmljs/qmljsdocument.h>

QT_FORWARD_DECLARE_CLASS(QTextDocument)

using namespace QmlDebug;

namespace Core {
class IEditor;
}

namespace QmlJS {
class ModelManagerInterface;
}

namespace QmlJSInspector {
namespace Internal {

class ClientProxy;


class QmlJSLiveTextPreview : public QObject
{
    Q_OBJECT

public:
    explicit QmlJSLiveTextPreview(const QmlJS::Document::Ptr &doc,
                                  const QmlJS::Document::Ptr &initDoc,
                                  ClientProxy *clientProxy,
                                  QObject *parent = 0);
    //void updateDocuments();

    void associateEditor(Core::IEditor *editor);
    void unassociateEditor(Core::IEditor *editor);
    void setActiveObject(const QmlDebugObjectReference &object);
    void mapObjectToQml(const QmlDebugObjectReference &object);
    void resetInitialDoc(const QmlJS::Document::Ptr &doc);

    void setClientProxy(ClientProxy *clientProxy);

    enum UnsyncronizableChangeType {
        NoUnsyncronizableChanges,
        AttributeChangeWarning,
        ElementChangeWarning
    };

signals:
    void selectedItemsChanged(const QList<QmlDebugObjectReference> &objects);
    void reloadQmlViewerRequested();
    void disableLivePreviewRequested();

public slots:
    void setApplyChangesToQmlInspector(bool applyChanges);
    void updateDebugIds();

private slots:
    void changeSelectedElements(QList<int> offsets, const QString &wordAtCursor);
    void documentChanged(QmlJS::Document::Ptr doc);
    void disableLivePreview();
    void reloadQmlViewer();

private:
    static QmlJS::ModelManagerInterface *modelManager();
    QList<int> objectReferencesForOffset(quint32 offset);
    QVariant castToLiteral(const QString &expression,
                           QmlJS::AST::UiScriptBinding *scriptBinding);
    void showSyncWarning(UnsyncronizableChangeType unsyncronizableChangeType,
                         const QString &elementName,
                         unsigned line, unsigned column);
    void showExperimentalWarning();

private:
    QHash<QmlJS::AST::UiObjectMember*, QList<int> > m_debugIds;
    QHash<QmlJS::Document::Ptr, QSet<QmlJS::AST::UiObjectMember *> > m_createdObjects;

    QmlJS::Document::Ptr m_previousDoc;
    QmlJS::Document::Ptr m_initialDoc; //the document that was loaded by the server
    QString m_filename;

    QList<QWeakPointer<TextEditor::BaseTextEditorWidget> > m_editors;

    bool m_applyChangesToQmlInspector;
    QmlJS::Document::Ptr m_docWithUnappliedChanges;
    QWeakPointer<ClientProxy> m_clientProxy;
    QList<int> m_lastOffsets;
    QmlJS::AST::UiObjectMember *m_nodeForOffset;
    bool m_updateNodeForOffset;

};

} // namespace Internal
} // namespace QmlJSInspector

#endif // SCRIPTBINDINGREWRITER_H
