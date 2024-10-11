#include "Camera.h"
#include <stdbool.h>
#include <stdio.h>
#include <unistd.h>
#include <pthread.h>
#include <opencv2/opencv.hpp>
#include <opencv2/highgui/highgui.hpp>

int main() {

    // TODO: Loop through this array
    // char const *pipelines[] = {"Video.mp4", "Video2.mp4"};

    char const *pipeline = "Video3.mp4";
    Camera camera = createCamera(pipeline);

    char const *pipeline2 = "Video2.mp4";
    Camera camera2 = createCamera(pipeline2);

    // FIXME: Make this per camera and share
    double framesPerSecond = 30.0;
    double secondsPerFrame = (double) (1 / framesPerSecond);
    printf("Playing at %f fps\n", framesPerSecond);

    // Move windows for display
    cv::namedWindow(pipeline);
    cv::namedWindow(pipeline2);
    cv::moveWindow(pipeline2, 640, 0);

    while (1) {
        // FIXME: Get the first frame before starting this loop, 
        // otherwise we will crash because the first frame has 
        // not been correctly created.
        pthread_mutex_lock(camera.mutex);
        if (camera.frame != NULL && !camera.frame->empty()) {
            cv::imshow(pipeline, *camera.frame);
        } else {
            printf("Capture is empty\n");
        }
        pthread_mutex_unlock(camera.mutex);

        pthread_mutex_lock(camera2.mutex);
        if (camera2.frame != NULL && !camera2.frame->empty()) {
            cv::imshow(pipeline2, *camera2.frame);
        } else {
            printf("Capture is empty\n");
        }
        pthread_mutex_unlock(camera2.mutex);

        // Hold frame for duration in milliseconds
        int key = cv::waitKey(secondsPerFrame * 1000);
        if (key == 27) { // ESC key
            break;
        }
    }
    // TODO: Release camera here
    releaseCamera(&camera);
    printf("Finishing...\n");
    return 0;
}