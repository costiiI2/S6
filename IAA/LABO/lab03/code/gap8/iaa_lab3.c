#include "pmsis.h"
#include "cpx.h"
#include "wifi.h"
#include "bsp/bsp.h"
#include "bsp/camera/himax.h"
#include "bsp/buffer.h"

#define IMG_ORIENTATION 0x0101
#define CAM_WIDTH 324
#define CAM_HEIGHT 244

void sendToSTM32(void);
void rx_wifi_task(void *parameters);
void send_image_via_wifi(unsigned char *image, uint16_t width, uint16_t height);
int open_pi_camera_himax(struct pi_device *device);
static void capture_done_cb(void *arg);
void camera_task(void *parameters);

static int wifiClientConnected = 0;
static struct pi_device camera;
unsigned char *imgBuff;
static pi_buffer_t buffer;
static SemaphoreHandle_t capture_sem = NULL;

void start(void)
{

    cpxInit();
    cpxEnableFunction(CPX_F_WIFI_CTRL);

    BaseType_t xTask;

    vTaskDelay(2000);
    cpxPrintToConsole(LOG_TO_CRTP, "\n\n*** IAA LAB03 ***\n\n");

    /* Create wifi listener task */
    xTask = xTaskCreate(rx_wifi_task, "rx_wifi_task", configMINIMAL_STACK_SIZE * 2,
                        NULL, tskIDLE_PRIORITY + 1, NULL);

    /* Open and init camera buffer */
    if (open_pi_camera_himax(&camera))
    {
        cpxPrintToConsole(LOG_TO_CRTP, "Failed to open camera\n");
        return;
    }

    pi_buffer_init(&buffer, PI_BUFFER_TYPE_L2, imgBuff);
    pi_buffer_set_format(&buffer, CAM_WIDTH, CAM_HEIGHT, 1, PI_BUFFER_FORMAT_GRAY);

    imgBuff = (unsigned char *)pmsis_l2_malloc(CAM_WIDTH * CAM_HEIGHT);

    /* Create Camera task */
    xTask = xTaskCreate(camera_task, "camera_task", configMINIMAL_STACK_SIZE * 4,
                        NULL, tskIDLE_PRIORITY + 1, NULL);

    capture_sem = xSemaphoreCreateBinary();

    uint32_t fcFreq = pi_freq_get(PI_FREQ_DOMAIN_FC);
    printf("FC Frequency: %d\n", fcFreq);

    while (1)
    {
        sendToSTM32();
        pi_yield();
        vTaskDelay(100);
    }
}

/**
 * @brief transfer UART to stm32
 * - Retrieves FC frequencies
 * - Sends a buffer from gap8 to STM32
 * - Need a program on stm32 to read uart
 */
void sendToSTM32(void)
{

    CPXPacket_t packet;
    // packet source , packet destination, function, route
    cpxInitRoutePacket(&packet, CPX_ROUTES_UART_TO_STM32, APP, &packet.route);

    // copy data to packet
    //  TODO: CHECK EXAMPLE stm_gap8_cpx.c to add uart data collection
  
  
    packet.data[0] = 0xBA;
    packet.dataLength = 4;
    xSemaphoreTake(capture_sem, portMAX_DELAY);
    cpxSendPacketBlocking(&packet);
    xSemaphoreGive(capture_sem);
    /* TODO */
}

/**
 * @brief Task wifi management
 * be able to:
 * - know if a PC is connected to the drone
 */
void rx_wifi_task(void *parameters)
{
    CPXPacket_t rpx;
    WiFiCTRLPacket_t wifiPacket;
    while (1)
    {
        // receive packets and store them in rpx
        cpxReceivePacketBlocking(CPX_F_WIFI_CTRL, &rpx);

        // cast it to a wifi packet
        wifiPacket = *(WiFiCTRLPacket_t *)rpx.data;
        // check if a client is connected
        switch (wifiPacket.cmd)
        {
        case WIFI_CTRL_STATUS_CLIENT_CONNECTED:
            wifiClientConnected = 1;
            break;
        case WIFI_CTRL_STATUS_WIFI_CONNECTED:
            wifiClientConnected = 0;
            break;
        default:
            break;
        }
    }
}

#define IMG_HEADER 0x01

typdef struct
{
    uint8_t header;
    uint16_t width;
    uint16_t height;
    uint16_t depth;
    uint16_t size;
    uint8_t data[0];
} __attribute__((packed)) ImagePacket_t;

/**
 * @brief transfer WIFI gap8 to PC
 * Send a gap8 buffer to the PC
 * Need python code to receives data on PC
 */
void send_image_via_wifi(unsigned char *image, uint16_t width, uint16_t height)
{

    ImagePacket_t packet_to_send;
    packet_to_send.header = IMG_HEADER;
    packet_to_send.width = width;
    packet_to_send.height = height;
    packet_to_send.depth = 1;
    packet_to_send.size = width * height;
    memcpy(packet_to_send.data, image, packet_to_send.size);

    CPXPacket_t packet;
    cpxInitRoutePacket(&packet, CPX_ROUTES_WIFI_TO_PC, APP, &packet.route);

    xSemaphoreTake(capture_sem, portMAX_DELAY);
    sendBufferViaCPXBlocking(&packet, (uint8_t *)&packet_to_send, sizeof(packet_to_send));
    xSemaphoreGive(capture_sem);

    /* TODO */
}

/**
 * @brief Callback called when a capture is completed by the camera
 * - must release the acquisition task in order to take a new capture
 */
static void capture_done_cb(void *arg)
{
    // stop the camera
    pi_camera_control(&camera, PI_CAMERA_CMD_STOP, 0);
    // release the semaphore to allow a new capture
    xSemaphoreGive(capture_sem);
    /* TODO */
}

/**
 * @brief Task enabling the acquisition/sending of an image
 * - Set the callback called at the end of a capture
 * - Starts a new capture
 * - Calls the function for sending the image by wifi
 */
void camera_task(void *parameters)
{
    while (1)
    {
        // start the camera and set the callback
        pi_camera_capture_async(&camera, imgBuff, CAM_WIDTH * CAM_HEIGHT, capture_done_cb, NULL);
        pi_camera_control(&camera, PI_CAMERA_CMD_START, 0);
        // wait for the end of the capture
        xSemaphoreTake(capture_sem, portMAX_DELAY);
        // send the image
        send_image_via_wifi(imgBuff, CAM_WIDTH, CAM_HEIGHT);
    }
    /* TODO */
}

int open_pi_camera_himax(struct pi_device *device)
{
    struct pi_himax_conf cam_conf;

    pi_himax_conf_init(&cam_conf);

    cam_conf.format = PI_CAMERA_QVGA;

    pi_open_from_conf(device, &cam_conf);
    if (pi_camera_open(device))
        return -1;

    // rotate image
    pi_camera_control(device, PI_CAMERA_CMD_START, 0);
    uint8_t set_value = 3;
    uint8_t reg_value;
    pi_camera_reg_set(device, IMG_ORIENTATION, &set_value);
    pi_time_wait_us(1000000);
    pi_camera_reg_get(device, IMG_ORIENTATION, &reg_value);
    if (set_value != reg_value)
    {
        cpxPrintToConsole(LOG_TO_CRTP, "Failed to rotate camera image\n");
        return -1;
    }
    pi_camera_control(device, PI_CAMERA_CMD_STOP, 0);
    pi_camera_control(device, PI_CAMERA_CMD_AEG_INIT, 0);

    return 0;
}

/* Program Entry. */
int main(void)
{
    pi_bsp_init();

    return pmsis_kickoff((void *)start);
}
