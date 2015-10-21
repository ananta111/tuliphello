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

#include<fstream>
#include "fabric.h"
#include "topology.h"
#include "ibautils/ib_fabric.h"
#include "ibautils/ib_parser.h"

PLUGIN(ImportInfinibandTopology)

static const char * paramHelp[] = {
  // File to Open
  HTML_HELP_OPEN() \
  HTML_HELP_DEF( "type", "pathname" ) \
  HTML_HELP_BODY() \
  "Path to file to import" \
  HTML_HELP_CLOSE(),
  
  // Import Type
  HTML_HELP_OPEN() \
  HTML_HELP_DEF( "type", "String Collection" ) \
  HTML_HELP_DEF( "values", "ibnetdiscover -p" ) \
  HTML_HELP_DEF( "default", "ibnetdiscover -p" ) \
  HTML_HELP_BODY() \
  "Import topology of given type. " \
  "Currently, only \'ibnetdiscover -p\' is supported. <BR>" \
  HTML_HELP_CLOSE(),
  
  // Preserve Data
  HTML_HELP_OPEN() \
  HTML_HELP_DEF( "type", "bool" ) \
  HTML_HELP_DEF( "default", "true" ) \
  HTML_HELP_BODY() \
  "Preserve all Infiniband Data (in memory) to allow later Infinband calls to use this imported graph." \
  HTML_HELP_CLOSE(),
  
  // Populate Fields
  HTML_HELP_OPEN() \
  HTML_HELP_DEF( "type", "bool" ) \
  HTML_HELP_DEF( "default", "true" ) \
  HTML_HELP_BODY() \
  "Populate property fields of every node and cable in Tulip to allow other tools to use data." \
  HTML_HELP_CLOSE(),
};

static const char IMPORT_TYPE_STRING[] = "ibnetdiscover -p";
enum import_t {
    IBNETDISCOVERP = 0,
    IBDIAGNET2FDBS
};

ImportInfinibandTopology::ImportInfinibandTopology(tlp::PluginContext* context)
  : tlp::ImportModule(context)
{
  addInParameter<std::string>("file::filename", paramHelp[0],"");
  addInParameter<tlp::StringCollection>("Import Type",paramHelp[1],IMPORT_TYPE_STRING);
  addInParameter<bool>("Preserve Data",paramHelp[2],"true");
  addInParameter<bool>("Populate Fields",paramHelp[3],"true");
}

namespace ib = infiniband;
namespace ibp = infiniband::parser;

bool ImportInfinibandTopology::importGraph()
{
  assert(graph);

  static const size_t STEPS = 5;
  if(pluginProgress)
  {
    pluginProgress->showPreview(false);
    pluginProgress->setComment("Starting to Import");
    pluginProgress->progress(0, STEPS);
  }

  bool preserveData = false;
  dataSet->get("Preserve Data", preserveData);
  bool populateFields = false;
  dataSet->get("Populate Fields", populateFields);

  ib::tulip_fabric_t * const fabric = preserveData ?  ib::tulip_fabric_t::find_fabric(graph, true) : new ib::tulip_fabric_t(graph);
  assert(fabric);

  /**
   * Open file to read and import per type
   */
  {
    tlp::StringCollection import_types;
    std::string filename;
    
    dataSet->get("file::filename", filename);
    std::ifstream ifs(filename.c_str());
    if(!ifs)
      return false;
    
    dataSet->get("Import Type", import_types);

    switch(import_types.getCurrent())
    {
      case IBNETDISCOVERP:
      {
          ibp::ibnetdiscover_p_t parser;
          ibp::ibnetdiscover_p_t::portmap_t portmap;
          
          if(pluginProgress)
          {
            pluginProgress->setComment("Starting to Import");
            pluginProgress->progress(1, STEPS);
          }

          if(!parser.parse(portmap, ifs))
          {
            if(pluginProgress)
              pluginProgress->setError("Unable to parse input file.");

            return false;
          }

          if(pluginProgress)
          {
            pluginProgress->setComment("Creating fabric from parsed cables");
            pluginProgress->progress(2, STEPS);
          }

          if(!fabric->add_cables(portmap))
          {
            if(pluginProgress)
              pluginProgress->setError("Unable to create fabric based on parsed cables.");

            return false;
          }

          if(pluginProgress)
          {
            pluginProgress->setComment("Building LID map");
            pluginProgress->progress(3, STEPS);
          }

          if(!fabric->build_lid_map(true))
          {
            if(pluginProgress)
              pluginProgress->setError("Unable to build LID map.");

            return false;
          }

          if(pluginProgress && populateFields)
          {
            pluginProgress->setComment("Populating Tulip Fields");
            pluginProgress->progress(4, STEPS);
          }

          /// Once a fabric is populated: populate the tulip graph
          fabric->populate(populateFields);

          break;
      }
    }
  }
  
  /**
   * Save fabric or release it
   */
  if(!preserveData)
    delete fabric;

  if(pluginProgress)
  {
    pluginProgress->setComment("Done");
    pluginProgress->progress(STEPS, STEPS);
  }

  return true;
}

