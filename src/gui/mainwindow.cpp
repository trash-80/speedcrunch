// This file is part of the SpeedCrunch project
// Copyright (C) 2004 Ariya Hidayat <ariya@kde.org>
// Copyright (C) 2005-2006 Johan Thelin <e8johan@gmail.com>
// Copyright (C) 2007 Ariya Hidayat <ariya@kde.org>
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


#include "mainwindow.hxx"

#include "aboutbox.hxx"
#include "autohidelabel.hxx"
#include "bookdock.hxx"
#include "constantsdock.hxx"
#include "deletevardlg.hxx"
#include "editor.hxx"
#include "functionsdock.hxx"
#include "historydock.hxx"
#include "insertfunctiondlg.hxx"
#include "insertvardlg.hxx"
#include "keypad.hxx"
#include "resultdisplay.hxx"
#include "tipwidget.hxx"
#include "variablesdock.hxx"

#include "3rdparty/util/binreloc.h"
#include "base/constants.hxx"
#include "base/evaluator.hxx"
#include "base/functions.hxx"
#include "base/settings.hxx"
#include "math/hmath.hxx"

#include <QAction>
#include <QActionGroup>
#include <QApplication>
#include <QClipboard>
#include <QCloseEvent>
#include <QDesktopServices>
#include <QFile>
#include <QFileDialog>
#include <QHBoxLayout>
#include <QLabel>
#include <QLocale>
#include <QMainWindow>
#include <QMenu>
#include <QMenuBar>
#include <QMessageBox>
#include <QPixmap>
#include <QPushButton>
#include <QRadioButton>
#include <QScrollBar>
#include <QShortcut>
#include <QStatusBar>
#include <QSystemTrayIcon>
#include <QTextStream>
#include <QTimer>
#include <QToolTip>
#include <QTranslator>
#include <QUrl>
#include <QVBoxLayout>
#include <QWidget>


struct Actions
{
  QAction * clearExpression;
  QAction * clearHistory;
  QAction * degree;
  QAction * deleteAllVariables;
  QAction * deleteVariable;
  QAction * digits15;
  QAction * digits2;
  QAction * digits3;
  QAction * digits50;
  QAction * digits8;
  QAction * digitsAuto;
  QAction * editCopy;
  QAction * editCopyResult;
  QAction * editPaste;
  QAction * helpAbout;
  QAction * helpAboutQt;
  QAction * helpGotoWebsite;
  QAction * helpTipOfTheDay;
  QAction * insertFunction;
  QAction * insertVariable;
  QAction * optionAlwaysOnTop;
  QAction * optionAutoCalc;
  QAction * optionAutoCompletion;
  QAction * optionDotAuto;
  QAction * optionDotComma;
  QAction * optionDotPoint;
  QAction * optionMinimizeToTray;
  QAction * optionHiliteSyntax;
  QAction * radian;
  QAction * radixCharAuto;
  QAction * radixCharComma;
  QAction * radixCharDot;
  QAction * scrollDown;
  QAction * scrollUp;
  QAction * selectExpression;
  QAction * sessionImport;
  QAction * sessionLoad;
  QAction * sessionQuit;
  QAction * sessionSave;
  QAction * sessionExport;
  QAction * showBook;
  QAction * showConstants;
  QAction * showFullScreen;
  QAction * showFunctions;
  QAction * showHistory;
  QAction * showKeypad;
  QAction * showMenuBar;
  QAction * showStatusBar;
  QAction * showVariables;
  QAction * formatBinary;
  QAction * formatEngineering;
  QAction * formatFixed;
  QAction * formatGeneral;
  QAction * formatHexadec;
  QAction * formatOctal;
  QAction * formatScientific;
  QAction * languageDefault;
  QAction * languageCa;
  QAction * languageCs;
  QAction * languageDe;
  QAction * languageEn;
  QAction * languageEs;
  QAction * languageEsAr;
  QAction * languageEu;
  QAction * languageFi;
  QAction * languageFr;
  QAction * languageHe;
  QAction * languageId;
  QAction * languageIt;
  QAction * languageNb;
  QAction * languageNl;
  QAction * languagePl;
  QAction * languagePt;
  QAction * languagePtBr;
  QAction * languageRo;
  QAction * languageRu;
  QAction * languageSv;
  QAction * languageTr;
  QAction * languageZhCn;
};


struct ActionGroups
{
  QActionGroup * angle;
  QActionGroup * digits;
  QActionGroup * format;
  QActionGroup * language;
  QActionGroup * radixChar;
};


struct Menus
{
  QMenu * angle;
  QMenu * behavior;
  QMenu * decimal;
  QMenu * edit;
  QMenu * format;
  QMenu * help;
  QMenu * language;
  QMenu * radixChar;
  QMenu * session;
  QMenu * settings;
  QMenu * trayIcon;
  QMenu * view;
};


struct Layouts
{
  QVBoxLayout * root;
  QHBoxLayout * keypad;
};


struct Widgets
{
  QWidget *         root;
  ResultDisplay *   display;
  Editor *          editor;
  Keypad *          keypad;
  AutoHideLabel *   autoCalcLabel;
  TipWidget *       tip;
  QSystemTrayIcon * trayIcon;
};


struct Docks
{
  BookDock *      book;
  ConstantsDock * constants;
  FunctionsDock * functions;
  HistoryDock *   history;
  VariablesDock * variables;
};


struct Conditions
{
  bool notifyMenuBarHidden;
  bool trayNotify;
  bool autoAns;
};


struct Status
{
  QLabel * angleUnit;
  QLabel * format;
};


struct MainWindow::Private
{
  MainWindow *    p;
  QTranslator *   translator;
  Constants *     constants;
  Evaluator *     evaluator;
  Functions *     functions;
  Settings        settings;
  Actions         actions;
  ActionGroups    actionGroups;
  Menus           menus;
  Layouts         layouts;
  Widgets         widgets;
  Docks           docks;
  Conditions      conditions;
  Status          status;

  Private();
  ~Private();
  void createUi();
  void createActions();
  void createActionGroups();
  void createActionShortcuts();
  void createMenus();
  void createStatusBar();
  void createFixedWidgets();
  void createKeypad();
  void createBookDock();
  void createConstantsDock();
  void createFunctionsDock();
  void createHistoryDock();
  void createVariablesDock();
  void createFixedConnections();
  void applySettings();
  void checkInitialFormat();
  void checkInitialPrecision();
  void checkInitialLanguage();
  void restoreFloatingDocks();
  void restoreHistory();
  void restoreVariables();
  void deleteKeypad();
  void deleteStatusBar();
  void deleteBookDock();
  void deleteConstantsDock();
  void deleteFunctionsDock();
  void deleteHistoryDock();
  void deleteVariablesDock();
  void syncWindowStateToSettings();
  void saveSettings();
  void setActionsText();
  void setMenusText();
  void setStatusBarText();

  static QTranslator * createTranslator( const QString & langCode );
};


MainWindow::Private::Private()
{
  translator = 0;

  widgets.keypad   = 0;
  widgets.trayIcon = 0;

  menus.trayIcon = 0;

  conditions.autoAns             = false;
  conditions.trayNotify          = true;
  conditions.notifyMenuBarHidden = true;

  docks.book      = 0;
  docks.history   = 0;
  docks.constants = 0;
  docks.functions = 0;
  docks.variables = 0;

  status.angleUnit = 0;
  status.format    = 0;

  Settings::settings = &settings;
};


MainWindow::Private::~Private()
{
  if ( widgets.trayIcon ) widgets.trayIcon->hide();
  if ( docks.book       ) deleteBookDock();
  if ( docks.constants  ) deleteConstantsDock();
  if ( docks.variables  ) deleteVariablesDock();
  if ( docks.functions  ) deleteFunctionsDock();
  if ( docks.history    ) deleteHistoryDock();
  Settings::settings = 0;
}


QTranslator * MainWindow::Private::createTranslator( const QString & langCode )
{
  QTranslator * translator = new QTranslator;
  QString localeFile = (langCode == "C") ? QLocale().name() : langCode;

  // fallback to English
  if ( localeFile == "C" )
    localeFile = "en";

#if defined(Q_OS_WIN32) || defined(Q_OS_MAC)
  QString appPath = QApplication::applicationFilePath();
  int ii = appPath.lastIndexOf( '/' );
  if ( ii > 0 )
      appPath.remove( ii, appPath.length() );
  QString fname = appPath + '/' + QString( "locale/" ) + localeFile;
  translator->load( fname );
  return translator;
#else

  BrInitError error;
  if ( br_init( & error ) == 0 && error != BR_INIT_ERROR_DISABLED )
  {
      qDebug( "Warning: BinReloc failed to initialize (error code %d)", error );
      qDebug( "Will fallback to hardcoded default path." );
  }

  char * dataDir = br_find_data_dir( 0 );
  QString localeDir = QString( dataDir ) + "/speedcrunch/locale";
  free( dataDir );
  if ( translator->load( localeFile, localeDir ) )
    return translator;
  else
    return 0;
#endif
}


void MainWindow::Private::createUi()
{
  createActions();
  createActionGroups();
  createActionShortcuts();
  createMenus();
  createFixedWidgets();
  createFixedConnections();

  p->setWindowTitle( "SpeedCrunch" );
  p->setWindowIcon( QPixmap( ":/speedcrunch.png" ) );
}


void MainWindow::Private::createActions()
{
  actions.clearExpression       = new QAction( p );
  actions.clearHistory          = new QAction( p );
  actions.degree                = new QAction( p );
  actions.deleteAllVariables    = new QAction( p );
  actions.deleteVariable        = new QAction( p );
  actions.digits15              = new QAction( p );
  actions.digits2               = new QAction( p );
  actions.digits3               = new QAction( p );
  actions.digits50              = new QAction( p );
  actions.digits8               = new QAction( p );
  actions.digitsAuto            = new QAction( p );
  actions.editCopy              = new QAction( p );
  actions.editCopyResult        = new QAction( p );
  actions.editPaste             = new QAction( p );
  actions.helpAbout             = new QAction( p );
  actions.helpAboutQt           = new QAction( p );
  actions.helpGotoWebsite       = new QAction( p );
  actions.helpTipOfTheDay       = new QAction( p );
  actions.insertFunction        = new QAction( p );
  actions.insertVariable        = new QAction( p );
  actions.optionAutoCalc        = new QAction( p );
  actions.optionAutoCompletion  = new QAction( p );
  actions.optionAlwaysOnTop     = new QAction( p );
  actions.optionMinimizeToTray  = new QAction( p );
  actions.optionHiliteSyntax    = new QAction( p );
  actions.radixCharAuto         = new QAction( p );
  actions.radixCharDot          = new QAction( p );
  actions.radixCharComma        = new QAction( p );
  actions.radian                = new QAction( p );
  actions.scrollDown            = new QAction( p );
  actions.scrollUp              = new QAction( p );
  actions.selectExpression      = new QAction( p );
  actions.sessionImport         = new QAction( p );
  actions.sessionLoad           = new QAction( p );
  actions.sessionQuit           = new QAction( p );
  actions.sessionSave           = new QAction( p );
  actions.sessionExport         = new QAction( p );
  actions.showBook              = new QAction( p );
  actions.showConstants         = new QAction( p );
  actions.showFullScreen        = new QAction( p );
  actions.showFunctions         = new QAction( p );
  actions.showHistory           = new QAction( p );
  actions.showKeypad            = new QAction( p );
  actions.showMenuBar           = new QAction( p );
  actions.showStatusBar         = new QAction( p );
  actions.showVariables         = new QAction( p );
  actions.formatBinary          = new QAction( p );
  actions.formatEngineering     = new QAction( p );
  actions.formatFixed           = new QAction( p );
  actions.formatGeneral         = new QAction( p );
  actions.formatHexadec         = new QAction( p );
  actions.formatOctal           = new QAction( p );
  actions.formatScientific      = new QAction( p );
  actions.languageDefault       = new QAction( p );

  actions.languageCa   = new QAction( QString::fromUtf8( "Català"              ), p );
  actions.languageCs   = new QAction( QString::fromUtf8( "Česky"               ), p );
  actions.languageDe   = new QAction( QString::fromUtf8( "Deutsch"             ), p );
  actions.languageEn   = new QAction( QString::fromUtf8( "English"             ), p );
  actions.languageEs   = new QAction( QString::fromUtf8( "Español"             ), p );
  actions.languageEsAr = new QAction( QString::fromUtf8( "Español Argentino"   ), p );
  actions.languageEu   = new QAction( QString::fromUtf8( "Euskara"             ), p );
  actions.languageFi   = new QAction( QString::fromUtf8( "Suomi"               ), p );
  actions.languageFr   = new QAction( QString::fromUtf8( "Français"            ), p );
  actions.languageHe   = new QAction( QString::fromUtf8( "עברית"               ), p );
  actions.languageId   = new QAction( QString::fromUtf8( "Bahasa Indonesia"    ), p );
  actions.languageIt   = new QAction( QString::fromUtf8( "Italiano"            ), p );
  actions.languageNb   = new QAction( QString::fromUtf8( "Norsk (Bokmål)"      ) + QChar( 0x200E ), p );
  actions.languageNl   = new QAction( QString::fromUtf8( "Nederlands"          ), p );
  actions.languagePl   = new QAction( QString::fromUtf8( "Polski"              ), p );
  actions.languagePt   = new QAction( QString::fromUtf8( "Português"           ), p );
  actions.languagePtBr = new QAction( QString::fromUtf8( "Português do Brasil" ), p );
  actions.languageRo   = new QAction( QString::fromUtf8( "Română"              ), p );
  actions.languageRu   = new QAction( QString::fromUtf8( "Русский"             ), p );
  actions.languageSv   = new QAction( QString::fromUtf8( "Svenska"             ), p );
  actions.languageTr   = new QAction( QString::fromUtf8( "Türkçe"              ), p );
  actions.languageZhCn = new QAction( QString::fromUtf8( "简化字"              ), p );

  actions.languageDefault->setData( QString( "C"     ) );
  actions.languageCa     ->setData( QString( "ca"    ) );
  actions.languageCs     ->setData( QString( "cs"    ) );
  actions.languageDe     ->setData( QString( "de"    ) );
  actions.languageEn     ->setData( QString( "en"    ) );
  actions.languageEs     ->setData( QString( "es"    ) );
  actions.languageEsAr   ->setData( QString( "es_AR" ) );
  actions.languageEu     ->setData( QString( "eu"    ) );
  actions.languageFi     ->setData( QString( "fi"    ) );
  actions.languageFr     ->setData( QString( "fr"    ) );
  actions.languageHe     ->setData( QString( "he"    ) );
  actions.languageId     ->setData( QString( "id"    ) );
  actions.languageIt     ->setData( QString( "it"    ) );
  actions.languageNb     ->setData( QString( "nb"    ) );
  actions.languageNl     ->setData( QString( "nl"    ) );
  actions.languagePl     ->setData( QString( "pl"    ) );
  actions.languagePt     ->setData( QString( "pt"    ) );
  actions.languagePtBr   ->setData( QString( "pt_BR" ) );
  actions.languageRo     ->setData( QString( "ro"    ) );
  actions.languageRu     ->setData( QString( "ru"    ) );
  actions.languageSv     ->setData( QString( "sv"    ) );
  actions.languageTr     ->setData( QString( "tr"    ) );
  actions.languageZhCn   ->setData( QString( "zh_CN" ) );

  actions.degree              ->setCheckable( true );
  actions.digits15            ->setCheckable( true );
  actions.digits2             ->setCheckable( true );
  actions.digits3             ->setCheckable( true );
  actions.digits50            ->setCheckable( true );
  actions.digits8             ->setCheckable( true );
  actions.digitsAuto          ->setCheckable( true );
  actions.optionAlwaysOnTop   ->setCheckable( true );
  actions.optionAutoCalc      ->setCheckable( true );
  actions.optionAutoCompletion->setCheckable( true );
  actions.optionMinimizeToTray->setCheckable( true );
  actions.optionHiliteSyntax  ->setCheckable( true );
  actions.radian              ->setCheckable( true );
  actions.radixCharAuto       ->setCheckable( true );
  actions.radixCharComma      ->setCheckable( true );
  actions.radixCharDot        ->setCheckable( true );
  actions.showStatusBar       ->setCheckable( true );
  actions.showBook            ->setCheckable( true );
  actions.showConstants       ->setCheckable( true );
  actions.showFullScreen      ->setCheckable( true );
  actions.showFunctions       ->setCheckable( true );
  actions.showHistory         ->setCheckable( true );
  actions.showKeypad          ->setCheckable( true );
  actions.showVariables       ->setCheckable( true );
  actions.formatBinary        ->setCheckable( true );
  actions.formatEngineering   ->setCheckable( true );
  actions.formatFixed         ->setCheckable( true );
  actions.formatGeneral       ->setCheckable( true );
  actions.formatHexadec       ->setCheckable( true );
  actions.formatOctal         ->setCheckable( true );
  actions.formatScientific    ->setCheckable( true );
  actions.languageDefault     ->setCheckable( true );
  actions.languageCa          ->setCheckable( true );
  actions.languageCs          ->setCheckable( true );
  actions.languageDe          ->setCheckable( true );
  actions.languageEn          ->setCheckable( true );
  actions.languageEs          ->setCheckable( true );
  actions.languageEsAr        ->setCheckable( true );
  actions.languageEu          ->setCheckable( true );
  actions.languageFi          ->setCheckable( true );
  actions.languageFr          ->setCheckable( true );
  actions.languageHe          ->setCheckable( true );
  actions.languageId          ->setCheckable( true );
  actions.languageIt          ->setCheckable( true );
  actions.languageNb          ->setCheckable( true );
  actions.languageNl          ->setCheckable( true );
  actions.languagePl          ->setCheckable( true );
  actions.languagePt          ->setCheckable( true );
  actions.languagePtBr        ->setCheckable( true );
  actions.languageRo          ->setCheckable( true );
  actions.languageRu          ->setCheckable( true );
  actions.languageSv          ->setCheckable( true );
  actions.languageTr          ->setCheckable( true );
  actions.languageZhCn        ->setCheckable( true );
}


void MainWindow::setAllText()
{
  QTranslator * tr = 0;
  tr = d->createTranslator( d->settings.language );
  if ( tr )
  {
    if ( d->translator )
    {
      qApp->removeTranslator( d->translator );
      delete d->translator;
    }

    qApp->installTranslator( tr );
    d->translator = tr;
  }
  else
  {
    qApp->removeTranslator( d->translator );
    d->translator = 0;
  }

  d->setMenusText();
  d->setActionsText();
  d->setStatusBarText();

  d->functions->retranslateText(); // can't be signal/slot to ensure order
  d->constants->retranslateText(); // same as above

  if ( d->docks.book )
  {
    d->docks.book->setLanguage( d->settings.language );
    d->docks.book->setWindowTitle( MainWindow::tr( "Math Book" ) );
  }

  // layout direction
  setWidgetsDirection();
}


void MainWindow::Private::setStatusBarText()
{
  if ( status.angleUnit )
  {
    QString angleUnit = (settings.angleMode == 'r') ?
      MainWindow::tr( "Radian" ) : MainWindow::tr( "Degree" );

    QString format;
    switch ( settings.format )
    {
      case 'b' : format = MainWindow::tr( "Binary"              ); break;
      case 'o' : format = MainWindow::tr( "Octal"               ); break;
      case 'h' : format = MainWindow::tr( "Hexadecimal"         ); break;
      case 'f' : format = MainWindow::tr( "Fixed decimal"       ); break;
      case 'n' : format = MainWindow::tr( "Engineering decimal" ); break;
      case 'e' : format = MainWindow::tr( "Scientific decimal"  ); break;
      case 'g' : format = MainWindow::tr( "General decimal"     ); break;
      default : break;
    }

    status.angleUnit->setText( angleUnit );
    status.format   ->setText( format    );

    status.angleUnit->setToolTip( MainWindow::tr( "Angle unit"    ) );
    status.format   ->setToolTip( MainWindow::tr( "Result format" ) );
  }
}


void MainWindow::Private::setActionsText()
{
  actions.clearExpression     ->setText( MainWindow::tr( "Clear E&xpression"        ) );
  actions.clearHistory        ->setText( MainWindow::tr( "Clear &History"           ) );
  actions.degree              ->setText( MainWindow::tr( "&Degree"                  ) );
  actions.deleteAllVariables  ->setText( MainWindow::tr( "Delete All V&ariables"    ) );
  actions.deleteVariable      ->setText( MainWindow::tr( "D&elete Variable..."      ) );
  actions.digits15            ->setText( MainWindow::tr( "&15 Decimal Digits"       ) );
  actions.digits2             ->setText( MainWindow::tr( "&2 Decimal Digits"        ) );
  actions.digits3             ->setText( MainWindow::tr( "&3 Decimal Digits"        ) );
  actions.digits50            ->setText( MainWindow::tr( "&50 Decimal Digits"       ) );
  actions.digits8             ->setText( MainWindow::tr( "&8 Decimal Digits"        ) );
  actions.digitsAuto          ->setText( MainWindow::tr( "&Automatic Precision"     ) );
  actions.editCopy            ->setText( MainWindow::tr( "&Copy"                    ) );
  actions.editCopyResult      ->setText( MainWindow::tr( "Copy Last &Result"        ) );
  actions.editPaste           ->setText( MainWindow::tr( "&Paste"                   ) );
  actions.helpAbout           ->setText( MainWindow::tr( "&About"                   ) );
  actions.helpAboutQt         ->setText( MainWindow::tr( "About &Qt"                ) );
  actions.helpGotoWebsite     ->setText( MainWindow::tr( "SpeedCrunch &Web Site..." ) );
  actions.helpTipOfTheDay     ->setText( MainWindow::tr( "&Tip of the Day"          ) );
  actions.insertFunction      ->setText( MainWindow::tr( "Insert &Function..."      ) );
  actions.insertVariable      ->setText( MainWindow::tr( "Insert &Variable..."      ) );
  actions.optionAutoCalc      ->setText( MainWindow::tr( "&Partial Results"         ) );
  actions.optionAutoCompletion->setText( MainWindow::tr( "Automatic &Completion"    ) );
  actions.optionAlwaysOnTop   ->setText( MainWindow::tr( "Always On &Top"           ) );
  actions.optionMinimizeToTray->setText( MainWindow::tr( "&Minimize To System Tray" ) );
  actions.optionHiliteSyntax  ->setText( MainWindow::tr( "Syntax &Highlighting"     ) );
  actions.radixCharAuto       ->setText( MainWindow::tr( "&System Default"          ) );
  actions.radixCharDot        ->setText( MainWindow::tr( "&Dot"                     ) );
  actions.radixCharComma      ->setText( MainWindow::tr( "&Comma"                   ) );
  actions.radian              ->setText( MainWindow::tr( "&Radian"                  ) );
  actions.scrollDown          ->setText( MainWindow::tr( "Scroll Display Down"      ) );
  actions.scrollUp            ->setText( MainWindow::tr( "Scroll Display Up"        ) );
  actions.selectExpression    ->setText( MainWindow::tr( "&Select Expression"       ) );
  actions.sessionImport       ->setText( MainWindow::tr( "&Import..."               ) );
  actions.sessionLoad         ->setText( MainWindow::tr( "&Load..."                 ) );
  actions.sessionQuit         ->setText( MainWindow::tr( "&Quit"                    ) );
  actions.sessionSave         ->setText( MainWindow::tr( "&Save..."                 ) );
  actions.sessionExport       ->setText( MainWindow::tr( "&Export..."               ) );
  actions.showBook            ->setText( MainWindow::tr( "Math &Book"               ) );
  actions.showConstants       ->setText( MainWindow::tr( "&Constants"               ) );
  actions.showFullScreen      ->setText( MainWindow::tr( "Full &Screen Mode"        ) );
  actions.showFunctions       ->setText( MainWindow::tr( "&Functions"               ) );
  actions.showHistory         ->setText( MainWindow::tr( "&History"                 ) );
  actions.showKeypad          ->setText( MainWindow::tr( "&Keypad"                  ) );
  actions.showMenuBar         ->setText( MainWindow::tr( "Hide &Menu Bar"           ) );
  actions.showStatusBar       ->setText( MainWindow::tr( "&Status Bar"              ) );
  actions.showVariables       ->setText( MainWindow::tr( "&Variables"               ) );
  actions.formatBinary        ->setText( MainWindow::tr( "&Binary"                  ) );
  actions.formatEngineering   ->setText( MainWindow::tr( "&Engineering"             ) );
  actions.formatFixed         ->setText( MainWindow::tr( "&Fixed Decimal"           ) );
  actions.formatGeneral       ->setText( MainWindow::tr( "&General"                 ) );
  actions.formatHexadec       ->setText( MainWindow::tr( "&Hexadecimal"             ) );
  actions.formatOctal         ->setText( MainWindow::tr( "&Octal"                   ) );
  actions.formatScientific    ->setText( MainWindow::tr( "&Scientific"              ) );
  actions.languageDefault     ->setText( MainWindow::tr( "System &Default"          ) );
}


void MainWindow::Private::createActionGroups()
{
  actionGroups.format = new QActionGroup( p );
  actionGroups.format->addAction( actions.formatBinary );
  actionGroups.format->addAction( actions.formatGeneral );
  actionGroups.format->addAction( actions.formatFixed );
  actionGroups.format->addAction( actions.formatEngineering );
  actionGroups.format->addAction( actions.formatScientific );
  actionGroups.format->addAction( actions.formatOctal );
  actionGroups.format->addAction( actions.formatHexadec );

  actionGroups.digits = new QActionGroup( p );
  actionGroups.digits->addAction( actions.digitsAuto );
  actionGroups.digits->addAction( actions.digits2 );
  actionGroups.digits->addAction( actions.digits3 );
  actionGroups.digits->addAction( actions.digits8 );
  actionGroups.digits->addAction( actions.digits15 );
  actionGroups.digits->addAction( actions.digits50 );

  actionGroups.angle = new QActionGroup( p );
  actionGroups.angle->addAction( actions.radian );
  actionGroups.angle->addAction( actions.degree );

  actionGroups.radixChar = new QActionGroup( p );
  actionGroups.radixChar->addAction( actions.radixCharAuto  );
  actionGroups.radixChar->addAction( actions.radixCharDot   );
  actionGroups.radixChar->addAction( actions.radixCharComma );

  actionGroups.language = new QActionGroup( p );
  actionGroups.language->addAction( actions.languageDefault );
  actionGroups.language->addAction( actions.languageCa );
  actionGroups.language->addAction( actions.languageCs );
  actionGroups.language->addAction( actions.languageDe );
  actionGroups.language->addAction( actions.languageEn );
  actionGroups.language->addAction( actions.languageEs );
  actionGroups.language->addAction( actions.languageEsAr );
  actionGroups.language->addAction( actions.languageEu );
  actionGroups.language->addAction( actions.languageFi );
  actionGroups.language->addAction( actions.languageFr );
  actionGroups.language->addAction( actions.languageHe );
  actionGroups.language->addAction( actions.languageId );
  actionGroups.language->addAction( actions.languageIt );
  actionGroups.language->addAction( actions.languageNb );
  actionGroups.language->addAction( actions.languageNl );
  actionGroups.language->addAction( actions.languagePl );
  actionGroups.language->addAction( actions.languagePt );
  actionGroups.language->addAction( actions.languagePtBr );
  actionGroups.language->addAction( actions.languageRo );
  actionGroups.language->addAction( actions.languageRu );
  actionGroups.language->addAction( actions.languageSv );
  actionGroups.language->addAction( actions.languageTr );
  actionGroups.language->addAction( actions.languageZhCn );
}


void MainWindow::Private::createActionShortcuts()
{
  actions.clearExpression ->setShortcut( Qt::Key_Escape                 );
  actions.clearHistory    ->setShortcut( Qt::CTRL + Qt::ALT + Qt::Key_N );
  actions.degree          ->setShortcut( Qt::Key_F10                    );
  actions.deleteVariable  ->setShortcut( Qt::CTRL + Qt::Key_D           );
  actions.editCopyResult  ->setShortcut( Qt::CTRL + Qt::Key_R           );
  actions.editCopy        ->setShortcut( Qt::CTRL + Qt::Key_C           );
  actions.editPaste       ->setShortcut( Qt::CTRL + Qt::Key_V           );
  actions.helpTipOfTheDay ->setShortcut( Qt::CTRL + Qt::Key_T           );
  actions.insertFunction  ->setShortcut( Qt::CTRL + Qt::Key_F           );
  actions.insertVariable  ->setShortcut( Qt::CTRL + Qt::Key_I           );
  actions.radian          ->setShortcut( Qt::Key_F9                     );
  actions.scrollDown      ->setShortcut( Qt::SHIFT + Qt::Key_PageDown   );
  actions.scrollUp        ->setShortcut( Qt::SHIFT + Qt::Key_PageUp     );
  actions.selectExpression->setShortcut( Qt::CTRL + Qt::Key_A           );
  actions.sessionLoad     ->setShortcut( Qt::CTRL + Qt::Key_L           );
  actions.sessionQuit     ->setShortcut( Qt::CTRL + Qt::Key_Q           );
  actions.sessionSave     ->setShortcut( Qt::CTRL + Qt::Key_S           );
  actions.showFullScreen  ->setShortcut( Qt::Key_F11                    );
  actions.showKeypad      ->setShortcut( Qt::CTRL + Qt::ALT + Qt::Key_K );
  actions.showBook        ->setShortcut( Qt::CTRL + Qt::Key_1           );
  actions.showConstants   ->setShortcut( Qt::CTRL + Qt::Key_2           );
  actions.showFunctions   ->setShortcut( Qt::CTRL + Qt::Key_3           );
  actions.showVariables   ->setShortcut( Qt::CTRL + Qt::Key_4           );
  actions.showHistory     ->setShortcut( Qt::CTRL + Qt::Key_5           );
#ifndef Q_OS_MAC
  actions.showMenuBar     ->setShortcut( Qt::CTRL + Qt::ALT + Qt::Key_M );
#endif
  actions.showStatusBar   ->setShortcut( Qt::CTRL + Qt::ALT + Qt::Key_B );
  actions.formatBinary    ->setShortcut( Qt::Key_F5                     );
  actions.formatGeneral   ->setShortcut( Qt::Key_F7                     );
  actions.formatHexadec   ->setShortcut( Qt::Key_F8                     );
  actions.formatOctal     ->setShortcut( Qt::Key_F6                     );
}


void MainWindow::Private::createMenus()
{
  // session
  menus.session = new QMenu( "", p );
  p->menuBar()->addMenu( menus.session );
  menus.session->addAction( actions.sessionLoad );
  menus.session->addAction( actions.sessionSave );
  menus.session->addSeparator();
  menus.session->addAction( actions.sessionImport );
  menus.session->addAction( actions.sessionExport );
  menus.session->addSeparator();
  menus.session->addAction( actions.sessionQuit );

  // edit
  menus.edit = new QMenu( "", p );
  p->menuBar()->addMenu( menus.edit );
  menus.edit->addAction( actions.editCopy );
  menus.edit->addAction( actions.editCopyResult );
  menus.edit->addAction( actions.editPaste );
  menus.edit->addAction( actions.selectExpression );
  menus.edit->addSeparator();
  menus.edit->addAction( actions.insertFunction );
  menus.edit->addAction( actions.insertVariable );
  menus.edit->addSeparator();
  menus.edit->addAction( actions.deleteVariable );
  menus.edit->addAction( actions.deleteAllVariables );
  menus.edit->addSeparator();
  menus.edit->addAction( actions.clearExpression );
  menus.edit->addAction( actions.clearHistory );

  // view
  menus.view = new QMenu( "", p );
  p->menuBar()->addMenu( menus.view );
  menus.view->addAction( actions.showKeypad );
  menus.view->addSeparator();
  menus.view->addAction( actions.showBook );
  menus.view->addAction( actions.showConstants );
  menus.view->addAction( actions.showFunctions );
  menus.view->addAction( actions.showVariables );
  menus.view->addAction( actions.showHistory );
  menus.view->addSeparator();
#ifndef Q_OS_MAC
  menus.view->addAction( actions.showMenuBar );
#endif
  menus.view->addAction( actions.showStatusBar );
  menus.view->addSeparator();
  menus.view->addAction( actions.showFullScreen );

  // settings
  menus.settings = new QMenu( "", p );
  p->menuBar()->addMenu( menus.settings );

  // settings / result format
  menus.format = menus.settings->addMenu( "" );
  menus.format->addAction( actions.formatBinary );
  menus.format->addAction( actions.formatOctal );

  // settings / result format / decimal
  menus.decimal = menus.format->addMenu( "" );
  menus.decimal->addAction( actions.formatGeneral );
  menus.decimal->addAction( actions.formatFixed );
  menus.decimal->addAction( actions.formatEngineering );
  menus.decimal->addAction( actions.formatScientific );
  menus.decimal->addSeparator();
  menus.decimal->addAction( actions.digitsAuto );
  menus.decimal->addAction( actions.digits2 );
  menus.decimal->addAction( actions.digits3 );
  menus.decimal->addAction( actions.digits8 );
  menus.decimal->addAction( actions.digits15 );
  menus.decimal->addAction( actions.digits50 );

  // settings / result format (continued)
  menus.format->addAction( actions.formatHexadec );

  // settings / angle mode
  menus.angle = menus.settings->addMenu( "" );
  menus.angle->addAction( actions.radian );
  menus.angle->addAction( actions.degree );

  // settings / behavior
  menus.behavior = menus.settings->addMenu( "" );
  menus.behavior->addAction( actions.optionAutoCalc       );
  menus.behavior->addAction( actions.optionAutoCompletion );
  menus.behavior->addAction( actions.optionHiliteSyntax   );
  menus.behavior->addAction( actions.optionAlwaysOnTop    );
  menus.behavior->addAction( actions.optionMinimizeToTray );

  // settings / radix character
  menus.radixChar = menus.settings->addMenu( "" );
  menus.radixChar->addAction( actions.radixCharAuto  );
  menus.radixChar->addSeparator();
  menus.radixChar->addAction( actions.radixCharDot   );
  menus.radixChar->addAction( actions.radixCharComma );

  // settings / language
  menus.language = menus.settings->addMenu( "" );
  menus.language->addAction( actions.languageDefault );
  menus.language->addSeparator();
  menus.language->addAction( actions.languageCa   );
  menus.language->addAction( actions.languageCs   );
  menus.language->addAction( actions.languageDe   );
  menus.language->addAction( actions.languageEn   );
  menus.language->addAction( actions.languageEs   );
  menus.language->addAction( actions.languageEsAr );
  menus.language->addAction( actions.languageEu   );
  menus.language->addAction( actions.languageFi   );
  menus.language->addAction( actions.languageFr   );
  menus.language->addAction( actions.languageHe   );
  menus.language->addAction( actions.languageId   );
  menus.language->addAction( actions.languageIt   );
  menus.language->addAction( actions.languageNb   );
  menus.language->addAction( actions.languageNl   );
  menus.language->addAction( actions.languagePl   );
  menus.language->addAction( actions.languagePt   );
  menus.language->addAction( actions.languagePtBr );
  menus.language->addAction( actions.languageRo   );
  menus.language->addAction( actions.languageRu   );
  menus.language->addAction( actions.languageSv   );
  menus.language->addAction( actions.languageTr   );
  menus.language->addAction( actions.languageZhCn );

  // help
  menus.help = new QMenu( "", p );
  p->menuBar()->addMenu( menus.help );
  menus.help->addAction( actions.helpTipOfTheDay );
  menus.help->addAction( actions.helpGotoWebsite );
  menus.help->addSeparator();
  menus.help->addAction( actions.helpAbout );
  menus.help->addAction( actions.helpAboutQt );

  // necessary after hiding the menu bar, so shortcuts still recognized
  p->addActions( p->menuBar()->actions() );
  p->addAction( actions.scrollDown );
  p->addAction( actions.scrollUp );
}


void MainWindow::Private::setMenusText()
{
  menus.session  ->setTitle( MainWindow::tr( "&Session"         ) );
  menus.edit     ->setTitle( MainWindow::tr( "&Edit"            ) );
  menus.view     ->setTitle( MainWindow::tr( "&View"            ) );
  menus.settings ->setTitle( MainWindow::tr( "Se&ttings"        ) );
  menus.format   ->setTitle( MainWindow::tr( "Result &Format"   ) );
  menus.decimal  ->setTitle( MainWindow::tr( "&Decimal"         ) );
  menus.angle    ->setTitle( MainWindow::tr( "&Angle Unit"      ) );
  menus.behavior ->setTitle( MainWindow::tr( "&Behavior"        ) );
  menus.radixChar->setTitle( MainWindow::tr( "Radix &Character" ) );
  menus.language ->setTitle( MainWindow::tr( "&Language"        ) );
  menus.help     ->setTitle( MainWindow::tr( "&Help"            ) );
}


void MainWindow::Private::createStatusBar()
{
  QStatusBar * bar = p->statusBar();

  status.angleUnit = new QLabel( bar );
  status.format    = new QLabel( bar );

  status.angleUnit->setFrameStyle( QFrame::StyledPanel | QFrame::Sunken );
  status.format   ->setFrameStyle( QFrame::StyledPanel | QFrame::Sunken );

  bar->addWidget( status.angleUnit );
  bar->addWidget( status.format    );

  setStatusBarText();
}


void MainWindow::Private::createFixedWidgets()
{
  // necessary objects
  constants = new Constants( p );
  functions = new Functions( settings.angleMode, p );
  evaluator = new Evaluator( functions, settings.getRadixChar(), p );

  // outer widget and layout
  widgets.root = new QWidget( p );
  p->setCentralWidget( widgets.root );

  layouts.root = new QVBoxLayout( widgets.root );
  layouts.root->setMargin( 0 );
  layouts.root->setSpacing( 0 );

  // display
  QHBoxLayout * displayLayout = new QHBoxLayout();
  displayLayout->setMargin( 5 );
  widgets.display = new ResultDisplay( settings.getRadixChar(), settings.format,
                                       settings.precision, widgets.root );
  displayLayout->addWidget( widgets.display );
  layouts.root->addLayout( displayLayout );

  // editor
  QHBoxLayout * editorLayout = new QHBoxLayout();
  editorLayout->setMargin( 5 );
  widgets.editor = new Editor( evaluator, functions, constants, settings.format,
                               settings.precision, settings.getRadixChar(),
                               widgets.root );
  widgets.editor->setFocus();
  QString editorStyle( "QTextEdit { font: bold %1pt }" );
  int editorPointSize = widgets.editor->font().pointSize();
  widgets.editor->setStyleSheet( editorStyle.arg( editorPointSize + 3 ) );
  widgets.editor->setFixedHeight( widgets.editor->sizeHint().height() );
  editorLayout->addWidget( widgets.editor );
  layouts.root->addLayout( editorLayout );

  // for autocalc
  widgets.autoCalcLabel = new AutoHideLabel( p );
  widgets.autoCalcLabel->hide();

  // for tip of the day and menu bar hiding message
  widgets.tip = new TipWidget( p );
  widgets.tip->hide();
}


void MainWindow::Private::createKeypad()
{
  widgets.keypad = new Keypad( settings.getRadixChar(), widgets.root );
  widgets.keypad->setFocusPolicy( Qt::NoFocus );

  connect( widgets.keypad, SIGNAL( buttonPressed( Keypad::Button ) ),
           p, SLOT( keypadButtonPressed( Keypad::Button ) ) );
  connect( p, SIGNAL( radixCharChanged( char ) ),
           widgets.keypad, SLOT( setRadixChar( char ) ) );
  connect( p, SIGNAL( retranslateText() ),
           widgets.keypad, SLOT( retranslateText() ) );

  layouts.keypad = new QHBoxLayout();
  layouts.keypad->addStretch();
  layouts.keypad->addWidget( widgets.keypad );
  layouts.keypad->addStretch();
  layouts.root->addLayout( layouts.keypad );

  widgets.keypad->show();
  widgets.display->scrollEnd();
}


void MainWindow::Private::createBookDock()
{
  QString booksDir;

#if defined(Q_OS_WIN32) || defined(Q_OS_MAC)
  QString appPath = QApplication::applicationFilePath();
  int ii = appPath.lastIndexOf( '/' );
  if ( ii > 0 )
      appPath.remove( ii, appPath.length() );
  booksDir = appPath + '/' + QString( "books/" );
#else
  BrInitError error;
  if ( br_init( & error ) == 0 && error != BR_INIT_ERROR_DISABLED )
  {
      qDebug( "Warning: BinReloc failed to initialize (error code %d)", error );
      qDebug( "Will fallback to hardcoded default path." );
  }

  char * dataDir = br_find_data_dir( 0 );
  booksDir = QString( dataDir ) + "/speedcrunch/books/";
  free( dataDir );
#endif

  docks.book = new BookDock( booksDir, "math_index.html", settings.language,
                             p );
  docks.book->setObjectName( "BookDock" );
  docks.book->setTitle( MainWindow::tr( "Math Book" ) );
  docks.book->installEventFilter( p );
  p->addDockWidget( Qt::RightDockWidgetArea, docks.book );

  connect( docks.book, SIGNAL( expressionSelected( const QString & ) ),
           p, SLOT( expressionSelected( const QString & ) ) );

  if ( docks.functions )
    p->tabifyDockWidget( docks.functions, docks.book );
  else if ( docks.variables )
    p->tabifyDockWidget( docks.variables, docks.book );
  else if ( docks.history )
    p->tabifyDockWidget( docks.history, docks.book );
  else if ( docks.constants )
    p->tabifyDockWidget( docks.constants, docks.book );

  settings.showBook = true;
}


void MainWindow::Private::createConstantsDock()
{
  docks.constants = new ConstantsDock( constants, settings.getRadixChar(), p );
  docks.constants->setObjectName( "ConstantsDock" );
  docks.constants->installEventFilter( p );
  p->addDockWidget( Qt::RightDockWidgetArea, docks.constants );

  connect( docks.constants, SIGNAL( constantSelected( const QString & ) ),
           p, SLOT( constantSelected( const QString & ) ) );
  connect( p, SIGNAL( radixCharChanged( char ) ),
           docks.constants, SLOT( setRadixChar( char ) ) );
  connect( p, SIGNAL( retranslateText() ),
           docks.constants, SLOT( retranslateText() ) );

  if ( docks.functions )
    p->tabifyDockWidget( docks.functions, docks.constants );
  else if ( docks.variables )
    p->tabifyDockWidget( docks.variables, docks.constants );
  else if ( docks.history )
    p->tabifyDockWidget( docks.history, docks.constants );
  else if ( docks.book )
    p->tabifyDockWidget( docks.book, docks.constants );

  settings.showConstants = true;
}


void MainWindow::Private::createFunctionsDock()
{
  docks.functions = new FunctionsDock( functions, p );
  docks.functions->setObjectName( "FunctionsDock" );
  docks.functions->installEventFilter( p );
  p->addDockWidget( Qt::RightDockWidgetArea, docks.functions );

  connect( docks.functions, SIGNAL( functionSelected( const QString & ) ),
           p, SLOT( functionSelected( const QString & ) ) );
  connect( p, SIGNAL( retranslateText() ),
           docks.functions, SLOT( retranslateText() ) );

  if ( docks.history )
    p->tabifyDockWidget( docks.history, docks.functions );
  else if ( docks.variables )
    p->tabifyDockWidget( docks.variables, docks.functions );
  else if ( docks.constants )
    p->tabifyDockWidget( docks.constants, docks.functions );
  else if ( docks.book )
    p->tabifyDockWidget( docks.book, docks.functions );

  settings.showFunctions = true;
}


void MainWindow::Private::createHistoryDock()
{
  docks.history = new HistoryDock( p );
  docks.history->setObjectName( "HistoryDock" );
  docks.history->installEventFilter( p );
  p->addDockWidget( Qt::RightDockWidgetArea, docks.history );

  connect( docks.history, SIGNAL( expressionSelected( const QString & ) ),
           p, SLOT( expressionSelected( const QString & ) ) );
  connect( p, SIGNAL( retranslateText() ),
           docks.history, SLOT( retranslateText() ) );

  docks.history->setHistory( widgets.editor->history() );

  if ( docks.functions )
    p->tabifyDockWidget( docks.functions, docks.history );
  else if ( docks.variables )
    p->tabifyDockWidget( docks.variables, docks.history );
  else if ( docks.constants )
    p->tabifyDockWidget( docks.constants, docks.history );
  else if ( docks.book )
    p->tabifyDockWidget( docks.book, docks.history );

  settings.showHistory = true;
}


void MainWindow::Private::createVariablesDock()
{
  docks.variables = new VariablesDock( settings.getRadixChar(), p );
  docks.variables->setObjectName( "VariablesDock" );
  docks.variables->installEventFilter( p );
  p->addDockWidget( Qt::RightDockWidgetArea, docks.variables );

  connect( docks.variables, SIGNAL( variableSelected( const QString & ) ),
           p, SLOT( variableSelected( const QString & ) ) );
  connect( p, SIGNAL( radixCharChanged( char ) ),
           docks.variables, SLOT( setRadixChar( char ) ) );
  connect( p, SIGNAL( retranslateText() ),
           docks.variables, SLOT( retranslateText() ) );

  docks.variables->updateList( evaluator );

  if ( docks.functions )
    p->tabifyDockWidget( docks.functions, docks.variables );
  else if ( docks.history )
    p->tabifyDockWidget( docks.history, docks.variables );
  else if ( docks.constants )
    p->tabifyDockWidget( docks.constants, docks.variables );
  else if ( docks.book )
    p->tabifyDockWidget( docks.book, docks.variables );

  settings.showVariables = true;
}


void MainWindow::Private::createFixedConnections()
{
  connect( actions.clearHistory,                SIGNAL( triggered()                           ), p,                     SLOT( clearHistory()                        ) );
  connect( actions.clearExpression,             SIGNAL( triggered()                           ), p,                     SLOT( clearExpression()                     ) );
  connect( actions.degree,                      SIGNAL( triggered()                           ), p,                     SLOT( degree()                              ) );
  connect( actions.deleteAllVariables,          SIGNAL( triggered()                           ), p,                     SLOT( deleteAllVariables()                  ) );
  connect( actions.deleteVariable,              SIGNAL( triggered()                           ), p,                     SLOT( deleteVariable()                      ) );
  connect( actions.digits15,                    SIGNAL( triggered()                           ), p,                     SLOT( digits15()                            ) );
  connect( actions.digits2,                     SIGNAL( triggered()                           ), p,                     SLOT( digits2()                             ) );
  connect( actions.digits3,                     SIGNAL( triggered()                           ), p,                     SLOT( digits3()                             ) );
  connect( actions.digits50,                    SIGNAL( triggered()                           ), p,                     SLOT( digits50()                            ) );
  connect( actions.digits8,                     SIGNAL( triggered()                           ), p,                     SLOT( digits8()                             ) );
  connect( actions.digitsAuto,                  SIGNAL( triggered()                           ), p,                     SLOT( digitsAuto()                          ) );
  connect( actions.editCopyResult,              SIGNAL( triggered()                           ), p,                     SLOT( copyResult()                          ) );
  connect( actions.editCopy,                    SIGNAL( triggered()                           ), widgets.editor,        SLOT( copy()                                ) );
  connect( actions.editPaste,                   SIGNAL( triggered()                           ), widgets.editor,        SLOT( paste()                               ) );
  connect( actions.selectExpression,            SIGNAL( triggered()                           ), p,                     SLOT( selectExpression()                    ) );
  connect( actions.helpAboutQt,                 SIGNAL( triggered()                           ), p,                     SLOT( aboutQt()                             ) );
  connect( actions.helpAbout,                   SIGNAL( triggered()                           ), p,                     SLOT( about()                               ) );
  connect( actions.helpGotoWebsite,             SIGNAL( triggered()                           ), p,                     SLOT( gotoWebsite()                         ) );
  connect( actions.helpTipOfTheDay,             SIGNAL( triggered()                           ), p,                     SLOT( showTipOfTheDay()                     ) );
  connect( actions.insertFunction,              SIGNAL( triggered()                           ), p,                     SLOT( insertFunction()                      ) );
  connect( actions.insertVariable,              SIGNAL( triggered()                           ), p,                     SLOT( insertVariable()                      ) );
  connect( actions.radian,                      SIGNAL( triggered()                           ), p,                     SLOT( radian()                              ) );
  connect( actions.scrollDown,                  SIGNAL( triggered()                           ), p,                     SLOT( scrollDown()                          ) );
  connect( actions.scrollUp,                    SIGNAL( triggered()                           ), p,                     SLOT( scrollUp()                            ) );
  connect( actions.sessionImport,               SIGNAL( triggered()                           ), p,                     SLOT( importSession()                       ) );
  connect( actions.sessionLoad,                 SIGNAL( triggered()                           ), p,                     SLOT( loadSession()                         ) );
  connect( actions.sessionQuit,                 SIGNAL( triggered()                           ), p,                     SLOT( close()                               ) );
  connect( actions.sessionSave,                 SIGNAL( triggered()                           ), p,                     SLOT( saveSession()                         ) );
  connect( actions.sessionExport,               SIGNAL( triggered()                           ), p,                     SLOT( exportSession()                       ) );
  connect( actions.showBook,                    SIGNAL( toggled( bool )                       ), p,                     SLOT( showBook( bool )                      ) );
  connect( actions.showConstants,               SIGNAL( toggled( bool )                       ), p,                     SLOT( showConstants( bool )                 ) );
  connect( actions.showFullScreen,              SIGNAL( toggled( bool )                       ), p,                     SLOT( showInFullScreen( bool )              ) );
  connect( actions.showFunctions,               SIGNAL( toggled( bool )                       ), p,                     SLOT( showFunctions( bool )                 ) );
  connect( actions.showHistory,                 SIGNAL( toggled( bool )                       ), p,                     SLOT( showHistory( bool )                   ) );
  connect( actions.showKeypad,                  SIGNAL( toggled( bool )                       ), p,                     SLOT( showKeypad( bool )                    ) );
  connect( actions.showMenuBar,                 SIGNAL( triggered()                           ), p,                     SLOT( showMenuBar()                         ) );
  connect( actions.showStatusBar,               SIGNAL( toggled( bool )                       ), p,                     SLOT( showStatusBar( bool )                 ) );
  connect( actions.showVariables,               SIGNAL( toggled( bool )                       ), p,                     SLOT( showVariables( bool )                 ) );
  connect( actions.formatBinary,                SIGNAL( triggered()                           ), p,                     SLOT( formatBinary()                        ) );
  connect( actions.formatEngineering,           SIGNAL( triggered()                           ), p,                     SLOT( formatEngineering()                   ) );
  connect( actions.formatFixed,                 SIGNAL( triggered()                           ), p,                     SLOT( formatFixed()                         ) );
  connect( actions.formatGeneral,               SIGNAL( triggered()                           ), p,                     SLOT( formatGeneral()                       ) );
  connect( actions.formatHexadec,               SIGNAL( triggered()                           ), p,                     SLOT( formatHexadec()                       ) );
  connect( actions.formatOctal,                 SIGNAL( triggered()                           ), p,                     SLOT( formatOctal()                         ) );
  connect( actions.formatScientific,            SIGNAL( triggered()                           ), p,                     SLOT( formatScientific()                    ) );
  connect( actions.optionAutoCalc,              SIGNAL( toggled( bool )                       ), p,                     SLOT( autoCalcToggled( bool )               ) );
  connect( actions.optionAutoCompletion,        SIGNAL( toggled( bool )                       ), p,                     SLOT( autoCompletionToggled( bool )         ) );
  connect( actions.optionAlwaysOnTop,           SIGNAL( toggled( bool )                       ), p,                     SLOT( alwaysOnTopToggled( bool )            ) );
  connect( actions.optionMinimizeToTray,        SIGNAL( toggled( bool )                       ), p,                     SLOT( minimizeToTrayToggled( bool )         ) );
  connect( actions.optionHiliteSyntax,          SIGNAL( toggled( bool )                       ), p,                     SLOT( hiliteSyntaxToggled( bool )           ) );
  connect( actions.radixCharAuto,               SIGNAL( triggered()                           ), p,                     SLOT( radixCharAutoActivated()              ) );
  connect( actions.radixCharDot,                SIGNAL( triggered()                           ), p,                     SLOT( radixCharDotActivated()               ) );
  connect( actions.radixCharComma,              SIGNAL( triggered()                           ), p,                     SLOT( radixCharCommaActivated()             ) );
  connect( actions.languageDefault,             SIGNAL( triggered()                           ), p,                     SLOT( changeLanguage()                      ) );
  connect( actions.languageCa,                  SIGNAL( triggered()                           ), p,                     SLOT( changeLanguage()                      ) );
  connect( actions.languageCs,                  SIGNAL( triggered()                           ), p,                     SLOT( changeLanguage()                      ) );
  connect( actions.languageDe,                  SIGNAL( triggered()                           ), p,                     SLOT( changeLanguage()                      ) );
  connect( actions.languageEn,                  SIGNAL( triggered()                           ), p,                     SLOT( changeLanguage()                      ) );
  connect( actions.languageEs,                  SIGNAL( triggered()                           ), p,                     SLOT( changeLanguage()                      ) );
  connect( actions.languageEsAr,                SIGNAL( triggered()                           ), p,                     SLOT( changeLanguage()                      ) );
  connect( actions.languageEu,                  SIGNAL( triggered()                           ), p,                     SLOT( changeLanguage()                      ) );
  connect( actions.languageFi,                  SIGNAL( triggered()                           ), p,                     SLOT( changeLanguage()                      ) );
  connect( actions.languageFr,                  SIGNAL( triggered()                           ), p,                     SLOT( changeLanguage()                      ) );
  connect( actions.languageHe,                  SIGNAL( triggered()                           ), p,                     SLOT( changeLanguage()                      ) );
  connect( actions.languageId,                  SIGNAL( triggered()                           ), p,                     SLOT( changeLanguage()                      ) );
  connect( actions.languageIt,                  SIGNAL( triggered()                           ), p,                     SLOT( changeLanguage()                      ) );
  connect( actions.languageNb,                  SIGNAL( triggered()                           ), p,                     SLOT( changeLanguage()                      ) );
  connect( actions.languageNl,                  SIGNAL( triggered()                           ), p,                     SLOT( changeLanguage()                      ) );
  connect( actions.languagePl,                  SIGNAL( triggered()                           ), p,                     SLOT( changeLanguage()                      ) );
  connect( actions.languagePt,                  SIGNAL( triggered()                           ), p,                     SLOT( changeLanguage()                      ) );
  connect( actions.languagePtBr,                SIGNAL( triggered()                           ), p,                     SLOT( changeLanguage()                      ) );
  connect( actions.languageRo,                  SIGNAL( triggered()                           ), p,                     SLOT( changeLanguage()                      ) );
  connect( actions.languageRu,                  SIGNAL( triggered()                           ), p,                     SLOT( changeLanguage()                      ) );
  connect( actions.languageSv,                  SIGNAL( triggered()                           ), p,                     SLOT( changeLanguage()                      ) );
  connect( actions.languageTr,                  SIGNAL( triggered()                           ), p,                     SLOT( changeLanguage()                      ) );
  connect( actions.languageZhCn,                SIGNAL( triggered()                           ), p,                     SLOT( changeLanguage()                      ) );
  connect( widgets.editor,                      SIGNAL( autoCalcEnabled( const QString & )    ), p,                     SLOT( showAutoCalc( const QString & )       ) );
  connect( widgets.editor,                      SIGNAL( autoCalcDisabled()                    ), p,                     SLOT( hideAutoCalc()                        ) );
  connect( widgets.editor,                      SIGNAL( returnPressed()                       ), p,                     SLOT( returnPressed()                       ) );
  connect( widgets.editor,                      SIGNAL( textChanged()                         ), p,                     SLOT( textChanged()                         ) );
  connect( widgets.display,                     SIGNAL( textSelected( const QString & )       ), widgets.editor,        SLOT( paste()                               ) );
  connect( widgets.display,                     SIGNAL( textSelected( const QString & )       ), widgets.editor,        SLOT( setFocus()                            ) );
  connect( p,                                   SIGNAL( formatChanged( char )                 ), widgets.editor,        SLOT( setFormat( char )                     ) );
  connect( p,                                   SIGNAL( precisionChanged( int )               ), widgets.editor,        SLOT( setPrecision( int )                   ) );
  connect( p,                                   SIGNAL( radixCharChanged( char )              ), widgets.editor,        SLOT( setRadixChar( char )                  ) );
  connect( p,                                   SIGNAL( formatChanged( char )                 ), widgets.display,       SLOT( setFormat( char )                     ) );
  connect( p,                                   SIGNAL( precisionChanged( int )               ), widgets.display,       SLOT( setPrecision( int )                   ) );
  connect( p,                                   SIGNAL( radixCharChanged( char )              ), widgets.display,       SLOT( setRadixChar( char )                  ) );
  connect( p,                                   SIGNAL( radixCharChanged( char )              ), evaluator,             SLOT( setRadixChar( char )                  ) );
  connect( p,                                   SIGNAL( angleModeChanged( char )              ), functions,             SLOT( setAngleMode( char )                  ) );
  connect( p,                                   SIGNAL( retranslateText()                     ), p,                     SLOT( setAllText()                          ) );
}


void MainWindow::Private::applySettings()
{
  // window size
  if ( settings.mainWindowSize != QSize(0, 0) )
    p->resize( settings.mainWindowSize );

  // window state
  p->restoreState( settings.mainWindowState );

  // full screen
  actions.showFullScreen->setChecked( settings.showFullScreen );

  // always-on-top
  actions.optionAlwaysOnTop->setChecked( settings.stayAlwaysOnTop );

  // angle mode
  if ( settings.angleMode == 'r' )
    actions.radian->setChecked( true );
  else if ( settings.angleMode == 'd' )
    actions.degree->setChecked( true );

  // language
  checkInitialLanguage();

  // history
  if ( settings.saveSession )
    restoreHistory();
  else
    p->clearHistory();

  // variables
  if ( settings.saveVariables )
    restoreVariables();

  // format
  checkInitialFormat();

  // precision
  checkInitialPrecision();

  // radix character
  if ( settings.isLocaleRadixChar() )
    actions.radixCharAuto->setChecked( true );
  else if ( settings.getRadixChar() == '.' )
    actions.radixCharDot->setChecked( true );
  else if ( settings.getRadixChar() == ',' )
    actions.radixCharComma->setChecked( true );

  // keypad
  actions.showKeypad->setChecked( settings.showKeypad );

  // menu bar
  p->menuBar()->setVisible( settings.showMenuBar );

  // autocalc
  if ( settings.autoCalc )
    actions.optionAutoCalc->setChecked( true );
  else
    p->autoCalcToggled( false );

  // autocomplete
  if ( settings.autoComplete )
    actions.optionAutoCompletion->setChecked( true );
  else
    p->autoCompletionToggled( false );

  // minimize to system tray
  actions.optionMinimizeToTray->setChecked( settings.minimizeToTray );

  // syntax hilite
  if ( settings.hiliteSyntax )
    actions.optionHiliteSyntax->setChecked( true );
  else
    p->hiliteSyntaxToggled( false );

  // docks
  actions.showBook     ->setChecked( settings.showBook      );
  actions.showConstants->setChecked( settings.showConstants );
  actions.showFunctions->setChecked( settings.showFunctions );
  actions.showHistory  ->setChecked( settings.showHistory   );
  actions.showVariables->setChecked( settings.showVariables );
  restoreFloatingDocks();

  // status bar
  actions.showStatusBar->setChecked( settings.showStatusBar );
}


void MainWindow::Private::checkInitialFormat()
{
  if ( settings.format == 'g' )
    actions.formatGeneral->setChecked( true );
  else if ( settings.format == 'f' )
    actions.formatFixed->setChecked( true );
  else if ( settings.format == 'n' )
    actions.formatEngineering->setChecked( true );
  else if ( settings.format == 'e' )
    actions.formatScientific->setChecked( true );
  else if ( settings.format == 'h' )
    actions.formatHexadec->setChecked( true );
  else if ( settings.format == 'o' )
    actions.formatOctal->setChecked( true );
  else if ( settings.format == 'b' )
    actions.formatBinary->setChecked( true );
}


void MainWindow::Private::checkInitialPrecision()
{
  if ( settings.precision <   0 )
    actions.digitsAuto->setChecked( true );
  else if ( settings.precision ==  2 )
    actions.digits2->setChecked( true );
  else if ( settings.precision ==  3 )
    actions.digits3->setChecked( true );
  else if ( settings.precision ==  8 )
    actions.digits8->setChecked( true );
  else if ( settings.precision == 15 )
    actions.digits15->setChecked( true );
  else if ( settings.precision == 50 )
    actions.digits50->setChecked( true );
}


void MainWindow::Private::checkInitialLanguage()
{
  if ( settings.language == "ca" )
    actions.languageCa->setChecked( true );
  else if ( settings.language == "cs" )
    actions.languageCs->setChecked( true );
  else if ( settings.language == "de" )
    actions.languageDe->setChecked( true );
  else if ( settings.language == "en" )
    actions.languageEn->setChecked( true );
  else if ( settings.language == "es" )
    actions.languageEs->setChecked( true );
  else if ( settings.language == "es_AR" )
    actions.languageEsAr->setChecked( true );
  else if ( settings.language == "eu" )
    actions.languageEu->setChecked( true );
  else if ( settings.language == "fi" )
    actions.languageFi->setChecked( true );
  else if ( settings.language == "fr" )
    actions.languageFr->setChecked( true );
  else if ( settings.language == "he" )
    actions.languageHe->setChecked( true );
  else if ( settings.language == "id" )
    actions.languageId->setChecked( true );
  else if ( settings.language == "it" )
    actions.languageIt->setChecked( true );
  else if ( settings.language == "nb" )
    actions.languageNb->setChecked( true );
  else if ( settings.language == "nl" )
    actions.languageNl->setChecked( true );
  else if ( settings.language == "pl" )
    actions.languagePl->setChecked( true );
  else if ( settings.language == "pt" )
    actions.languagePt->setChecked( true );
  else if ( settings.language == "pt_BR" )
    actions.languagePtBr->setChecked( true );
  else if ( settings.language == "ro" )
    actions.languageRo->setChecked( true );
  else if ( settings.language == "ru" )
    actions.languageRu->setChecked( true );
  else if ( settings.language == "sv" )
    actions.languageSv->setChecked( true );
  else if ( settings.language == "tr" )
    actions.languageTr->setChecked( true );
  else if ( settings.language == "zn_CN" )
    actions.languageZhCn->setChecked( true );
  else
    actions.languageDefault->setChecked( true );
}


void MainWindow::Private::saveSettings()
{
  // history
  settings.history        = widgets.editor->history();
  settings.historyResults = widgets.editor->historyResults();

  // variables
  if ( settings.saveVariables )
  {
    settings.variables.clear();
    QVector<Variable> vars = evaluator->variables();
    for ( int i = 0; i < vars.count(); i++ )
    {
      QString name = vars[i].name;
      char * value = HMath::formatScientific( vars[i].value, DECPRECISION );
      settings.variables.append( QString( "%1=%2" ).arg( name )
                                                   .arg( QString( value ) ) );
      free( value );
    }
  }

  syncWindowStateToSettings();

  settings.save();
}


void MainWindow::Private::syncWindowStateToSettings()
{
  // main window
  settings.mainWindowState = p->saveState();
  settings.mainWindowSize  = p->size();

  // docks
  if ( docks.book )
  {
    settings.bookDockFloating = docks.book->isFloating();
    settings.bookDockLeft     = docks.book->x();
    settings.bookDockTop      = docks.book->y();
    settings.bookDockWidth    = docks.book->width();
    settings.bookDockHeight   = docks.book->height();
  }
  if ( docks.history )
  {
    settings.historyDockFloating = docks.history->isFloating();
    settings.historyDockLeft     = docks.history->x();
    settings.historyDockTop      = docks.history->y();
    settings.historyDockWidth    = docks.history->width();
    settings.historyDockHeight   = docks.history->height();
  }
  if ( docks.functions )
  {
    settings.functionsDockFloating = docks.functions->isFloating();
    settings.functionsDockLeft     = docks.functions->x();
    settings.functionsDockTop      = docks.functions->y();
    settings.functionsDockWidth    = docks.functions->width();
    settings.functionsDockHeight   = docks.functions->height();
  }
  if ( docks.variables )
  {
    settings.variablesDockFloating = docks.variables->isFloating();
    settings.variablesDockLeft     = docks.variables->x();
    settings.variablesDockTop      = docks.variables->y();
    settings.variablesDockWidth    = docks.variables->width();
    settings.variablesDockHeight   = docks.variables->height();
  }
  if ( docks.constants )
  {
    settings.constantsDockFloating = docks.constants->isFloating();
    settings.constantsDockLeft     = docks.constants->x();
    settings.constantsDockTop      = docks.constants->y();
    settings.constantsDockWidth    = docks.constants->width();
    settings.constantsDockHeight   = docks.constants->height();
  }
}


// public

MainWindow::MainWindow()
  : QMainWindow(), d( new MainWindow::Private )
{
  d->p = this;

  d->settings.load();

  d->createUi();
  d->applySettings();

  emit retranslateText();

  QTimer::singleShot( 0, this, SLOT( activate() ) );
}


MainWindow::~MainWindow()
{
  delete d;
}


bool MainWindow::event( QEvent * e )
{
  if ( e->type() == QEvent::WindowStateChange )
    if ( windowState() & Qt::WindowMinimized )
      if ( d->settings.minimizeToTray )
      {
        QTimer::singleShot( 100, this, SLOT( minimizeToTray() ) );
        return true;
      }

  return QMainWindow::event( e );
}


// public slots

void MainWindow::about()
{
  AboutBox dlg;
  dlg.exec();
}


void MainWindow::aboutQt()
{
  QMessageBox::aboutQt( this, tr( "About Qt" ) );
}


void MainWindow::clearHistory()
{
  d->widgets.display->clear();
  d->widgets.editor->clearHistory();
  if ( d->settings.showHistory )
    d->docks.history->clear();
  d->settings.history.clear();
  d->settings.historyResults.clear();
  QTimer::singleShot( 0, d->widgets.editor, SLOT( setFocus() ) );
}


void MainWindow::clearExpression()
{
  d->widgets.editor->clear();
  QTimer::singleShot( 0, d->widgets.editor, SLOT( setFocus() ) );
}


void MainWindow::copyResult()
{
  QClipboard * cb = QApplication::clipboard();
  HNumber num = d->evaluator->get( "ans" );
  char * strToCopy = HMath::format( num, d->settings.format,
                                    d->settings.precision );
  QString final( strToCopy );
  if ( d->widgets.display->radixChar() == ',' )
    final.replace( '.', ',' );
  cb->setText( final, QClipboard::Clipboard );
  free( strToCopy );
}


void MainWindow::degree()
{
  if ( d->settings.angleMode == 'd' )
    return;

  d->settings.angleMode = 'd';

  if ( d->status.angleUnit )
    d->status.angleUnit->setText( tr( "Degree" ) );

  emit angleModeChanged( 'd' );
}


void MainWindow::deleteAllVariables()
{
  d->evaluator->clearVariables();

  if ( d->settings.showVariables )
    d->docks.variables->updateList( d->evaluator );
}


void MainWindow::deleteVariable()
{
  DeleteVariableDlg dlg( d->evaluator );
  dlg.exec();

  if ( d->settings.showVariables )
    d->docks.variables->updateList( d->evaluator );
}


void MainWindow::digits2()
{
  setPrecision( 2 );
}


void MainWindow::digits3()
{
  setPrecision( 3 );
}


void MainWindow::digits8()
{
  setPrecision( 8 );
}


void MainWindow::digits15()
{
  setPrecision( 15 );
}


void MainWindow::digits50()
{
  setPrecision( 50 );
}


void MainWindow::digitsAuto()
{
  setPrecision( -1 );
}


void MainWindow::selectExpression()
{
  activateWindow();
  d->widgets.editor->selectAll();
  d->widgets.editor->setFocus();
}


void MainWindow::gotoWebsite()
{
  QDesktopServices::openUrl(
    QUrl( QString::fromLatin1("http://www.speedcrunch.org") ) );
}


void MainWindow::hideAutoCalc()
{
  d->widgets.autoCalcLabel->hideText();
}


void MainWindow::insertFunction()
{
  InsertFunctionDlg dlg( d->functions );

  if ( dlg.exec() == InsertFunctionDlg::Accepted )
  {
    QString funcName = dlg.functionName();
    if ( ! funcName.isEmpty() )
      d->widgets.editor->insert( funcName );
  }
}


void MainWindow::insertVariable()
{
  InsertVariableDlg dlg( d->evaluator, d->settings.getRadixChar() );

  if ( dlg.exec() == InsertVariableDlg::Accepted )
  {
    QString varName = dlg.variableName();
    if ( ! varName.isEmpty() )
      d->widgets.editor->insert( varName );
  }
}


void MainWindow::loadSession()
{
  QString errMsg  = tr( "File %1 is not a valid session" );
  QString filters = tr( "SpeedCrunch Sessions (*.sch);;All Files (*)" );
  QString fname   = QFileDialog::getOpenFileName( this, tr( "Load Session" ),
                                                  QString::null, filters );
  if ( fname.isEmpty() )
    return;

  QFile file( fname );
  if ( ! file.open( QIODevice::ReadOnly ) )
  {
    QMessageBox::critical( this, tr( "Error" ),
                           tr( "Can't read from file %1" ).arg( fname ) );
    return;
  }

  QTextStream stream( & file );

  // version of the format
  QString version = stream.readLine();
  if ( version != "0.10" )
  {
    QMessageBox::critical( this, tr( "Error" ), errMsg.arg( fname ) );
    return;
  }

  // number of calculations
  bool ok;
  int noCalcs = stream.readLine().toInt( &ok );
  if ( ok == false || noCalcs < 0 )
  {
    QMessageBox::critical( this, tr( "Error" ), errMsg.arg( fname ) );
    return;
  }

  // ask for merge with current session
  QString mergeMsg = tr( "Merge session being loaded with current session?\n"
                         "If no, current variables and display will be "
                         "cleared." );
  QMessageBox::StandardButton but
    = QMessageBox::question( this, tr( "Question" ), mergeMsg,
                             QMessageBox::Yes | QMessageBox::No
                             | QMessageBox::Cancel, QMessageBox::Yes );

  if ( but == QMessageBox::Cancel )
  {
    return;
  }
  else if ( but == QMessageBox::No )
  {
    d->widgets.display->clear();
    deleteAllVariables();
    clearHistory();
  }

  // expressions and results
  QStringList expLs;
  QStringList resLs;
  for ( int i = 0; i < noCalcs; i++ )
  {
    QString exp = stream.readLine();
    QString res = stream.readLine();
    if ( exp.isNull() || res.isNull() )
    {
      QMessageBox::critical( this, tr( "Error" ), errMsg.arg( fname ) );
      return;
    }
    expLs.append( exp );
    resLs.append( res );
  }

  // FIXME: this keeps the history in three (finally even four) different places.
  // There should be just one central history and result list, and the clients
  // editor, display, settings and historyDock should retrieve their data from there.
  // FIXME: Actually there should be three lists: expressions, values and formats.
  d->widgets.display->appendHistory( expLs, resLs );
  d->widgets.editor->appendHistory( expLs, resLs );

  if ( d->settings.showHistory )
    d->docks.history->appendHistory( expLs );

  // variables
  int noVars = stream.readLine().toInt( &ok );
  if ( ok == false || noVars < 0 )
  {
    QMessageBox::critical( this, tr( "Error" ), errMsg.arg( fname ) );
    return;
  }
  for ( int i = 0; i < noVars; i++ )
  {
    QString var = stream.readLine();
    QString val = stream.readLine();
    if ( var.isNull() || val.isNull() )
    {
      QMessageBox::critical( this, tr( "Error" ), errMsg.arg( fname ) );
      return;
    }
    HNumber num( val.toAscii().data() );
    if (  num != HNumber::nan() )
      d->evaluator->set( var, num );
  }

  file.close();
}


void MainWindow::importSession()
{
  QString filters = tr( "All Files (*)" );
  QString fname = QFileDialog::getOpenFileName( this, tr( "Import Session" ),
                                                QString::null, filters );
  if ( fname.isEmpty() )
    return;

  QFile file( fname );
  if ( ! file.open( QIODevice::ReadOnly ) )
  {
    QMessageBox::critical( this, tr( "Error" ),
                           tr( "Can't read from file %1" ).arg( fname ) );
    return;
  }

  // ask for merge with current session
  QString mergeMsg = tr( "Merge session being imported with current session?\n"
                         "If no, current variables and display will be "
                         "cleared." );
  QMessageBox::StandardButton but
    = QMessageBox::question( this, tr( "Question" ), mergeMsg,
                             QMessageBox::Yes | QMessageBox::No
                             | QMessageBox::Cancel, QMessageBox::Yes );

  if ( but == QMessageBox::Cancel )
  {
    return;
  }
  else if ( but == QMessageBox::No )
  {
    d->widgets.display->clear();
    deleteAllVariables();
    clearHistory();
  }

  QTextStream stream( & file );
  QString exp = stream.readLine();
  bool ignoreAll = false;
  while ( ! exp.isNull() )
  {
    d->widgets.editor->setText( exp );

    QString str = d->evaluator->autoFix( exp );

    d->evaluator->setExpression( str );

    HNumber result = d->evaluator->evalUpdateAns();
    if ( ! d->evaluator->error().isEmpty() )
    {
      if ( ! ignoreAll )
      {
        QMessageBox::StandardButton but
          = QMessageBox::warning( this, tr( "Error" ),
              tr( "Ignore error?" ) + "\n" + d->evaluator->error(),
              QMessageBox::Yes | QMessageBox::YesToAll | QMessageBox::Cancel,
              QMessageBox::Yes );

        if ( but == QMessageBox::Cancel )
          return;
        else if ( but == QMessageBox::YesToAll )
          ignoreAll = true;
      }
    }
    else
    {
      d->widgets.display->append( str, result );
      char * num = HMath::formatScientific( result, DECPRECISION );
      d->widgets.editor->appendHistory( str, num );
      free( num );
      d->widgets.editor->setAnsAvailable( true );
      if ( d->settings.showVariables )
        d->docks.variables->updateList( d->evaluator );
      if ( d->settings.showHistory )
        d->docks.history->append( str );

      d->widgets.editor->setText( str );
      d->widgets.editor->selectAll();
      d->widgets.editor->stopAutoCalc();
      d->widgets.editor->stopAutoComplete();
      d->conditions.autoAns = true;
    }

    exp = stream.readLine();
  }

  file.close();

  QTimer::singleShot( 0, d->widgets.editor, SLOT( setFocus() ) );

  if ( ! isActiveWindow() )
    activateWindow();
}


void MainWindow::alwaysOnTopToggled( bool b )
{
  d->settings.stayAlwaysOnTop = b;

  QPoint cur = mapToGlobal( QPoint(0, 0) );
  if ( b )
    setWindowFlags( windowFlags() | Qt::WindowStaysOnTopHint );
  else
    setWindowFlags( windowFlags() & (~ Qt::WindowStaysOnTopHint) );
  move( cur );
  show();
}


void MainWindow::autoCalcToggled( bool b )
{
  d->settings.autoCalc = b;
  d->widgets.editor->setAutoCalcEnabled( b );
}


void MainWindow::autoCompletionToggled( bool b )
{
  d->settings.autoComplete = b;
  d->widgets.editor->setAutoCompleteEnabled( b );
}


void MainWindow::minimizeToTrayToggled( bool b )
{
  if ( b && ! d->widgets.trayIcon && QSystemTrayIcon::isSystemTrayAvailable() )
  {
    d->conditions.trayNotify = true;
    d->widgets.trayIcon = new QSystemTrayIcon( this );
    d->widgets.trayIcon->setToolTip( "SpeedCrunch" );
    d->widgets.trayIcon->setIcon( QPixmap( ":/speedcrunch.png" ) );

    d->menus.trayIcon = new QMenu( this );
    d->menus.trayIcon->addAction( d->actions.editCopyResult );
    d->menus.trayIcon->addSeparator();
    d->menus.trayIcon->addAction( d->actions.sessionQuit    );

    d->widgets.trayIcon->setContextMenu( d->menus.trayIcon );
    connect( d->widgets.trayIcon,
             SIGNAL( activated( QSystemTrayIcon::ActivationReason ) ),
             this,
             SLOT( trayIconActivated( QSystemTrayIcon::ActivationReason ) ) );
  }
  else
  {
    if ( d->widgets.trayIcon )
      delete d->widgets.trayIcon;
    d->widgets.trayIcon = 0;
  }

  d->settings.minimizeToTray = b;
}


void MainWindow::hiliteSyntaxToggled( bool b )
{
  d->widgets.editor->setSyntaxHighlight( b );
  d->settings.hiliteSyntax = b;
}


void MainWindow::radian()
{
  if ( d->settings.angleMode == 'r' )
    return;

  d->settings.angleMode = 'r';

  if ( d->status.angleUnit )
    d->status.angleUnit->setText( tr( "Radian" ) );

  emit angleModeChanged( 'r' );
}


void MainWindow::saveSession()
{
  QString filters = tr( "SpeedCrunch Sessions (*.sch);;All Files (*)" );
  QString fname = QFileDialog::getSaveFileName( this, tr( "Save Session" ),
                                                QString::null, filters );
  if ( fname.isEmpty() )
    return;

  QFile file( fname );
  if ( ! file.open( QIODevice::WriteOnly ) )
  {
    QMessageBox::critical( this, tr( "Error" ),
                           tr( "Can't write to file %1" ).arg( fname ) );
    return;
  }

  QTextStream stream( & file );

  // version of the format
  stream << "0.10" << "\n";

  // number of calculations
  stream << d->widgets.display->count() << "\n";

  // expressions and results
  stream << d->widgets.display->asText() << "\n";

  // number of variables
  int noVars = d->evaluator->variables().count();
  stream << noVars - 2 << "\n"; // exclude pi and phi

  // variables
  for ( int i = 0; i < noVars; i++ )
  {
    Variable var = d->evaluator->variables()[i];
    if ( var.name != "pi" && var.name != "phi" )
    {
      char * value = HMath::format( var.value );
      stream << var.name << "\n" << value << "\n";
      free( value );
    }
  }

  file.close();
}


void MainWindow::exportSession()
{
  QString filters = tr( "All Files (*)" );
  QString fname = QFileDialog::getSaveFileName( this, tr( "Export Session" ),
                                                QString::null, filters );
  if ( fname.isEmpty() )
    return;

  QFile file( fname );
  if ( ! file.open( QIODevice::WriteOnly ) )
  {
    QMessageBox::critical( this, tr( "Error" ),
                           tr( "Can't write to file %1" ).arg( fname ) );
    return;
  }

  QTextStream stream( & file );

  // expressions and results
  stream << d->widgets.display->asText() << "\n";

  file.close();
}


void MainWindow::setWidgetsDirection()
{
  if ( (d->settings.language == "C" && QLocale().language() == QLocale::Hebrew)
       || d->settings.language == "he" )
  {
    qApp->setLayoutDirection( Qt::RightToLeft );
  }
  else
  {
    qApp->setLayoutDirection( Qt::LeftToRight );
  }
}


void MainWindow::scrollDown()
{
  QScrollBar * sb = d->widgets.display->verticalScrollBar();
  int value = sb->value() + 40;
  sb->setValue( value );
}


void MainWindow::scrollUp()
{
  QScrollBar * sb = d->widgets.display->verticalScrollBar();
  int value = sb->value() - 40;
  sb->setValue( value );
}


void MainWindow::showMenuBar()
{
  menuBar()->setVisible( ! menuBar()->isVisible() );
  d->settings.showMenuBar = menuBar()->isVisible();

  if ( ! menuBar()->isVisible() && d->conditions.notifyMenuBarHidden )
  {
    showMenuBarTip();
    d->conditions.notifyMenuBarHidden = false;
  }

  d->widgets.display->scrollEnd();
}


void MainWindow::showStatusBar( bool b )
{
  b ? d->createStatusBar() : d->deleteStatusBar();
  d->settings.showStatusBar = b;
}


void MainWindow::showAutoCalc( const QString & msg )
{
  QPoint p = d->widgets.editor->mapToParent( QPoint(0, 0) );
  d->widgets.autoCalcLabel->move( p );
  d->widgets.autoCalcLabel->showText( msg );
}


void MainWindow::showInFullScreen( bool b )
{
  d->settings.showFullScreen = b;
  b ? showFullScreen() : showNormal();
}


bool MainWindow::eventFilter( QObject * o, QEvent * e )
{
  if ( o == d->docks.book )
  {
    if ( e->type() == QEvent::Close )
    {
      d->deleteBookDock();
      return true;
    }
    return false;
  }

  if ( o == d->docks.constants )
  {
    if ( e->type() == QEvent::Close )
    {
      d->deleteConstantsDock();
      return true;
    }
    return false;
  }

  if ( o == d->docks.functions )
  {
    if ( e->type() == QEvent::Close )
    {
      d->deleteFunctionsDock();
      return true;
    }
    return false;
  }

  if ( o == d->docks.history )
  {
    if ( e->type() == QEvent::Close )
    {
      d->deleteHistoryDock();
      return true;
    }
    return false;
  }

  if ( o == d->docks.variables )
  {
    if ( e->type() == QEvent::Close )
    {
      d->deleteVariablesDock();
      return true;
    }
    return false;
  }

  return QMainWindow::eventFilter( o, e );
}


void MainWindow::Private::deleteKeypad()
{
  p->disconnect( widgets.keypad );
  delete widgets.keypad;
  widgets.keypad = 0;

  layouts.root->removeItem( layouts.keypad );
  delete layouts.keypad;
  layouts.keypad = 0;
}


void MainWindow::Private::deleteStatusBar()
{
  delete status.angleUnit;
  status.angleUnit = 0;

  delete status.format;
  status.format = 0;

  p->setStatusBar( 0 );
}


void MainWindow::Private::deleteBookDock()
{
  Q_ASSERT( docks.book );

  p->removeDockWidget( docks.book );
  p->disconnect( docks.book );
  delete docks.book;
  docks.book = 0;
  actions.showBook->blockSignals( true );
  actions.showBook->setChecked( false );
  actions.showBook->blockSignals( false );
  settings.showBook = false;
}


void MainWindow::Private::deleteConstantsDock()
{
  Q_ASSERT( docks.constants );

  p->removeDockWidget( docks.constants );
  p->disconnect( docks.constants );
  delete docks.constants;
  docks.constants = 0;
  actions.showConstants->blockSignals( true );
  actions.showConstants->setChecked( false );
  actions.showConstants->blockSignals( false );
  settings.showConstants = false;
}


void MainWindow::Private::deleteFunctionsDock()
{
  Q_ASSERT( docks.functions );

  p->removeDockWidget( docks.functions );
  p->disconnect( docks.functions );
  delete docks.functions;
  docks.functions = 0;
  actions.showFunctions->blockSignals( true );
  actions.showFunctions->setChecked( false );
  actions.showFunctions->blockSignals( false );
  settings.showFunctions = false;
}


void MainWindow::Private::deleteHistoryDock()
{
  Q_ASSERT( docks.history );

  p->removeDockWidget( docks.history );
  p->disconnect( docks.history );
  delete docks.history;
  docks.history = 0;
  actions.showHistory->blockSignals( true );
  actions.showHistory->setChecked( false );
  actions.showHistory->blockSignals( false );
  settings.showHistory = false;
}


void MainWindow::Private::deleteVariablesDock()
{
  Q_ASSERT( docks.variables );

  p->removeDockWidget( docks.variables );
  p->disconnect( docks.variables );
  delete docks.variables;
  docks.variables = 0;
  actions.showVariables->blockSignals( true );
  actions.showVariables->setChecked( false );
  actions.showVariables->blockSignals( false );
  settings.showVariables = false;
}


void MainWindow::showFunctions( bool b )
{
  if ( b )
    d->createFunctionsDock();
  else
    d->deleteFunctionsDock();
}


void MainWindow::showBook( bool b )
{
  if ( b )
    d->createBookDock();
  else
    d->deleteBookDock();
}


void MainWindow::showConstants( bool b )
{
  if ( b )
    d->createConstantsDock();
  else
    d->deleteConstantsDock();
}


void MainWindow::showHistory( bool b )
{
  if ( b )
    d->createHistoryDock();
  else
    d->deleteHistoryDock();
}


void MainWindow::showVariables( bool b )
{
  if ( b )
    d->createVariablesDock();
  else
    d->deleteVariablesDock();
}


void MainWindow::showKeypad( bool b )
{
  d->settings.showKeypad = b;
  if ( b )
    d->createKeypad();
  else
    d->deleteKeypad();
}


void MainWindow::showMenuBarTip()
{
  QString msg = tr( "The menu bar is now hidden. "
                    "To make it visible again, press Ctrl+Alt+M." );

  //QPoint p = mapFromGlobal( d->widgets.display->mapToGlobal( QPoint(0, 0) ) )
  //                          += QPoint(5, 5);
  d->widgets.tip->move( 0, 0 );
  d->widgets.tip->showText( msg, tr( "Warning" ) );
}


void MainWindow::showTipOfTheDay()
{
  //QPoint p = mapFromGlobal( d->widgets.display->mapToGlobal( QPoint(0, 0) ) )
  //                          += QPoint(5, 5);
  d->widgets.tip->move( 0, 0 );

  int tipNo = qrand() % 4;
  QString msg;
  switch ( tipNo )
  {
    case 0:
      msg = tr( "To insert a function using keyboard, use Ctrl+F shorcut. "
                "From the dialog, you can choose the function you want to "
                "insert." );
      break;
    case 1:
      msg = tr( "To insert a variable using keyboard, use Ctrl+I shorcut. "
                "From the dialog, you can choose the variable you want to "
                "insert." );
      break;
    case 2:
      msg = tr( "Use variable <i>pi</i> to use pi constant." );
      break;

    case 3:
      msg = tr( "Use <i>;</i> (semicolon) to separate the parameters in "
                "functions." );
      break;
    default:
      break;
  }

  d->widgets.tip->showText( msg, tr( "Tip of the day" ) );
}


void MainWindow::formatBinary()
{
  d->actionGroups.digits->setDisabled( true );
  setFormat( 'b' );

  if ( d->status.format )
    d->status.format->setText( tr( "Binary" ) );
}


void MainWindow::formatEngineering()
{
  d->actionGroups.digits->setEnabled( true );
  setFormat( 'n' );

  if ( d->status.format )
    d->status.format->setText( tr( "Engineering decimal" ) );
}


void MainWindow::formatFixed()
{
  d->actionGroups.digits->setEnabled( true );
  setFormat( 'f' );

  if ( d->status.format )
    d->status.format->setText( tr( "Fixed decimal" ) );
}


void MainWindow::formatGeneral()
{
  d->actionGroups.digits->setEnabled( true );
  setFormat( 'g' );

  if ( d->status.format )
    d->status.format->setText( tr( "General decimal" ) );
}


void MainWindow::formatHexadec()
{
  d->actionGroups.digits->setDisabled( true );
  setFormat( 'h' );

  if ( d->status.format )
    d->status.format->setText( tr( "Hexadecimal" ) );
}


void MainWindow::formatOctal()
{
  d->actionGroups.digits->setDisabled( true );
  setFormat( 'o' );

  if ( d->status.format )
    d->status.format->setText( tr( "Octal" ) );
}


void MainWindow::formatScientific()
{
  d->actionGroups.digits->setEnabled( true );
  setFormat( 'e' );

  if ( d->status.format )
    d->status.format->setText( tr( "Scientific decimal" ) );
}


// protected slots

void MainWindow::activate()
{
  activateWindow();
  raise();
  d->widgets.editor->setFocus();
}


void MainWindow::constantSelected( const QString & c )
{
  if ( c.isEmpty() )
    return;

  QString str( c );
  str.replace( '.', d->widgets.editor->radixChar() );
  d->widgets.editor->insert( str );

  QTimer::singleShot( 0, d->widgets.editor, SLOT( setFocus() ) );

  if ( ! isActiveWindow () )
    activateWindow();
}


void MainWindow::expressionSelected( const QString & expr )
{
  QTextCursor cursor = d->widgets.editor->textCursor();
  d->widgets.editor->blockSignals( true );
  cursor.insertText( expr );
  d->widgets.editor->blockSignals( false );
  QTimer::singleShot( 0, d->widgets.editor, SLOT( setFocus() ) );
  d->widgets.editor->setTextCursor( cursor );

  if ( ! isActiveWindow () )
    activateWindow();
}


void MainWindow::functionSelected( const QString & e )
{
  if ( e.isEmpty() )
    return;

  d->widgets.editor->insert( e );
  d->widgets.editor->insert( "(" );

  QTimer::singleShot( 0, d->widgets.editor, SLOT( setFocus() ) );

  if ( ! isActiveWindow () )
    activateWindow();
}


void MainWindow::keypadButtonPressed( Keypad::Button b )
{
  d->widgets.editor->blockSignals( true ); // prevent completion
  switch ( b )
  {
    case Keypad::Key0        : d->widgets.editor->insert( "0"     ); break;
    case Keypad::Key1        : d->widgets.editor->insert( "1"     ); break;
    case Keypad::Key2        : d->widgets.editor->insert( "2"     ); break;
    case Keypad::Key3        : d->widgets.editor->insert( "3"     ); break;
    case Keypad::Key4        : d->widgets.editor->insert( "4"     ); break;
    case Keypad::Key5        : d->widgets.editor->insert( "5"     ); break;
    case Keypad::Key6        : d->widgets.editor->insert( "6"     ); break;
    case Keypad::Key7        : d->widgets.editor->insert( "7"     ); break;
    case Keypad::Key8        : d->widgets.editor->insert( "8"     ); break;
    case Keypad::Key9        : d->widgets.editor->insert( "9"     ); break;
    case Keypad::KeyPlus     : d->widgets.editor->insert( "+"     ); break;
    case Keypad::KeyMinus    : d->widgets.editor->insert( "-"     ); break;
    case Keypad::KeyTimes    : d->widgets.editor->insert( "*"     ); break;
    case Keypad::KeyDivide   : d->widgets.editor->insert( "/"     ); break;
    case Keypad::KeyEE       : d->widgets.editor->insert( "e"     ); break;
    case Keypad::KeyLeftPar  : d->widgets.editor->insert( "("     ); break;
    case Keypad::KeyRightPar : d->widgets.editor->insert( ")"     ); break;
    case Keypad::KeyRaise    : d->widgets.editor->insert( "^"     ); break;
    case Keypad::KeyPercent  : d->widgets.editor->insert( "%"     ); break;
    case Keypad::KeyFactorial: d->widgets.editor->insert( "!"     ); break;
    case Keypad::KeyX        : d->widgets.editor->insert( "x"     ); break;
    case Keypad::KeyXEquals  : d->widgets.editor->insert( "x="    ); break;
    case Keypad::KeyPi       : d->widgets.editor->insert( "pi"    ); break;
    case Keypad::KeyAns      : d->widgets.editor->insert( "ans"   ); break;
    case Keypad::KeyLn       : d->widgets.editor->insert( "ln("   ); break;
    case Keypad::KeyExp      : d->widgets.editor->insert( "exp("  ); break;
    case Keypad::KeySin      : d->widgets.editor->insert( "sin("  ); break;
    case Keypad::KeyCos      : d->widgets.editor->insert( "cos("  ); break;
    case Keypad::KeyTan      : d->widgets.editor->insert( "tan("  ); break;
    case Keypad::KeyAcos     : d->widgets.editor->insert( "acos(" ); break;
    case Keypad::KeyAtan     : d->widgets.editor->insert( "atan(" ); break;
    case Keypad::KeyAsin     : d->widgets.editor->insert( "asin(" ); break;
    case Keypad::KeySqrt     : d->widgets.editor->insert( "sqrt(" ); break;

    case Keypad::KeyRadixChar:
      d->widgets.editor->insert( QString( d->widgets.keypad->radixChar() ) );
      break;

    case Keypad::KeyClear:
      clearExpression();
      break;

    default: break;
  }

  QTimer::singleShot( 0, d->widgets.editor, SLOT( setFocus() ) );
  d->widgets.editor->blockSignals( false );

  if ( b == Keypad::KeyEquals)
    d->widgets.editor->evaluate();
}


void MainWindow::minimizeToTray()
{
  if ( d->widgets.trayIcon )
  {
    hide();
    d->widgets.trayIcon->show();
    if ( d->conditions.trayNotify )
      QTimer::singleShot( 500, this, SLOT( showTrayMessage() ) );
    d->conditions.trayNotify = false;
  }
}


void MainWindow::Private::restoreVariables()
{
  for ( int k = 0; k < settings.variables.count(); k++ )
  {
    evaluator->setExpression( settings.variables[k] );
    evaluator->eval();
    QStringList list = settings.variables[k].split( "=" );
    evaluator->set( list[0], HNumber( list[1].toAscii().data() ) );
  }

  // free memory
  settings.variables.clear();
}


void MainWindow::Private::restoreFloatingDocks()
{
  if ( settings.showBook && settings.bookDockFloating
       && ! docks.book->isFloating() )
  {
    docks.book->hide();
    docks.book->setFloating( true );
    docks.book->move( settings.bookDockLeft, settings.bookDockTop );
    docks.book->resize( settings.bookDockWidth, settings.bookDockHeight );
    docks.book->show();
  }

  if ( settings.showHistory && settings.historyDockFloating
       && ! docks.history->isFloating() )
  {
    docks.history->hide();
    docks.history->setFloating( true );
    docks.history->move( settings.historyDockLeft, settings.historyDockTop );
    docks.history->resize( settings.historyDockWidth,
                           settings.historyDockHeight );
    docks.history->show();
  }

  if ( settings.showFunctions && settings.functionsDockFloating
       && ! docks.functions->isFloating() )
  {
    docks.functions->hide();
    docks.functions->setFloating( true );
    docks.functions->move( settings.functionsDockLeft,
                           settings.functionsDockTop );
    docks.functions->resize( settings.functionsDockWidth,
                             settings.functionsDockHeight );
    docks.functions->show();
  }

  if ( settings.showVariables && settings.variablesDockFloating
       && ! docks.variables->isFloating() )
  {
    docks.variables->hide();
    docks.variables->setFloating( true );
    docks.variables->move( settings.variablesDockLeft,
                           settings.variablesDockTop );
    docks.variables->resize( settings.variablesDockWidth,
                             settings.variablesDockHeight );
    docks.variables->show();
  }

  if ( settings.showConstants && settings.constantsDockFloating
       && ! docks.constants->isFloating() )
  {
    docks.constants->hide();
    docks.constants->setFloating( true );
    docks.constants->move( settings.constantsDockLeft,
                           settings.constantsDockTop );
    docks.constants->resize( settings.constantsDockWidth,
                             settings.constantsDockHeight );
    docks.constants->show();
  }
}


void MainWindow::Private::restoreHistory()
{
  if ( settings.historyResults.count() != settings.history.count() )
  {
    p->clearHistory();
    return;
  }

  widgets.editor->setHistory( settings.history );
  widgets.editor->setHistoryResults( settings.historyResults );
  widgets.display->appendHistory( settings.history, settings.historyResults );

  if ( docks.history )
    docks.history->setHistory( widgets.editor->history() );

  // free some useless memory
  settings.history.clear();
  settings.historyResults.clear();
}


void MainWindow::returnPressed()
{
  QString str = d->evaluator->autoFix( d->widgets.editor->text() );
  if ( str.isEmpty() )
    return;

  d->evaluator->setExpression( str );

  HNumber result = d->evaluator->evalUpdateAns();
  if ( ! d->evaluator->error().isEmpty() )
  {
    QMessageBox::warning( this, tr( "Error" ), d->evaluator->error() );
  }
  else
  {
    d->widgets.display->append( str, result );
    char * num = HMath::formatScientific( result, DECPRECISION );
    d->widgets.editor->appendHistory( str, num );
    free( num );
    d->widgets.editor->setAnsAvailable( true );
    if ( d->settings.showVariables )
      d->docks.variables->updateList( d->evaluator );
    if ( d->settings.showHistory )
      d->docks.history->append( str );
  }

  d->widgets.editor->setText( str );
  d->widgets.editor->selectAll();
  d->widgets.editor->stopAutoCalc();
  d->widgets.editor->stopAutoComplete();
  d->conditions.autoAns = true;

  QTimer::singleShot( 0, d->widgets.editor, SLOT( setFocus() ) );

  if ( ! isActiveWindow() )
    activateWindow();
}


void MainWindow::showTrayMessage()
{
  QString msg = tr( "SpeedCrunch is minimized.\nLeft click the icon to "
                    "restore it or right click for options." );
  if ( menuBar()->layoutDirection() == Qt::RightToLeft )
    msg += QChar( 0x200E );
  if ( d->widgets.trayIcon )
    d->widgets.trayIcon->showMessage( QString(), msg, QSystemTrayIcon::NoIcon,
                                      4000 );
}


void MainWindow::textChanged()
{
  if ( d->conditions.autoAns )
  {
    QString expr = d->evaluator->autoFix( d->widgets.editor->text() );
    if ( expr.isEmpty() )
      return;

    Tokens tokens = d->evaluator->scan( expr );
    if ( tokens.count() == 1 )
    {
      if (    tokens[0].asOperator() == Token::Plus
           || tokens[0].asOperator() == Token::Minus
           || tokens[0].asOperator() == Token::Asterisk
           || tokens[0].asOperator() == Token::Slash
           || tokens[0].asOperator() == Token::Caret )
      {
        d->conditions.autoAns = false;
        expr.prepend( "ans" );
        d->widgets.editor->setText( expr );
        d->widgets.editor->setCursorPosition( expr.length() );
      }
    }
  }
}


void MainWindow::trayIconActivated( QSystemTrayIcon::ActivationReason r )
{
  if ( r == QSystemTrayIcon::Context )
  {
    d->menus.trayIcon->show();
  }
  else
  {
    showNormal();
    activateWindow();
    d->widgets.editor->setFocus();
    QTimer::singleShot( 0, d->widgets.trayIcon, SLOT( hide() ) );

    // work around docks do not reappear if floating
    if ( d->docks.history )
      if ( d->docks.history->isFloating() )
      {
        d->docks.history->hide();
        d->docks.history->show();
      }
    if ( d->docks.functions )
      if ( d->docks.functions->isFloating() )
      {
        d->docks.functions->hide();
        d->docks.functions->show();
      }
    if ( d->docks.variables )
      if ( d->docks.variables->isFloating() )
      {
        d->docks.variables->hide();
        d->docks.variables->show();
      }
    if ( d->docks.constants )
      if ( d->docks.constants->isFloating() )
      {
        d->docks.constants->hide();
        d->docks.constants->show();
      }
  }
}


void MainWindow::variableSelected( const QString & v )
{
  if ( v.isEmpty() )
    return;

  d->widgets.editor->blockSignals( true );
  d->widgets.editor->insert( v );
  d->widgets.editor->blockSignals( false );

  QTimer::singleShot( 0, d->widgets.editor, SLOT( setFocus() ) );

  if ( ! isActiveWindow () )
    activateWindow();
}


void MainWindow::radixCharAutoActivated()
{
  setRadixChar(0);
}


void MainWindow::radixCharDotActivated()
{
  setRadixChar('.');
}


void MainWindow::radixCharCommaActivated()
{
  setRadixChar(',');
}


// protected

void MainWindow::closeEvent( QCloseEvent * e )
{
  d->saveSettings();
  e->accept();
}


void MainWindow::setPrecision( int p )
{
  if ( d->settings.precision == p )
    return;

  d->settings.precision = p;
  emit precisionChanged( p );
}


void MainWindow::setFormat( char c )
{
  if ( d->settings.format == c )
    return;

  d->settings.format = c;
  emit formatChanged( c );
}


void MainWindow::setRadixChar( char c )
{
  char oldRadixChar = d->settings.getRadixChar();
  if (c == 0)
    d->settings.setRadixChar();
  else
    d->settings.setRadixChar( c );
  c = d->settings.getRadixChar();
  if ( oldRadixChar != c )
    emit radixCharChanged( c );
}


void MainWindow::changeLanguage()
{
  QAction * a = d->actionGroups.language->checkedAction();
  QString lang = a->data().toString();
  if ( d->settings.language != lang )
    d->settings.language = lang;

  emit retranslateText();
}
