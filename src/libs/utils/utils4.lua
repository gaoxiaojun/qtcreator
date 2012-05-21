dofile (solution().basedir .. "/build/qt4.lua")
dofile (solution().basedir .. "/src/private_headers4.lua")

project "Utils"
kind "SharedLib"
language "C++"

qt4.QtPre()

private_headers.Configure()

-- From utils.pro
qt4.QT { "core", "gui", "network" }
qt4.HEADERS { "proxyaction.h" }
files { "proxyaction.cpp" }

if (os.is("windows")) then
    links
    {
          "User32"
        , "iphlpapi"
        , "Ws2_32"
    }
end

-- Botan Dependencies
links { "Botan" }
includedirs { solution().basedir .. "/src/libs/3rdparty/botan/build" }

-- From utils-lib.pri
defines { "QTCREATOR_UTILS_LIB" }
includedirs { ".", "..", solution().basedir .. "/src", "qt_moc" }
qt4.QT { "network", "script" }

files
{
      "environment.cpp"
    , "environmentmodel.cpp"
    , "qtcprocess.cpp"
    , "reloadpromptutils.cpp"
    , "settingsselector.cpp"
    , "stringutils.cpp"
    , "filesearch.cpp"
    , "pathchooser.cpp"
    , "pathlisteditor.cpp"
    , "wizard.cpp"
    , "filewizardpage.cpp"
    , "filewizarddialog.cpp"
    , "filesystemwatcher.cpp"
    , "projectintropage.cpp"
    , "basevalidatinglineedit.cpp"
    , "filenamevalidatinglineedit.cpp"
    , "projectnamevalidatinglineedit.cpp"
    , "codegeneration.cpp"
    , "newclasswidget.cpp"
    , "classnamevalidatinglineedit.cpp"
    , "linecolumnlabel.cpp"
    , "fancylineedit.cpp"
    , "qtcolorbutton.cpp"
    , "savedaction.cpp"
    , "submiteditorwidget.cpp"
    , "synchronousprocess.cpp"
    , "savefile.cpp"
    , "fileutils.cpp"
    , "textfileformat.cpp"
    , "submitfieldwidget.cpp"
    , "consoleprocess.cpp"
    , "uncommentselection.cpp"
    , "parameteraction.cpp"
    , "treewidgetcolumnstretcher.cpp"
    , "checkablemessagebox.cpp"
    , "styledbar.cpp"
    , "stylehelper.cpp"
    , "iwelcomepage.cpp"
    , "fancymainwindow.cpp"
    , "detailsbutton.cpp"
    , "detailswidget.cpp"
    , "changeset.cpp"
    , "filterlineedit.cpp"
    , "faketooltip.cpp"
    , "htmldocextractor.cpp"
    , "navigationtreeview.cpp"
    , "crumblepath.cpp"
    , "historycompleter.cpp"
    , "buildablehelperlibrary.cpp"
    , "annotateditemdelegate.cpp"
    , "fileinprojectfinder.cpp"
    , "ipaddresslineedit.cpp"
    , "statuslabel.cpp"
    , "ssh/sshsendfacility.cpp"
    , "ssh/sshremoteprocess.cpp"
    , "ssh/sshpacketparser.cpp"
    , "ssh/sshpacket.cpp"
    , "ssh/sshoutgoingpacket.cpp"
    , "ssh/sshkeygenerator.cpp"
    , "ssh/sshkeyexchange.cpp"
    , "ssh/sshincomingpacket.cpp"
    , "ssh/sshcryptofacility.cpp"
    , "ssh/sshconnection.cpp"
    , "ssh/sshchannelmanager.cpp"
    , "ssh/sshchannel.cpp"
    , "ssh/sshcapabilities.cpp"
    , "ssh/sftppacket.cpp"
    , "ssh/sftpoutgoingpacket.cpp"
    , "ssh/sftpoperation.cpp"
    , "ssh/sftpincomingpacket.cpp"
    , "ssh/sftpdefs.cpp"
    , "ssh/sftpchannel.cpp"
    , "ssh/sshremoteprocessrunner.cpp"
    , "ssh/sshconnectionmanager.cpp"
    , "ssh/sshkeypasswordretriever.cpp"
    , "ssh/sftpfilesystemmodel.cpp"
    , "ssh/sshkeycreationdialog.cpp"
    , "outputformatter.cpp"
    , "flowlayout.cpp"
    , "networkaccessmanager.cpp"
    , "persistentsettings.cpp"
    , "completingtextedit.cpp"
    , "json.cpp"
    , "portlist.cpp"
    , "tcpportsgatherer.cpp"
    , "appmainwindow.cpp"
    , "basetreeview.cpp"
    , "qtcassert.cpp"
}

if (os.is("windows")) then
    files
    {
          "consoleprocess_win.cpp"
        , "winutils.cpp"
    }
    qt4.HEADERS { "winutils.h" }
else
    files { "consoleprocess_unix.cpp" }
end

if (os.is("linux")) then
    qt4.HEADERS { "unixutils.h" }
    files { "unixutils.cpp" }
end


qt4.HEADERS
{
      "environment.h"
    , "environmentmodel.h"
    , "qtcprocess.h"
    , "utils_global.h"
    , "reloadpromptutils.h"
    , "settingsselector.h"
    , "stringutils.h"
    , "filesearch.h"
    , "listutils.h"
    , "pathchooser.h"
    , "pathlisteditor.h"
    , "wizard.h"
    , "filewizardpage.h"
    , "filewizarddialog.h"
    , "filesystemwatcher.h"
    , "projectintropage.h"
    , "basevalidatinglineedit.h"
    , "filenamevalidatinglineedit.h"
    , "projectnamevalidatinglineedit.h"
    , "codegeneration.h"
    , "newclasswidget.h"
    , "classnamevalidatinglineedit.h"
    , "linecolumnlabel.h"
    , "fancylineedit.h"
    , "qtcolorbutton.h"
    , "savedaction.h"
    , "submiteditorwidget.h"
    , "consoleprocess.h"
    , "consoleprocess_p.h"
    , "synchronousprocess.h"
    , "savefile.h"
    , "fileutils.h"
    , "textfileformat.h"
    , "submitfieldwidget.h"
    , "uncommentselection.h"
    , "parameteraction.h"
    , "treewidgetcolumnstretcher.h"
    , "checkablemessagebox.h"
    , "qtcassert.h"
    , "styledbar.h"
    , "stylehelper.h"
    , "iwelcomepage.h"
    , "fancymainwindow.h"
    , "detailsbutton.h"
    , "detailswidget.h"
    , "changeset.h"
    , "filterlineedit.h"
    , "faketooltip.h"
    , "htmldocextractor.h"
    , "navigationtreeview.h"
    , "crumblepath.h"
    , "historycompleter.h"
    , "buildablehelperlibrary.h"
    , "annotateditemdelegate.h"
    , "fileinprojectfinder.h"
    , "ipaddresslineedit.h"
    , "ssh/sshsendfacility_p.h"
    , "ssh/sshremoteprocess.h"
    , "ssh/sshremoteprocess_p.h"
    , "ssh/sshpacketparser_p.h"
    , "ssh/sshpacket_p.h"
    , "ssh/sshoutgoingpacket_p.h"
    , "ssh/sshkeygenerator.h"
    , "ssh/sshkeyexchange_p.h"
    , "ssh/sshincomingpacket_p.h"
    , "ssh/sshexception_p.h"
    , "ssh/ssherrors.h"
    , "ssh/sshcryptofacility_p.h"
    , "ssh/sshconnection.h"
    , "ssh/sshconnection_p.h"
    , "ssh/sshchannelmanager_p.h"
    , "ssh/sshchannel_p.h"
    , "ssh/sshcapabilities_p.h"
    , "ssh/sshbotanconversions_p.h"
    , "ssh/sftppacket_p.h"
    , "ssh/sftpoutgoingpacket_p.h"
    , "ssh/sftpoperation_p.h"
    , "ssh/sftpincomingpacket_p.h"
    , "ssh/sftpdefs.h"
    , "ssh/sftpchannel.h"
    , "ssh/sftpchannel_p.h"
    , "ssh/sshremoteprocessrunner.h"
    , "ssh/sshconnectionmanager.h"
    , "ssh/sshpseudoterminal.h"
    , "ssh/sshkeypasswordretriever_p.h"
    , "ssh/sftpfilesystemmodel.h"
    , "ssh/sshkeycreationdialog.h"
    , "statuslabel.h"
    , "outputformatter.h"
    , "outputformat.h"
    , "flowlayout.h"
    , "networkaccessmanager.h"
    , "persistentsettings.h"
    , "completingtextedit.h"
    , "json.h"
    , "multitask.h"
    , "runextensions.h"
    , "portlist.h"
    , "tcpportsgatherer.h"
    , "appmainwindow.h"
    , "basetreeview.h"
    , "crumblepath.cpp"
    , "submiteditorwidget.cpp"
    , "qtcolorbutton.cpp"
    , "wizard.cpp"
    , "ssh/sshconnectionmanager.cpp"
}

-- The following files need to be excluded because of a conflicting usage of moc in
-- this project (it's annoying). This moc'd file is actually included instead of
-- compiled in.
excludes { "qt_moc/moc_completingtextedit.cpp" }

qt4.FORMS
{
      "filewizardpage.ui"
    , "projectintropage.ui"
    , "newclasswidget.ui"
    , "submiteditorwidget.ui"
    , "ssh/sshkeycreationdialog.ui"
}

qt4.RESOURCES { "utils.qrc" }

qt4.Configure()
Configure()

