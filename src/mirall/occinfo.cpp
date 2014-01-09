// This file is generated by kxml_compiler from occinfo.xml.
// All changes you do to this file will be lost.

#include "occinfo.h"

#include <QtDebug>
#include <QFile>
#include <QDomDocument>
#include <QtCore/QtDebug>
#include <QtCore/QFile>

namespace Mirall {

void Owncloudclient::setVersion( const QString &v )
{
  mVersion = v;
}

QString Owncloudclient::version() const
{
  return mVersion;
}

void Owncloudclient::setVersionstring( const QString &v )
{
  mVersionstring = v;
}

QString Owncloudclient::versionstring() const
{
  return mVersionstring;
}

void Owncloudclient::setWeb( const QString &v )
{
  mWeb = v;
}

QString Owncloudclient::web() const
{
  return mWeb;
}

Owncloudclient Owncloudclient::parseElement( const QDomElement &element, bool *ok )
{
  if ( element.tagName() != QLatin1String("owncloudclient") ) {
    qCritical() << "Expected 'owncloudclient', got '" << element.tagName() << "'.";
    if ( ok ) *ok = false;
    return Owncloudclient();
  }

  Owncloudclient result = Owncloudclient();

  QDomNode n;
  for( n = element.firstChild(); !n.isNull(); n = n.nextSibling() ) {
    QDomElement e = n.toElement();
    if ( e.tagName() == QLatin1String("version") ) {
      result.setVersion( e.text() );
    }
    else if ( e.tagName() == QLatin1String("versionstring") ) {
      result.setVersionstring( e.text() );
    }
    else if ( e.tagName() == QLatin1String("web") ) {
      result.setWeb( e.text() );
    }
  }


  if ( ok ) *ok = true;
  return result;
}

void Owncloudclient::writeElement( QXmlStreamWriter &xml )
{
  xml.writeStartElement( QLatin1String("owncloudclient") );
  if ( !version().isEmpty() ) {
    xml.writeTextElement(  QLatin1String("version"), version() );
  }
  if ( !versionstring().isEmpty() ) {
    xml.writeTextElement(  QLatin1String("versionstring"), versionstring() );
  }
  if ( !web().isEmpty() ) {
    xml.writeTextElement(  QLatin1String("web"), web() );
  }
  xml.writeEndElement();
}

Owncloudclient Owncloudclient::parseFile( const QString &filename, bool *ok )
{
  QFile file( filename );
  if ( !file.open( QIODevice::ReadOnly ) ) {
    qCritical() << "Unable to open file '" << filename << "'";
    if ( ok ) *ok = false;
    return Owncloudclient();
  }

  QString errorMsg;
  int errorLine, errorCol;
  QDomDocument doc;
  if ( !doc.setContent( &file, false, &errorMsg, &errorLine, &errorCol ) ) {
    qCritical() << errorMsg << " at " << errorLine << "," << errorCol;
    if ( ok ) *ok = false;
    return Owncloudclient();
  }

  bool documentOk;
  Owncloudclient c = parseElement( doc.documentElement(), &documentOk );
  if ( ok ) {
    *ok = documentOk;
  }
  return c;
}

Owncloudclient Owncloudclient::parseString( const QString &xml, bool *ok )
{
  QString errorMsg;
  int errorLine, errorCol;
  QDomDocument doc;
  if ( !doc.setContent( xml, false, &errorMsg, &errorLine, &errorCol ) ) {
    qCritical() << errorMsg << " at " << errorLine << "," << errorCol;
    if ( ok ) *ok = false;
    return Owncloudclient();
  }

  bool documentOk;
  Owncloudclient c = parseElement( doc.documentElement(), &documentOk );
  if ( ok ) {
    *ok = documentOk;
  }
  return c;
}

bool Owncloudclient::writeFile( const QString &filename )
{
  QFile file( filename );
  if ( !file.open( QIODevice::WriteOnly ) ) {
    qCritical() << "Unable to open file '" << filename << "'";
    return false;
  }

  QXmlStreamWriter xml( &file );
  xml.setAutoFormatting( true );
  xml.setAutoFormattingIndent( 2 );
  xml.writeStartDocument( QLatin1String("1.0") );
  writeElement( xml );
  xml.writeEndDocument();
  file.close();

  return true;
}

} // namespace Mirall