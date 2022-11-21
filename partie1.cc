// partie1.cc

#include "ns3/core-module.h"
#include "ns3/network-module.h"
#include "ns3/point-to-point-module.h"
#include "ns3/point-to-point-layout-module.h" // en plus par rapport a ndn-simple
#include "ns3/ndnSIM-module.h"
#include "ns3/netanim-module.h"  // Pour utiliser NetAnim



namespace ns3 {

int
main(int argc, char* argv[])
{

    // definir les parametres par defaut pour les liens PointToPoint
	
	Config::SetDefault("ns3::PointToPointNetDevice::DataRate", StringValue("1Mbps"));
 	Config::SetDefault("ns3::PointToPointChannel::Delay", StringValue("10ms"));
	Config::SetDefault("ns3::QueueBase::MaxSize", StringValue("10p"));

	std::string animfile="AnimPartie1.xml";
	
       CommandLine cmd;
       cmd.Parse(argc, argv);
	
	
	// Créer la topologie
	
	PointToPointHelper p2p;
	PointToPointGridHelper grid(3, 4, p2p);
	grid.BoundingBox(100, 100, 200, 200); // (x,y) for upper left and lower right
	
	// Installer la pile ndn au niveau de tous noeuds de ma grille
	ndn::StackHelper ndnHelper;
	ndnHelper.InstallAll();
	
	//choisir les noeuds qui vont executer des applications 
	//choisir des conteneurs pour producteur(s) et consommateur(s)
	// un fournisseur et deux consommateurs dans ce cas
	Ptr<Node> producer = grid.GetNode(1, 0);
	Ptr<Node> consumer1 = grid.GetNode(0, 1);
	Ptr<Node> consumer2 = grid.GetNode(0, 3);
	
	//installer les applications sur les noeuds
	std::string prefix = "/ETS";
	
	ndn::AppHelper cHelper("ns3::ndn::ConsumerBatches");
	cHelper.SetPrefix(prefix);
	cHelper.SetAttribute ("Batches", StringValue ("1s 1 4s 2 6s 1"));
   	cHelper.Install (consumer1);
  	cHelper.SetAttribute ("Batches", StringValue ("2s 1 3s 1 6s 2"));
 	cHelper.Install (consumer2);

	ndn::AppHelper pHelper ("ns3::ndn::Producer");
	pHelper.SetPrefix (prefix);
	pHelper.SetAttribute ("PayloadSize", StringValue("1024"));
	pHelper.Install (producer);

	// configurer le FIB
	//installer le routage global sur les noeuds (ici tous les noeuds)
	ndn ::GlobalRoutingHelper ndnGlobalRoutingHelper;
	ndnGlobalRoutingHelper.InstallAll(); 
	
	// ajouter le préfix (specifier quel noeud export quel prefixe)
	ndnGlobalRoutingHelper.AddOrigins(prefix , producer);
	
	//calculer et installer les FIBs sur les noeuds
   	ndnGlobalRoutingHelper.CalculateRoutes();

	
	// lancer l'exécution (en cédulant l'arrêt avec la fonction Stop)
	Simulator::Stop(Seconds(7.0));
	
	// Pour utiliser NetAnim
	
	AnimationInterface anim(animfile);
	//anim.EnablePacketMetadata();
	
	// Pour utiliser le Ascii tracer
	AsciiTraceHelper ascii;
	p2p.EnableAsciiAll(ascii.CreateFileStream("asciiPartie1.tr"));
	
	// lancer la simulation
	Simulator::Run();
	Simulator::Destroy();
	
	return 0;
}	
} // namespace ns3

int
main(int argc, char* argv[])
{
  return ns3::main(argc, argv);
}
