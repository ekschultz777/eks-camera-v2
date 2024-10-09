#include "Camera.h"
#include <stdbool.h>
#include <stdio.h>
#include <unistd.h>
#include <pthread.h>
#include <opencv2/opencv.hpp>
#include <opencv2/highgui/highgui.hpp>

int main() {
    char const *pipeline = "Video.mp4";

    cv::Mat frame;
    pthread_t thread;
    pthread_mutex_t mutex;
    pthread_mutex_init(&mutex, NULL);
    Camera camera = createCamera(pipeline, &thread, &mutex, &frame);
    startCamera(&camera);

    char const *pipeline2 = "Video2.mp4";
    cv::Mat frame2;
    pthread_t thread2;
    pthread_mutex_t mutex2;
    pthread_mutex_init(&mutex2, NULL);
    Camera camera2 = createCamera(pipeline2, &thread2, &mutex2, &frame2);
    startCamera(&camera2);

    while (1) {
        // TODO: Get the first frame before starting this loop
        pthread_mutex_lock(&mutex);
        if (!camera.frame->empty() && !camera2.frame->empty()) {
            cv::imshow("frame", *camera.frame);
            cv::imshow("frame2", *camera2.frame);
        } else {
            printf("Capture is empty\n");
        }
        pthread_mutex_unlock(&mutex);

        // FIXME: Make this per camera and share
        double framesPerSecond = 30.0;
        double secondsPerFrame = (double) (1 / framesPerSecond);
        printf("Playing at %f fps\n", framesPerSecond);

        // Hold frame for duration in milliseconds
        int key = cv::waitKey(secondsPerFrame * 1000);
        if (key == 27) { // ESC key
            break;
        }
    }
    // TODO: Release camera here
    // releaseCamera(&camera);
    printf("Finishing...\n");
    return 0;
}