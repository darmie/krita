/*
 * Copyright (c) 2017 Wolthera van Hövell tot Westerflier <griffinvalley@gmail.com>
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program; if not, write to the Free Software
 *  Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301, USA.
 */
#include "kis_edge_detection_filter.h"
#include "kis_wdg_edge_detection.h"
#include <kis_edge_detection_kernel.h>
#include <kis_convolution_kernel.h>
#include <kis_convolution_painter.h>

#include <KoColorSpaceRegistry.h>
#include <KoColorModelStandardIds.h>

#include <filter/kis_filter_category_ids.h>
#include <filter/kis_filter_configuration.h>
#include <kis_selection.h>
#include <kis_paint_device.h>
#include <kis_processing_information.h>
#include "kis_lod_transform.h"

#include <kpluginfactory.h>

#include <klocalizedstring.h>
#include <filter/kis_filter_registry.h>

K_PLUGIN_FACTORY_WITH_JSON(KritaEdgeDetectionFilterFactory, "kritaedgedetection.json", registerPlugin<KritaEdgeDetectionFilter>();)

KritaEdgeDetectionFilter::KritaEdgeDetectionFilter(QObject *parent, const QVariantList &)
    : QObject(parent)
{
    KisFilterRegistry::instance()->add(KisFilterSP(new KisEdgeDetectionFilter()));
}

KritaEdgeDetectionFilter::~KritaEdgeDetectionFilter()
{
}

KisEdgeDetectionFilter::KisEdgeDetectionFilter(): KisFilter(id(), FiltersCategoryEdgeDetectionId, i18n("&Edge Detection..."))
{
    setSupportsPainting(true);
    setSupportsAdjustmentLayers(true);
    setSupportsLevelOfDetail(true);
    setColorSpaceIndependence(FULLY_INDEPENDENT);
    setShowConfigurationWidget(true);
}

void KisEdgeDetectionFilter::processImpl(KisPaintDeviceSP device, const QRect &rect, const KisFilterConfigurationSP config, KoUpdater *progressUpdater) const
{
    Q_ASSERT(device != 0);

    KisFilterConfigurationSP configuration = config ? config : new KisFilterConfiguration(id().id(), 1);

    KisLodTransformScalar t(device);

    QVariant value;
    configuration->getProperty("horizRadius", value);
    float horizontalRadius = t.scale(value.toFloat());
    configuration->getProperty("vertRadius", value);
    float verticalRadius = t.scale(value.toFloat());

    QBitArray channelFlags;
    if (configuration) {
        channelFlags = configuration->channelFlags();
    }

    KisEdgeDetectionKernel::FilterType type = KisEdgeDetectionKernel::SobelVector;
    if (config->getString("type") == "prewitt") {
        type = KisEdgeDetectionKernel::Prewit;
    } else if (config->getString("type") == "simple") {
        type = KisEdgeDetectionKernel::Simple;
    }

    KisEdgeDetectionKernel::FilterOutput output = KisEdgeDetectionKernel::pythagorean;
    if (config->getString("output") == "xGrowth") {
        output = KisEdgeDetectionKernel::xGrowth;
    } else if (config->getString("output") == "xFall") {
        output = KisEdgeDetectionKernel::xFall;
    } else if (config->getString("output") == "yGrowth") {
        output = KisEdgeDetectionKernel::yGrowth;
    } else if (config->getString("output") == "yFall") {
        output = KisEdgeDetectionKernel::yFall;
    } else if (config->getString("output") == "radian") {
        output = KisEdgeDetectionKernel::radian;
    }

    KisEdgeDetectionKernel::applyEdgeDetection(device,
                                               rect,
                                               horizontalRadius,
                                               verticalRadius,
                                               type,
                                               channelFlags,
                                               progressUpdater,
                                               output,
                                               config->getBool("transparency", false));
}

KisFilterConfigurationSP KisEdgeDetectionFilter::factoryConfiguration() const
{
    KisFilterConfigurationSP config = new KisFilterConfiguration(id().id(), 1);
    config->setProperty("horizRadius", 1);
    config->setProperty("vertRadius", 1);
    config->setProperty("type", "prewitt");
    config->setProperty("output", "pythagorean");
    config->setProperty("lockAspect", true);
    config->setProperty("transparency", false);

    return config;
}

KisConfigWidget *KisEdgeDetectionFilter::createConfigurationWidget(QWidget *parent, const KisPaintDeviceSP dev, bool) const
{
    Q_UNUSED(dev);
    return new KisWdgEdgeDetection(parent);
}

QRect KisEdgeDetectionFilter::neededRect(const QRect &rect, const KisFilterConfigurationSP _config, int lod) const
{
    KisLodTransformScalar t(lod);

    QVariant value;
    /**
     * NOTE: integer division by two is done on purpose,
     *       because the kernel size is always odd
     */
    const int halfWidth = _config->getProperty("horizRadius", value) ? KisEdgeDetectionKernel::kernelSizeFromRadius(t.scale(value.toFloat())) / 2 : 5;
    const int halfHeight = _config->getProperty("vertRadius", value) ? KisEdgeDetectionKernel::kernelSizeFromRadius(t.scale(value.toFloat())) / 2 : 5;

    return rect.adjusted(-halfWidth * 2, -halfHeight * 2, halfWidth * 2, halfHeight * 2);
}

QRect KisEdgeDetectionFilter::changedRect(const QRect &rect, const KisFilterConfigurationSP _config, int lod) const
{
    KisLodTransformScalar t(lod);

    QVariant value;

    const int halfWidth = _config->getProperty("horizRadius", value) ? KisEdgeDetectionKernel::kernelSizeFromRadius(t.scale(value.toFloat())) / 2 : 5;
    const int halfHeight = _config->getProperty("vertRadius", value) ? KisEdgeDetectionKernel::kernelSizeFromRadius(t.scale(value.toFloat())) / 2 : 5;

    return rect.adjusted( -halfWidth, -halfHeight, halfWidth, halfHeight);
}

#include "kis_edge_detection_filter.moc"
