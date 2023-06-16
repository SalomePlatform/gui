// Copyright (C) 2014-2023  CEA/DEN, EDF R&D, OPEN CASCADE
//
// This library is free software; you can redistribute it and/or
// modify it under the terms of the GNU Lesser General Public
// License as published by the Free Software Foundation; either
// version 2.1 of the License, or (at your option) any later version.
//
// This library is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
// Lesser General Public License for more details.
//
// You should have received a copy of the GNU Lesser General Public
// License along with this library; if not, write to the Free Software
// Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307 USA
//
// See http://www.salome-platform.org/ or email : webmaster.salome@opencascade.com
//

#include "PVViewer_OutputWindow.h"

#include <vtkObjectFactory.h>

vtkStandardNewMacro(PVViewer_OutputWindow)

PVViewer_OutputWindow::PVViewer_OutputWindow()
{
}

PVViewer_OutputWindow::~PVViewer_OutputWindow()
{
}

unsigned int PVViewer_OutputWindow::getTextCount() const
{
  return count(MESSAGE_TYPE_TEXT);
}

unsigned int PVViewer_OutputWindow::getErrorCount() const
{
  return count(MESSAGE_TYPE_ERROR);
}

unsigned int PVViewer_OutputWindow::getWarningCount() const
{
  return count(MESSAGE_TYPE_WARNING);
}

unsigned int PVViewer_OutputWindow::getGenericWarningCount() const
{
  return count(MESSAGE_TYPE_GENERIC_WARNING);
}

unsigned int PVViewer_OutputWindow::getDebugCount() const
{
  return count(MESSAGE_TYPE_DEBUG);
}

void PVViewer_OutputWindow::DisplayText(const char* text)
{
  MessageTypes type = GetCurrentMessageType();
  myCounter[type] = count(type) + 1;
  switch (type)
  {
  case MESSAGE_TYPE_ERROR:
    qCritical(text);
    break;
  case MESSAGE_TYPE_WARNING:
  case MESSAGE_TYPE_GENERIC_WARNING:
    qWarning(text);
    break;
  case MESSAGE_TYPE_DEBUG:
    qDebug(text);
    break;
  case MESSAGE_TYPE_TEXT:
  default:
    qInfo(text);
    break;
  }
}

int PVViewer_OutputWindow::count(const MessageTypes& type) const
{
  return myCounter.value(type, 0);
}
