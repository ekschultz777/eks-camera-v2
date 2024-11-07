#include <stdbool.h>
#include <pthread.h>
#include <opencv2/opencv.hpp>
#include <opencv2/highgui/highgui.hpp>

#ifndef CAMERA_H
#define CAMERA_H

enum Status {
    INITIALIZED = 0,
    READY = 1,
    RUNNING = 2,
    PAUSED = 3
};

typedef struct {
    pthread_t *threadId;
    pthread_mutex_t *mutex;
    char const *pipeline;
    cv::Mat *frame;
    enum Status status;
} Camera;

void initCamera(Camera *camera, char const *pipe);
void startCamera(Camera *);
void stopCamera(Camera *);
void updateCamera(Camera *);
void releaseCamera(Camera *);

#endif