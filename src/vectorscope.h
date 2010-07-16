/***************************************************************************
 *   Copyright (C) 2010 by Simon Andreas Eugster (simon.eu@gmail.com)      *
 *   This file is part of kdenlive. See www.kdenlive.org.                  *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 ***************************************************************************/

#ifndef VECTORSCOPE_H
#define VECTORSCOPE_H

#include <QtCore>
#include "renderer.h"
#include "monitor.h"
#include "colorplaneexport.h"
#include "colortools.h"
#include "ui_vectorscope_ui.h"

class Render;
class Monitor;
class Vectorscope_UI;

enum PAINT_MODE { PAINT_GREEN = 0, PAINT_ORIG = 1, PAINT_CHROMA = 2, PAINT_YUV = 3, PAINT_BLACK = 4, PAINT_GREEN2 = 5 };
enum BACKGROUND_MODE { BG_NONE = 0, BG_YUV = 1, BG_CHROMA = 2 };

class Vectorscope : public QWidget, public Ui::Vectorscope_UI {
    Q_OBJECT

public:
    Vectorscope(Monitor *projMonitor, Monitor *clipMonitor, QWidget *parent = 0);
    ~Vectorscope();

protected:
    void paintEvent(QPaintEvent *);
    void resizeEvent(QResizeEvent *event);
    void mousePressEvent(QMouseEvent *);
    void mouseMoveEvent(QMouseEvent *event);

private:
    Monitor *m_projMonitor;
    Monitor *m_clipMonitor;
    Render *m_activeRender;

    ColorTools *m_colorTools;
    ColorPlaneExport *m_colorPlaneExport;

    QAction *m_aExportBackground;
    QAction *m_aAxisEnabled;
    QAction *m_a75PBox;
    QAction *m_aRealtime;

    /** How to represent the pixels on the scope (green, original color, ...) */
    int iPaintMode;

    /** Custom scaling of the vectorscope */
    float m_scaling;

    /** Number of pixels to skip for getting realtime updates */
    int m_skipPixels;

    QPoint mapToCanvas(QRect inside, QPointF point);
    QPoint centerPoint, pR75, pG75, pB75, pCy75, pMg75, pYl75;

    bool circleEnabled;
    QPoint mousePos;

    /** Updates the dimension. Only necessary when the widget has been resized. */
    void updateDimensions();
    bool initialDimensionUpdateDone;
    QRect m_scopeRect;
    int cw;


    /** The vectorscope color distribution.
        Class variable as calculated by a thread. */
    QImage m_scope;
    QImage m_wheel;

    void calculateScope();
    QFuture<void> m_scopeCalcThread;
    QFuture<QImage> m_wheelCalcThread;

    /** This semaphore that guards QFuture m_scopeCalcThread is necessary for avoiding
        deadlocks. If not present, then an incoming new frame might trigger a new thread
        at the wrong point in time, causing a deadlock. Nasty ;) */
    QSemaphore semaphore;

    /** Prods the Scope calculation thread. If it is running, do nothing. If it is not,
      run a new thread.
      Returns true if a new thread has been started. */
    bool prodCalcThread();
    bool prodWheelThread();

    /** Counts the number of frames that have been rendered in one of the monitors.
      The frame number will be reset when the vectorscope starts calculating the
      current frame. */
    QAtomicInt newFrames;
    /** Counts the number of other changes that should cause the vectorscope to be
      recalculated. This is for example a resizeEvent. In this case, no new frames
      are generated, but the scope has to be updated in any case (also if auto-update
      is not enabled). */
    QAtomicInt newChanges;
    /** Counts the number of changes concerning the background wheel */
    QAtomicInt newWheelChanges;

signals:
    void signalScopeCalculationFinished(const unsigned int &mseconds, const unsigned int &skipPixels);

private slots:
    void slotMagnifyChanged();
    void slotBackgroundChanged();
    void slotActiveMonitorChanged(bool isClipMonitor);
    void slotRenderZoneUpdated();
    void slotScopeCalculationFinished(unsigned int mseconds, unsigned int skipPixels);
    void slotWheelCalculationFinished();
    void slotUpdateScope();
    void slotUpdateWheel();
    void slotExportBackground();
};

#endif // VECTORSCOPE_H
