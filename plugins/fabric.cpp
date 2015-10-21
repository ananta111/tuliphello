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
#include "fabric.h"
#include "ibautils/ib_fabric.h"
#include "ibautils/regex.h"

namespace ib = infiniband;

/**
 * Static fabric pointer to allow preserveData option for imports
 */
ib::tulip_fabric_t::map_t ib::tulip_fabric_t::map = ib::tulip_fabric_t::map_t();

ib::tulip_fabric_t * ib::tulip_fabric_t::find_fabric(tlp::Graph * const graph, bool create)
{
  const map_t::iterator fabric_itr = map.find(graph);

  if(fabric_itr != map.end())
    return fabric_itr->second;
  else
  {
    if(!create)
      return NULL;
    else ///Create new fabric
    {
      ib::tulip_fabric_t *const fabric = new ib::tulip_fabric_t(graph);
      assert(fabric);

      std::pair<map_t::iterator, bool> result = map.insert(std::make_pair(graph, fabric));
      assert(result.second);

      return result.first->second;
    }
  }
}

tlp::node& ib::tulip_fabric_t::get_entity_node(const ib::guid_t& guid)
{
  entities_t::iterator entityitr = find_entity(guid);
  if(entityitr == entities.end()) abort();
  
  entity_nodes_t::iterator itr = entity_nodes.find(&entityitr->second);
  if(itr != entity_nodes.end())
    return itr->second;
  
  ///should not happen!
  assert(itr != entity_nodes.end());
  abort();
}

ib::tulip_fabric_t::tulip_fabric_t(tlp::Graph * const _graph)
  : graph(_graph)
{
  assert(graph);
}

void ib::tulip_fabric_t::populate(const bool populateFields)
{
  tlp::StringProperty * viewLabel = 0;
  ///Using string for GUID since integer is 32bits (on x86)
  tlp::StringProperty * ibGuid = 0;
  tlp::IntegerProperty * ibPortNum = 0;
  tlp::IntegerProperty * ibLid = 0;
  tlp::IntegerProperty * ibHca = 0;
  tlp::StringProperty * ibWidth = 0;
  tlp::StringProperty * ibSpeed = 0;
  tlp::StringProperty * ibName = 0;
  tlp::StringProperty * ibLeaf = 0;
  tlp::StringProperty * ibSpine = 0;

  if(populateFields)
  {
    viewLabel = graph->getProperty<tlp::StringProperty>("viewLabel");
    ibGuid = graph->getProperty<tlp::StringProperty>("ibGuid");
    ibWidth = graph->getProperty<tlp::StringProperty>("ibWidth");
    ibSpeed = graph->getProperty<tlp::StringProperty>("ibSpeed");
    ibName = graph->getProperty<tlp::StringProperty>("ibName");
    ibLeaf = graph->getProperty<tlp::StringProperty>("ibLeaf");
    ibSpine = graph->getProperty<tlp::StringProperty>("ibSpine");
    ibPortNum = graph->getProperty<tlp::IntegerProperty >("ibPortNum");
    ibLid = graph->getProperty<tlp::IntegerProperty >("ibLid");
    ibHca = graph->getProperty<tlp::IntegerProperty >("ibHca");
  }
  
  /**
   * Create the tulip graph by having the following
   * 1 node = 1 entity
   * 2 edges = 1 cable (1 edge in each direction)
   */

  /**
   * reserve 2 edges per cable
   */
  graph->reserveEdges(get_portmap().size() * 2);
  
  /**
   * Walk every entity and create every node
   */
  for(
    ib::fabric_t::entities_t::const_iterator 
      itr = get_entities().begin(),
      eitr = get_entities().end();
    itr != eitr;
    ++itr
  )
  {
    const ib::entity_t &entity = itr->second;
    
    if(entity_nodes.find(const_cast<ib::entity_t*>(&entity)) == entity_nodes.end())
    {
      /**
       * Create node and insert it into map
       */
      tlp::node node = graph->addNode();
      assert(node.isValid());
      assert(graph->getRoot()->isElement(node));
      
      std::pair<ib::tulip_fabric_t::entity_nodes_t::const_iterator, bool> result = entity_nodes.insert(std::make_pair(const_cast<ib::entity_t*>(&entity), node));
      assert(result.second); ///should never fail!
      assert(result.first->second == node);
      
      if(populateFields)
      {
        typedef ib::entity_t l;
        viewLabel->setNodeValue(node, entity.label(l::LABEL_ENTITY_ONLY));
        ibName->setNodeValue(node, entity.label(l::LABEL_NAME_ONLY));
        ibLeaf->setNodeValue(node, entity.label(l::LABEL_LEAF_ONLY));
        ibSpine->setNodeValue(node, entity.label(l::LABEL_SPINE_ONLY));
        ibPortNum->setNodeValue(node, entity.ports.size()); ///define list of known port count on this entity
        ibGuid->setNodeValue(node, regex::string_cast_uint(entity.guid));
        ibLid->setNodeValue(node, entity.lid());
        ibHca->setNodeValue(node, entity.hca());
      }
    }
  }
    
  /**
    * Walk every port and create every edge
    */
  for(
    ib::fabric_t::portmap_guidport_t::const_iterator
      itr = get_portmap().begin(),
      eitr = get_portmap().end();
    itr != eitr;
    ++itr
  )
  {
    ib::port_t const * const port = itr->second;
    assert(port);
    
    if(port->connection)
    {
      tlp::node n1 = get_entity_node(port->guid);
      tlp::node n2 = get_entity_node(port->connection->guid);
      assert(n1.isValid()); assert(n2.isValid());
      
      tlp::edge edge = graph->addEdge(n1, n2);
      assert(edge.isValid());
      
      std::pair<ib::tulip_fabric_t::port_edges_t::iterator, bool> result = port_edges.insert(std::make_pair(const_cast<ib::port_t*>(port), edge));
      assert(result.second); ///should never fail!
      assert(result.first->second == edge);
     
      if(populateFields)
      {
        typedef ib::port_t l;
        ibName->setEdgeValue(edge, port->label(l::LABEL_FULL));
        ///Dump full label for edges with both ports
        const std::string label = port->label() + " <--> " + port->connection->label();
        viewLabel->setEdgeValue(edge, label);
        
        ibGuid->setEdgeValue(edge, regex::string_cast_uint(port->guid));
        ibWidth->setEdgeValue(edge, port->width);
        ibSpeed->setEdgeValue(edge, port->speed);
        ibLeaf->setEdgeValue(edge, regex::string_cast_uint(port->leaf));
        ibSpine->setEdgeValue(edge, regex::string_cast_uint(port->spine));
        ibPortNum->setEdgeValue(edge, port->port);
        ibLid->setEdgeValue(edge, port->lid);
        ibHca->setEdgeValue(edge, port->hca);
      }
    }
  }

}




