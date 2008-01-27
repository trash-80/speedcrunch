// This file is part of the SpeedCrunch project
// Copyright (C) 2004 Ariya Hidayat <ariya@kde.org>
// Copyright (C) 2007-2008 Helder Correia <helder.pereira.correia@gmail.com>
//
// This program is free software; you can redistribute it and/or
// modify it under the terms of the GNU General Public License
// as published by the Free Software Foundation; either version 2
// of the License, or (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program; see the file COPYING.  If not, write to
// the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
// Boston, MA 02110-1301, USA.


#include "3rdparty/util/binreloc.h"
#include "gui/mainwindow.hxx"

#include <QApplication>
#include <QDir>
#include <QFileInfo>
#include <QLocale>
#include <QTextCodec>
#include <QTranslator>


QTranslator * createTranslator()
{
  QString locale = QLocale().name();
  QString localeShort = locale.left( 2 ).toLower();

  bool foundTranslator = false;

  QString qmFile;
  QFileInfo fi;
  QTranslator* translator = 0;

#ifdef Q_OS_WIN32
  if ( ! foundTranslator )
  {
    qmFile = QString("crunch_") + locale + ".qm";
    fi = QFileInfo( qmFile );
    if( fi.exists() )
    {
      translator = new QTranslator( 0 );
      translator->load( qmFile );

      foundTranslator = true;
    }
  }

  if ( ! foundTranslator )
  {
    qmFile = QString("crunch_") + localeShort + ".qm";
    fi = QFileInfo( qmFile );
    if ( fi.exists() )
    {
      translator = new QTranslator( 0 );
      translator->load( qmFile );

      foundTranslator = true;
    }
  }
#endif // Q_OS_WIN32

    BrInitError error;

    if ( br_init( &error ) == 0 && error != BR_INIT_ERROR_DISABLED)
    {
        printf( "Warning: BinReloc failed to initialize (error code %d)\n", error );
        printf( "Will fallback to hardcoded default path.\n" );
    }

    // Search with the following order:
    // (1) install prefix + share/crunch, e.g. "/usr/local/share/crunch"
    // (2) install prefix + share, e.g. "/usr/local/share"
    // (3) current directory

    // item (1)
    QString shareDir = QString(br_find_data_dir(0)).append("/crunch");
    QDir qmPath( shareDir );
    qmPath.cd( "crunch" );

    if ( ! foundTranslator )
    {
      qmFile = qmPath.absolutePath() + "/crunch_" + locale + ".qm";
      fi = QFileInfo( qmFile );
      if ( fi.exists() )
      {
        translator = new QTranslator( 0 );
        translator->load( qmFile );
        foundTranslator = true;
      }
    }

    if ( ! foundTranslator )
    {
      qmFile = qmPath.absolutePath() + "/crunch_" + localeShort + ".qm";
      fi = QFileInfo( qmFile );
      if( fi.exists() )
      {
        translator = new QTranslator( 0 );
        translator->load( qmFile );
        foundTranslator = true;
       }
    }

    // item (2), fallback is item (3)
    qmPath = QDir( br_find_data_dir(".") );

    if ( ! foundTranslator )
    {
      qmFile = qmPath.absolutePath() + "/crunch_" + locale + ".qm";
      fi = QFileInfo( qmFile );
      if ( fi.exists() )
      {
        translator = new QTranslator( 0 );
        translator->load( qmFile );
        foundTranslator = true;
      }
    }

    if ( ! foundTranslator )
    {
      qmFile = qmPath.absolutePath() + "/crunch_" + localeShort + ".qm";
      fi = QFileInfo( qmFile );
      if ( fi.exists() )
      {
        translator = new QTranslator( 0 );
        translator->load( qmFile );
        foundTranslator = true;
      }
    }

    if ( foundTranslator )
      return translator;
    else
      return new QTranslator( 0 );
}


int main( int argc, char * * argv )
{
  QApplication app( argc, argv );
  app.installTranslator( createTranslator() );

  MainWindow * v = new MainWindow();
  v->show();

  //a.connect( &a, SIGNAL(lastWindowClosed()), &a, SLOT(quit()) );
  app.connect( v, SIGNAL( quitApplication() ), &app, SLOT( quit() ) );
  return app.exec();
}
