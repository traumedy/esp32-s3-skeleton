#include <Arduino.h>
#include <Arducam_Mega.h>
#include <SPI.h>

static const char* _TAG = "main";

CAM_IMAGE_MODE imageMode = CAM_IMAGE_MODE_QVGA;
// const int CS = D7; // ESP32-E
const int CS = T9; // ESP32-S3
const int ledPin = LED_BUILTIN;
Arducam_Mega myCAM(CS);
bool ledState = false;


// put function declarations here:

void setLED(bool on)
{
    digitalWrite (ledPin, on);
}

void setupCameraSettings()
{
    Serial.println("SetupCameraSettings");
    // Turn off auto exposure and set absolute exposure
    myCAM.setAutoExposure(1);
    // myCAM .setAbsoluteExposure(100);
    Serial.println("Auto Exposure Set to 0 and Absolute Exposure Set to 100");

    // Turn off auto white balance and set it to a fixed mode
    myCAM.setAutoWhiteBalance(1);
    // myCAM.setAutoWhiteBalanceMode(CAM_WHITE_BALANCE_MODE_SUNNY);
    Serial.println("Auto White Balance turned off and set to Sunny mode");

    // Set image saturation, exposure value, and contrast
    myCAM.setSaturation(CAM_STAURATION_LEVEL_DEFAULT);
    Serial.println("Saturation set to default");

    myCAM.setEV(CAM_EV_LEVEL_DEFAULT);
    Serial.println("Exposure Value set to default");

    myCAM.setContrast(CAM_CONTRAST_LEVEL_DEFAULT);
    Serial.println("Contrast set to default");

    // Set image brightness and sharpness
    myCAM.setBrightness(CAM_BRIGHTNESS_LEVEL_DEFAULT);
    Serial.println("Brightness set to default");

    myCAM.setSharpness(CAM_SHARPNESS_LEVEL_AUTO);
    Serial.println("Sharpness set to auto");

    // Set color effects to none
    myCAM.setColorEffect(CAM_COLOR_FX_NONE);
    Serial.println("Color Effect set to none");

    // Set the image quality to default
    myCAM.setImageQuality(HIGH_QUALITY);
    Serial.println("Image Quality set to default");
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
    Serial.println("Image size (bytes): " + String(*length));
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

    pinMode (ledPin, OUTPUT);

    Serial.begin(115200);

#if 1
    // Wait for serial port to connect
    while (!Serial)
    {
    } 
#endif

    Serial.println("setup() entered");

    Serial.println("Calling Arducam_Mega.begin()");
    myCAM.begin();
    Serial.println("Arducam_Mega.begin() called");

    Serial.println("Calling setupCameraSettings()");
    setupCameraSettings();
    Serial.println("setupCameraSettings() called");

    Serial.println("setup() finished");
}

void loop() {
  // put your main code here, to run repeatedly:

    setLED(ledState);
    ledState = !ledState;
    String message;
    message = "Free internal heap: " +
        String(esp_get_free_internal_heap_size()) +
        "  Minimum free heap size: " + 
        String(esp_get_minimum_free_heap_size());
    Serial.println(message);

    Serial.println("Capturing image...");
    size_t length = 0;
    uint8_t* imageData = captureImage(&length);
    Serial.println("Image captured, length " + String(length));
    delete[] imageData;

    message = "Free internal heap: " +
        String(esp_get_free_internal_heap_size()) +
        "  Minimum free heap size: " + 
        String(esp_get_minimum_free_heap_size());
    Serial.println(message);

    delay(2000);
}

