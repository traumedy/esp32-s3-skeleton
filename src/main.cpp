#include <Arduino.h>
#include <Arducam_Mega.h>
#include <SPI.h>

static const char* _TAG = "main";

CAM_IMAGE_MODE imageMode = CAM_IMAGE_MODE_QVGA;
// const int CS = D7; // ESP32-E
const int CS = T9; // ESP32-S3
Arducam_Mega myCAM(CS);

// put function declarations here:

void setupCameraSettings()
{
    ESP_LOGD(_TAG, "SetupCameraSettings");
    // Turn off auto exposure and set absolute exposure
    myCAM.setAutoExposure(1);
    // myCAM .setAbsoluteExposure(100);
    ESP_LOGD(_TAG, "Auto Exposure Set to 0 and Absolute Exposure Set to 100");

    // Turn off auto white balance and set it to a fixed mode
    myCAM.setAutoWhiteBalance(1);
    // myCAM.setAutoWhiteBalanceMode(CAM_WHITE_BALANCE_MODE_SUNNY);
    ESP_LOGD(_TAG, "Auto White Balance turned off and set to Sunny mode");

    // Set image saturation, exposure value, and contrast
    myCAM.setSaturation(CAM_STAURATION_LEVEL_DEFAULT);
    ESP_LOGD(_TAG, "Saturation set to default");

    myCAM.setEV(CAM_EV_LEVEL_DEFAULT);
    ESP_LOGD(_TAG, "Exposure Value set to default");

    myCAM.setContrast(CAM_CONTRAST_LEVEL_DEFAULT);
    ESP_LOGD(_TAG, "Contrast set to default");

    // Set image brightness and sharpness
    myCAM.setBrightness(CAM_BRIGHTNESS_LEVEL_DEFAULT);
    ESP_LOGD(_TAG, "Brightness set to default");

    myCAM.setSharpness(CAM_SHARPNESS_LEVEL_AUTO);
    ESP_LOGD(_TAG, "Sharpness set to auto");

    // Set color effects to none
    myCAM.setColorEffect(CAM_COLOR_FX_NONE);
    ESP_LOGD(_TAG, "Color Effect set to none");

    // Set the image quality to default
    myCAM.setImageQuality(HIGH_QUALITY);
    ESP_LOGD(_TAG, "Image Quality set to default");
}

uint8_t *captureImage(size_t *length)
{
    CamStatus status = myCAM.takePicture(imageMode, CAM_IMAGE_PIX_FMT_JPG);
    if (status != CAM_ERR_SUCCESS)
    {
        ESP_LOGE(_TAG, "Arducam::takePicture() failed: %d", status);
        return nullptr;
    }
    *length = myCAM.getTotalLength();
    ESP_LOGD(_TAG, "Image size (bytes): %d", *length);
    uint8_t *imageBuffer = new uint8_t[*length];
    if (imageBuffer == nullptr)
    {
        ESP_LOGE(_TAG, "Failed to allocate memory for image.");
        *length = 0;
        return nullptr;
    }

    // Read image data into buffer
    size_t bytesTotalToRead = *length;
    size_t bytesRead = 0;
    while (bytesRead < bytesTotalToRead)
    {
        uint8_t readLen = 255;
        if (bytesTotalToRead - bytesRead < 255)
        {
            readLen = bytesTotalToRead - bytesRead;
        }
        bytesRead += myCAM.readBuff(imageBuffer + bytesRead, readLen);
    }

    return imageBuffer;
}

void setup() {
    ESP_LOGD(_TAG, "setup() entered");

    ESP_LOGD(_TAG, "Calling Arducam_Mega.begin()");
    myCAM.begin();
    ESP_LOGD(_TAG, "Arducam_Mega.begin() called");

    ESP_LOGD(_TAG, "Calling setupCameraSettings()");
    setupCameraSettings();
    ESP_LOGD(_TAG, "setupCameraSettings() called");

    ESP_LOGD(_TAG, "setup() finished");
}

void loop() {
  // put your main code here, to run repeatedly:

    ESP_LOGD(_TAG, "Free internal heap: %d  Minimum free heap size: %d",
        esp_get_free_internal_heap_size(), esp_get_minimum_free_heap_size());

    ESP_LOGD(_TAG, "Capturing image...");
    size_t length = 0;
    uint8_t* imageData = captureImage(&length);
    ESP_LOGD(_TAG, "Image captured, length %u", length);
    delete[] imageData;

    ESP_LOGD(_TAG, "Free internal heap: %d  Minimum free heap size: %d",
        esp_get_free_internal_heap_size(), esp_get_minimum_free_heap_size());

    delay(2000);
}

