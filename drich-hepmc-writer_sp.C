//
// root -l 'rich-hepmc-writer.cxx("out.hepmc", 100)'
//

#include "HepMC3/GenEvent.h"
#include "HepMC3/ReaderAscii.h"
#include "HepMC3/WriterAscii.h"
#include "HepMC3/Print.h"

#include <iostream>
#include <random>
#include <cmath>
#include <math.h>
#include <TMath.h>

using namespace HepMC3;

/** Generate single particle event with fixed three momentum **/
// n_events is number of events
// etaBinID defines etaBin We want to simulate
// mom is particle momentum
// pdg is particle PDG code


void drich_hepmc_writer_sp(int n_events, int etaBinID, double mom, int pdg)
{
  auto *DatabasePDG = new TDatabasePDG();
  
  auto *particle = DatabasePDG->GetParticle(pdg);
 
  double etaL=0, etaU=0;   //   lower and upper limit of the eta bins
  const char * out_fname = "out.hepmc";
  double etaVals [3][2] ={{1.2,2.0},{2.0,2.5},{2.5,3.5}};
  if(etaBinID == 0){ etaL = etaVals[0][0] ; etaU = etaVals[0][1];} 
  if(etaBinID == 1){ etaL = etaVals[1][0] ; etaU = etaVals[1][1];} 
  if(etaBinID == 2){ etaL = etaVals[2][0] ; etaU = etaVals[2][1];} 
  printf("%d particles (PDG: %d) will be shot of mom %0.2lf at eta: %0.2lf-%0.2lf in %s file\n",n_events,pdg,mom,etaL,etaU,out_fname);

  WriterAscii hepmc_output(out_fname);
  int events_parsed = 0;
  GenEvent evt(Units::GEV, Units::MM);

  //std::random_device rd;
  unsigned int seed = 0x12345678;//(unsigned int)abs(rd());
  std::cout << "init seed for random generator is " << seed << std::endl;
  // Random number generator
  TRandom *rdmn_gen = new TRandom(seed);

  for (events_parsed = 0; events_parsed < n_events; events_parsed++) { //event loop
    //FourVector(px,py,pz,e,pdgid,status)
    // type 4 is beam
    // pdgid 2212 - proton
    ////Mother particle; We really don't care
    // Type 4 means mother particle
    GenParticlePtr p1 =
        std::make_shared<GenParticle>(FourVector(0.0, 0.0, 12.0, 12.0), 11, 4);
    GenParticlePtr p2 = std::make_shared<GenParticle>(
        FourVector(0.0, 0.0, 100.0, 100.004), 2212, 4); 

    GenVertexPtr v1 = std::make_shared<GenVertex>();//FourVector(0,0,30,0));
    v1->add_particle_in(p1);
    v1->add_particle_in(p2);


    /// Daughter particles; We care for RICH analysis
    // type 1 is final state; 211: pion; FIXME: give a proper mass;

    for(int iq=0; iq<1; iq++){ // daughter particle loop  
      Double_t eta =0;
      eta = rdmn_gen->Uniform(etaL,etaU);
      Double_t th    = 2*std::atan(exp(-eta));
      Double_t p =0.0;
      p     = rdmn_gen->Uniform(mom, mom+0.1);
      Double_t phi   = rdmn_gen->Uniform(0.0, 2*M_PI);

      Double_t px    = p * std::cos(phi) * std::sin(th);
      Double_t py    = p * std::sin(phi) * std::sin(th);
      Double_t pz    = p * std::cos(th);

      //cout<<"px,py,pz: "<<px<<" "<<py<<" "<<pz<<endl;
      
      GenParticlePtr pq = std::make_shared<GenParticle>(FourVector(
								   px, py, pz,
								   sqrt(p*p + pow(particle->Mass(), 2))),
							pdg, 1);
      v1->add_particle_out(pq);
    }//iq	
    evt.add_vertex(v1);

    if (events_parsed == 0) {
      std::cout << "First event: " << std::endl;
      Print::listing(evt);
    }

    hepmc_output.write_event(evt);
    if (events_parsed % 10000 == 0) {
      std::cout << "Event: " << events_parsed << std::endl;
    }
    evt.clear();
  }
  hepmc_output.close();
  std::cout << "Events parsed and written: " << events_parsed << std::endl;
  exit(0);
}
