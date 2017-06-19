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
  
class HelloWorld: public tlp::Algorithm {
private:
  int v=0; //number of node;
  int **adjacent_matrix;
public:
  PLUGININFORMATION("HelloWorld",
                    "Ananta",
                    "06/16/15",
                    "HelloWorld Program.",
                    "alpha",
                    "Infiniband") 
  
  HelloWorld(tlp::PluginContext* context);

  /**
   * @brief import infiniband routes
   * @warning currently only works if static data is retained from import
   */
  void initMap(const Graph *graph, int v){
        this->v = v;
        adjacent_matrix = new int* [v];
        for(int i = 0; i<v; i++){
            adjacent_matrix[i] = new int[v];
        }

        //Initialize the adjacent_matrix
        for(int i = 0; i<v; i++){
            for(int j = 0; j<v; j++){
                adjacent_matrix[i][j] = 0;
            }
        }

        //Set up the adjacent_matrix
        tlp::Iterator<edge> *ite = graph->getEdges();
        while(ite->hasNext()){
            edge e = ite->next();
            int s = graph->source(e).id, t = graph->target(e).id;
            if(!adjacent_matrix[s][t]){
                adjacent_matrix[s][t] = 1;
                adjacent_matrix[t][s] = 1;
            }
        }
    }

  int min_distance(int dist[], bool visited[]);
  void dijkstra(int src);
  void printResult(int dist[]);
  bool run();
};

#endif // IB_ROUTES_H

