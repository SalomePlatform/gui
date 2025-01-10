// Copyright (C) 2017-2025  CEA, EDF
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
// Author : Anthony GEAY (EDF R&D)

#ifndef __QREMOTEFILEBROWSER__
#define __QREMOTEFILEBROWSER__

#include "RemoteFileBrowser.h"

#include "QWidget"
#include "QTreeView"
#include "QMimeData"
#include "QThread"

class AnotherTreeView;
class QMachineBrowser;
class QRemoteFileSystemModel;
class TopDirDataStructure;

class QREMOTEFILEBROWSER_EXPORT LoadingThread : public QThread
{
  Q_OBJECT
public:
  LoadingThread(QThread *th, QMachineBrowser *mb):_fatherThread(th),_mb(mb),_model(nullptr) { }
  void setGeneratedModel(QRemoteFileSystemModel *model) { _model=model; }
  QRemoteFileSystemModel *generatedModel() const { return _model; }
signals:
  void letsGenerateModel(TopDirDataStructure *fds);
protected:
  void run();
private:
  QThread *_fatherThread;
  QMachineBrowser *_mb;
  QRemoteFileSystemModel *_model;
};

class QREMOTEFILEBROWSER_EXPORT QRemoteFileBrowser : public QWidget
{
  Q_OBJECT
public:
  QRemoteFileBrowser(QWidget *parent);
  QMachineBrowser *machineBrower() const { return _mb; }
public slots:
  void onLocationChanged();
  void locationHasBeenChanged();
private:
  AnotherTreeView *_treeView;
  QMachineBrowser *_mb;
};

class QREMOTEFILEBROWSER_EXPORT QRemoteFileTransfer : public QWidget
{
public:
  QRemoteFileTransfer(QWidget *parent=0);
private:
  QRemoteFileBrowser *_left;
  QRemoteFileBrowser *_right;
};

class DirDataStructure;
class TopDirDataStructure;

class QREMOTEFILEBROWSER_EXPORT DataStructure : public QObject
{
  Q_OBJECT
public:
  DataStructure(QObject *parent, const QString& name):QObject(parent),_name(name),_selected(false) { }
  void select() { _selected=true; }
  void unselect() { _selected=false; }
  bool isSelected() const { return _selected; }
  virtual bool isFile() const = 0;
  virtual QString nameOnDrop() const = 0;
  const DirDataStructure *getDirParent() const;
  bool isRoot() const { return getDirParent()==NULL; }
  const TopDirDataStructure *getRoot() const;
  std::vector<const DataStructure *> getItermediateElts(const TopDirDataStructure *tpds) const;
  virtual int size() const = 0;
  QString entryForRSyncSrc() const;
  virtual QString entryForRSyncDest() const = 0;
  QString name() const;
  const QString& fullName() const { return _name; }
  void removeFileArgs(QString& prg, QStringList& args) const;
private:
  QString _name;
  bool _selected;
};

class QREMOTEFILEBROWSER_EXPORT FileDataStructure : public DataStructure
{
public:
  FileDataStructure(DirDataStructure *dds, const QString& name);
  bool isFile() const { return true; }
  int size() const { return 0; }
  QString entryForRSyncDest() const;
  QString nameOnDrop() const;
};

class QREMOTEFILEBROWSER_EXPORT DirDataStructure : public DataStructure
{
public:
  DirDataStructure(DirDataStructure *dds, const QString& name):DataStructure(dds,name),_is_loaded(false),_is_expanded(false) { }
  DirDataStructure(QObject *dds, const QString& name):DataStructure(dds,name),_is_loaded(false) { }
  bool isFile() const { return false; }
  int size() const { load(); return children().size(); }
  QString entryForRSyncDest() const;
  QString nameOnDrop() const { return name(); }
  const DataStructure *operator[](int pos) const;
  int posOf(const DataStructure *ds) const;
  bool load() const;
  void markAsLoaded() const { _is_loaded=true; }
  void setExpanded(bool status) { _is_expanded=status; }
  bool isExpanded() const { return _is_expanded; }
private:
  mutable bool _is_loaded;
  mutable bool _is_expanded;
};

class FileLoader;

class QREMOTEFILEBROWSER_EXPORT TopDirDataStructure : public DirDataStructure
{
public:
  TopDirDataStructure(QObject *dds, FileLoader *fl);
  virtual ~TopDirDataStructure();
  FileLoader *getLoader() const { return _fl; }
  bool isOK() const { return _isOK; }
  QString entryForRSync(const QString& fn) const;
  QString getMachine() const;
  void removeFileArgsImpl(const QString& filePath, QString& prg, QStringList& args) const;
private:
  FileLoader *_fl;
  bool _isOK;
};

class QRemoteFileSystemModel;

class QREMOTEFILEBROWSER_EXPORT MyTreeView : public QTreeView
{
  Q_OBJECT
public:
  MyTreeView(QWidget *parent);
  void mousePressEvent(QMouseEvent *event);
  void mouseReleaseEvent(QMouseEvent *event);
  void mouseMoveEvent(QMouseEvent *event);
  void keyPressEvent(QKeyEvent *event);
  void dragEnterEvent(QDragEnterEvent *event);
  void dragMoveEvent(QDragMoveEvent *event);
  void dragLeaveEvent(QDragLeaveEvent *event);
  void dropEvent(QDropEvent *event);
  QRemoteFileSystemModel *zeModel();
  void emitResetModel();
public slots:
  void itemExpanded(const QModelIndex &index);
  void itemCollapsed(const QModelIndex &index);
signals:
  void somethingChangedDueToFileModif();
private:
  void itemExpandedStatus(const QModelIndex &index, bool status);
  void drawBranches(QPainter *painter, const QRect &rect, const QModelIndex &index) const;
  void paintEvent(QPaintEvent *event);
private:
  QPoint _start_pos;
  // during drag drop _sel is the element under the mouse on the drop site
  DataStructure *_sel;
  // during drag drop _slider_pos is the pos of vertical slider on drop site
  int _slider_pos;
};

class AnotherTreeView;

class QREMOTEFILEBROWSER_EXPORT AnotherTreeViewPainter
{
public:
  virtual void paint(AnotherTreeView *atv, QPaintEvent *event) const = 0;
};

class QREMOTEFILEBROWSER_EXPORT AnotherTreeViewWaitPainter : public AnotherTreeViewPainter
{
public:
  void paint(AnotherTreeView *atv, QPaintEvent *event) const;
};

class QREMOTEFILEBROWSER_EXPORT AnotherTreeViewNothingPainter : public AnotherTreeViewPainter
{
public:
  void paint(AnotherTreeView *atv, QPaintEvent *event) const;
};

class QREMOTEFILEBROWSER_EXPORT AnotherTreeView : public QWidget
{
  Q_OBJECT
public:
  AnotherTreeView(QWidget *parent);
  void generateModel(QMachineBrowser *mb);
  QSize sizeHint() const;
  QSize minimumSizeHint() const;
  int getAngle() const { return _angle; }
  ~AnotherTreeView();
public slots:
  void goGenerate(TopDirDataStructure *fds);
  void modelHasBeenGenerated();
signals:
  void modelHasBeenGeneratedSignal(bool isOK);
  void somethingChangedDueToFileModif();
protected:
  void paintEvent(QPaintEvent *event);
  void timerEvent(QTimerEvent *e);
private:
  int _timerId;
  int _angle;
  AnotherTreeViewPainter *_painter;
  MyTreeView *_tw;
  LoadingThread *_th;
};

class QREMOTEFILEBROWSER_EXPORT FileLoader
{
protected:
  FileLoader(const QString& dirName):_dirName(dirName) { }
public:
  QString getDirName() const { return _dirName; }
  virtual bool load(DirDataStructure *parent) const = 0;
  virtual QString prettyPrint() const = 0;
  virtual QString entryForRSync(const QString& fn) const = 0;
  virtual QString getMachine() const = 0;
  virtual void removeFileArgs(const QString& filePath, QString& prg, QStringList& args) const = 0;
  virtual ~FileLoader() { }
private:
  QString _dirName;
};

class QREMOTEFILEBROWSER_EXPORT LocalFileLoader : public FileLoader
{
public:
  LocalFileLoader(const QString& dirName):FileLoader(dirName) { }
  bool load(DirDataStructure *parent) const;
  void fillArgs(const QString& dn, QString& prg, QStringList& args) const;
  QString prettyPrint() const;
  QString entryForRSync(const QString& fn) const;
  QString getMachine() const;
  void removeFileArgs(const QString& filePath, QString& prg, QStringList& args) const;
};

class QREMOTEFILEBROWSER_EXPORT RemoteFileLoader : public FileLoader
{
public:
  RemoteFileLoader(const QString& machine,const QString& dirName):FileLoader(dirName),_machine(machine) { }
  bool load(DirDataStructure *parent) const;
  void fillArgs(const QString& dn, QString& prg, QStringList& args) const;
  QString prettyPrint() const;
  QString entryForRSync(const QString& fn) const;
  QString getMachine() const;
  void removeFileArgs(const QString& filePath, QString& prg, QStringList& args) const;
private:
  QString _machine;
};

class QREMOTEFILEBROWSER_EXPORT SelectionMimeData : public QMimeData
{
  Q_OBJECT
public:
  SelectionMimeData(const QModelIndexList& los):_los(los) { }
  const QModelIndexList& getSelection() const { return _los; }
private:
  QModelIndexList _los;
};

class QREMOTEFILEBROWSER_EXPORT QRemoteFileSystemModel : public QAbstractItemModel
{
  Q_OBJECT
public:
  QRemoteFileSystemModel(QObject *parent, FileLoader *fl);
  QRemoteFileSystemModel(QObject *parent, TopDirDataStructure *fds);
  QVariant headerData(int section, Qt::Orientation orientation, int role) const;
  QModelIndex parent(const QModelIndex&) const;
  QModelIndex index(int, int, const QModelIndex&) const;
  int rowCount(const QModelIndex&) const;
  int columnCount(const QModelIndex&) const;
  QVariant data(const QModelIndex&, int) const;
  void emitResetModel();
  FileLoader *getLoader() const { return _fds->getLoader(); }
  bool isOK() const { return _fds->isOK(); }
  QString getMachine() const { return _fds->getMachine(); }
private:
  TopDirDataStructure *_fds;
};

#endif
