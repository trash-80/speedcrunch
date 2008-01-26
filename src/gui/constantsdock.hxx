// This file is part of the SpeedCrunch project
// Copyright (C) 2007 Ariya Hidayat <ariya@kde.org>
// Copyright (C) 2008 Helder Correia <helder.pereira.correia@gmail.com>
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


#ifndef CONSTANTSDOCK_HXX
#define CONSTANTSDOCK_HXX


#include <QDockWidget>


class Constants;
struct ConstantsDockPrivate;

class QTreeWidgetItem;


class ConstantsDock : public QDockWidget
{
  Q_OBJECT

  public:
    explicit ConstantsDock( QWidget * parent );
    ~ConstantsDock();
    QChar radixChar() const;

  signals:
    void constantSelected( const QString & c );

  public slots:
    void setRadixChar( QChar );

  protected slots:
    void filter();
    void handleItem( QTreeWidgetItem * item );
    void triggerFilter();
    void updateList();

  private:
    ConstantsDockPrivate * const d;
};


#endif
