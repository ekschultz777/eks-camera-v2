#include "Camera.h"
#include <stdbool.h>
#include <stdio.h>
#include <unistd.h>
#include <pthread.h>
#include <opencv2/opencv.hpp>
#include <opencv2/highgui/highgui.hpp>

// int main() {
//     char const *pipeline = "Video.mp4";
//     Camera camera = createCamera(pipeline);
//     startCamera(&camera);
//     printf("Finishing...\n");
//     return 0;
// }


// /*
// Working with single background pthread with reference to frame which is used on the main queue.
cv::Mat frame;
pthread_t thread;
pthread_mutex_t mutex;

void *runCameraOperation(void *arg) {
    cv::VideoCapture capture("Video.mp4");
    if (!capture.isOpened()) {
        printf("!!! Failed to open file: %s\n", "Video.mp4");
        return NULL;
    }

    double framesPerSecond = 30.0;
    double secondsPerFrame = (double) ((1 / framesPerSecond) * 1000); // Get ms
    printf("Playing at %f fps\n", framesPerSecond);

    while (1) {
        if (!capture.read(frame)) {
            printf("Failed to capture frame; Aborting...\n");
            break;
        }
        pthread_mutex_lock(&mutex);
        frame = frame;
        pthread_mutex_unlock(&mutex);
        usleep(secondsPerFrame * 1000);
    }
    return NULL;
}

int main() {
    pthread_mutex_init(&mutex, NULL);
    pthread_create(&thread, NULL, runCameraOperation, NULL);
    double framesPerSecond = 30.0;
    double secondsPerFrame = (double) ((1 / framesPerSecond) * 1000); // Get ms
    printf("Playing at %f fps\n", framesPerSecond);

    while (1) {     
        // Thread is always empty. It's not updating here.
        pthread_mutex_lock(&mutex);
        if (!frame.empty()) {
            cv::imshow("frame", frame);
        }
        pthread_mutex_unlock(&mutex);

        char key = cv::waitKey(secondsPerFrame);
        if (key == 27) // ESC
            break;
    }
    return 0;
}
// */