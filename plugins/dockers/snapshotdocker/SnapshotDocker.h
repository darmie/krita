/* This file is part of the KDE project
 * Copyright (C) 2010 Matus Talcik <matus.talcik@gmail.com>
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Library General Public
 * License as published by the Free Software Foundation; either
 * version 2 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Library General Public License for more details.
 *
 * You should have received a copy of the GNU Library General Public License
 * along with this library; see the file COPYING.LIB.  If not, write to
 * the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
 * Boston, MA 02110-1301, USA.
 */
#ifndef SNAPSHOT_DOCKER_H_
#define SNAPSHOT_DOCKER_H_

#include <QDockWidget>
#include <QScopedPointer>

#include <kis_mainwindow_observer.h>
#include <klocalizedstring.h>

#include <KoShapeController.h>
#include <KoCanvasBase.h>

class SnapshotDocker : public QDockWidget, public KisMainwindowObserver
{
    Q_OBJECT
public:
    SnapshotDocker();
    ~SnapshotDocker() override;

    QString observerName() override { return "SnapshotDocker"; }

    void setViewManager(KisViewManager* viewManager) override;
    void setCanvas(KoCanvasBase *canvas) override;
    void unsetCanvas() override;

private Q_SLOTS:
    void slotBnAddClicked();
    void slotBnSwitchToClicked();
    void slotBnRemoveClicked();

private:
    struct Private;
    QScopedPointer<Private> m_d;
};

#endif
