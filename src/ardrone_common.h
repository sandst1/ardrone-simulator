/*
Copyright (C) 2007-2011, PARROT SA, all rights reserved.

DISCLAIMER
The APIs is provided by PARROT and contributors "AS IS" and any express or implied warranties, including, but not limited to, the implied warranties of merchantability and fitness for a particular purpose are disclaimed. In no event shall PARROT and contributors be liable for any direct, indirect, incidental, special, exemplary, or consequential damages (including, but not limited to, procurement of substitute goods or services; loss of use, data, or profits; or business interruption) however caused and on any theory of liability, whether in contract, strict liability, or tort (including negligence or otherwise) arising in any way out of the use of this software, even if advised of the possibility of such damage.
*/
#ifndef ARDRONE_COMMON_H
#define ARDRONE_COMMON_H

#define bool_t  int32_t
typedef float               float32_t;
typedef double              float64_t;

#include <stdint.h>
#include <stddef.h>



#define _ATTRIBUTE_PACKED_  __attribute__ ((packed))

typedef enum {
  ARDRONE_FLY_MASK            = 1 << 0,  /*!< FLY MASK : (0) ardrone is landed, (1) ardrone is flying */
  ARDRONE_VIDEO_MASK          = 1 << 1,  /*!< VIDEO MASK : (0) video disable, (1) video enable */
  ARDRONE_VISION_MASK         = 1 << 2,  /*!< VISION MASK : (0) vision disable, (1) vision enable */
  ARDRONE_CONTROL_MASK        = 1 << 3,  /*!< CONTROL ALGO : (0) euler angles control, (1) angular speed control */
  ARDRONE_ALTITUDE_MASK       = 1 << 4,  /*!< ALTITUDE CONTROL ALGO : (0) altitude control inactive (1) altitude control active */
  ARDRONE_USER_FEEDBACK_START = 1 << 5,  /*!< USER feedback : Start button state */
  ARDRONE_COMMAND_MASK        = 1 << 6,  /*!< Control command ACK : (0) None, (1) one received */
  ARDRONE_FW_FILE_MASK        = 1 << 7,  /* Firmware file is good (1) */
  ARDRONE_FW_VER_MASK         = 1 << 8,  /* Firmware update is newer (1) */
//  ARDRONE_FW_UPD_MASK         = 1 << 9,  /* Firmware update is ongoing (1) */
  ARDRONE_NAVDATA_DEMO_MASK   = 1 << 10, /*!< Navdata demo : (0) All navdata, (1) only navdata demo */
  ARDRONE_NAVDATA_BOOTSTRAP   = 1 << 11, /*!< Navdata bootstrap : (0) options sent in all or demo mode, (1) no navdata options sent */
  ARDRONE_MOTORS_MASK  	      = 1 << 12, /*!< Motors status : (0) Ok, (1) Motors problem */
  ARDRONE_COM_LOST_MASK       = 1 << 13, /*!< Communication Lost : (1) com problem, (0) Com is ok */
  ARDRONE_VBAT_LOW            = 1 << 15, /*!< VBat low : (1) too low, (0) Ok */
  ARDRONE_USER_EL             = 1 << 16, /*!< User Emergency Landing : (1) User EL is ON, (0) User EL is OFF*/
  ARDRONE_TIMER_ELAPSED       = 1 << 17, /*!< Timer elapsed : (1) elapsed, (0) not elapsed */
  ARDRONE_ANGLES_OUT_OF_RANGE = 1 << 19, /*!< Angles : (0) Ok, (1) out of range */
  ARDRONE_ULTRASOUND_MASK     = 1 << 21, /*!< Ultrasonic sensor : (0) Ok, (1) deaf */
  ARDRONE_CUTOUT_MASK         = 1 << 22, /*!< Cutout system detection : (0) Not detected, (1) detected */
  ARDRONE_PIC_VERSION_MASK    = 1 << 23, /*!< PIC Version number OK : (0) a bad version number, (1) version number is OK */
  ARDRONE_ATCODEC_THREAD_ON   = 1 << 24, /*!< ATCodec thread ON : (0) thread OFF (1) thread ON */
  ARDRONE_NAVDATA_THREAD_ON   = 1 << 25, /*!< Navdata thread ON : (0) thread OFF (1) thread ON */
  ARDRONE_VIDEO_THREAD_ON     = 1 << 26, /*!< Video thread ON : (0) thread OFF (1) thread ON */
  ARDRONE_ACQ_THREAD_ON       = 1 << 27, /*!< Acquisition thread ON : (0) thread OFF (1) thread ON */
  ARDRONE_CTRL_WATCHDOG_MASK  = 1 << 28, /*!< CTRL watchdog : (1) delay in control execution (> 5ms), (0) control is well scheduled */
  ARDRONE_ADC_WATCHDOG_MASK   = 1 << 29, /*!< ADC Watchdog : (1) delay in uart2 dsr (> 5ms), (0) uart2 is good */
  ARDRONE_COM_WATCHDOG_MASK   = 1 << 30, /*!< Communication Watchdog : (1) com problem, (0) Com is ok */
  ARDRONE_EMERGENCY_MASK      = 1 << 31  /*!< Emergency landing : (0) no emergency, (1) emergency */
} def_ardrone_state_mask_t;


/**** Navdata ****/

#define NAVDATA_SEQUENCE_DEFAULT  1

#define NAVDATA_HEADER  0x55667788

#define NAVDATA_MAX_SIZE 2048
#define NAVDATA_MAX_CUSTOM_TIME_SAVE 20

typedef enum _navdata_tag_t {
  NAVDATA_DEMO_TAG = 0,
  NAVDATA_TIME_TAG,
  NAVDATA_RAW_MEASURES_TAG,
  NAVDATA_PHYS_MEASURES_TAG,
  NAVDATA_GYROS_OFFSETS_TAG,
  NAVDATA_EULER_ANGLES_TAG,
  NAVDATA_REFERENCES_TAG,
  NAVDATA_TRIMS_TAG,
  NAVDATA_RC_REFERENCES_TAG,
  NAVDATA_PWM_TAG,
  NAVDATA_ALTITUDE_TAG,
  NAVDATA_VISION_RAW_TAG,
  NAVDATA_VISION_OF_TAG,
  NAVDATA_VISION_TAG,
  NAVDATA_VISION_PERF_TAG,
  NAVDATA_TRACKERS_SEND_TAG,
  NAVDATA_VISION_DETECT_TAG,
  NAVDATA_WATCHDOG_TAG,
  NAVDATA_ADC_DATA_FRAME_TAG,
  NAVDATA_CKS_TAG = 0xFFFF
} navdata_tag_t;

typedef struct _navdata_option_t {
  uint16_t  tag;
  uint16_t  size;
#if defined _MSC_VER
  uint8_t   *data;
#else
  uint8_t   data[];
#endif
} navdata_option_t;

typedef struct _navdata_t {
  uint32_t    header;
  uint32_t    ardrone_state;
  uint32_t    sequence;
  bool_t      vision_defined;

  navdata_option_t  options[1];
}_ATTRIBUTE_PACKED_ navdata_t;


typedef struct _navdata_demo_t {
  uint16_t    tag;
  uint16_t    size;

  uint32_t    ctrl_state;             /*!< instance of #def_ardrone_state_mask_t */
  uint32_t    vbat_flying_percentage; /*!< battery voltage filtered (mV) */

  float32_t   theta;                  /*!< UAV's attitude */
  float32_t   phi;                    /*!< UAV's attitude */
  float32_t   psi;                    /*!< UAV's attitude */

  int32_t     altitude;               /*!< UAV's altitude */

  float32_t   vx;                     /*!< UAV's estimated linear velocity */
  float32_t   vy;                     /*!< UAV's estimated linear velocity */
  float32_t   vz;                     /*!< UAV's estimated linear velocity */

  uint32_t    num_frames;			  /*!< streamed frame index */ // Not used -> To integrate in video stage.

  // Camera parameters compute by detection
//  matrix33_t  detection_camera_rot; /*!<  Deprecated ! Don't use ! */
//  vector31_t  detection_camera_trans; /*!<  Deprecated ! Don't use ! */
//  uint32_t	  detection_tag_index; /*!<  Deprecated ! Don't use ! */
//  uint32_t	  detection_camera_type; /*!<  Deprecated ! Don't use ! */

  // Camera parameters compute by drone
//  matrix33_t  drone_camera_rot;
//  vector31_t  drone_camera_trans;
}_ATTRIBUTE_PACKED_ navdata_demo_t;

/// Last navdata option that *must* be included at the end of all navdata packet
/// + 6 bytes
typedef struct _navdata_cks_t {
  uint16_t  tag;
  uint16_t  size;

  // Checksum for all navdatas (including options)
  uint32_t  cks;
}_ATTRIBUTE_PACKED_ navdata_cks_t;

#endif
