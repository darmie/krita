/* Copyright (C) 2017 Boudewijn Rempt <boud@valdyas.org>

   This library is free software; you can redistribute it and/or
   modify it under the terms of the GNU Library General Public
   License as published by the Free Software Foundation; either
   version 2 of the License, or (at your option) any later version.

   This library is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   Library General Public License for more details.

   You should have received a copy of the GNU Library General Public License
   along with this library; see the file COPYING.LIB.  If not, write to
   the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
 * Boston, MA 02110-1301, USA.
*/
#include "TestNode.h"
#include <QTest>
#include <QColor>
#include <QDataStream>

#include <KritaVersionWrapper.h>
#include <Node.h>
#include <Krita.h>

#include <KoColorSpaceRegistry.h>
#include <KoColorProfile.h>
#include <KoColor.h>

#include <kis_image.h>
#include <kis_fill_painter.h>
#include <kis_paint_layer.h>

void TestNode::testSetColorSpace()
{
    KisImageSP image = new KisImage(0, 100, 100, KoColorSpaceRegistry::instance()->rgb8(), "test");
    KisNodeSP layer = new KisPaintLayer(image, "test1", 255);
    Node node(image, layer);
    QStringList profiles = Krita().profiles("GRAYA", "U16");
    node.setColorSpace("GRAYA", "U16", profiles.first());
    QVERIFY(layer->colorSpace()->colorModelId().id() == "GRAYA");
    QVERIFY(layer->colorSpace()->colorDepthId().id() == "U16");
    QVERIFY(layer->colorSpace()->profile()->name() == "gray built-in");
}

void TestNode::testSetColorProfile()
{
    KisImageSP image = new KisImage(0, 100, 100, KoColorSpaceRegistry::instance()->rgb8(), "test");
    KisNodeSP layer = new KisPaintLayer(image, "test1", 255);
    Node node(image, layer);
    QStringList profiles = Krita().profiles("RGBA", "U8");
    Q_FOREACH(const QString &profile, profiles) {
        node.setColorProfile(profile);
        QVERIFY(layer->colorSpace()->profile()->name() == profile);
    }
}

void TestNode::testPixelData()
{
    KisImageSP image = new KisImage(0, 100, 100, KoColorSpaceRegistry::instance()->rgb8(), "test");
    KisNodeSP layer = new KisPaintLayer(image, "test1", 255);
    KisFillPainter gc(layer->paintDevice());
    gc.fillRect(0, 0, 100, 100, KoColor(Qt::red, layer->colorSpace()));
    Node node(image, layer);
    QByteArray ba = node.pixelData(0, 0, 100, 100);
    QDataStream ds(ba);
    do {
        quint8 channelvalue;
        ds >> channelvalue;
        QVERIFY(channelvalue == 0);
        ds >> channelvalue;
        QVERIFY(channelvalue == 0);
        ds >> channelvalue;
        QVERIFY(channelvalue == 255);
        ds >> channelvalue;
        QVERIFY(channelvalue == 255);
    } while (!ds.atEnd());

    QDataStream ds2(&ba, QIODevice::WriteOnly);
    for (int i = 0; i < 100 * 100; i++) {
        ds2 << 255;
        ds2 << 255;
        ds2 << 255;
        ds2 << 255;
    }

    node.setPixelData(ba, 0, 0, 100, 100);
    for (int i = 0; i < 100 ; i++) {
        for (int j = 0; j < 100 ; j++) {
            QColor pixel;
            layer->paintDevice()->pixel(i, j, &pixel);
            QVERIFY(pixel == QColor(Qt::black));
        }
    }
}

void TestNode::testProjectionPixelData()
{
    KisImageSP image = new KisImage(0, 100, 100, KoColorSpaceRegistry::instance()->rgb8(), "test");
    KisNodeSP layer = new KisPaintLayer(image, "test1", 255);
    KisFillPainter gc(layer->paintDevice());
    gc.fillRect(0, 0, 100, 100, KoColor(Qt::gray, layer->colorSpace()));
    Node node(image, layer);
    QByteArray ba = node.projectionPixelData(0, 0, 100, 100);
    QDataStream ds(ba);
    for (int i = 0; i < 100 * 100; i++) {
        quint8 channelvalue;
        ds >> channelvalue;
        QVERIFY(channelvalue == 0xA4);
        ds >> channelvalue;
        QVERIFY(channelvalue == 0xA0);
        ds >> channelvalue;
        QVERIFY(channelvalue == 0xA0);
        ds >> channelvalue;
        QVERIFY(channelvalue == 0xFF);
    }
}

void TestNode::testThumbnail()
{
    KisImageSP image = new KisImage(0, 100, 100, KoColorSpaceRegistry::instance()->rgb8(), "test");
    KisNodeSP layer = new KisPaintLayer(image, "test1", 255);
    KisFillPainter gc(layer->paintDevice());
    gc.fillRect(0, 0, 100, 100, KoColor(Qt::gray, layer->colorSpace()));
    Node node(image, layer);
    QImage thumb = node.thumbnail(10, 10);
    thumb.save("thumb.png");
    QVERIFY(thumb.width() == 10);
    QVERIFY(thumb.height() == 10);
    // Our thumbnail calculator in KisPaintDevice cannot make a filled 10x10 thumbnail from a 100x100 device,
    // it makes it 10x10 empty, then puts 8x8 pixels in there... Not a bug in the Node class
    for (int i = 0; i < 8; ++i) {
        for (int j = 0; j < 8; ++j) {
            QVERIFY(thumb.pixelColor(i, j) == QColor(Qt::gray));
        }
    }
}

void TestNode::testMergeDown()
{
    KisImageSP image = new KisImage(0, 100, 100, KoColorSpaceRegistry::instance()->rgb8(), "test");

    KisNodeSP layer = new KisPaintLayer(image, "test1", 255);
    {
        KisFillPainter gc(layer->paintDevice());
        gc.fillRect(0, 0, 100, 100, KoColor(Qt::gray, layer->colorSpace()));
    }
    image->addNode(layer);

    KisNodeSP layer2 = new KisPaintLayer(image, "test2", 255);
    {
        KisFillPainter gc(layer2->paintDevice());
        gc.fillRect(0, 0, 100, 100, KoColor(Qt::gray, layer2->colorSpace()));
    }
    image->addNode(layer2);
    Node n1(image, layer);
    Node *n2 = n1.mergeDown();
    delete n2;
}

QTEST_MAIN(TestNode)

