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
#include "ibautils/ib_fabric.h"

#ifndef IB_TOPOLOGY_H
#define IB_TOPOLOGY_H

/// http://tulip.labri.fr/Documentation/current/tulip-dev/html/plugin_development.html#plugins-import
///  http://tulip.labri.fr/Documentation/current/doxygen/html/clique_import_2cliqueimport_8cpp-example.html

class ImportInfinibandTopology: public tlp::ImportModule {
public:
  PLUGININFORMATION("Infiniband Topology Import",
                    "Nathan Rini",
                    "04/15/15",
                    "Import physical topology of Infiniband fabric. Each infiniband chip will be represented as a node and each cable as 2 directional edges.",
                    "alpha",
                    "Infiniband") 
  
  ImportInfinibandTopology(tlp::PluginContext* context);

  bool importGraph();
};

#endif // IB_TOPOLOGY_H

