#include <stdbool.h>
#include <pthread.h>
#include <opencv2/opencv.hpp>
#include <opencv2/highgui/highgui.hpp>

#ifndef CAMERA_H
#define CAMERA_H

typedef struct {
    pthread_t *thread;
    pthread_mutex_t *mutex;
    char const *pipeline;
    cv::Mat *frame;
} Camera;

Camera createCamera(char const *pipe);
void startCamera(Camera *);
void stopCamera(Camera *);
void updateCamera(Camera *);
void releaseCamera(Camera *);

#endif