#include <vorbis/vorbisfile.h>
#include <dirent.h>
#include <fstream>

#include "environment.hpp"
#include "gamevars.hpp"
#include "player.hpp"
#include "config.hpp"
#include "allibs.hpp"
#include "audio.hpp"
#include "game.hpp"

/**
 * \brief Prepares sound system
 *
 * The AudioEnvironment contains all sound sources and manages sound files.
 * It will automatically load all sound files in the <basedir>/SOUNDS_DIR directory.
 * .ogg files are always supported, .wav files if building with ALUT (USE_ALUT)
 * Using ALUT is not recommended as the functions used are deprecated.
 */
AudioEnvironment::AudioEnvironment()
{
	std::cout<<"###############"<<std::endl;
	std::cout<<"###  AUDIO  ###"<<std::endl;
	std::cout<<"###############"<<std::endl;

	ALCenum error;
	m_device = alcOpenDevice(NULL);
	if (!m_device)
	{
		onError("Could not open default device");
		return;
	}

	m_context = alcCreateContext(m_device, NULL);
	if (!m_context)
	{
		onError("Could not bind audio context to device");
		return;
	}

	if (!alcMakeContextCurrent(m_context))
	{
		onError("alcMakeContextCurrent failed");
		return;
	}

	error = alGetError();
	if (error != AL_NO_ERROR)
	{
		onError("Unknown audio error, ID " + std::to_string(error));
	}

	alDopplerVelocity(SPEED_OF_SOUND);

	// Load all audio files in the SOUNDS_DIR folder
	loadSources();

	// Use relative velocities and positions, player doesnt move
	alListenerfv(AL_POSITION, SimpleVec3d().toFloatArray());
	alListenerfv(AL_VELOCITY, SimpleVec3d().toFloatArray());
}

/**
 * \brief Destructor for AudioEnvironment
 *
 * Closes hardware and frees memory of the source files.
 */
AudioEnvironment::~AudioEnvironment()
{
	for (auto f : m_files)
		delete f;

	alcMakeContextCurrent(NULL);
	alcDestroyContext(m_context);
	alcCloseDevice(m_device);
}

/**
 * \brief Updates the AudioEnvironment
 *
 * Updates position and velocity of managed AudioNodes. Also manages bound AudioNodes.
 * Applys movement and orientation of the player.
 */
void AudioEnvironment::step ()
{
	SimpleVec3d look = game->getPlayer()->getLookAxis();
	SimpleVec3d up = game->getPlayer()->getUpAxis();
	ALfloat orientation[] = 
	{
		(float)look.x, (float)look.y, (float)look.z,
		(float)up.x, (float)up.y, (float)up.z
	};
	alListenerfv(AL_ORIENTATION, orientation);

	// Update bound nodes: delete obsolete ones and update positions
	for (std::vector<AudioNode*>::iterator it = m_bound_nodes.begin();
		it != m_bound_nodes.end();)
	{
		if ((*it) == nullptr || ((*it)->getDeleteOnFinish() && (*it)->getFinished()))
		{
			m_bound_nodes.erase(it);
			delete (*it);
			(*it) = nullptr;
		}
		else
			++it;
	}

	SimpleVec3d pos = game->getPlayer()->getPos();
	SimpleVec3d vel = game->getPlayer()->getVelocity();
	for (AudioNode *node : m_bound_nodes)
		if (node != nullptr) node->updatePos(pos, vel);

	// Update AudioNodes that are bound to objects, so that there is
	// hardly any delay between listener update and source udpate
	for (auto obj : game->getWorldEnv()->getObjects())
	{
		AudioObject *audio = dynamic_cast<AudioObject *>(obj);
		if (audio) audio->stepAudio();
	}
}

/**
 * \brief Makes an AudioNode alway be where the player is
 * \param node The AudioNode to bind
 */
void AudioEnvironment::bindNode(AudioNode *node)
{
	m_bound_nodes.push_back(node);
}

/**
 * \brief Outputs the error and closes the game
 * \param error The error to output
 */
void AudioEnvironment::onError(std::string error)
{
	std::cout<<"Sound system error occured: " << error << std::endl;
	std::exit(EXIT_FAILURE);
}

/**
 * \brief Load all audio files
 *
 * Scans [BASEDIR]/SOUNDS_DIR for audio files
 * They must either be .ogg or with USE_ALUT can also be .wav
 * .ppl (PlanetherPlayList) and .txt files are ignored, other files reported.
 */
void AudioEnvironment::loadSources()
{
	// Scan directory
	std::cout<<"Loading sources:"<<std::endl;
	DIR *dir;
	struct dirent *file;

	dir = opendir(std::string(getBasedir() + SOUNDS_DIR).c_str());
	assert(dir);

	while (( file = readdir(dir) ))
	{
		if (file->d_name[0] != '.')
		{
			// Load each file
			std::string filename(file->d_name);

			// Output filename with spaces (to make same length for every one of them)
			std::string outname = filename;
			while (outname.length() < 20) outname += " ";

			std::string fn_ext = std::string(filename).substr(filename.length()-3, 3);
			if (fn_ext == "ogg")
			{
				std::cout<<"- "<<outname<<" [..]";
				m_files.push_back(loadOGG(filename));
			}
			else if (fn_ext == "txt"); // ignore
			else if (fn_ext == "ppl"); // ignore, PlanetherPlayList
			#ifdef USE_ALUT
			else if (fn_ext == "wav")
			{
				std::cout<<"- "<<outname<<" [..]";
				m_files.push_back(loadWAV(filename));
			}
			#endif
			else
			{
				std::cout<<"Error: Audio file with unknown filename extension found: "
					 << filename << std::endl;
				std::exit(EXIT_FAILURE);
			}
		}
	}
}

#ifdef USE_ALUT
/**
 * \brief Load a .wav file
 * \param filename The file to load
 *
 * Deprecated! Do not use this, only for testing. USE_ALUT must be enabled.
 */
AudioFile *AudioEnvironment::loadWAV(std::string filename)
{
	AudioFile *file = new AudioFile();
	file->filename = filename;

	ALsizei size, freq;
	ALenum format;
	ALvoid *data;

	// Generate an empty sound buffer
	alGenBuffers((ALuint)1, &file->buffer);

	// Load WAV file into buffer
	std::string path_s = getBasedir() + SOUNDS_DIR + filename;
	char *path = new char[path_s.length() + 1];
	strcpy(path, path_s.c_str());
	alutLoadWAVFile((ALbyte *)path, &format, &data, &size, &freq, &file->loop);
	alBufferData(file->buffer, format, data, size, freq);

	if (alGetError() == AL_NO_ERROR)
		std::cout<<"\b\b\b"<<"OK]"<<std::endl;
	else
	{
		std::cout<<"\b\b\b"<<"ERROR] - file broken"<<std::endl;
		std::exit(EXIT_FAILURE);
	}

	return file;
}
#endif

#define OGGBUFFER 65536 // 64 KB

/**
 * \brief Load a .ogg file to an AudioFile
 * \param filename The file to load (relative to [BASEDIR]/SOUNDS_DIR)
 *
 * Creates buffer and fills it with OGG sound data.
 */
AudioFile *AudioEnvironment::loadOGG(std::string filename)
{
	AudioFile *file = new AudioFile();
	file->filename = filename;
	ALenum format;

	std::vector<char> buffer_vec;
	char data[OGGBUFFER];    // Local fixed size array

	FILE *f;
	OggVorbis_File ogg;

	// Generate an empty sound buffer
	alGenBuffers((ALuint)1, &file->buffer);

	// Open File
	f = fopen((getBasedir() + SOUNDS_DIR + filename).c_str(), "rb");
	vorbis_info *info;
	ov_open(f, &ogg, NULL, 0);

	info = ov_info(&ogg, -1);

	// Mono / Stereo
  	if (info->channels == 1)
		format = AL_FORMAT_MONO16;
	else
		format = AL_FORMAT_STEREO16;

	for(;;)
	{
		// Copy data over to the buffer until no more is left
		long num = ov_read(&ogg, data, OGGBUFFER, 0, 2, 1, 0);
		if (num <= 0) break;
		buffer_vec.insert(buffer_vec.end(), data, data + num);

	}

	alBufferData(file->buffer, format, &buffer_vec[0], buffer_vec.size(), info->rate);

	ov_clear(&ogg);

	if (alGetError() == AL_NO_ERROR)
		std::cout<<"\b\b\b"<<"OK]"<<std::endl;
	else
	{
		std::cout<<"\b\b\b"<<"ERROR] - file broken"<<std::endl;
		std::exit(EXIT_FAILURE);
	}

	return file;
}

/**
 * \brief Returns the AudioFile with the given name
 * \param filename The filename of the AudioFile to retrieve
 */
AudioFile *AudioEnvironment::requestFile(std::string filename)
{
	for (auto file : m_files)
	{
		if (file->filename == filename)
			return file;
	}

	onError("Could not find file " + filename);
	return nullptr;
}

/*
	AudioNode: Object that can play sounds
*/

/**
 * \brief An AudioNode is a node with a position in the AudioEnvironment that can play sound
 *
 * The creates an empty source and initializes some default values for it.
 */
AudioNode::AudioNode () :
m_finished(false),
m_del_on_finish(false)
{
	// Initialize a default configuration for source 
	alGenSources((ALuint)1, &m_source);

	alSourcef(m_source, AL_PITCH, 1);
	alSourcef(m_source, AL_GAIN, 1);
	alSourcei(m_source, AL_LOOPING, AL_FALSE);
}

/**
 * Frees the source
 */
AudioNode::~AudioNode ()
{
	alDeleteSources(1, &m_source);
}

/**
 * \brief Binds an AudioFile to the AudioNode
 *
 * Multiple files can also be bound. The way be played one after the other (if setLoop is false).
 */
void AudioNode::addFile(std::string filename)
{
	alSourcei(m_source, AL_BUFFER, game->getAudioEnv()->requestFile(filename)->buffer);
}

/**
 * \brief Update the position of the AudioNode in the AudioEnvironment
 * \param pos The position to set
 * \param vel The velocity to set
 *
 * To be called every time the object that owns the AudioNode moves.
 */
void AudioNode::updatePos(SimpleVec3d pos, SimpleVec3d vel)
{
	// Uses relative positions and velocities
	alSourcefv(m_source, AL_POSITION, (pos - game->getPlayer()->getPos()).toFloatArray());
	alSourcefv(m_source, AL_VELOCITY, (vel - game->getPlayer()->getVelocity()).toFloatArray());
	step();	
}

/**
 * \brief Update finished value of the AudioNode
 *
 * Do not call this directly!
 */
void AudioNode::step()
{
	ALenum state;
	alGetSourcei(m_source, AL_SOURCE_STATE, &state);
	m_finished = (state != AL_PLAYING);
}

/**
 * \brief Returns whether the AudioNode has finished playing
 * \return True, if the AudioNode has finished. False, if still playing.
 */
bool AudioNode::getFinished()
{
	return m_finished;
}

/**
 * Sets whether the AudioEnvironment is supposed to take care of deleting the AudioNode when it has
 * finished playing.
 * \param value True, if the AudioEnvironment is supposed to delete the object
 */
void AudioNode::setDeleteOnFinish(bool value)
{
	m_del_on_finish = value;
}

/**
 * \brief Returns whether the AudioEnvironment is supposed to delete this AudioNode when finished
 * \return The value set via setDeleteOnFinish()
 */
bool AudioNode::getDeleteOnFinish()
{
	return m_del_on_finish;
}

/**
 * \brief Set the pitch of the playing sound
 * \param pitch The pitch value
 */
void AudioNode::setPitch(float pitch)
{
	alSourcef(m_source, AL_PITCH, pitch);
}

/**
 * \brief Set the gain of the playing sound
 * \param gain The gain value
 */
void AudioNode::setGain(float gain)
{
	alSourcef(m_source, AL_GAIN, gain);
}

/**
 * \brief Set whether the sound should play in a loop
 * \param loop AL_TRUE if supposed to play in a loop, AL_FALSE otherwise
 */
void AudioNode::setLoop(ALboolean loop)
{
	alSourcei(m_source, AL_LOOPING, loop);
}

/**
 * \brief Start playback of the AudioNode
 */
void AudioNode::play()
{
	alSourcePlay(m_source);
	m_finished = false;
}

/**
 * \brief Stop playback of the AudioNode
 */
void AudioNode::stop()
{
	alSourceStop(m_source);
	m_finished = true;
}

/*
	Background music
*/

/**
 * \brief Manager that plays different background soundtracks repeatedly
 *
 * Reads list of background soundtracks from a list at [BASEDIR]/SOUNDS_DIR/background.ppl
 */
BackgroundMusicManager::BackgroundMusicManager() :
current_track(-1)
{
	// Create AudioNode to play music on
	m_audio = new AudioNode();

	// Read available music files.
	std::ifstream file;
	file.open(getBasedir() + std::string(SOUNDS_DIR) + "background.ppl");

	std::string line;
	while (std::getline(file, line))
		m_files.push_back(line);

	m_audio->setGain(config->getDouble("music_volume", 0.8));
	game->getAudioEnv()->bindNode(m_audio);
}

/**
 * The destructor deletes the own AudioNode
 */
BackgroundMusicManager::~BackgroundMusicManager()
{
	delete m_audio;
	std::cout<<"~BackgroundMusicManager"<<std::endl;
}

/**
 * Updates the BackgroundMusicManager, plays a random new track when old one is finished
 */
void BackgroundMusicManager::step()
{
	if (m_audio->getFinished())
	{
		// Play a track that is not the same as the current one
		int8_t oldtrack = current_track;
		while (oldtrack == current_track) current_track = rand() % m_files.size();

		m_audio->addFile(m_files.at(current_track));
		m_audio->play();
	}
}
