#ifndef CONFIG_HPP
#define CONFIG_HPP

namespace config
{
	//simulation
	static const char* mode = "barneshut"; //"precise" or "barneshut"
	static const unsigned int particlesCount = 150000;
	static const bool drawParticles = true;
	static const float gForce = 1.2;
	static const float minForceDistance = 3.0;

	//hardware
	static const unsigned int cpuCores = 4;

	//view
	static const float startViewZoom = 0.25;	

	//UI
	static const bool showUI = true;
	
	//recording
	static const bool record = false;
	static const bool recordLimit = false;
	static const unsigned int recordForT = 60 * 14;
	static const char* recordFilename = "recording.bin";
	static const bool playFromRecord = true;

	//barnes hut
	static const float alpha = 0.9;

}

#endif 
