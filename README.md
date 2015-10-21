# Tulip Infiniband Plugins
Plugins to import and work with Infiniband networks in Tulip

Tulip Project: http://tulip.labri.fr/

NCAR has created a patch for the seamless import of Infiniband network topologies and related data. This patch consists of 3 tulip plugins that utilize libibautils (included in thirdparty). It has been tested against 4.7.0 release and SVN revision 10831. It adds a new linking requirement of Google's RE2 library and is setup to gracefully avoid compilation if RE2 is not provided. It does not require linking against OFED. An example screenshot of an imported fabric is attached. This plugin set has been tested on several 4+k node Infiniband clusters. The tulip plugins are licensed as LGPLv3 to match tulip and libibautils is licensed under a BSD license. The three new plugins are described as follows:
* Infiniband Topology Import:
 * This plugin will import the entire Infiniband fabric based on the output of the 'ibnetdiscover -p' command. Each Infiniband chip is created as a node and each physical cable is created as 2 directional edges.
* Infiniband CSV Importer:
 * This plugin imports CSV files created by the commonly created by Infiniband Monitoring applications that produce aggregated hardware counter values. The generally come in the form of hex encoded GUID, decimal port number and then a value (or set of them). This plugin exists to correctly import or correlate the CSV to the existing IB fabric that has already been loaded into Tulip. The current use of this import to get the traffic measurements for running fabrics.
* Infiniband Topology Import Routes:
 * This plugin imports the file ibdiagnet2.fdbs created by 'ibdiagnet -r' command. Currently, it fills out the ibRoutesOutbound field with number of routes outbound on a given cable (directional edge).

