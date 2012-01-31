/****************************************************************************
**
** Copyright (C) 2012 Nokia Corporation and/or its subsidiary(-ies).
** Contact: http://www.qt-project.org/
**
** This file is part of the QtDeclarative module of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:LGPL$
** GNU Lesser General Public License Usage
** This file may be used under the terms of the GNU Lesser General Public
** License version 2.1 as published by the Free Software Foundation and
** appearing in the file LICENSE.LGPL included in the packaging of this
** file. Please review the following information to ensure the GNU Lesser
** General Public License version 2.1 requirements will be met:
** http://www.gnu.org/licenses/old-licenses/lgpl-2.1.html.
**
** In addition, as a special exception, Nokia gives you certain additional
** rights. These rights are described in the Nokia Qt LGPL Exception
** version 1.1, included in the file LGPL_EXCEPTION.txt in this package.
**
** GNU General Public License Usage
** Alternatively, this file may be used under the terms of the GNU General
** Public License version 3.0 as published by the Free Software Foundation
** and appearing in the file LICENSE.GPL included in the packaging of this
** file. Please review the following information to ensure the GNU General
** Public License version 3.0 requirements will be met:
** http://www.gnu.org/copyleft/gpl.html.
**
** Other Usage
** Alternatively, this file may be used in accordance with the terms and
** conditions contained in a signed written agreement between you and Nokia.
**
**
**
**
**
**
** $QT_END_LICENSE$
**
****************************************************************************/

#include "qdeclarativecustomparser_p.h"
#include "qdeclarativecustomparser_p_p.h"

#include "qdeclarativecompiler_p.h"

#include <QtCore/qdebug.h>

QT_BEGIN_NAMESPACE

using namespace QDeclarativeScript;

/*!
    \class QDeclarativeCustomParser
    \brief The QDeclarativeCustomParser class allows you to add new arbitrary types to QML.
    \internal

    By subclassing QDeclarativeCustomParser, you can add a parser for
    building a particular type.

    The subclass must implement compile() and setCustomData(), and register
    itself in the meta type system by calling the macro:

    \code
    QML_REGISTER_CUSTOM_TYPE(Module, MajorVersion, MinorVersion, Name, TypeClass, ParserClass)
    \endcode
*/

/*
    \fn QByteArray QDeclarativeCustomParser::compile(const QList<QDeclarativeCustomParserProperty> & properties)

    The custom parser processes \a properties, and returns
    a QByteArray containing data meaningful only to the
    custom parser; the type engine will pass this same data to
    setCustomData() when making an instance of the data.

    Errors must be reported via the error() functions.

    The QByteArray may be cached between executions of the system, so
    it must contain correctly-serialized data (not, for example,
    pointers to stack objects).
*/

/*
    \fn void QDeclarativeCustomParser::setCustomData(QObject *object, const QByteArray &data)

    This function sets \a object to have the properties defined
    by \a data, which is a block of data previously returned by a call
    to compile().

    Errors should be reported using qmlInfo(object).

    The \a object will be an instance of the TypeClass specified by QML_REGISTER_CUSTOM_TYPE.
*/

QDeclarativeCustomParserNode 
QDeclarativeCustomParserNodePrivate::fromObject(QDeclarativeScript::Object *root)
{
    QDeclarativeCustomParserNode rootNode;
    rootNode.d->name = root->typeName;
    rootNode.d->location = root->location.start;

    for (Property *p = root->properties.first(); p; p = root->properties.next(p)) {
        rootNode.d->properties << fromProperty(p);
    }

    if (root->defaultProperty)
        rootNode.d->properties << fromProperty(root->defaultProperty);

    return rootNode;
}

QDeclarativeCustomParserProperty 
QDeclarativeCustomParserNodePrivate::fromProperty(QDeclarativeScript::Property *p)
{
    QDeclarativeCustomParserProperty prop;
    prop.d->name = p->name().toString();
    prop.d->isList = p->values.isMany();
    prop.d->location = p->location.start;

    if (p->value) {
        QDeclarativeCustomParserNode node = fromObject(p->value);
        QList<QDeclarativeCustomParserProperty> props = node.properties();
        for (int ii = 0; ii < props.count(); ++ii)
            prop.d->values << QVariant::fromValue(props.at(ii));
    } else {
        for (QDeclarativeScript::Value *v = p->values.first(); v; v = p->values.next(v)) {
            v->type = QDeclarativeScript::Value::Literal;

            if(v->object) {
                QDeclarativeCustomParserNode node = fromObject(v->object);
                prop.d->values << QVariant::fromValue(node);
            } else {
                prop.d->values << QVariant::fromValue(v->value);
            }

        }
    }

    return prop;
}

QDeclarativeCustomParserNode::QDeclarativeCustomParserNode()
: d(new QDeclarativeCustomParserNodePrivate)
{
}

QDeclarativeCustomParserNode::QDeclarativeCustomParserNode(const QDeclarativeCustomParserNode &other)
: d(new QDeclarativeCustomParserNodePrivate)
{
    *this = other;
}

QDeclarativeCustomParserNode &QDeclarativeCustomParserNode::operator=(const QDeclarativeCustomParserNode &other)
{
    d->name = other.d->name;
    d->properties = other.d->properties;
    d->location = other.d->location;
    return *this;
}

QDeclarativeCustomParserNode::~QDeclarativeCustomParserNode()
{
    delete d; d = 0;
}

QString QDeclarativeCustomParserNode::name() const
{
    return d->name;
}

QList<QDeclarativeCustomParserProperty> QDeclarativeCustomParserNode::properties() const
{
    return d->properties;
}

QDeclarativeScript::Location QDeclarativeCustomParserNode::location() const
{
    return d->location;
}

QDeclarativeCustomParserProperty::QDeclarativeCustomParserProperty()
: d(new QDeclarativeCustomParserPropertyPrivate)
{
}

QDeclarativeCustomParserProperty::QDeclarativeCustomParserProperty(const QDeclarativeCustomParserProperty &other)
: d(new QDeclarativeCustomParserPropertyPrivate)
{
    *this = other;
}

QDeclarativeCustomParserProperty &QDeclarativeCustomParserProperty::operator=(const QDeclarativeCustomParserProperty &other)
{
    d->name = other.d->name;
    d->isList = other.d->isList;
    d->values = other.d->values;
    d->location = other.d->location;
    return *this;
}

QDeclarativeCustomParserProperty::~QDeclarativeCustomParserProperty()
{
    delete d; d = 0;
}

QString QDeclarativeCustomParserProperty::name() const
{
    return d->name;
}

bool QDeclarativeCustomParserProperty::isList() const
{
    return d->isList;
}

QDeclarativeScript::Location QDeclarativeCustomParserProperty::location() const
{
    return d->location;
}

QList<QVariant> QDeclarativeCustomParserProperty::assignedValues() const
{
    return d->values;
}

void QDeclarativeCustomParser::clearErrors()
{
    exceptions.clear();
}

/*!
    Reports an error with the given \a description.

    This can only be used during the compile() step. For errors during setCustomData(), use qmlInfo().

    An error is generated referring to the position of the element in the source file.
*/
void QDeclarativeCustomParser::error(const QString& description)
{
    Q_ASSERT(object);
    QDeclarativeError error;
    QString exceptionDescription;
    error.setLine(object->location.start.line);
    error.setColumn(object->location.start.column);
    error.setDescription(description);
    exceptions << error;
}

/*!
    Reports an error in parsing \a prop, with the given \a description.

    An error is generated referring to the position of \a node in the source file.
*/
void QDeclarativeCustomParser::error(const QDeclarativeCustomParserProperty& prop, const QString& description)
{
    QDeclarativeError error;
    QString exceptionDescription;
    error.setLine(prop.location().line);
    error.setColumn(prop.location().column);
    error.setDescription(description);
    exceptions << error;
}

/*!
    Reports an error in parsing \a node, with the given \a description.

    An error is generated referring to the position of \a node in the source file.
*/
void QDeclarativeCustomParser::error(const QDeclarativeCustomParserNode& node, const QString& description)
{
    QDeclarativeError error;
    QString exceptionDescription;
    error.setLine(node.location().line);
    error.setColumn(node.location().column);
    error.setDescription(description);
    exceptions << error;
}

/*!
    If \a script is a simply enum expression (eg. Text.AlignLeft),
    returns the integer equivalent (eg. 1).

    Otherwise, returns -1.
*/
int QDeclarativeCustomParser::evaluateEnum(const QByteArray& script) const
{
    return compiler->evaluateEnum(script);
}

/*!
    Resolves \a name to a type, or 0 if it is not a type. This can be used
    to type-check object nodes.
*/
const QMetaObject *QDeclarativeCustomParser::resolveType(const QString& name) const
{
    return compiler->resolveType(name);
}

/*!
    Rewrites \a value and returns an identifier that can be
    used to construct the binding later. \a name
    is used as the name of the rewritten function.
*/
QDeclarativeBinding::Identifier QDeclarativeCustomParser::rewriteBinding(const QDeclarativeScript::Variant &value, const QString& name)
{
    return compiler->rewriteBinding(value, name);
}

/*!
    Returns a rewritten \a handler. \a name
    is used as the name of the rewritten function.
*/
QString QDeclarativeCustomParser::rewriteSignalHandler(const QDeclarativeScript::Variant &value, const QString &name)
{
    return compiler->rewriteSignalHandler(value , name);
}

QT_END_NAMESPACE
