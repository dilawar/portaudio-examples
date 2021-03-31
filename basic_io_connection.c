#include <stdio.h>
#include <stdlib.h>
#include <portaudio.h>

static PaError err;
static PaStream *stream = NULL;

/*
 * FUNCTION: PaErrorTest()
 *
 * Inputs:	error		error generated by Portaudio
 *
 * Returns:	return on stderr the nature of the error in a meaningful way.
 *
 */
void PaErrorTest(PaError error) {
	if (error != paNoError) {
		// using Pa_Terminate is crucial to avoid resource leaks
		Pa_Terminate();
		fprintf(stderr, "Error: %s\n", Pa_GetErrorText(error));
		exit(EXIT_FAILURE);
	}
}

static void PaInit() {
	static int initialized;

	if(!initialized) {
		err = Pa_Initialize();

		PaErrorTest(err);
		initialized = 1;
	}
}

static void PaClose(PaStream *stream) {
	err = Pa_StopStream(stream);
	PaErrorTest(err);
	Pa_Terminate();
}

/*
 * FUNCTION: PlayrecCallback()
 *
 *
 * Inputs:	input_buffer		array of interleaved input samples
 *		output_buffer		array of interleaved output samples
 *		frames_per_buffer	number of frames to be processed
 *		time_info		struct with time in seconds
 *		status_flags		flags indicating whether input and/or
 *					output buffers have been inserted or
 *					will be dropped to overcome underflow
 *					or overflow conditions
 *		user_data		pointer to the StreamInfoStruct for this
 *					stream,	as passed to Pa_OpenStream()
 *
 * Return:	paComplete or paAbort to stop the stream if either user_data
 *		is NULL or stopStream has been set, or paContinue for the
 *		stream to continue running.
 *
 */
int g_num_no_imputs = 0;
int PlayrecCallback(const void *input_buffer,
			   void *output_buffer,
			   unsigned long frames_per_buffer,
			   const PaStreamCallbackTimeInfo* time_info,
			   PaStreamCallbackFlags status_flags,
			   void *user_data) {
	float *out = (float *) output_buffer;
	const float *in = (const float *)input_buffer;
	unsigned int i;

	(void) time_info;
	(void) status_flags;
	(void) user_data;

	if(input_buffer != NULL) {
		for(i=0; i<frames_per_buffer; i++) {
			*out++ = *in++;
			*out++ = *in++;
		}
	}
	return paContinue;
}

int main (int argc, char *argv[])
{
	// Definition of variables we need
	PaStreamParameters in_params, out_params; // structures for I/Os configuration
	int num_channels = 2; // 2 channels (stereo)
	int samplerate = 44100; // sample rate
	int device_id = 0; // sound card device ID (is dependent of your system
	int frames_per_buffer = 256; // audio buffer for portaudio
	char sample_format = paFloat32; // type of sample format


	// Initialization of portaudio
	PaInit();

	// INPUTS
	in_params.device = device_id;
	in_params.channelCount = num_channels;
	in_params.sampleFormat = sample_format;
	in_params.suggestedLatency =
		Pa_GetDeviceInfo(in_params.device)->defaultLowInputLatency;
	in_params.hostApiSpecificStreamInfo = NULL;
	// OUTPUTS
	out_params.device = device_id + 1;
	out_params.channelCount = num_channels;
	out_params.sampleFormat = sample_format;
	out_params.suggestedLatency =
		Pa_GetDeviceInfo(out_params.device)->defaultLowOutputLatency;
	out_params.hostApiSpecificStreamInfo = NULL;

	/* Configuration of stream
	 *
	 * We call the callback we'll need to avoid blocking I/Os
	 */
	err = Pa_OpenStream(
			   &stream,
			   &in_params,
			   &out_params,
			   samplerate,
			   frames_per_buffer,
			   paNoFlag, /* portaudio will clip for us */
			   PlayrecCallback,
			   NULL);
	PaErrorTest(err);

	// Starting of the stream
	err = Pa_StartStream(stream);
	PaErrorTest(err);

	// close the program whenever you want
	printf("Hit ENTER to stop program.\n");
	getchar();
	// stop the stream
	PaClose(stream);

	return EXIT_SUCCESS;
}
