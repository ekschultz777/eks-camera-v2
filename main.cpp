#include "Camera.h"
#include <stdbool.h>
#include <stdio.h>
#include <unistd.h>
#include <pthread.h>
#include <opencv2/opencv.hpp>
#include <opencv2/highgui/highgui.hpp>

// int main() {

//     // TODO: Loop through this array
//     // char const *pipelines[] = {"Video.mp4", "Video2.mp4"};

//     char const *pipeline = "Video3.mp4";
//     Camera camera = createCamera(pipeline);

//     char const *pipeline2 = "Video2.mp4";
//     Camera camera2 = createCamera(pipeline2);

//     // FIXME: Make this per camera and share
//     double framesPerSecond = 30.0;
//     double secondsPerFrame = (double) (1 / framesPerSecond);
//     printf("Playing at %f fps\n", framesPerSecond);

//     // Move windows for display
//     cv::namedWindow(pipeline);
//     cv::namedWindow(pipeline2);
//     cv::moveWindow(pipeline2, 640, 0);

//     while (1) {
//         // FIXME: Get the first frame before starting this loop, 
//         // otherwise we will crash because the first frame has 
//         // not been correctly created.
//         pthread_mutex_lock(camera.mutex);
//         if (camera.frame != NULL && !camera.frame->empty()) {
//             cv::imshow(pipeline, *camera.frame);
//         } else {
//             printf("Capture is empty\n");
//         }
//         pthread_mutex_unlock(camera.mutex);

//         pthread_mutex_lock(camera2.mutex);
//         if (camera2.frame != NULL && !camera2.frame->empty()) {
//             cv::imshow(pipeline2, *camera2.frame);
//         } else {
//             printf("Capture is empty\n");
//         }
//         pthread_mutex_unlock(camera2.mutex);

//         // Hold frame for duration in milliseconds
//         int key = cv::waitKey(secondsPerFrame * 1000);
//         if (key == 27) { // ESC key
//             break;
//         }
//     }
//     // TODO: Release camera here
//     releaseCamera(&camera);
//     printf("Finishing...\n");
//     return 0;
// }

// Run  on background thread
void *openFrame(void *arg) {
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
        printf("%s frame location %p on thread %p\n", camera->pipeline, camera->frame, camera->thread);
        if (!capture.read(*camera->frame)) {
            printf("Failed to capture frame\n");
        }
        pthread_mutex_unlock(camera->mutex);
        // Sleep frame duration in microseconds
        usleep(secondsPerFrame * 1000 * 1000);
    }
    return NULL;
}

int main() {
    char const *pipelines[] = {"Video2.mp4", "Video3.mp4"};
    const int pipelinesLength = (sizeof(pipelines) / sizeof(pipelines[0]));

    // FIXME: Make this per camera and share
    double framesPerSecond = 30.0;
    double secondsPerFrame = (double) (1 / framesPerSecond);
    printf("Playing at %f fps\n", framesPerSecond);

    Camera cameras[2];

    // Move windows for display
    for (int i=0; i<pipelinesLength; i++) {
        pthread_t *threadId = (pthread_t *)malloc(sizeof(pthread_t));
        pthread_mutex_t *mutex = (pthread_mutex_t *)malloc(sizeof(pthread_mutex_t));
        pthread_mutex_init(mutex, NULL);
        cv::Mat *frame = (cv::Mat *)malloc(sizeof(cv::Mat));
        cameras[i] = { 
            .threadId = threadId,
            .mutex = mutex,
            .pipeline = pipelines[i],
            .frame = frame,
        };
        if (pthread_create(cameras[i].threadId, NULL, openFrame, &cameras[i]) != 0) {
            printf("Failed to properly initialize thread!\n");
        }

        printf("%d\n",i);
        printf("%s\n", pipelines[i]);
        cv::namedWindow(pipelines[i]);
        // Move window by 640px to the right of the last frame
        cv::moveWindow(pipelines[i], i*640, 0);
    }

    while (1) {
        // FIXME: Get the first frame before starting this loop, 
        // otherwise we will crash because the first frame has 
        // not been correctly created.
        for (int i=0; i<pipelinesLength; i++) {
            pthread_mutex_lock(cameras[i].mutex);
            if (cameras[i].frame != NULL && !cameras[i].frame->empty()) {
                cv::imshow(cameras[i].pipeline, *cameras[i].frame);
            } else {
                printf("Capture is empty from %s\n", pipelines[i]);
            }
            pthread_mutex_unlock(cameras[i].mutex);
        }

        // Hold frame for duration in milliseconds
        int key = cv::waitKey(secondsPerFrame * 1000);
        if (key == 27) { // ESC key
            for (int i=0; i<pipelinesLength; i++) {
                // TODO: Free all allocated pointers memory here
                // cameras[i] = NULL;
            }
            break;
        }
    }
    // TODO: Release camera here
    for (int i=0; i<pipelinesLength; i++) {
        releaseCamera(&cameras[i]);
    }
    printf("Finishing...\n");
    return 0;
}