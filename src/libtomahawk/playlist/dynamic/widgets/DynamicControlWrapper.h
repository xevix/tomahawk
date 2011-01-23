/****************************************************************************************
 * Copyright (c) 2010-2011 Leo Franchi <lfranchi@kde.org>                               *
 *                                                                                      *
 * This program is free software; you can redistribute it and/or modify it under        *
 * the terms of the GNU General Public License as published by the Free Software        *
 * Foundation; either version 2 of the License, or (at your option) any later           *
 * version.                                                                             *
 *                                                                                      *
 * This program is distributed in the hope that it will be useful, but WITHOUT ANY      *
 * WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A      *
 * PARTICULAR PURPOSE. See the GNU General Public License for more details.             *
 *                                                                                      *
 * You should have received a copy of the GNU General Public License along with         *
 * this program.  If not, see <http://www.gnu.org/licenses/>.                           *
 ****************************************************************************************/

#ifndef DYNAMIC_CONTROL_WRAPPER_H
#define DYNAMIC_CONTROL_WRAPPER_H

#include <QWidget>

#include "typedefs.h"

class QGridLayout;
class ReadOrWriteWidget;
class QStackedLayout;
class QEvent;
class QToolButton;
class QHBoxLayout;
class QComboBox;
class QLabel;;

namespace Tomahawk
{

/**
* This abstraction object manages the widgets for 1 dynamic playlist control, laid out in the desired layout
*/
class DynamicControlWrapper : public QObject
{
    Q_OBJECT 
public:
    explicit DynamicControlWrapper( const dyncontrol_ptr& control, QGridLayout* layout, int row, bool isLocal = false, QWidget* parent = 0 );
    virtual ~DynamicControlWrapper();
           
//     virtual void enterEvent(QEvent* );
//     virtual void leaveEvent(QEvent* );
    
    dyncontrol_ptr control() const;
    
    void removeFromLayout();
signals:
    void collapse();
    void removeControl();
    void changed();
    
private slots:
    void typeSelectorChanged( const QString& selectedType, bool firstLoad = false );
    
private:
    QToolButton* initButton();
    QWidget* createDummy( QWidget* fromW );
    
    bool m_isLocal, m_mouseOver;
    
    QWidget* m_parent;
    int m_row;
    QStackedLayout* m_plusL;
    QToolButton* m_minusButton;
    
    dyncontrol_ptr m_control;
    ReadOrWriteWidget* m_typeSelector;
    ReadOrWriteWidget* m_matchSelector;
    ReadOrWriteWidget* m_entryWidget;
    QGridLayout* m_layout;
};
    
};

#endif

class QPaintEvent;

class QMouseEvent;