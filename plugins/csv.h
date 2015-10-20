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

#ifndef IB_CSV_H
#define IB_CSV_H

/**
 * @brief Import Infiniband CSV files
 *
 * Like the routes import, this also uses the data structs from
 * first import. This is not required but much faster.
 *
 */
class ImportInfinibandCSV: public tlp::Algorithm {
public:
  PLUGININFORMATION("Infiniband CSV Importer",
                    "Nathan Rini",
                    "06/18/15",
                    "Apply properties to IB fabric based on input from CSV which uses hex GUID and port as key entries.",
                    "alpha",
                    "Infiniband") 
  
  ImportInfinibandCSV(tlp::PluginContext* context);

  /**
   * @brief import infiniband properties from CSV
   * @warning currently only works if static data is retained from topology import
   */
  bool run();
};

#endif // IB_CSV_H

