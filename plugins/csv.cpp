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
#include <tulip/CSVParser.h>
#include "csv.h"
#include "fabric.h"
#include "ibautils/ib_fabric.h"
#include "ibautils/regex.h"

PLUGIN(ImportInfinibandCSV)

static const char * paramHelp[] = {
  // File to Open
  HTML_HELP_OPEN() \
  HTML_HELP_DEF( "type", "pathname" ) \
  HTML_HELP_BODY() \
  "Path to ibdiagnet2.fdbs file to import" \
  HTML_HELP_CLOSE(),

  // Guid Column
  HTML_HELP_OPEN() \
  HTML_HELP_DEF( "type", "integer" ) \
  HTML_HELP_DEF( "default", "1" ) \
  HTML_HELP_BODY() \
  "Column number containing GUIDs." \
  HTML_HELP_CLOSE(),

  // Portnum Column
  HTML_HELP_OPEN() \
  HTML_HELP_DEF( "type", "integer" ) \
  HTML_HELP_DEF( "default", "2" ) \
  HTML_HELP_BODY() \
  "Column number containing port number." \
  HTML_HELP_CLOSE(),

  // data Column
  HTML_HELP_OPEN() \
  HTML_HELP_DEF( "type", "integer" ) \
  HTML_HELP_DEF( "default", "4" ) \
  HTML_HELP_BODY() \
  "Column number containing data." \
  HTML_HELP_CLOSE(),

  // data field name
  HTML_HELP_OPEN() \
  HTML_HELP_DEF( "type", "string" ) \
  HTML_HELP_DEF( "default", "ibMetric" ) \
  HTML_HELP_BODY() \
  "Field name to assign data to from CSV." \
  HTML_HELP_CLOSE()

};

ImportInfinibandCSV::ImportInfinibandCSV(tlp::PluginContext* context)
  : tlp::Algorithm(context)
{
  addInParameter<std::string>("file::filename", paramHelp[0],"");
  addInParameter<int>("GUID Column", paramHelp[1],"1");
  addInParameter<int>("Portnum Column", paramHelp[2],"2");
  addInParameter<int>("Data Column", paramHelp[3],"4");
  addInParameter<std::string>("Data Name", paramHelp[4],"ibMetric");
}

namespace ib = infiniband;
/**
 * todo: tulip lacks a int64 property currently
 */
typedef tlp::DoubleProperty MetricProperty;

/**
 * @brief Interface to inherit to get and treat data from csv files with CSVParser object.
 */
class handler_t : public tlp::CSVContentHandler {
public:
  const uint guid_column;
  const uint portnum_column;
  const uint data_column;
  MetricProperty * const metrics;
  ib::tulip_fabric_t * const  fabric;

  handler_t(
    const uint _guid_column,
    const uint _portnum_column,
    const uint _data_column,
    MetricProperty * const _metrics,
      ib::tulip_fabric_t * const _fabric
  ) :
    guid_column(_guid_column),
    portnum_column(_portnum_column),
    data_column(_data_column),
    metrics(_metrics),
    fabric(_fabric)
  {
    assert(fabric);
    assert(metrics);
  }

  /**
   * Function called at the beginning of the file parsing.
   */
  virtual void begin()
  {

  }

  /**
   * Function called for each line in the file.
   * @param row The number of the row.
   * @param lineTokens The tokens.
   * @example guid,port,time_interval(ns),bytes,nanoseconds since epoch
   */
  virtual void line(unsigned int row, const std::vector<std::string>& lineTokens)
  {
    //std::cout << "read: " << row << ": " << lineTokens.size() << std::endl;
    //for(std::vector<std::string>::const_iterator itr = lineTokens.begin(), eitr = lineTokens.end(); itr != eitr; ++itr)
    //  std::cout << "\tread item: " << *itr << std::endl;

    const size_t var_count = lineTokens.size();

    if(
       ///make sure the field is actually there
       var_count < guid_column      ||
       var_count < portnum_column   ||
       var_count < data_column      ||
       !( ///column starts at 1
         guid_column > 0  &&
         portnum_column > 0 &&
         data_column > 0
       )
    )
      return;

    ib::guid_t guid     = regex::uint_cast_hex_string<ib::guid_t>(lineTokens.at(guid_column - 1));
    ib::port_num_t portnum = regex::uint_cast_string<ib::port_num_t>(lineTokens.at(portnum_column - 1));
    int64_t metric = regex::int_cast_string<int64_t >(lineTokens.at(data_column - 1));

#ifndef NDEBUG
    std::cout << "guid: " << regex::string_cast_uint(guid) <<
      " port: " << regex::string_cast_uint(portnum) <<
      " metric: " << regex::string_cast_uint(metric) << std::endl;
#endif

    const ib::tulip_fabric_t::portmap_guidport_t::const_iterator p_itr = fabric->get_portmap().find(ib::port_t::key_guid_port_t(guid, portnum));
    if(p_itr == fabric->get_portmap().end())
    {
      ///Unknown port?
#ifndef NDEBUG
      std::cerr << "unknown guid: " << regex::string_cast_uint(guid) <<
        " port: " << regex::string_cast_uint(portnum) <<
        " metric: " << regex::string_cast_uint(metric) << std::endl;
#endif
      return;
    }

    assert(p_itr->second);
    const ib::port_t * const port = p_itr->second;

    ib::tulip_fabric_t::port_edges_t::iterator e_itr = fabric->port_edges.find(const_cast<ib::port_t * const>(port));
    if(e_itr == fabric->port_edges.end())
    {
      ///Unknown edge for known port?
#ifndef NDEBUG
      std::cerr << "unknown edge for known port: guid: " << regex::string_cast_uint(guid) <<
        " port: " << regex::string_cast_uint(portnum) <<
        " metric: " << regex::string_cast_uint(metric) << std::endl;
#endif
      return;
    }

    tlp::edge &edge = e_itr->second;

    metrics->setEdgeValue(edge, static_cast<double>(metric));
  }

  /**
   * Function called at the end of the parsing.
   * @param rowNumber the number of row read in the file.
   * @param columnNumber The column number for the line with the greatest column number.
   */
  virtual void end(unsigned int rowNumber, unsigned int columnNumber)
  {

  }
};


bool ImportInfinibandCSV::run()
{
  assert(graph);

  static const size_t STEPS = 5;
  if(pluginProgress)
  {
    pluginProgress->showPreview(false);
    pluginProgress->setComment("Starting to Import CSV");
    pluginProgress->progress(0, STEPS);
  }

  /**
   * while this does not import
   * nodes/edges, it imports properties
   * for an existing fabric
   */

  ib::tulip_fabric_t * const fabric = ib::tulip_fabric_t::find_fabric(graph, false);
  if(!fabric)
  {
    if(pluginProgress)
      pluginProgress->setError("Unable find fabric. Make sure to preserve data when importing data.");

    return false;
  }

  if(pluginProgress)
  {
    pluginProgress->setComment("Found Fabric");
    pluginProgress->progress(1, STEPS);
  }

  /**
   * get config
   */
  std::string filename;
  int guid_column;
  int portnum_column;
  int data_column;
  std::string data_name;

  dataSet->get("file::filename", filename);
  dataSet->get("GUID Column", guid_column);
  dataSet->get("Portnum Column", portnum_column);
  dataSet->get("Data Column", data_column);
  dataSet->get("Data Name", data_name);

  /**
   * use tulips simple CSV parser
   */
  tlp::CSVSimpleParser parser(filename, ",");

  if(pluginProgress)
  {
    pluginProgress->setComment("Parsing CSV");
    pluginProgress->progress(2, STEPS);
  }

  MetricProperty * ibMetric = graph->getProperty<MetricProperty>(data_name);
  assert(ibMetric);

  //std::cerr << "opening file " << filename << std::endl;
  handler_t *handler = new handler_t(guid_column, portnum_column, data_column, ibMetric, fabric);
  assert(handler);
  parser.parse(handler, pluginProgress);
  delete handler;

  if(pluginProgress)
  {
    pluginProgress->setComment("Parsing CSV complete");
    pluginProgress->progress(STEPS, STEPS);
  }

  return true;
}

