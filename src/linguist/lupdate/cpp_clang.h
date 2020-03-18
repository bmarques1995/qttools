/****************************************************************************
**
** Copyright (C) 2019 The Qt Company Ltd.
** Contact: https://www.qt.io/licensing/
**
** This file is part of the Qt Linguist of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:GPL-EXCEPT$
** Commercial License Usage
** Licensees holding valid commercial Qt licenses may use this file in
** accordance with the commercial license agreement provided with the
** Software or, alternatively, in accordance with the terms contained in
** a written agreement between you and The Qt Company. For licensing terms
** and conditions see https://www.qt.io/terms-conditions. For further
** information use the contact form at https://www.qt.io/contact-us.
**
** GNU General Public License Usage
** Alternatively, this file may be used under the terms of the GNU
** General Public License version 3 as published by the Free Software
** Foundation with exceptions as appearing in the file LICENSE.GPL3-EXCEPT
** included in the packaging of this file. Please review the following
** information to ensure the GNU General Public License requirements will
** be met: https://www.gnu.org/licenses/gpl-3.0.html.
**
** $QT_END_LICENSE$
**
****************************************************************************/

#ifndef CLANG_CPP_H
#define CLANG_CPP_H

#include "lupdate.h"

#include <QtCore/qloggingcategory.h>
#include <QtCore/qregularexpression.h>

#if defined(Q_CC_MSVC)
# pragma warning(push)
# pragma warning(disable: 4100)
# pragma warning(disable: 4146)
# pragma warning(disable: 4267)
# pragma warning(disable: 4624)
#endif

#include <clang/Basic/SourceLocation.h>
#include <clang/Basic/SourceManager.h>

#if defined(Q_CC_MSVC)
# pragma warning(pop)
#endif

#include <vector>

QT_BEGIN_NAMESPACE

inline QDebug operator<<(QDebug out, const std::string& str)
{
    out << QString::fromStdString(str);
    return out;
}
Q_DECLARE_LOGGING_CATEGORY(lcClang)

#define LUPDATE_CLANG_VERSION_CHECK(major, minor, patch) ((major<<16)|(minor<<8)|(patch))
#define LUPDATE_CLANG_VERSION LUPDATE_CLANG_VERSION_CHECK(LUPDATE_CLANG_VERSION_MAJOR, \
    LUPDATE_CLANG_VERSION_MINOR, LUPDATE_CLANG_VERSION_PATCH)

// Local storage of translation information (information from the AST and linguist side)
struct TranslationRelatedStore
{
    QString callType;
    QString rawCode;
    QString funcName;
    qint64 locationCol = -1;
    QString contextArg;
    QString contextRetrieved;
    QString contextRetrievedTempNOOP;
    QString lupdateSource;
    QString lupdateLocationFile;
    qint64 lupdateLocationLine = -1;
    QString lupdateId;
    QString lupdateSourceWhenId;
    QString lupdateIdMetaData;
    QString lupdateMagicMetaData;
    QHash<QString, QString> lupdateAllMagicMetaData;
    QString lupdateComment;
    QString lupdateExtraComment;
    QString lupdatePlural;
    clang::SourceLocation sourceLocation;

    bool isValid() const
    {
        return !lupdateLocationFile.isEmpty() && (lupdateLocationLine > -1) && (locationCol > -1);
    }

    clang::SourceLocation callLocation(const clang::SourceManager &sourceManager)
    {
        if (sourceLocation.isInvalid()) {
            auto sourceFile = sourceManager.getFileManager()
                .getFile(lupdateLocationFile.toStdString());
#if (LUPDATE_CLANG_VERSION >= LUPDATE_CLANG_VERSION_CHECK(10,0,0))
            sourceLocation = sourceManager.translateFileLineCol(sourceFile.get(),
                lupdateLocationLine, locationCol);
#else
            sourceLocation = sourceManager.translateFileLineCol(sourceFile, lupdateLocationLine,
                locationCol);
#endif
        }
        return sourceLocation;
    }

    void printStore() const
    {
        qCDebug(lcClang) << "------------------ Printing Store----------------------------------\n";
        qCDebug(lcClang)
            << "callType            : " << callType << "\n"
            << "rawCode             : \n" << rawCode << "\n"
            << "funcName            : " << funcName << "\n"
            << "LocationCol         : " << locationCol << "\n"
            << "contextArg          : " << contextArg << "\n"
            << "contextRetrieved    : " << contextRetrieved << "\n"
            << "lupdateSource       : " << lupdateSource << "\n"
            << "lupdateLocationFile : " << lupdateLocationFile << "\n"
            << "lupdateLocationLine : " << lupdateLocationLine << "\n"
            << "lupdateId           : " << lupdateId << "\n"
            << "lupdateIdMetaData   : " << lupdateIdMetaData << "\n"
            << "lupdateMagicMetaData: " << lupdateMagicMetaData << "\n"
            << "lupdateComment      : " << lupdateComment << "\n"
            << "lupdateExtraComment : " << lupdateExtraComment << "\n"
            << "lupdatePlural       : " << lupdatePlural;
        qCDebug(lcClang) << "-------------------------------------------------------------------\n";
    }
};
using TranslationStores = std::vector<TranslationRelatedStore>;

struct Stores
{
    TranslationStores Preprocessor;
    TranslationStores AST;
    TranslationStores QDeclareTrWithContext;
    TranslationStores QNoopTranlsationWithContext;
};

namespace LupdatePrivate
{
    enum QuoteCompulsary
    {
        None = 0x01,
        Left = 0x02,                // Left quote is mandatory
        Right = 0x04,               // Right quote is mandatory
        LeftAndRight = Left | Right // Both quotes are mandatory
    };

    /*
        Removes the quotes around the lupdate extra, ID meta data, magic and
        ID prefix comments and source string literals.
        Depending on the given compulsory option, quotes can be unbalanced and
        still some text is returned. This is to mimic the old lupdate behavior.
    */
    static QString cleanQuote(llvm::StringRef s, QuoteCompulsary quote)
    {
        if (s.empty())
            return {};
        s = s.trim();
        if (!s.consume_front("\"") && ((quote & Left) != 0))
            return {};
        if (!s.consume_back("\"") && ((quote & Right) != 0))
            return {};
        return QString::fromStdString(s);
    }

    /*
        Removes the quotes and a possible existing string literal prefix
        for a given string literal coming from the source code. Do not use
        to clean the quotes around the lupdate translator specific comments.
    */
    static QString cleanQuote(const std::string &token)
    {
        if (token.empty())
            return {};

        const QString string = QString::fromStdString(token).trimmed();
        const int index = string.indexOf(QLatin1Char('"'));
        if (index <= 0)
            return LupdatePrivate::cleanQuote(token, QuoteCompulsary::LeftAndRight);

        QRegularExpressionMatch result;
        if (string.at(index - 1) == QLatin1Char('R')) {
            static const QRegularExpression rawStringLiteral {
                QStringLiteral(
                    "(?:\\bu8|\\b[LuU])??R\\\"([^\\(\\)\\\\ ]{0,16})\\((?<characters>.*)\\)\\1\\\""
                ), QRegularExpression::DotMatchesEverythingOption };
            result = rawStringLiteral.match(string);
        } else {
            static const QRegularExpression stringLiteral {
                QStringLiteral(
                    "(?:\\bu8|\\b[LuU])+?\\\"(?<characters>[^\\\"\\\\]*(?:\\\\.[^\\\"\\\\]*)*)\\\""
                )
            };
            result = stringLiteral.match(string);
        }
        if (result.hasMatch())
            return result.captured(QStringLiteral("characters"));
        return string;
    }
}

namespace ClangCppParser
{
    void loadCPP(Translator &translator, const QStringList &filenames, ConversionData &cd);

    void fillTranslator(Translator *tor, Stores &stores);
    void fillTranslator(Translator *tor, TranslationRelatedStore store);

    TranslatorMessage fillTranslatorMessage(const TranslationRelatedStore &store,
        const QString &id, bool plural, bool isID);

    void handleTr(Translator *tor, const TranslationRelatedStore &store, bool plural);
    void handleTrId(Translator *tor, const TranslationRelatedStore &store, bool plural);
    void handleTranslate(Translator *tor, const TranslationRelatedStore &store, bool plural);

    void correctAstTranslationContext(Stores &stores);
    void correctNoopTanslationContext(Stores &stores);
}

QT_END_NAMESPACE

#endif
