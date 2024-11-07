// #include <stdio.h>
// #include <stdlib.h>
// #include <pthread.h>
// #include <opencv2/opencv.hpp>
// #include <opencv2/highgui/highgui.hpp>
#include "Camera.h"
#include <stdbool.h>
#include <pthread.h>
#include <opencv2/opencv.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <unistd.h>

using namespace cv;

// Run  on background thread
void *operation(void *arg) {
    Camera *camera = (Camera *)arg;

    pthread_mutex_lock(camera->mutex);
    printf("Created pipeline string: %s\n", camera->pipeline);
    cv::VideoCapture capture(camera->pipeline);

    if (!(capture).isOpened()) {
        printf("Failed to open file: %s\n", camera->pipeline);
        return NULL;
    }
    pthread_mutex_unlock(camera->mutex);

    // FIXME: Share this using camera struct
    double framesPerSecond = 30.0;
    double secondsPerFrame = (double) (1 / framesPerSecond);
    printf("Playing %s at %f fps\n", camera->pipeline, framesPerSecond);

    while (1) {
        if (camera == NULL) {
            break;
        }
        pthread_mutex_lock(camera->mutex);
        // TODO: Synchronize frames here using a different flag then running
        switch (camera->status) {
            case INITIALIZED:
                camera->status = READY;
                pthread_mutex_unlock(camera->mutex);
                continue;
            case READY:
                break;
            case RUNNING:
                if (!capture.read(*camera->frame)) {
                    printf("Failed to capture frame\n");
                }
            case PAUSED:
                break;
        }
        pthread_mutex_unlock(camera->mutex);
        // Sleep frame duration in microseconds
        usleep(secondsPerFrame * 1000 * 1000);
    }
    return NULL;
}

void initCamera(Camera *camera, char const *pipe) {
    pthread_t *threadId = (pthread_t *)malloc(sizeof(pthread_t));
    pthread_mutex_t *mutex = (pthread_mutex_t *)malloc(sizeof(pthread_mutex_t));
    pthread_mutex_init(mutex, NULL);
    cv::Mat *frame = (cv::Mat *)malloc(sizeof(cv::Mat));
    *camera = {
        .threadId = threadId,
        .mutex = mutex,
        .pipeline = pipe,
        .frame = frame,
        .status = INITIALIZED,
    };
    if (pthread_create(camera->threadId, NULL, operation, camera) != 0) {
        printf("Failed to properly initialize thread!\n");
    }

}

void startCamera(Camera *camera) {

}

void stopCamera(Camera *camera) {

}

void updateCamera(Camera *camera) {

}

void releaseCamera(Camera *camera) {
    pthread_mutex_lock(camera->mutex);
    pthread_mutex_destroy(camera->mutex);
    free(camera->mutex);
    pthread_cancel(*camera->threadId);
    free(camera->threadId);
    free((void *)camera->frame);
    free((void *)camera);
    camera->threadId = NULL;
    camera->mutex = NULL;
    camera->pipeline = NULL;
    camera->frame = NULL;
    camera = NULL;
}