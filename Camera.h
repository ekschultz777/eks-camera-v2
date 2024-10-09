#include <stdbool.h>
#include <pthread.h>
#include <opencv2/opencv.hpp>
#include <opencv2/highgui/highgui.hpp>

#ifndef CAMERA_H
#define CAMERA_H

typedef struct {
    // pthread_t thread;
    pthread_mutex_t *mutex;
    char const *pipeline;
    // cv::VideoCapture capture;
    cv::Mat *frame;
    bool running;
} Camera;

Camera createCamera(char const *pipe, pthread_t *thread, pthread_mutex_t *mutex, cv::Mat *frame);
void startCamera(Camera *);
void stopCamera(Camera *);
void updateCamera(Camera *);
void releaseCamera(Camera *);

#endif