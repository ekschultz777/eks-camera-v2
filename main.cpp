#include "Camera.h"
#include <stdbool.h>
#include <stdio.h>
#include <unistd.h>
#include <pthread.h>
#include <opencv2/opencv.hpp>
#include <opencv2/highgui/highgui.hpp>

int main() {
    // TODO: make this a dynamic array of unknown length
    char const *pipelines[] = {"Video.mp4", "Video2.mp4", "Video3.mp4"};
    const int pipelinesLength = (sizeof(pipelines) / sizeof(pipelines[0]));

    Camera *cameras[3];

    // Move windows for display
    for (int i=0; i<pipelinesLength; i++) {
        Camera *camera = (Camera *)malloc(sizeof(Camera));
        initCamera(camera, pipelines[i]);
        cameras[i] = camera;
        cv::namedWindow(pipelines[i]);
        // Move window by 640px to the right of the last frame
        cv::moveWindow(pipelines[i], i*640, 0);
    }

    while (1) {
        // Set flag to allow cameras to begin reading frames.
        bool autoStart = true;
        for (int i=0; i<pipelinesLength; i++) {
            pthread_mutex_lock(cameras[i]->mutex);
            if (cameras[i]->status != READY) {
                autoStart = false;
            }
            pthread_mutex_unlock(cameras[i]->mutex);
            if (autoStart == false) {
                break;
            }
        }

        if (autoStart) {
            for (int i=0; i<pipelinesLength; i++) {
                pthread_mutex_lock(cameras[i]->mutex);
                cameras[i]->status = RUNNING;
                pthread_mutex_unlock(cameras[i]->mutex);
            }
        }

        // Show all camera frames
        for (int i=0; i<pipelinesLength; i++) {
            pthread_mutex_lock(cameras[i]->mutex);
            if (cameras[i]->frame != NULL && !cameras[i]->frame->empty()) {
                cv::imshow(cameras[i]->pipeline, *cameras[i]->frame);
            } else {
                printf("Capture is empty from %s\n", pipelines[i]);
            }
            pthread_mutex_unlock(cameras[i]->mutex);
        }

        // Refresh rate in milliseconds. Currently using 60Hz
        int key = cv::waitKey((1.0 / 60.0) * 1000.0);
        if (key == 27) { // ESC key
            break;
        }
    }
    for (int i=0; i<pipelinesLength; i++) {
        releaseCamera(cameras[i]);
    }
    printf("Finishing...\n");
    return 0;
}