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

    while (1) {
        // TODO: Get the first frame before starting this loop
        pthread_mutex_lock(&mutex);
        if (!camera.frame->empty()) {
            cv::imshow("frame", *camera.frame);
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