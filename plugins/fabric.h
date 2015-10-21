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

#include <map>
#include <tulip/TulipPluginHeaders.h>
#include "ibautils/ib_fabric.h"

#ifndef IB_TULIP_FABRIC_H
#define IB_TULIP_FABRIC_H

namespace infiniband
{
  
/**
 * @brief Tulip Fabric 
 * class exists to merge tulip and the IB fabric
 *
 * This class has a static function to find and create a map per graph
 * During import it is expected that the data structures of the fabric
 * will be preserved for later imports of data, such as routes. By preserving
 * the data, all the existing maps are only made once resulting in a dramatic
 * import speedup and all around easier code to work with. This has the 
 * drawback that any changes made by the user to the graph will have 
 * undefined results if any futher imports are made.
 *
 */
class tulip_fabric_t;
class tulip_fabric_t : public infiniband::fabric_t
{
public:
  tulip_fabric_t(tlp::Graph * const _graph);

  typedef std::map<entity_t*, tlp::node> entity_nodes_t;
  typedef std::map<port_t*, tlp::edge> port_edges_t;

  tlp::Graph * const graph;

  /**
   * @brief map of entity -> node
   */
  entity_nodes_t entity_nodes;
  
  /** 
   * @brief map of port -> edge
   */
  port_edges_t port_edges;
  
  /**
  * @brief get entity node 
  * @warning node must always exist before calling this
  * @param guid entity guid
  * @return entity node
  */
  tlp::node &get_entity_node(const guid_t &guid);

  /**
   * @brief get fabric for given graph
   * @param graph ptr to tulip graph
   * @param create create the tulip fabric if one is not found
   * @return pointer to tulip_fabric instance for requested graph or NULL if create=false
   */
  static tulip_fabric_t * find_fabric(tlp::Graph * const graph, bool create);

  /**
   * @brief Populate Tulip based on IB fabric
   */
  void populate(const bool populateFields);
private:
  /**
   * @brief type for static fabric map
   */
  typedef std::map<tlp::Graph*, tulip_fabric_t *> map_t;
  static map_t map;
};

}

#endif // IB_TULIP_FABRIC_H

