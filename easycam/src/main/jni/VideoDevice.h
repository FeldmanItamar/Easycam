/*
 * VideoDevice.h
 *
 *  Created on: Nov 4, 2014
 *      Author: Eric
 */

#ifndef VIDEODEVICE_H_
#define VIDEODEVICE_H_

#include <jni.h>
#include <cstdio>

using namespace std;

typedef struct {
    void* start;
    size_t length;
} buffer;

enum DeviceType {UTV007, EMPIA, STK1160, SOMAGIC, NO_DEVICE};
enum VideoStandard {NTSC, PAL};

typedef unsigned char uint8;
class VideoDevice;

typedef struct {
	DeviceType device_type;     // Type of easycap device
	VideoStandard standard_id;  // Region standard (NTSC/PAL)
	int frame_width;
	int frame_height;
	char* device_name;  		// location of the device file (/dev/videoX)
	int num_buffers;			// number of buffers to allocate

	// V4L2 vars that need to be device specific
	__u32 pixel_format;

	// Pointer to the correct YUV call to convert to RGB
	//void (VideoDevice::* ConvertToARGB)(int);    // Dont need this unless I decide to do processing in JNI
} DeviceSettings;

/* Class: Video Device
 * Encapsulates a V4l2 device, exposing all functionality
 * related to opening, streaming, and closing a device.
 */
class VideoDevice {
public:
	VideoDevice(DeviceSettings devSets);
	virtual ~VideoDevice();

	/* The below functions control a capture device.  A note about how they work:
	 * A device must be opened, initialzed, and set to streamon (start_capture)
	 * before you can capture frames.  You can "pause" the stream with
	 * stop_cature.  stop_device deletes buffers and closes the device completely.
	 * If you stop a device it must be opened, initialized, and the stream turned
	 * on before you can run process capture again.
	 */
	int open_device();   // attempt to open device_name and get file descriptor
	int init_device();   // initialize buffers
	int start_capture(); // enqueue buffers and turn stream on
	int stop_capture();  // turn stream off
	buffer* process_capture();   // dequeue a frame and bring it to user space
	void stop_device();  // shut down device

	// static function to detect a device
	static DeviceType detect_device(const char* dev_name);

	bool video_device_attached() {
		if (file_descriptor == -1)
			return false;
		else
			return true;
	}

	int get_buffer_length() {
		if (frame_buffers)
			return frame_buffers[0].length;
		else
			return 0;
	}


private:
	unsigned int buffer_count;  // actual number of buffers allocated

	buffer* frame_buffers;

	int file_descriptor;
	DeviceSettings device_sets;

	int curBufferIndex;  // the index for the last buffer read into memory

	int uninit_device();
	int close_device();
	int init_mmap();
	int read_frame();

};

#endif /* VIDEODEVICE_H_ */
