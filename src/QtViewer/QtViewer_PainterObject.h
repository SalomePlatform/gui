#ifndef QTVIEWER_PAINTEROBJECT_H
#define QTVIEWER_PAINTEROBJECT_H

#include "QtViewer.h"
#include <QPainter>

class QTVIEWER_API QtViewer_PainterObject 
{
public:

  QtViewer_PainterObject();
  ~QtViewer_PainterObject();

  virtual void draw(QPainter* painter); 
};
#endif
