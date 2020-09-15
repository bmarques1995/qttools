/****************************************************************************
**
** Copyright (C) 2021 The Qt Company Ltd.
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

#include <QtCore>
#include "my_include.h"

int X = 111222332444;

// Q_OBJECT declared in an other file
class FooChild : public Foo
{
    const char *c_noop = QT_TR_NOOP("context Foo. noop");
};

// No Q_OBJECT. Q_DECLARE_TR_FUNCTIONS. It is enough for tr, it is enough for QT_TR_NOOP
class OnlyQDeclare
{
    Q_DECLARE_TR_FUNCTIONS(ONLY_Q)
    const char *c_noop = QT_TR_NOOP("context ONLY_Q. noop");
    const char *c_noop_n = QT_TR_N_NOOP("context ONLY_Q. noop N");
};

// Show the message warning that the QT_TR_NOOP is ignored because no context could be found.
class Nothing
{
    const char *c_noop = QT_TR_NOOP("context Nothing. noop with no Q_OBJECT nor Q_DECLARE_TR_FUNCTIONS");
};

// The closest Q_OBJECT or Q_DECLARE_TR_FUNCTIONS gives the context
class WithQObject
{
  Q_OBJECT
  class Nested
  {
    Q_DECLARE_TR_FUNCTIONS(IN_NESTED)
    QString c_tr = tr("context IN_NESTED. tr");
    const char *c_noop = QT_TR_NOOP("context IN_NESTED. noop");
    const char *c_noop_translate = QT_TRANSLATE_NOOP("my_arg", "context my_arg. translate_noop");
  };
};

// The closest Q_OBJECT or Q_DECLARE_TR_FUNCTIONS gives the context
class WithQDeclareTr
{
  Q_DECLARE_TR_FUNCTIONS(A_CONTEXT)
  class Nested
  {
    Q_OBJECT
    QString c_tr = tr("context WithQDeclareTr::Nested. tr");
    const char *c_noop = QT_TR_NOOP("context WithQDeclareTr::Nested. noop");
  };
};

// Case where Q_OBJECT is not seen. Behavior reproduces tr function behavior
namespace test1 {
  class AA
  {
   Q_OBJECT
  public:
    class A {};
  };

  class B : public AA::A
  {
    //QString c_tr = tr(" the Q_OBJECT is not seen"); // this will generate an error.
    const char *c_noop = QT_TR_NOOP("the Q_OBJECT is not seen"); // Once exploring the bases of B (that is AA:A)
                                                        // we are not looking for the parents
                                                        // AA is the parent of A in terms of CXXContextDecl::getParent()
                                                        // This reproduced tr behavior.
  };
}

namespace test2 {
  class AA
  {
    Q_OBJECT
  public:
    class A {};
  };

  class C {};

  class BB
  {
    Q_OBJECT
  public:
    class B : public AA::A, public C
    {
      QString c_tr = tr("context test2::BB. tr");
      const char *c_noop = QT_TR_NOOP("context test2::BB. noop");
    };
  };
}

namespace test3 {
  class AA
  {
    Q_OBJECT
  public:
    class A {};
  };

  class D {Q_OBJECT};
  class C : public D {};

  class BB
  {
    Q_OBJECT
  public:
    class B : public AA::A, public C
    {
      QString c_tr = tr("context test3::D. tr");
      const char *c_noop = QT_TR_NOOP("context test3::D. noop");
    };
  };
}

namespace test4 {

  class A
  {
    Q_OBJECT
  public:
    class B
    {
    public:
      class C
      {
    Q_OBJECT
      public:
    class D
    {
      QString c_tr = tr("context is test4::A::B::C. tr");
      const char *c_noop = QT_TR_NOOP("context is test4::A::B::C. noop");
    };
      };
    };
  };

}
