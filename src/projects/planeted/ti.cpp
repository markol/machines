/*
 * T I . C P P
 * (c) Charybdis Limited, 1996. All Rights Reserved.
 */

#define _INSTANTIATE_TEMPLATE_CLASSES

#include "stdlib/string.hpp"

#include "ctl/vector.hpp"
#include "ctl/list.hpp"
#include "machphys/machphys.hpp"
#include "planeted/scenfile.hpp"
#include "planeted/editconstr.hpp"
#include "planeted/editmach.hpp"
#include "planeted/editart.hpp"

class MotionControl;
class WeaponCombo;
class W4dEntity;

void TerrainDemoDummyFunction1()
{
	static ctl_list<MotionControl*> 								motionList;
    static ctl_vector< string >    									dummyVectorString;
	static ctl_vector< PedConstructionEditor::ConstructionMapping > dummyConstructionMappingCombo;
	static ctl_vector< PedMachineEditor::MachineMapping > 			dummyMachineMappingCombo;
	static ctl_vector< PedArtefactEditor::ArtefactMapping > 		dummyArtefactMapping;
	static ctl_vector< PedArtefactEditor::ArtefactGroup >			dummyArtefactGroup;
	static ctl_vector< PedScenarioFile::MineralSite >				dummyMineralSiteVector;
	static ctl_vector< PedScenarioFile::RaceAI >					dummyRaceAIVector;
	static ctl_vector< PedScenarioFile::AssemblyPoint >				dummyAssemblyPointVector;
	static ctl_vector< PedScenarioFile::Miscellanious >				dummyMiscellaniousVector;
	static ctl_vector< PedScenarioFile::Construction >				dummyConstructionVector;
	static ctl_vector< PedScenarioFile::Machine >					dummyMachineVector;
	static ctl_vector< PedScenarioFile::RaceInfo >					dummyRaceInfoVector;
	static ctl_vector< MachPhys::WeaponCombo > 					dummyWeaponCombo;

}

/* End TI.CPP *****************************************************/
