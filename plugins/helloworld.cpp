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
  /*addInParameter<std::string>("file::filename", paramHelp[0],"");*/
    
}

namespace ib = infiniband;
namespace ibp = infiniband::parser;

//Implementing min_distance function
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
    pluginProgress->setComment("Calculating the number of hops..");
    pluginProgress->progress(0, STEPS);
  }

  

  if(pluginProgress)
  {
    pluginProgress->setComment("Dijkstra's Algorithm Applied..");
    pluginProgress->progress(3, STEPS);
  }

  
      

  tlp::Iterator<node> *itnod = graph->getNodes();
  int v = 0; 
  
  //calculating the number of nodes "v"
  while( itnod->hasNext()){
    
    v++;
  }
  
  //initializing adjacency matrix
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
  
  //djistra's algorithm implementation 
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
  
  //Printing the minimum number of hops required to reach all other nodes from node '0'. 
  for(int i = 0; i<v; i++){
    cout<<"From node "<<i<<": "<<dist[i]<<" hops is required"<<endl;
  }

  
        
  
  
  

  if(pluginProgress)
  {
    pluginProgress->setComment("Dijkstra's algorithm implementation complete..");
    pluginProgress->progress(STEPS, STEPS);
  }

  return true;
}

