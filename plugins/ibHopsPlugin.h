/**
 *
 * This file is part of Tulip (www.tulip-software.org)
 *
 * Authors: David Auber and the Tulip development Team
 * from LaBRI, University of Bordeaux, University Corporation
 * for Atmospheric Research
 *
 * Tulip is free software; you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License
 * as published by the Free Software Foundation, either version 3
 * of the License, or (at your option) any later version.
 *
 * Tulip is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
 * See the GNU General Public License for more details.
 *
 */

#pragma once

#include <tulip/TulipPluginHeaders.h>



#ifndef IB_ROUTES_H
#define IB_ROUTES_H

class IbHops: public tlp::Algorithm {


public:
    PLUGININFORMATION("ibHops",
    "Ananta",
    "06/16/15",
    "Dijkstra's implementation.",
    "alpha",
    "Infiniband")

    IbHops(tlp::PluginContext* context);

    /**
     * @brief import infiniband routes
     * @warning currently only works if static data is retained from import
     */

    int min_distance(int dist[], bool visited[],int v);
    void printPath(int parent[], int j);
    bool run();
};

#endif // IB_ROUTES_H

