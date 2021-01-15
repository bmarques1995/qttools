/****************************************************************************
**
** Copyright (C) 2021 The Qt Company Ltd.
** Contact: https://www.qt.io/licensing/
**
** This file is part of the tools applications of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:COMM$
**
** Commercial License Usage
** Licensees holding valid commercial Qt licenses may use this file in
** accordance with the commercial license agreement provided with the
** Software or, alternatively, in accordance with the terms contained in
** a written agreement between you and The Qt Company. For licensing terms
** and conditions see https://www.qt.io/terms-conditions. For further
** information use the contact form at https://www.qt.io/contact-us.
**
** $QT_END_LICENSE$
**
**
**
**
**
**
**
**
****************************************************************************/

/*
  editdistance.cpp
*/

#include "editdistance.h"

QT_BEGIN_NAMESPACE

int editDistance(const QString &s, const QString &t)
{
#define D(i, j) d[(i)*n + (j)]
    int i;
    int j;
    int m = s.length() + 1;
    int n = t.length() + 1;
    int *d = new int[m * n];
    int result;

    for (i = 0; i < m; ++i)
        D(i, 0) = i;
    for (j = 0; j < n; ++j)
        D(0, j) = j;
    for (i = 1; i < m; ++i) {
        for (j = 1; j < n; ++j) {
            if (s[i - 1] == t[j - 1]) {
                D(i, j) = D(i - 1, j - 1);
            } else {
                int x = D(i - 1, j);
                int y = D(i - 1, j - 1);
                int z = D(i, j - 1);
                D(i, j) = 1 + qMin(qMin(x, y), z);
            }
        }
    }
    result = D(m - 1, n - 1);
    delete[] d;
    return result;
#undef D
}

QString nearestName(const QString &actual, const QSet<QString> &candidates)
{
    if (actual.isEmpty())
        return QString();

    int deltaBest = 10000;
    int numBest = 0;
    QString best;

    for (const auto &candidate : candidates) {
        if (candidate[0] == actual[0]) {
            int delta = editDistance(actual, candidate);
            if (delta < deltaBest) {
                deltaBest = delta;
                numBest = 1;
                best = candidate;
            } else if (delta == deltaBest) {
                ++numBest;
            }
        }
    }

    if (numBest == 1 && deltaBest <= 2 && actual.length() + best.length() >= 5)
        return best;

    return QString();
}

QT_END_NAMESPACE
