//************************************* email  ************************************************//
#include "ESP32_MailClient.h"
// REPLACE WITH YOUR NETWORK CREDENTIALS
// To send Email using Gmail use port 465 (SSL) and SMTP Server smtp.gmail.com
// YOU MUST ENABLE less secure app option https://myaccount.google.com/lesssecureapps?pli=1
#define emailSenderAccount    "Parinazy43@gmail.com"
#define emailSenderPassword   "jnffpobsmwawthmo"
#define emailRecipient        "parinazyousefi1@gmail.com"
#define smtpServer            "smtp.gmail.com"
#define smtpServerPort        465
#define emailSubject          "ESP32 Test"
// The Email Sending data object contains config and data to send
SMTPData smtpData;
// Callback function to get the Email sending status
void sendCallback(SendStatus info);

//Attachment
unsigned long int fileSize;
uint8_t * fileContent;
char * attachFilename;
unsigned long int logCount = 0;
//************************************************************************************************//


/*
  ESP32-CAM Load images from SD card to enroll faces and recognize faces automatically.
  Author : ChungYi Fu (Kaohsiung, Taiwan)  2021-6-29 21:30
  https://www.facebook.com/francefu
*/

//Face recognition The number of registered images of the same person's face
#define ENROLL_CONFIRM_TIMES 5

//Face recognition registration number
#define FACE_ID_SAVE_NUMBER 1 //<-- 7

//The resolution CIF image can be obtained from the webpage get-still button and saved in the SD cardhttp://192.168.xxx.xxx/capture  (FRAMESIZE_CIF)
String filepath[5] = {"/1.jpg", "/2.jpg", "/3.jpg", "/4.jpg", "/5.jpg"
                      };  //1.jpg, 2.jpg, ...., 35.jpg
int image_width = 320;
int image_height = 240;


//Set the name of the person displayed by face recognition
String recognize_face_matched_name[1] = {"PARINAZ" };

#include "soc/soc.h"             //Used for unstable power supply and restarting
#include "soc/rtc_cntl_reg.h"    //Used for unstable power supply without rebooting
#include "esp_camera.h"          //video function
#include "img_converters.h"      //Image format conversion function
#include "fb_gfx.h"              // image drawing function
#include "fd_forward.h"          //Face detection function
#include "fr_forward.h"          //Face recognition function
#include "FS.h"                  // file system function
#include "SD_MMC.h"              //SD card access function

#include <time.h>
#include <WiFi.h>

//Anxinke ESP32-CAM module pin setting
#define PWDN_GPIO_NUM     32
#define RESET_GPIO_NUM    -1
#define XCLK_GPIO_NUM      0
#define SIOD_GPIO_NUM     26
#define SIOC_GPIO_NUM     27

#define Y9_GPIO_NUM       35
#define Y8_GPIO_NUM       34
#define Y7_GPIO_NUM       39
#define Y6_GPIO_NUM       36
#define Y5_GPIO_NUM       21
#define Y4_GPIO_NUM       19
#define Y3_GPIO_NUM       18
#define Y2_GPIO_NUM        5
#define VSYNC_GPIO_NUM    25
#define HREF_GPIO_NUM     23
#define PCLK_GPIO_NUM     22

//<-- Add Wifi parameters
const char* ssid     = "wifi name";
//const char* password = "password";

//<-- Add internet time library parameters
long timezone = -5; //<-- North America/Toronto
byte daysavetime = 0;

//<-- Add log file variable
char * logPath;

//initial value
static mtmn_config_t mtmn_config = {0};
static face_id_list id_list = {0};
int8_t enroll_id = 0;
int counter0 = 0;
int counter1 = 0;
int L_day = 0;
int N_day = 0;
String mess;
int counter2 = 0;
//char  newmess;
//int nowday = 0 ;
//https://github.com/espressif/esp-dl/blob/master/face_detection/README.md
box_array_t *net_boxes = NULL;


//<-- File Access Utilities
void createDir(fs::FS &fs, const char * path) {
  Serial.printf("Creating Dir: %s\n", path);
  if (fs.mkdir(path)) {
    Serial.println("Dir created");
  } else {
    Serial.println("mkdir failed");
  }
}

void initFile(fs::FS &fs) {
  Serial.printf("Initializing the file: %s\n", logPath);

  File file = fs.open(logPath, FILE_APPEND);
  if (!file) {
    Serial.println("Failed to initialize file for appending");
    return;
  }
  file.close();
}

void logFile(const char * message) {

  logCount++;

  Serial.printf("Adding log to file: %s\n", logPath);

  configTime(3600 * timezone, daysavetime * 3600, "time.nist.gov", "0.pool.ntp.org", "1.pool.ntp.org");
  struct tm tmstruct ;
  delay(2000);
  tmstruct.tm_year = 0;
  getLocalTime(&tmstruct, 5000);

  char logMessage[13 + strlen(message)];
  sprintf(logMessage , "[%02d:%02d:%02d] %s\n", tmstruct.tm_hour , tmstruct.tm_min, tmstruct.tm_sec , message );

  File file = SD_MMC.open(logPath, FILE_APPEND);
  if (!file) {
    Serial.println("Failed to open the file");
    return;
  }
  if (file.print(logMessage)) {
    Serial.println("Log was written");
  } else {
    Serial.println("Log failed");
  }
  file.close();

  /*if (logCount > 3) {
    sendEmail();
    logCount = 0;
  }*/
}

void readFile(const char * path) {
  Serial.printf("Reading file: %s\n", path);

  File file = SD_MMC.open(path);
  if (!file) {
    Serial.println("Failed to open file for reading");
    return;
  }

  fileSize = file.size();
  fileContent = (uint8_t *) malloc(fileSize + 1);

  unsigned long int idx = 0;
  while (file.available()) {
    fileContent[idx++] = file.read();
  }
  file.close();
}

void setupTimer(){
  struct tm timeinfo;
  if(getLocalTime(&timeinfo)){
  N_day=  (timeinfo.tm_mday);
    L_day= (timeinfo.tm_mday);
    }    
}





//<-- File utilities are finished!

void setupWifi() {

  Serial.print("Connecting to ");
  Serial.println(ssid);

  WiFi.begin(ssid);

  while (WiFi.status() != WL_CONNECTED) {
    delay(700);
    Serial.print(".");
  }

  Serial.println();
  Serial.println("WiFi connected");
  Serial.println(WiFi.localIP());

}

char * getDateAsFilename() {
  Serial.println("Contacting Time Server");
  configTime(3600 * timezone, daysavetime * 3600, "time.nist.gov", "0.pool.ntp.org", "1.pool.ntp.org");
  struct tm tmstruct ;
  delay(2000);
  tmstruct.tm_year = 0;
  getLocalTime(&tmstruct, 5000);
  char * date = (char*) malloc(11);
  sprintf(date , "%d-%02d-%02d", (tmstruct.tm_year) + 1900, ( tmstruct.tm_mon) + 1, tmstruct.tm_mday);
  return date;
}


void setupSD(char * filename) {

  if (!SD_MMC.begin()) {
    Serial.println("Card Mount Failed");
    return;
  }

  uint8_t cardType = SD_MMC.cardType();

  if (cardType == CARD_NONE) {
    Serial.println("No SD card attached");
    return;
  }

  //check if log directory is exists

  File logs = SD_MMC.open("/logs");
  if (!logs) {
    Serial.println("Directory is not exists.Create it");
    createDir(SD_MMC, "/logs");
  }

  //  if(!logs.isDirectory()){
  //     Serial.println("");
  //     return;
  //  }

  String path = "/logs/";
  String sFilename = String(filename) + ".log";

  attachFilename = (char *) malloc(15);
  sFilename.toCharArray(attachFilename, 15);

  path += sFilename;
  logPath = (char*) malloc(path.length() + 1);
  path.toCharArray(logPath , path.length() + 1);
  initFile(SD_MMC);
  return;
}

void sendEmail() {
  smtpData.setLogin(smtpServer, smtpServerPort, emailSenderAccount, emailSenderPassword);
  smtpData.setSTARTTLS(true);
  smtpData.setSender("ESP32-Robot", emailSenderAccount);
  smtpData.setPriority("High");
  smtpData.setSubject(emailSubject);
  smtpData.setMessage("<div style=\"color:#2f4468;\"><h1>Today Log File</h1><p>- please refer to attachement</p></div>", true);
  smtpData.addRecipient(emailRecipient);

  readFile(logPath);
  smtpData.addAttachData(attachFilename, "plain/txt", fileContent, fileSize);
  smtpData.setSendCallback(sendCallback);
  if (!MailClient.sendMail(smtpData))
    Serial.println("Error sending Email, " + MailClient.smtpErrorReason());
  free(fileContent);
  smtpData.empty();
}


void FaceMatched(int faceid) {  //Recognize the registered face to execute command control
  if (faceid == 0) {
    logFile("Match ID for PARINAZ YOUSEFI");
    delay(300);
    Serial.write('!');
        delay(1000);
  }
  else if (faceid == 1) {
  }
  else if (faceid == 2) {
  }
  else if (faceid == 3) {
  }
  else if (faceid == 4) {
  }
  else if (faceid == 5) {
  }
  else if (faceid == 6) {
  }
  else {
  }
}

void FaceNoMatched() {   //Recognize as a stranger's face and execute command control
  counter0++;
  if (counter0 == 4) {
    logFile("Face did not matched!");
    delay(300);
    Serial.write('^');
    delay(1000);
    sendEmail2();
    counter0 = 0;
    
  }
}

void setup() {
  WRITE_PERI_REG(RTC_CNTL_BROWN_OUT_REG, 0);  //The setting of rebooting when the power is turned off and unstable

  Serial.begin(115200);
  //Serial.setDebugOutput(true);  //Enable diagnostic output
  //Serial.println();

  //Video configuration setting https://github.com/espressif/esp32-camera/blob/master/driver/include/esp_camera.h
  camera_config_t config;
  config.ledc_channel = LEDC_CHANNEL_0;
  config.ledc_timer = LEDC_TIMER_0;
  config.pin_d0 = Y2_GPIO_NUM;
  config.pin_d1 = Y3_GPIO_NUM;
  config.pin_d2 = Y4_GPIO_NUM;
  config.pin_d3 = Y5_GPIO_NUM;
  config.pin_d4 = Y6_GPIO_NUM;
  config.pin_d5 = Y7_GPIO_NUM;
  config.pin_d6 = Y8_GPIO_NUM;
  config.pin_d7 = Y9_GPIO_NUM;
  config.pin_xclk = XCLK_GPIO_NUM;
  config.pin_pclk = PCLK_GPIO_NUM;
  config.pin_vsync = VSYNC_GPIO_NUM;
  config.pin_href = HREF_GPIO_NUM;
  config.pin_sscb_sda = SIOD_GPIO_NUM;
  config.pin_sscb_scl = SIOC_GPIO_NUM;
  config.pin_pwdn = PWDN_GPIO_NUM;
  config.pin_reset = RESET_GPIO_NUM;
  config.xclk_freq_hz = 20000000;
  config.pixel_format = PIXFORMAT_JPEG;

  //
  // WARNING!!! PSRAM IC required for UXGA resolution and high JPEG quality
  //            Ensure ESP32 Wrover Module or other board with PSRAM is selected
  //            Partial images will be transmitted if image exceeds buffer size
  //
  // if PSRAM IC present, init with UXGA resolution and higher JPEG quality
  //                      for larger pre-allocated frame buffer.
  if (psramFound()) { //Whether there is PSRAM (Psuedo SRAM) memory IC
    config.frame_size = FRAMESIZE_UXGA;
    config.jpeg_quality = 10;
    config.fb_count = 2;
  } else {
    config.frame_size = FRAMESIZE_SVGA;
    config.jpeg_quality = 12;
    config.fb_count = 1;
  }

  //Video initialization
  esp_err_t err = esp_camera_init(&config);
  if (err != ESP_OK) {
    Serial.printf("Camera init failed with error 0x%x", err);
    ESP.restart();
  }


  //You can customize the default size of the video frame (resolution size)
  sensor_t * s = esp_camera_sensor_get();
  // initial sensors are flipped vertically and colors are a bit saturated
  if (s->id.PID == OV3660_PID) {
    s->set_vflip(s, 1); // flip it back
    s->set_brightness(s, 1); // up the brightness just a bit
    s->set_saturation(s, -2); // lower the saturation
  }
  // drop down frame size for higher initial frame rate
  s->set_framesize(s, FRAMESIZE_CIF);     //resolutionUXGA(1600x1200), SXGA(1280x1024), XGA(1024x768), SVGA(800x600), VGA(640x480), CIF(400x296), QVGA(320x240), HQVGA(240x176), QQVGA(160x120), QXGA(2048x1564 for OV3660)

  //s->set_vflip(s, 1);  // flip vertically
  //s->set_hmirror(s, 1);   //horizontal mirroring

  //flash (GPIO4)
  ledcAttachPin(4, 4);
  ledcSetup(4, 5000, 8);
  pinMode(4, OUTPUT);
  digitalWrite(4, LOW);

  encrollImageSD();  //Read SD card image file to register face

  //<-- Add Log Section
  //Serial.println('new section...');
  setupWifi();
  char * date = getDateAsFilename(); //<-- 2023-03-30
  setupSD(date);
  logFile(">> In Setup...");
  free(date);
 setupTimer();  
}


void loop() {
  EmailTrigger();
  Serial.flush();
  if (Serial.available() > 0 ) {
    mess = Serial.readString();
    if ( mess == "Start")    {              
      Serial.println("Face Recognition Start");
      delay(500);
      for (int i = 0; i <= 4; i++) {
        counter1 = 1;
        faceRecognition();
        delay(200);
      }
      Serial.println("Face Recognition End");
    }
    if ( mess == "Sendmail") {
      sendEmail();
    }
  }
}
void encrollImageSD() {
  //Face detection parameter setting  https://github.com/espressif/esp-face/blob/master/face_detection/README.md
  mtmn_config.type = FAST;  //FAST or NORMAL
  mtmn_config.min_face = 80;
  mtmn_config.pyramid = 0.707;
  mtmn_config.pyramid_times = 4;
  mtmn_config.p_threshold.score = 0.6;
  mtmn_config.p_threshold.nms = 0.7;
  mtmn_config.p_threshold.candidate_number = 20;
  mtmn_config.r_threshold.score = 0.7;
  mtmn_config.r_threshold.nms = 0.7;
  mtmn_config.r_threshold.candidate_number = 10;
  mtmn_config.o_threshold.score = 0.7;
  mtmn_config.o_threshold.nms = 0.7;
  mtmn_config.o_threshold.candidate_number = 1;

  // SD card initialization
  if (!SD_MMC.begin()) {
    Serial.println("Card Mount Failed");
    ESP.restart();
  }

  fs::FS &fs = SD_MMC;

  face_id_init(&id_list, FACE_ID_SAVE_NUMBER, ENROLL_CONFIRM_TIMES);
  dl_matrix3du_t *aligned_face = NULL;
  int8_t left_sample_face = NULL;
  dl_matrix3du_t *image_matrix = NULL;

  for (int j = 0; j < sizeof(filepath) / sizeof(*filepath); j++) {
    File file = fs.open(filepath[j]);
    Serial.println("detect file: " + filepath[j]);
    if (!file) {
      Serial.println("Failed to open file for reading");
      SD_MMC.end();
    } else {
      Serial.println("file size: " + String(file.size()));
      char *buf;
      buf = (char*) malloc (sizeof(char) * file.size());
      long i = 0;
      while (file.available()) {
        buf[i] = file.read();
        i++;
      }

      image_matrix = dl_matrix3du_alloc(1, image_width, image_height, 3);  // allocate internal memory
      if (!image_matrix) {
        Serial.println("dl_matrix3du_alloc failed");
      } else {
        fmt2rgb888((uint8_t*)buf, file.size(), PIXFORMAT_JPEG, image_matrix->item);  //Image format conversion to RGB format
        box_array_t *net_boxes = face_detect(image_matrix, &mtmn_config);  // Execute face detection to obtain face frame data
        if (net_boxes) {
          //Serial.println("faces = " + String(net_boxes->len));  //Number of detected faces
          //Serial.println();
          for (int i = 0; i < net_boxes->len; i++) { // List face position and size
            //Serial.println("index = " + String(i));
            int x = (int)net_boxes->box[i].box_p[0];
            //Serial.println("x = " + String(x));
            int y = (int)net_boxes->box[i].box_p[1];
            //Serial.println("y = " + String(y));
            int w = (int)net_boxes->box[i].box_p[2] - x + 1;
            //Serial.println("width = " + String(w));
            int h = (int)net_boxes->box[i].box_p[3] - y + 1;
            //Serial.println("height = " + String(h));
            //Serial.println();


            //Register face
            if (i == 0) {
              aligned_face = dl_matrix3du_alloc(1, FACE_WIDTH, FACE_HEIGHT, 3);
              if (align_face(net_boxes, image_matrix, aligned_face) == ESP_OK) {
                if (!aligned_face) {
                  ////Serial.println("Could not allocate face recognition buffer");
                }
                else {
                  left_sample_face = enroll_face(&id_list, aligned_face);

                  if (left_sample_face == (ENROLL_CONFIRM_TIMES - 1)) {
                    enroll_id = id_list.tail;
                    //Serial.printf("Enrolling Face ID: %d\n", enroll_id);
                  }
                  Serial.printf("Enrolling Face ID: %d sample %d\n", enroll_id, ENROLL_CONFIRM_TIMES - left_sample_face);
                  if (left_sample_face == 0) {
                    enroll_id = id_list.tail;
                    Serial.printf("Enrolled Face ID: %d\n", enroll_id);
                  }
                  ////Serial.println();
                }
                dl_matrix3du_free(aligned_face);
              }
            }
          }
          dl_lib_free(net_boxes->score);
          dl_lib_free(net_boxes->box);
          dl_lib_free(net_boxes->landmark);
          dl_lib_free(net_boxes);
          net_boxes = NULL;
        }
        else {
          ////Serial.println("No Face");    //no face detected
          ////Serial.println();
        }
        dl_matrix3du_free(image_matrix);
      }
      free(buf);
    }
    file.close();
  }

  SD_MMC.end();
  ////Serial.println();

  pinMode(4, OUTPUT);
  digitalWrite(4, LOW);
}

void faceRecognition() {
  camera_fb_t * fb = NULL;
  fb = esp_camera_fb_get();
  if (!fb) {
    Serial.println("Camera capture failed");
    ESP.restart();
  }
  size_t out_len, out_width, out_height;
  uint8_t * out_buf;
  bool s;
  dl_matrix3du_t *image_matrix = dl_matrix3du_alloc(1, fb->width, fb->height, 3);
  if (!image_matrix) {
    esp_camera_fb_return(fb);
    ////Serial.println("dl_matrix3du_alloc failed");
    return;
  }
  out_buf = image_matrix->item;
  out_len = fb->width * fb->height * 3;
  out_width = fb->width;
  out_height = fb->height;
  s = fmt2rgb888(fb->buf, fb->len, fb->format, out_buf);
  esp_camera_fb_return(fb);
  if (!s) {
    dl_matrix3du_free(image_matrix);
    ////Serial.println("to rgb888 failed");
    return;
  }
  box_array_t *net_boxes = face_detect(image_matrix, &mtmn_config);  //Execute face detection
  if (net_boxes) {
    run_face_recognition(image_matrix, net_boxes);  //Execute face recognition
    dl_lib_free(net_boxes->score);
    dl_lib_free(net_boxes->box);
    dl_lib_free(net_boxes->landmark);
    dl_lib_free(net_boxes);
    net_boxes = NULL;
  }
  dl_matrix3du_free(image_matrix);
}

//Face recognition function
static int run_face_recognition(dl_matrix3du_t *image_matrix, box_array_t *net_boxes) {
  dl_matrix3du_t *aligned_face = NULL;
  int matched_id = 0;

  aligned_face = dl_matrix3du_alloc(1, FACE_WIDTH, FACE_HEIGHT, 3);
  if (!aligned_face) {
    ////Serial.println("Could not allocate face recognition buffer");
    return matched_id;
  }
  if (align_face(net_boxes, image_matrix, aligned_face) == ESP_OK) {
    matched_id = recognize_face(&id_list, aligned_face);  // face recognition

    if (matched_id >= 0) {
      //      Serial.printf("Match Face Name: %s\n", recognize_face_matched_name[matched_id]);
      int name_length = sizeof(recognize_face_matched_name) / sizeof(recognize_face_matched_name[0]);
      if (matched_id < name_length) {
        //Display the recognized name on the video screen
        //Serial.printf("Match Face Name: %s\n", recognize_face_matched_name[matched_id]);

      }
      else {
        //Serial.printf("Match Face Name: No name\n");

      }

      //<-- additional task on recognision
      FaceMatched(matched_id);  //Recognize the registered face to execute command control

    } else {
      //Serial.println("No Match Found"); // Recognized as a stranger's face
      ////Serial.println();

      matched_id = -1;
      FaceNoMatched();  //Recognize as a stranger's face and execute command control
    }
  } else {
    Serial.println("Face Not Aligned");


  }

  dl_matrix3du_free(aligned_face);
  return matched_id;
}
//****************************************************Email ***************************************//
// Callback function to get the Email sending status
void sendCallback(SendStatus msg) {
  // Print the current status
  Serial.println(msg.info());
  // Do something when complete
  if (msg.success()) {
    Serial.println("----------------");
  }
}
//*************************************************Email End *************************************//
void EmailTrigger() {
   counter2++;
  if (counter2 == 5000){
 struct tm timeinfo;
  if(getLocalTime(&timeinfo)){
  //Serial.println(&timeinfo, "%A, %B %d %Y %H:%M:%S");
  
  N_day=  (timeinfo.tm_mday);
  if (N_day != L_day) { 
    sendEmail();
  }
  L_day= (timeinfo.tm_mday);
    }
counter2 = 0;      
}
}
/******************************NOTIFICATION***************************/
void sendEmail2() {
  smtpData.setLogin(smtpServer, smtpServerPort, emailSenderAccount, emailSenderPassword);
  smtpData.setSTARTTLS(true);
  smtpData.setSender("ESP32-Robot", emailSenderAccount);
  smtpData.setPriority("High");
  smtpData.setSubject(emailSubject);
  smtpData.setMessage("<div style=\"color:#2f4468;\"><h1>NOTIFICATION</h1><p>UNAUTHORIZED USER HAS TRIED TO ENTER</p></div>", true);
  smtpData.addRecipient(emailRecipient);

  readFile(logPath);
 // smtpData.addAttachData(attachFilename, "plain/txt", fileContent, fileSize);
  smtpData.setSendCallback(sendCallback);
  if (!MailClient.sendMail(smtpData))
    Serial.println("Error sending Email, " + MailClient.smtpErrorReason());
  free(fileContent);
  smtpData.empty();
}
