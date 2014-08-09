#include "allibs.hpp"
#include <string>
#include <vector>

#include "util.hpp"
#include "objects.hpp"

#ifndef _AUDIO_H
#define _AUDIO_H

/// Object that can play sound
class AudioNode
{
	public:
		AudioNode();
		~AudioNode();

		// Handle the source finishing playback
		void step();

		// Update Position (+ Velocity) of the source; also triggers step()
		void updatePos(SimpleVec3d pos, SimpleVec3d vel = SimpleVec3d());

		void addFile(std::string filename);
		void play();
		void stop();
		void setPitch(float pitch);
		void setGain(float gain);
		void setLoop(ALboolean loop);
		bool getFinished();
		void setDeleteOnFinish(bool value);
		bool getDeleteOnFinish();

	private:
		ALuint m_source;
		bool m_finished;
		bool m_del_on_finish;
			
};

/// Reference to a file that has been loaded into an OpenAL buffer
class AudioFile
{
	public:
		AudioFile() {};
		~AudioFile()
			{ alDeleteBuffers(1, &buffer); }

		std::string filename;
		ALuint buffer;
		ALboolean loop;
};

/// Contains and manages AudioNodes and loads AudioFiles
class AudioEnvironment
{
	public:
		AudioEnvironment();
		~AudioEnvironment();

		// Add node, e.g. as background music: Always to be played next to the listener
		void bindNode(AudioNode *node);

		void step();

		AudioFile *requestFile(std::string filename);

	private:
		void loadSources();
		void onError(std::string error);

		#ifdef USE_ALUT
		static AudioFile *loadWAV(std::string filename);
		#endif
		static AudioFile *loadOGG(std::string filename);

		ALCdevice *m_device;
		ALCcontext *m_context;

		std::vector<AudioFile*> m_files;
		std::vector<GLuint> m_sources;
		std::vector<AudioNode*>m_bound_nodes;
};

/// Object in the WorldEnvironment that wants to be called when the AudioEnvironment is updated
class AudioObject
{
	public:
		AudioObject() {};
		virtual void stepAudio() {};
};

/// Randomly plays background music
class BackgroundMusicManager
{
	public:
		BackgroundMusicManager();
		~BackgroundMusicManager();

		void step();

	private:
		AudioNode *m_audio;
		std::vector<std::string> m_files;
		int8_t current_track;
};

#endif
