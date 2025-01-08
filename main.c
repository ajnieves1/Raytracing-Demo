#include <stdio.h>
#include <SDL.h>
#include <math.h>

#define WIDTH 1200 // Width of the window
#define HEIGHT 600 // Height of the window
#define COLOR_WHITE 0xffffffff // White color in hexadecimal
#define COLOR_BLACK 0x00000000 // Black color in hexadecimal
#define RAYS_NUM 500 // Number of rays to generate, increase this if you want a more solid light
#define COLOR_RAY 0xe3d26f // Color for the rays
#define RAY_THICKNESS 5 // Thickness of the rays

// Structure to represent a circle with a center (x, y) and radius (r)
struct Circle {
    double x; // X-coordinate of the center
    double y; // Y-coordinate of the center
    double r; // Radius of the circle
};

// Structure to represent a ray with a starting position (xStart, yStart) and an angle
struct Ray {
    double xStart; // X coordinate of the ray's starting point
    double yStart; // Ycoordinate of the ray's starting point
    double angle; // Angle of the ray in radians
};

// Function to draw a filled circle on the SDL surface
void FillCircle(SDL_Surface* surface, struct Circle circle, Uint32 color) {
    double radiusSquared = pow(circle.r, 2); // Calculate the square of the radius
    for (double x = circle.x - circle.r; x <= circle.x + circle.r; x++) { // Iterate over xcoordinates in the bounding box
        for (double y = circle.y - circle.r; y <= circle.y + circle.r; y++) { // Iterate over ycoordinates
            double distanceSquared = pow(x - circle.x, 2) + pow(y - circle.y, 2); // Calculate squared distance from the center
            if (distanceSquared < radiusSquared) { // Check if the point is inside the circle
                SDL_Rect pixel = (SDL_Rect){x, y, 1, 1}; // Create a rectangle representing the pixel
                SDL_FillRect(surface, &pixel, color); // Fill the pixel with the specified color
            }
        }
    }
}

// Function to generate an array of rays originating from a circle
void generateRays(struct Circle circle, struct Ray rays[RAYS_NUM]) {
    for (int i = 0; i < RAYS_NUM; i++) { // Loop through the number of rays
        double angle = ((double) i / RAYS_NUM) * 2 * M_PI; // Calculate the angle for the current ray
        struct Ray ray = {circle.x, circle.y, angle}; // Initialize the ray's starting point and angle
        rays[i] = ray; // Store the ray in the array
    }
}

// Function to draw rays and detect collisions with a circle
void FillRays(SDL_Surface* surface, struct Ray rays[RAYS_NUM], Uint32 color, struct Circle object) {
    double radiusSquared = pow(object.r, 2); // Precompute the square of the object's radius

    for (int i = 0; i < RAYS_NUM; i++) { // Iterate over all rays
        struct Ray ray = rays[i]; // Get the current ray

        int endOfScreen = 0; // Flag to indicate if the ray has reached the screen boundary

        double step = 1; // Step size for ray movement
        double xDraw = ray.xStart; // Start x-coordinate of the ray
        double yDraw = ray.yStart; // Start y-coordinate of the ray
        while (!endOfScreen) { // Continue until the ray hits the object or the screen boundary
            xDraw += step * cos(ray.angle); // Increment x-coordinate based on the angle (Main idea here is creating rays at 0, pi/2, pi, 3pi/2, and 2pi)
            yDraw += step * sin(ray.angle); // Increment y-coordinate based on the angle
            SDL_Rect pixel = (SDL_Rect){xDraw, yDraw, RAY_THICKNESS, RAY_THICKNESS}; // Create a rectangle for the ray segment
            SDL_FillRect(surface, &pixel, color); // Draw the ray segment

            if (xDraw < 0 || xDraw > WIDTH) { // Check if the ray has reached the screen boundary horizontally
                endOfScreen = 1;
            }
            if (yDraw < 0 || yDraw > HEIGHT) { // Check if the ray has reached the screen boundary vertically
                endOfScreen = 1;
            }

            double distanceSquared = pow(xDraw - object.x, 2) + pow(yDraw - object.y, 2); // Calculate the squared distance to the object
            if (distanceSquared < radiusSquared) { // Check if the ray hits the object
                break; // Stop drawing the ray
            }
        }
    }
}

int main(int argc, char* argv[]) {
    SDL_Init(SDL_INIT_VIDEO); // Initialize SDL
    SDL_Window* window = SDL_CreateWindow("Raytracing", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, WIDTH, HEIGHT, 0); // Create the SDL window
    SDL_Surface* surface = SDL_GetWindowSurface(window); // Get the surface associated with the window
    struct Circle circle = {200, 200, 40}; // Define the circle emitting rays
    struct Circle shadowCircle = {600, 300, 140}; // Define the object casting a shadow
    SDL_Rect eraseRect = {0, 0, WIDTH, HEIGHT}; // Rectangle used to clear the screen, prevents multiple circles being drawn when dragging the circle casting the rays

    struct Ray rays[RAYS_NUM]; // Array to store the rays
    generateRays(circle, rays); // Generate initial rays

    double obstacleSpeedY = 1; // Speed of the shadow circle
    int simulationRunning = 1; // Flag to keep the simulation running
    SDL_Event event; // SDL event for handling user input
    while (simulationRunning) { // Main simulation loop
        while (SDL_PollEvent(&event)) { // Handle SDL events
            if (event.type == SDL_QUIT) { // Check if the user requested to quit ( hitting the x button in top right of window)
                simulationRunning = 0; // Stop the simulation
            }
            if (event.type == SDL_MOUSEMOTION && event.motion.state != 0) { // Check if the mouse is being dragged
                circle.x = event.motion.x; // Update the circle's xcoordinate to match the mouse
                circle.y = event.motion.y; // Update the circle's y coordinate to match the mouse
                generateRays(circle, rays); // Regenerate rays based on the new circle position
            }
        }
        SDL_FillRect(surface, &eraseRect, COLOR_BLACK); // Clear the screen (avoid infinite circles being drawn so we only have 1 circle we can interact with)
        FillRays(surface, rays, COLOR_RAY, shadowCircle); // Draw the rays
        FillCircle(surface, circle, COLOR_WHITE); // Draw the circle emitting rays
        FillCircle(surface, shadowCircle, COLOR_WHITE); // Draw the shadow circle

        shadowCircle.y += obstacleSpeedY; // Move the shadow circle vertically
        if (shadowCircle.y - shadowCircle.r < 0) { // Reverse direction if the shadow circle goes off the top of window
            obstacleSpeedY = -obstacleSpeedY;
        }
        if (shadowCircle.y + shadowCircle.r > HEIGHT) { // Reverse direction if the shadow circle goes off the bottom of window
            obstacleSpeedY = -obstacleSpeedY;
        }

        SDL_UpdateWindowSurface(window); // Update the window with the new drawing
        SDL_Delay(10); // Delay to control the frame rate
    }
}
