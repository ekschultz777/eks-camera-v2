#include "Camera.h"
#include <stdbool.h>
#include <stdio.h>
#include <unistd.h>
#include <pthread.h>
#include <opencv2/opencv.hpp>
#include <opencv2/highgui/highgui.hpp>

// Run  on background thread
// void *openFrame(void *arg) {
//     Camera *camera = (Camera *)arg;

//     pthread_mutex_lock(camera->mutex);
//     printf("Created pipeline string: %s\n", camera->pipeline);
//     cv::VideoCapture capture(camera->pipeline);

//     if (!(capture).isOpened()) {
//         printf("Failed to open file: %s\n", camera->pipeline);
//         return NULL;
//     }
//     pthread_mutex_unlock(camera->mutex);

//     // FIXME: Share this using camera struct
//     double framesPerSecond = 30.0;
//     double secondsPerFrame = (double) (1 / framesPerSecond);
//     printf("Playing %s at %f fps\n", camera->pipeline, framesPerSecond);

//     while (1) {
//         if (camera == NULL) {
//             break;
//         }
//         pthread_mutex_lock(camera->mutex);
//         printf("%s frame location %p on thread %p\n", camera->pipeline, camera->frame, camera->threadId);
//         if (!capture.read(*camera->frame)) {
//             printf("Failed to capture frame\n");
//         }
//         pthread_mutex_unlock(camera->mutex);
//         // Sleep frame duration in microseconds
//         usleep(secondsPerFrame * 1000 * 1000);
//     }
//     return NULL;
// }

int main() {
    char const *pipelines[] = {"Video2.mp4", "Video3.mp4"};
    const int pipelinesLength = (sizeof(pipelines) / sizeof(pipelines[0]));

    // FIXME: Make this per camera and share
    double framesPerSecond = 30.0;
    double secondsPerFrame = (double) (1 / framesPerSecond);
    printf("Playing at %f fps\n", framesPerSecond);

    Camera *cameras[2];

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
        // FIXME: Get the first frame before starting this loop, 
        // otherwise we will crash because the first frame has 
        // not been correctly created.
        
        // Do not show any cameras until all cameras are ready.
        bool autoStart = true;
        for (int i=0; i<pipelinesLength; i++) {
            // When a camera is ready to display, the status will be set to PAUSED
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
            // TODO: Add logic to synchronize frames here.
            pthread_mutex_lock(cameras[i]->mutex);
            if (cameras[i]->frame != NULL && !cameras[i]->frame->empty()) {
                cv::imshow(cameras[i]->pipeline, *cameras[i]->frame);
            } else {
                printf("Capture is empty from %s\n", pipelines[i]);
            }
            pthread_mutex_unlock(cameras[i]->mutex);
        }

        // Hold frame for duration in milliseconds
        int key = cv::waitKey(secondsPerFrame * 1000);
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