/****************************************************************************
**
** Copyright (C) 2019 The Qt Company Ltd.
** Contact: https://www.qt.io/licensing/
**
** This file is part of the tools applications of the Qt Toolkit.
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
#include "testcpp.h"

namespace TestQDoc {

/*!
    \module TestCPP
    \title QDoc Test C++ Classes
    \brief A test module page.
*/

/*!
    \namespace TestQDoc
    \inmodule TestCPP
    \brief A namespace.

    \section1 Usage
    This namespace is for testing QDoc output.
*/

/*!
    \class TestQDoc::Test
    \inmodule TestCPP
    \brief A class in a namespace.
*/

/*!
    \macro QDOCTEST_MACRO
    \relates TestQDoc
*/

/*!
    Function that takes a parameter \a v.
    Also returns the value of \a v.
*/
int Test::someFunction(int v)
{
    return v;
}

/*!
    \fn void TestQDoc::Test::inlineFunction()

    \brief An inline function, documented using the \CMDFN QDoc command.
*/

} // namespace TestQDoc
