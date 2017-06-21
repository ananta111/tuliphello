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
#include "helloworld.h"
#include "fabric.h"
#include "ibautils/ib_fabric.h"
#include "ibautils/ib_parser.h"
#include "ibautils/regex.h"

using namespace tlp;
using namespace std;

/*PLUGIN(ImportInfinibandRoutes)*/
PLUGIN(HelloWorld)

static const char * paramHelp[] = {
  // File to Open
  HTML_HELP_OPEN() \
  /*HTML_HELP_DEF( "type", "pathname")*/ \
  HTML_HELP_BODY() \
  "Hello World" \
  HTML_HELP_CLOSE()
};

HelloWorld::HelloWorld(tlp::PluginContext* context)
  : tlp::Algorithm(context)
{
  addInParameter<std::string>("file::filename", paramHelp[0],"");
    
}

namespace ib = infiniband;
namespace ibp = infiniband::parser;

//Implementing min_distance
int HelloWorld::min_distance(int dist[], bool visited[], int v){
  int min = INT_MAX;
    int min_index = 0;

    for(int i = 0; i<v; i++){
        if(!visited[i] && dist[i] < min)
            min = dist[i], min_index = i;
    }

    return min_index;
}





bool HelloWorld::run()
{
  assert(graph);

  static const size_t STEPS = 5;
  if(pluginProgress)
  {
    pluginProgress->showPreview(false);
    pluginProgress->setComment("Starting to Import Routes");
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
   * Open file to read and import per type
   */
  std::string filename;
  
  dataSet->get("file::filename", filename);
  std::ifstream ifs(filename.c_str());
  if(!ifs)
  {
    if(pluginProgress)
      pluginProgress->setError("Unable open source file.");

    return false;
  }

  if(pluginProgress)
  {
    pluginProgress->progress(2, STEPS);
    pluginProgress->setComment("Parsing Routes.");
  }
  
  ibp::ibdiagnet_fwd_db parser;
  /*if(!parser.parse(*fabric, ifs))
  {
    if(pluginProgress)
      pluginProgress->setError("Unable parse routes file.");
    return false;
  }*/

  if(pluginProgress)
  {
    pluginProgress->setComment("Parsing Routes complete.");
    pluginProgress->progress(3, STEPS);
  }

  ifs.close();
      
  /**
   * calculate routes outbound
   * from every port on the fabric
   */
  tlp::IntegerProperty * ibHops = graph->getProperty<tlp::IntegerProperty >("ibHops");
  assert(ibHops);

  if(pluginProgress)
  {
    pluginProgress->setComment("Calculating Route oversubscription.");
    pluginProgress->progress(4, STEPS);
  }

  /*for(
    ib::fabric_t::entities_t::const_iterator
    itr = fabric->get_entities().begin(),
    eitr = fabric->get_entities().end();
    itr != eitr;
    ++itr
  )
  {
    const ib::entity_t &entity = itr->second;
    for(
      ib::entity_t::routes_t::const_iterator
      ritr =  entity.get_routes().begin(),
      reitr = entity.get_routes().end();
      ritr != reitr;
      ++ritr
    )
    {
      const ib::entity_t::routes_t::mapped_type set = ritr->second;
      const ib::entity_t::portmap_t::const_iterator port_itr = entity.ports.find(ritr->first);
      if(port_itr != entity.ports.end())
      {
        const ib::port_t* const port = port_itr->second;
        const ib::tulip_fabric_t::port_edges_t::const_iterator edge_itr = fabric->port_edges.find(const_cast<ib::port_t*>(port));
        if(edge_itr != fabric->port_edges.end())
        {
          const tlp::edge &edge = edge_itr->second;
        
          ibRoutesOutbound->setEdgeValue(edge, ritr->second.size());
        }
      }
    }
  }*/
  tlp::Iterator<node> *itnod = graph->getNodes();
  int v = 0; 
  
  
  while( itnod->hasNext()){
    node n = itnod->next();
    v++;
  }
  
  //initialize matrix
  int **adjacent_matrix;
  adjacent_matrix = new int* [v];
  for(int i = 0; i<v; i++){
    adjacent_matrix[i] = new int[v];
  }
  
  for(int i = 0; i<v; i++){
    for(int j = 0; j<v; j++){
      adjacent_matrix[i][j] = 0;
    }
  }
  tlp::Iterator<edge> *ite = graph->getEdges();
  while(ite->hasNext()){
    edge e = ite->next();
    int s = graph->source(e).id, t = graph->target(e).id;
    if(!adjacent_matrix[s][t]){
      adjacent_matrix[s][t]=1;
      adjacent_matrix[t][s]=1;
    }
  }
  
  //djistra implementation 
  int dist[v];
  bool visited[v];
  for(int i =0;i<v;i++){
    dist[i] = INT_MAX, visited[i]=false;
  }
  
  dist[0]=0;
  
  for(int count=0;count<v-1;count++){
    int u = HelloWorld::min_distance(dist,visited,v);
    visited[u] = true;
    
    for (int i =0;i<v;i++){
      if (!visited[i] && adjacent_matrix[u][i] && dist[u] != INT_MAX && (dist[u] + adjacent_matrix[u][i]) < dist[i])
        dist[i] = dist[u] + adjacent_matrix[u][i];
    }
  }
  
  //Get ibHops into the spreadsheet
  int k = 0;
  while(itnod->hasNext()){
    node m = itnod->next();
    ibHops->setNodeValue(m, &dist[k]);
    k++;
  }
  
    
  
  //Print Distance
  for(int i = 0; i<v; i++){
    
    cout<<i<<": "<<dist[i]<<endl;
  }

  
        
  
  
  

  if(pluginProgress)
  {
    pluginProgress->setComment("Calculating Route oversubscription complete.");
    pluginProgress->progress(STEPS, STEPS);
  }

  return true;
}
