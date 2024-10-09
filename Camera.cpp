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
    printf("Playing at %f fps\n", framesPerSecond);
    cv::Mat frame;
    while (1) {

        if (!capture.read(frame)) {
            printf("Failed to capture frame\n");
        }
        pthread_mutex_lock(camera->mutex);
        *camera->frame = frame;
        pthread_mutex_unlock(camera->mutex);
        // Sleep frame duration in microseconds
        usleep(secondsPerFrame * 1000 * 1000);
    }
    return NULL;
}

// Run  on main thread
Camera createCamera(char const *pipe, pthread_t *thread, pthread_mutex_t *mutex, cv::Mat *frame) {
    // Cast to C++ string for OpenCV
    // std::string const pipeString = pipe;
    // printf("Created pipeline string: %s\n", pipeString.c_str());

    // Declare variabless
    // pthread_t thread;
    // // TODO: Replace these when using GStreamer
    // // int captureType = cv::CAP_GSTREAMER;
    // // cv::VideoCapture capture = cv::VideoCapture(pipeString, captureType);
    // cv::VideoCapture capture;
    bool running = false;
    // cv::Mat frame;
    // pthread_mutex_t mutex;
    pthread_mutex_init(mutex, NULL);
    Camera camera = { 
        .mutex = mutex,
        .pipeline = pipe,
        // .capture = capture,
        .frame = frame,
        .running = running,
    };
    if (pthread_create(thread, NULL, operation, &camera) != 0) {
        printf("Failed to properly initialize thread!\n");
    }

    // Send them to a struct that keeps pointers (We need to 
    // safely access each value from its pointer on a different 
    // thread)
    // Camera camera = { 
    //     .thread = thread,
    //     .mutex = mutex,
    //     .pipeline = pipe,
    //     .capture = capture,
    //     .frame = frame,
    //     .running = running,
    // };
    // pthread_mutex_lock(camera.mutex);
    // if (pthread_create(&thread, NULL, operation, &camera) != 0) {
    //     printf("Failed to properly initialize thread!\n");
    // }
    // pthread_mutex_unlock(camera.mutex);
    // cv::Mat frame;
    // (*camera.capture).read(frame);
    return camera;
}

// Run  on main thread
void startCamera(Camera *camera) {
    // pthread_mutex_init(camera->mutex, NULL);
    // if (pthread_create(&camera->thread, NULL, operation, &camera) != 0) {
    //     printf("Failed to properly initialize thread!\n");
    // }
    if ((camera->running) != true) {
        camera->running = true;
    }
}

void stopCamera(Camera *camera) {

}

void updateCamera(Camera *camera) {

}

// void releaseCamera(Camera *camera) {
//     // pthread_mutex_unlock(&(camera->mutex));
//     pthread_mutex_lock(camera->mutex);
//     pthread_cancel(*(camera->thread));
//     pthread_mutex_unlock(camera->mutex);
// }

// ChatGPT generated
void releaseCamera(Camera *camera) {
    pthread_mutex_lock(camera->mutex);
    // pthread_cancel(*camera->thread);
    // pthread_join(*(camera->thread), NULL); // Ensure the thread has finished
    // delete camera->thread;
    // delete camera.capture; // Release VideoCapture
    delete camera->frame; // Release frame
    // delete camera->running; // Release running flag
    pthread_mutex_destroy(camera->mutex); // Destroy mutex
    delete camera->mutex; // Release mutex memory
    camera = nullptr; // Avoid dangling pointer
}

// int main() {
//     return 0;
// }

// It seems that the lock is there because we need mutual exclusion on gstreamer as it is a shared resource. 

/* Prototype code copied from Python
class Camera:
    def __init__(self):
        # Initialize instance variables
        # OpenCV video capture element
        self.video_capture = None
        # The last captured image from the camera
        self.frame = None
        self.grabbed = False
        # The thread where the video capture runs
        self.read_thread = None
        self.read_lock = threading.Lock()
        self.running = False

    def open(self, gstreamer_pipeline_string):
        try:
            self.video_capture = cv2.VideoCapture(
                gstreamer_pipeline_string, cv2.CAP_GSTREAMER
            )
            # Grab the first frame to start the video capturing
            self.grabbed, self.frame = self.video_capture.read()

        except RuntimeError:
            self.video_capture = None
            print("Unable to open camera")
            print("Pipeline: " + gstreamer_pipeline_string)


    def start(self):
        if self.running:
            print('Video capturing is already running')
            return None
        # create a thread to read the camera image
        if self.video_capture != None:
            self.running = True
            self.read_thread = threading.Thread(target=self.updateCamera)
            self.read_thread.start()
        return self

    def stop(self):
        self.running = False
        # Kill the thread
        self.read_thread.join()
        self.read_thread = None

    def updateCamera(self):
        # This is the thread to read images from the camera
        while self.running:
            try:
                grabbed, frame = self.video_capture.read()
                with self.read_lock:
                    self.grabbed = grabbed
                    self.frame = frame
            except RuntimeError:
                print("Could not read image from camera")
        # FIX ME - stop and cleanup thread
        # Something bad happened

    def read(self):
        with self.read_lock:
            frame = self.frame.copy()
            grabbed = self.grabbed
        return grabbed, frame

    def release(self):
        if self.video_capture != None:
            self.video_capture.release()
            self.video_capture = None
        # Now kill the thread
        if self.read_thread != None:
            self.read_thread.join()
*/