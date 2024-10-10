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
    // std::string const pipeString = camera->pipeline;
    printf("Created pipeline string: %s\n", camera->pipeline);
    cv::VideoCapture capture(camera->pipeline);

    if (!(capture).isOpened()) {
        printf("!!! Failed to open file: %s\n", "Video.mp4");
        return NULL;
    }
    pthread_mutex_unlock(camera->mutex);

    // FIXME: Share this using camera struct
    double framesPerSecond = 30.0;
    double secondsPerFrame = (double) (1 / framesPerSecond);
    printf("Playing %s at %f fps\n", camera->pipeline, framesPerSecond);
    cv::Mat frame;


    while (1) {
        pthread_mutex_lock(camera->mutex);
        if (!capture.read(frame)) {
            printf("Failed to capture frame\n");
        }
        camera->frame = &frame;
        pthread_mutex_unlock(camera->mutex);
        // Sleep frame duration in microseconds
        usleep(secondsPerFrame * 1000 * 1000);
    }
    return NULL;
}

// Run  on main thread
Camera createCamera(char const *pipe) {
    bool running = false;
    // pthread_mutex_init(mutex, NULL);
    cv::Mat frame;
    pthread_t thread;
    pthread_mutex_t mutex;
    pthread_mutex_init(&mutex, NULL);
    Camera camera = { 
        .thread = &thread,
        .mutex = &mutex,
        .pipeline = pipe,
        .frame = &frame,
        .running = running,
    };
    if (pthread_create(&thread, NULL, operation, &camera) != 0) {
        printf("Failed to properly initialize thread!\n");
    }
    return camera;
}

void startCamera(Camera *camera) {
    
}

void stopCamera(Camera *camera) {

}

void updateCamera(Camera *camera) {

}

void releaseCamera(Camera *camera) {
    pthread_mutex_lock(camera->mutex);
    pthread_cancel(*camera->thread);
    pthread_join(*(camera->thread), NULL);
    *camera->frame = NULL;
    camera->frame = nullptr;
    camera = nullptr;
}