#include "stdafx.h"
#include "Lines.h"

curve::curve() {}

instrument::instrument(int num) {
	string sampleName;
	this->eps = 10;
	switch (num)
	{
	case 0://crash
		sampleName = "crash.mp3";
		this->color = 30;//yellow
		this->name = "Crash";
		break;
	case 1://drop
		sampleName = "drop.mp3";
		this->color = 70;//green
		this->name = "Drop";
		break;
	case 2://bass
		sampleName = "bass.mp3";
		this->color = 95;//cyan
		this->name = "Bass";
		break;
	case 3://kick
		sampleName = "kick.mp3";
		this->color = 120;//blue
		this->name = "Kick";
		break;
	case 4://snare
		sampleName = "snare.mp3";
		this->color = 140;//purple
		this->name = "Snare";
		break;
	case 5://pop
		sampleName = "pop.mp3";
		this->color = 165;//pink
		this->name = "Pop";
		break;
	case 6://clap
		sampleName = "clap.mp3";
		this->color = 179;//red
		this->name = "Clap";
		break;
	}
	this->smp = BASS_StreamCreateFile(FALSE, sampleName.c_str(), 0, 0, 0);
}

instrument::instrument() {}

instrument::~instrument() {
	BASS_SampleFree(this->smp);
}