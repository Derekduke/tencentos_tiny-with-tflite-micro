/* Copyright 2019 The TensorFlow Authors. All Rights Reserved.

Licensed under the Apache License, Version 2.0 (the "License");
you may not use this file except in compliance with the License.
You may obtain a copy of the License at

    http://www.apache.org/licenses/LICENSE-2.0

Unless required by applicable law or agreed to in writing, software
distributed under the License is distributed on an "AS IS" BASIS,
WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
See the License for the specific language governing permissions and
limitations under the License.
==============================================================================*/

#include "tensorflow/lite/micro/examples/person_detection_experimental/image_provider.h"
#include "tensorflow/lite/micro/examples/person_detection_experimental/model_settings.h"
#include "tensorflow/lite/micro/examples/person_detection_experimental/person_image_data.h"
#include "tensorflow/lite/micro/examples/person_detection_experimental/no_person_image_data.h"

#define OV2640_PIXEL_WIDTH  ((uint16_t)96)
#define OV2640_PIXEL_HEIGHT ((uint16_t)96)
extern uint8_t modelBuffer[OV2640_PIXEL_WIDTH*OV2640_PIXEL_HEIGHT];
TfLiteStatus GetImage(tflite::ErrorReporter* error_reporter, int image_width,
                      int image_height, int channels, int8_t* image_data) {
  //for (int i = 0; i < image_width * image_height * channels; ++i) {
	for (int i = 0; i < OV2640_PIXEL_WIDTH * OV2640_PIXEL_HEIGHT * 1; ++i) {
    //image_data[i] = g_no_person_data[i]; // �������� -- �����������ͬ
	//image_data[i] = g_person_data[i]; // �������� -- ��������������ͬ
	image_data[i] = modelBuffer[i]; //ʵ������ͷͼ���ȡ
  }
  return kTfLiteOk;
}
