/****************************************************************************
**
** Copyright (C) 2012 Digia Plc and/or its subsidiary(-ies).
** Contact: http://www.qt-project.org/legal
**
** This file is part of the QtQuick module of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:LGPL$
** Commercial License Usage
** Licensees holding valid commercial Qt licenses may use this file in
** accordance with the commercial license agreement provided with the
** Software or, alternatively, in accordance with the terms contained in
** a written agreement between you and Digia.  For licensing terms and
** conditions see http://qt.digia.com/licensing.  For further information
** use the contact form at http://qt.digia.com/contact-us.
**
** GNU Lesser General Public License Usage
** Alternatively, this file may be used under the terms of the GNU Lesser
** General Public License version 2.1 as published by the Free Software
** Foundation and appearing in the file LICENSE.LGPL included in the
** packaging of this file.  Please review the following information to
** ensure the GNU Lesser General Public License version 2.1 requirements
** will be met: http://www.gnu.org/licenses/old-licenses/lgpl-2.1.html.
**
** In addition, as a special exception, Digia gives you certain additional
** rights.  These rights are described in the Digia Qt LGPL Exception
** version 1.1, included in the file LGPL_EXCEPTION.txt in this package.
**
** GNU General Public License Usage
** Alternatively, this file may be used under the terms of the GNU
** General Public License version 3.0 as published by the Free Software
** Foundation and appearing in the file LICENSE.GPL included in the
** packaging of this file.  Please review the following information to
** ensure the GNU General Public License version 3.0 requirements will be
** met: http://www.gnu.org/copyleft/gpl.html.
**
**
** $QT_END_LICENSE$
**
****************************************************************************/

#ifndef DIRECTEDVECTOR_H
#define DIRECTEDVECTOR_H
#include "qquickdirection_p.h"
QT_BEGIN_HEADER

QT_BEGIN_NAMESPACE

class QQuickItem;
class QQuickTargetDirection : public QQuickDirection
{
    Q_OBJECT
    Q_PROPERTY(qreal targetX READ targetX WRITE setTargetX NOTIFY targetXChanged)
    Q_PROPERTY(qreal targetY READ targetY WRITE setTargetY NOTIFY targetYChanged)
    //If targetItem is set, X/Y are ignored. Aims at middle of item, use variation for variation
    Q_PROPERTY(QQuickItem* targetItem READ targetItem WRITE setTargetItem NOTIFY targetItemChanged)

    Q_PROPERTY(qreal targetVariation READ targetVariation WRITE setTargetVariation NOTIFY targetVariationChanged)

    //TODO: An enum would be better
    Q_PROPERTY(bool proportionalMagnitude READ proportionalMagnitude WRITE setProportionalMagnitude NOTIFY proprotionalMagnitudeChanged)
    Q_PROPERTY(qreal magnitude READ magnitude WRITE setMagnitude NOTIFY magnitudeChanged)
    Q_PROPERTY(qreal magnitudeVariation READ magnitudeVariation WRITE setMagnitudeVariation NOTIFY magnitudeVariationChanged)

public:
    explicit QQuickTargetDirection(QObject *parent = 0);
    virtual const QPointF sample(const QPointF &from);

    qreal targetX() const
    {
        return m_targetX;
    }

    qreal targetY() const
    {
        return m_targetY;
    }

    qreal targetVariation() const
    {
        return m_targetVariation;
    }

    qreal magnitude() const
    {
        return m_magnitude;
    }

    bool proportionalMagnitude() const
    {
        return m_proportionalMagnitude;
    }

    qreal magnitudeVariation() const
    {
        return m_magnitudeVariation;
    }

    QQuickItem* targetItem() const
    {
        return m_targetItem;
    }

signals:

    void targetXChanged(qreal arg);

    void targetYChanged(qreal arg);

    void targetVariationChanged(qreal arg);

    void magnitudeChanged(qreal arg);

    void proprotionalMagnitudeChanged(bool arg);

    void magnitudeVariationChanged(qreal arg);

    void targetItemChanged(QQuickItem* arg);

public slots:
    void setTargetX(qreal arg)
    {
        if (m_targetX != arg) {
            m_targetX = arg;
            emit targetXChanged(arg);
        }
    }

    void setTargetY(qreal arg)
    {
        if (m_targetY != arg) {
            m_targetY = arg;
            emit targetYChanged(arg);
        }
    }

    void setTargetVariation(qreal arg)
    {
        if (m_targetVariation != arg) {
            m_targetVariation = arg;
            emit targetVariationChanged(arg);
        }
    }

    void setMagnitude(qreal arg)
    {
        if (m_magnitude != arg) {
            m_magnitude = arg;
            emit magnitudeChanged(arg);
        }
    }

    void setProportionalMagnitude(bool arg)
    {
        if (m_proportionalMagnitude != arg) {
            m_proportionalMagnitude = arg;
            emit proprotionalMagnitudeChanged(arg);
        }
    }

    void setMagnitudeVariation(qreal arg)
    {
        if (m_magnitudeVariation != arg) {
            m_magnitudeVariation = arg;
            emit magnitudeVariationChanged(arg);
        }
    }

    void setTargetItem(QQuickItem* arg)
    {
        if (m_targetItem != arg) {
            m_targetItem = arg;
            emit targetItemChanged(arg);
        }
    }

private:
    qreal m_targetX;
    qreal m_targetY;
    qreal m_targetVariation;
    bool m_proportionalMagnitude;
    qreal m_magnitude;
    qreal m_magnitudeVariation;
    QQuickItem *m_targetItem;
};

QT_END_NAMESPACE
QT_END_HEADER
#endif // DIRECTEDVECTOR_H
