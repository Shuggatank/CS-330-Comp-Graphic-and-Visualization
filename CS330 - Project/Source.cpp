#include <iostream>         // cout, cerr
#include <cstdlib>          // EXIT_FAILURE



#include <glad\glad.h>
#include <GLFW/glfw3.h>     // GLFW library




#define STB_IMAGE_IMPLEMENTATION
#include "include/stb_image.h"      // Image loading Utility functions

// GLM Math Header inclusions
#include <glm/glm.hpp>
#include <glm/gtx/transform.hpp>
#include <glm/gtc/type_ptr.hpp>

//#include "shader.h"

#include "include/camera.h" // Camera class
#include "cylinder.h"
#include "torus.h"

#ifndef M_PI
#define M_PI 3.14159265
#endif

using namespace std; // Standard namespace

/*Shader program Macro*/
#ifndef GLSL
#define GLSL(Version, Source) "#version " #Version " core \n" #Source
#endif

// Unnamed namespace
namespace
{
    const char* const WINDOW_TITLE = "CS330 Project - Dipesh Patel"; // Macro for window title

    // Variables for window width and height
    const int WINDOW_WIDTH = 800;
    const int WINDOW_HEIGHT = 600;

    // Stores the GL data relative to a given mesh
    struct GLMesh
    {
        GLuint vao;         // Handle for the vertex array object
        GLuint vbo;         // Handle for the vertex buffer object
        GLuint nVertices;   // Number of indices of the mesh
        
    };

    // Main GLFW window
    GLFWwindow* gWindow = nullptr;
    // Triangle mesh data
    GLMesh gMesh;
    // Texture id
    GLuint gTextureId;
    glm::vec2 gUVScale(1.0f, 1.0f);
    GLint gTexWrapMode = GL_MIRRORED_REPEAT;
    // Shader program
    GLuint gProgramId;
    GLuint gLampProgramId;

    GLMesh table;
    GLMesh baseknife;
    GLMesh blade;
    GLMesh cylinder;
    GLMesh charger;
    GLMesh watchface;
    GLMesh watchband;
    GLMesh chargercenter;
    GLMesh chargercable;
    GLMesh booklet;
    
    
    GLuint gTextureIdBlack;

    GLuint gTextureIdMetal;
 
    GLuint gTextureIdWood;

    GLuint gTextureIdCarbonfiber;

    GLuint gTextureIdWatchface;

    GLuint gTextureIdWatchband;

    GLuint gTextureIdGrayconnector;

    GLuint gTextureIdBooklet;

    GLuint gTextureIdBookletspec;

    GLuint gTextureIdGrayconnectorspec;

    GLuint gTextureIdWoodspec;

    GLuint gTextureIdBlackspec;

    GLuint gTextureIdMetalspec;

    GLuint gTextureIdWatchfacespec;

    GLuint gTextureIdWatchbandspec;

   

    // camera
    Camera gCamera(glm::vec3(0.0f, 2.5f, 9.0f));
    Camera cameraTarget(glm::vec3(5.0f, 0.0f, 0.0f));
    float gLastX = WINDOW_WIDTH / 2.0f;
    float gLastY = WINDOW_HEIGHT / 2.0f;
    bool gFirstMouse = true;
    bool viewProjection = true;

    // timing
    float gDeltaTime = 0.0f; // time between current frame and last frame
    float gLastFrame = 0.0f;

    // Subject position and scale
    glm::vec3 gPosition(0.0f, 0.0f, 0.0f);
    glm::vec3 gScale(2.0f);

    glm::vec3 bladePosition(0.0f, 0.0f, 0.35f);

    // Cube and light color
    glm::vec3 gObjectColor(1.0f, 1.0f, 1.0f);
    glm::vec3 gLightColor(0.2f, 0.2f, 0.4f);
    glm::vec3 gLightColor2(1.5f, 1.5f, 1.5f);

    // Light position and scale
    glm::vec3 gLightPosition(-5.0f, 3.0f, -6.0f);
    glm::vec3 gLightScale(0.5f);

    glm::vec3 gLightPosition2(5.0f, 5.0f, 3.0f);
    glm::vec3 gLightScale2(0.5f);

    glm::vec3 gLightDirection(-0.2f, -1.0f, -0.3f);

    // Lamp animation
    bool gIsLampOrbiting = false;


}

/* User-defined Function prototypes to:
 * initialize the program, set the window size,
 * redraw graphics on the window when resized,
 * and render graphics on the screen
 */
bool UInitialize(int, char* [], GLFWwindow** window);
void UResizeWindow(GLFWwindow* window, int width, int height);
void UProcessInput(GLFWwindow* window);
void UMousePositionCallback(GLFWwindow* window, double xpos, double ypos);
void UMouseScrollCallback(GLFWwindow* window, double xoffset, double yoffset);
void UMouseButtonCallback(GLFWwindow* window, int button, int action, int mods);
void UCreateMesh(GLMesh& mesh);
void UDestroyMesh(GLMesh& mesh);
bool UCreateTexture(const char* filename, GLuint& textureId);
void UDestroyTexture(GLuint textureId);
void URender();
bool UCreateShaderProgram(const char* vtxShaderSource, const char* fragShaderSource, GLuint& programId);
void UDestroyShaderProgram(GLuint programId);


/* Vertex Shader Source Code*/
const GLchar* vertexShaderSource = GLSL(440,
    layout(location = 0) in vec3 position; // VAP position 0 for vertex position data
    layout(location = 1) in vec3 normal; // VAP position 1 for normals
    layout(location = 2) in vec2 textureCoordinate;

    out vec3 vertexNormal; // For outgoing normals to fragment shader
    out vec3 vertexFragmentPos; // For outgoing color / pixels to fragment shader
    out vec2 vertexTextureCoordinate;

//Global variables for the transform matrices
uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

    void main()
    {
        gl_Position = projection * view * model * vec4(position, 1.0f); // transforms vertices to clip coordinates
        vertexTextureCoordinate = textureCoordinate;
        vertexFragmentPos = vec3(model * vec4(position, 1.0f)); // Gets fragment / pixel position in world space only (exclude view and projection)

        vertexNormal = mat3(transpose(inverse(model))) * normal; // get normal vectors in world space only and exclude normal translation properties
    }
);


/* Fragment Shader Source Code*/
const GLchar* fragmentShaderSource = GLSL(440,
    in vec2 vertexTextureCoordinate;
    in vec3 vertexNormal; // For incoming normals
    in vec3 vertexFragmentPos; // For incoming fragment position

    out vec4 fragmentColor;

    // Uniform / Global variables for object color, light color, light position, and camera/view position
    uniform vec3 objectColor;
    uniform vec3 lightColor;
    uniform vec3 lightColor2;
    uniform vec3 lightPos;
    uniform vec3 lightPos2;
    uniform vec3 viewPosition;
    uniform vec3 lightDirection;
    uniform sampler2D uTexture; // Useful when working with multiple textures
    uniform sampler2D specTexture;
    uniform vec2 uvScale;

void main()
{
    /*Phong lighting model calculations to generate ambient, diffuse, and specular components*/

        //Calculate Ambient lighting*/
    float ambientStrength = 0.1f; // Set ambient or global lighting strength
    vec3 ambient = ambientStrength * lightColor; // Generate ambient light color

    //Calculate Diffuse lighting*/
    vec3 norm = normalize(vertexNormal); // Normalize vectors to 1 unit
    vec3 lightDirection = normalize(lightPos - vertexFragmentPos); // Calculate distance (light direction) between light source and fragments/pixels on cube
    vec3 lightDirection2 = normalize(lightPos2 - vertexFragmentPos);
    float impact = max(dot(norm, lightDirection), 0.0);// Calculate diffuse impact by generating dot product of normal and light
    float impact2 = max(dot(norm, lightDirection2), 0.0);// Calculate diffuse impact by generating dot product of normal and light
    vec3 diffuse = impact * lightColor; // Generate diffuse light color
    vec3 diffuse2 = impact2 * lightColor2; // Generate diffuse light color

    //Calculate Specular lighting*/
    float specularIntensity = 5.8f; // Set specular light strength
    float highlightSize = 26.0f; // Set specular highlight size
    vec3 viewDir = normalize(viewPosition - vertexFragmentPos); // Calculate view direction
    vec3 reflectDir = reflect(-lightDirection, norm);// Calculate reflection vector
    vec3 reflectDir2 = reflect(-lightDirection2, norm);// Calculate reflection vector
    //Calculate specular component
    float specularComponent = pow(max(dot(viewDir, reflectDir), 0.0), highlightSize);
    float specularComponent2 = pow(max(dot(viewDir, reflectDir2), 0.0), highlightSize);
    vec3 specular = specularIntensity * specularComponent * lightColor * vec3(texture(specTexture, vertexTextureCoordinate * uvScale));
    vec3 specular2 = specularIntensity * specularComponent2 * lightColor * vec3(texture(specTexture, vertexTextureCoordinate * uvScale));

    // Texture holds the color to be used for all three components
    vec4 textureColor = texture(uTexture, vertexTextureCoordinate * uvScale);

    // Calculate phong result
    vec3 phong = (ambient + diffuse + diffuse2 + specular + specular2) * textureColor.xyz;

    fragmentColor = vec4(phong, 1.0); // Send lighting results to GPU

}
);

/* Lamp Shader Source Code*/
const GLchar* lampVertexShaderSource = GLSL(440,

    layout(location = 0) in vec3 position; // VAP position 0 for vertex position data

        //Uniform / Global variables for the  transform matrices
uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

void main()
{
    gl_Position = projection * view * model * vec4(position, 1.0f); // Transforms vertices into clip coordinates
}
);


/* Fragment Shader Source Code*/
const GLchar* lampFragmentShaderSource = GLSL(440,

    out vec4 fragmentColor; // For outgoing lamp color (smaller cube) to the GPU

void main()
{
    fragmentColor = vec4(1.0f); // Set color to white (1.0f,1.0f,1.0f) with alpha 1.0
}
);


// Images are loaded with Y axis going down, but OpenGL's Y axis goes up, so let's flip it
void flipImageVertically(unsigned char* image, int width, int height, int channels)
{
    for (int j = 0; j < height / 2; ++j)
    {
        int index1 = j * width * channels;
        int index2 = (height - 1 - j) * width * channels;

        for (int i = width * channels; i > 0; --i)
        {
            unsigned char tmp = image[index1];
            image[index1] = image[index2];
            image[index2] = tmp;
            ++index1;
            ++index2;
        }
    }
}


int main(int argc, char* argv[])
{
    if (!UInitialize(argc, argv, &gWindow))
        return EXIT_FAILURE;

    // Create the mesh
    UCreateMesh(gMesh); // Calls the function to create the Vertex Buffer Object

    // Create the shader program
    if (!UCreateShaderProgram(vertexShaderSource, fragmentShaderSource, gProgramId))
        return EXIT_FAILURE;

    if (!UCreateShaderProgram(lampVertexShaderSource, lampFragmentShaderSource, gLampProgramId))
        return EXIT_FAILURE;

    // Load texture
    const char* texFilename = "../resources/textures/black.jpg";
    if (!UCreateTexture(texFilename, gTextureIdBlack))
    {
        cout << "Failed to load texture " << texFilename << endl;
        return EXIT_FAILURE;
    }

    texFilename = "../resources/textures/metal.jpg";
    if (!UCreateTexture(texFilename, gTextureIdMetal))
    {
        cout << "Failed to load texture " << texFilename << endl;
        return EXIT_FAILURE;
    }
    texFilename = "../resources/textures/wood.jpg";
    if (!UCreateTexture(texFilename, gTextureIdWood))
    {
        cout << "Failed to load texture " << texFilename << endl;
        return EXIT_FAILURE;
    }

    texFilename = "../resources/textures/carbonfiber2.jpg"; 
    if (!UCreateTexture(texFilename, gTextureIdCarbonfiber))
    {
        cout << "Failed to load texture " << texFilename << endl;
        return EXIT_FAILURE;
    }

    texFilename = "../resources/textures/watchface.png";
    if (!UCreateTexture(texFilename, gTextureIdWatchface))
    {
        cout << "Failed to load texture " << texFilename << endl;
        return EXIT_FAILURE;
    }

    texFilename = "../resources/textures/band.jpg";
    if (!UCreateTexture(texFilename, gTextureIdWatchband))
    {
        cout << "Failed to load texture " << texFilename << endl;
        return EXIT_FAILURE;
    }

    texFilename = "../resources/textures/grayconnector.jpg";
    if (!UCreateTexture(texFilename, gTextureIdGrayconnector))
    {
        cout << "Failed to load texture " << texFilename << endl;
        return EXIT_FAILURE;
    }

    texFilename = "../resources/textures/booklet.png";
    if (!UCreateTexture(texFilename, gTextureIdBooklet))
    {
        cout << "Failed to load texture " << texFilename << endl;
        return EXIT_FAILURE;
    }

    texFilename = "../resources/textures/bookletSpec.png";
    if (!UCreateTexture(texFilename, gTextureIdBookletspec))
    {
        cout << "Failed to load texture " << texFilename << endl;
        return EXIT_FAILURE;
    }

    texFilename = "../resources/textures/woodSpecularMap.jpg";
    if (!UCreateTexture(texFilename, gTextureIdWoodspec))
    {
        cout << "Failed to load texture " << texFilename << endl;
        return EXIT_FAILURE;

    }

    texFilename = "../resources/textures/blackSpecularMap.jpg";
    if (!UCreateTexture(texFilename, gTextureIdBlackspec))
    {
        cout << "Failed to load texture " << texFilename << endl;
        return EXIT_FAILURE;

    }

    texFilename = "../resources/textures/metalSpecularMap.jpg";
    if (!UCreateTexture(texFilename, gTextureIdMetalspec))
    {
        cout << "Failed to load texture " << texFilename << endl;
        return EXIT_FAILURE;

    }

    texFilename = "../resources/textures/watchfacespec.png";
    if (!UCreateTexture(texFilename, gTextureIdWatchfacespec))
    {
        cout << "Failed to load texture " << texFilename << endl;
        return EXIT_FAILURE;

    }

    texFilename = "../resources/textures/bandSpecularMap.jpg";
    if (!UCreateTexture(texFilename, gTextureIdWatchbandspec))
    {
        cout << "Failed to load texture " << texFilename << endl;
        return EXIT_FAILURE;
    }

    texFilename = "../resources/textures/grayconnectorSpec.jpg";
    if (!UCreateTexture(texFilename, gTextureIdGrayconnectorspec))
    {
        cout << "Failed to load texture " << texFilename << endl;
        return EXIT_FAILURE;
    }

    // tell opengl for each sampler to which texture unit it belongs to (only has to be done once)
    glUseProgram(gProgramId);
    // We set the texture as texture unit 0
    glUniform1i(glGetUniformLocation(gProgramId, "uTexture"), 0);

    

    // We set the texture as texture unit 1
    glUniform1i(glGetUniformLocation(gProgramId, "specTexture"), 1);



    // Sets the background color of the window to black (it will be implicitely used by glClear)
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);

    // render loop
    // -----------
    while (!glfwWindowShouldClose(gWindow))
    {
        // per-frame timing
        // --------------------
        float currentFrame = glfwGetTime();
        gDeltaTime = currentFrame - gLastFrame;
        gLastFrame = currentFrame;

        // input
        // -----
        UProcessInput(gWindow);

        // Render this frame
        URender();

        glfwPollEvents();
    }

    // Release mesh data
    UDestroyMesh(gMesh);

    // Release texture
    UDestroyTexture(gTextureId);
    UDestroyTexture(gTextureIdBlack);
    UDestroyTexture(gTextureIdMetal);
    UDestroyTexture(gTextureIdWood);
    UDestroyTexture(gTextureIdWoodspec);
    UDestroyTexture(gTextureIdBlackspec);
    UDestroyTexture(gTextureIdMetalspec);
    UDestroyTexture(gTextureIdCarbonfiber);
    UDestroyTexture(gTextureIdWatchface);
    UDestroyTexture(gTextureIdWatchfacespec);
    UDestroyTexture(gTextureIdWatchband);
    UDestroyTexture(gTextureIdWatchbandspec);
    UDestroyTexture(gTextureIdGrayconnector);
    UDestroyTexture(gTextureIdGrayconnectorspec);
    UDestroyTexture(gTextureIdBooklet);
    UDestroyTexture(gTextureIdBookletspec);

    


    // Release shader program
    UDestroyShaderProgram(gProgramId);
    UDestroyShaderProgram(gLampProgramId);

    exit(EXIT_SUCCESS); // Terminates the program successfully
}




// Initialize GLFW, GLEW, and create a window
bool UInitialize(int argc, char* argv[], GLFWwindow** window)
{
    // GLFW: initialize and configure
    // ------------------------------
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 4);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

#ifdef __APPLE__
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif

    // GLFW: window creation
    // ---------------------
    * window = glfwCreateWindow(WINDOW_WIDTH, WINDOW_HEIGHT, WINDOW_TITLE, NULL, NULL);
    if (*window == NULL)
    {
        std::cout << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return false;
    }
    glfwMakeContextCurrent(*window);
    glfwSetFramebufferSizeCallback(*window, UResizeWindow);
    glfwSetCursorPosCallback(*window, UMousePositionCallback);
    glfwSetScrollCallback(*window, UMouseScrollCallback);
    glfwSetMouseButtonCallback(*window, UMouseButtonCallback);

    // tell GLFW to capture our mouse
    glfwSetInputMode(*window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

    // glad: load all OpenGL function pointers
   // ---------------------------------------
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        std::cout << "Failed to initialize GLAD" << std::endl;
        return -1;
    }





    // Displays GPU OpenGL version
    cout << "INFO: OpenGL Version: " << glGetString(GL_VERSION) << endl;

    return true;
}


// process all input: query GLFW whether relevant keys are pressed/released this frame and react accordingly
void UProcessInput(GLFWwindow* window)
{
    static const float cameraSpeed = 2.5f;

    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);

    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
        gCamera.ProcessKeyboard(FORWARD, gDeltaTime);
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
        gCamera.ProcessKeyboard(BACKWARD, gDeltaTime);
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
        gCamera.ProcessKeyboard(LEFT, gDeltaTime);
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
        gCamera.ProcessKeyboard(RIGHT, gDeltaTime);
    if (glfwGetKey(window, GLFW_KEY_Q) == GLFW_PRESS)
        gCamera.ProcessKeyboard(UP, gDeltaTime);
    if (glfwGetKey(window, GLFW_KEY_E) == GLFW_PRESS)
        gCamera.ProcessKeyboard(DOWN, gDeltaTime);
    if (glfwGetKey(window, GLFW_KEY_P) == GLFW_PRESS)
        viewProjection = !viewProjection;

    


    // Pause and resume lamp orbiting
    static bool isLKeyDown = false;
    if (glfwGetKey(window, GLFW_KEY_L) == GLFW_PRESS && !gIsLampOrbiting)
        gIsLampOrbiting = true;
    else if (glfwGetKey(window, GLFW_KEY_K) == GLFW_PRESS && gIsLampOrbiting)
        gIsLampOrbiting = false;


}


// glfw: whenever the window size changed (by OS or user resize) this callback function executes
void UResizeWindow(GLFWwindow* window, int width, int height)
{
    glViewport(0, 0, width, height);
}


// glfw: whenever the mouse moves, this callback is called
// -------------------------------------------------------
void UMousePositionCallback(GLFWwindow* window, double xpos, double ypos)
{
    if (gFirstMouse)
    {
        gLastX = xpos;
        gLastY = ypos;
        gFirstMouse = false;
    }

    float xoffset = xpos - gLastX;
    float yoffset = gLastY - ypos; // reversed since y-coordinates go from bottom to top

    gLastX = xpos;
    gLastY = ypos;

    gCamera.ProcessMouseMovement(xoffset, yoffset);
}


// glfw: whenever the mouse scroll wheel scrolls, this callback is called
// ----------------------------------------------------------------------
void UMouseScrollCallback(GLFWwindow* window, double xoffset, double yoffset)
{
    gCamera.ProcessMouseScroll(yoffset);
}

// glfw: handle mouse button events
// --------------------------------
void UMouseButtonCallback(GLFWwindow* window, int button, int action, int mods)
{
    switch (button)
    {
    case GLFW_MOUSE_BUTTON_LEFT:
    {
        if (action == GLFW_PRESS)
            cout << "Left mouse button pressed" << endl;
        else
            cout << "Left mouse button released" << endl;
    }
    break;

    case GLFW_MOUSE_BUTTON_MIDDLE:
    {
        if (action == GLFW_PRESS)
            cout << "Middle mouse button pressed" << endl;
        else
            cout << "Middle mouse button released" << endl;
    }
    break;

    case GLFW_MOUSE_BUTTON_RIGHT:
    {
        if (action == GLFW_PRESS)
            cout << "Right mouse button pressed" << endl;
        else
            cout << "Right mouse button released" << endl;
    }
    break;

    default:
        cout << "Unhandled mouse button event" << endl;
        break;
    }
}


// Functioned called to render a frame
void URender()
{
    // Lamp orbits around the origin
    const float angularVelocity = glm::radians(45.0f);
    if (gIsLampOrbiting)
    {
        glm::vec4 newPosition = glm::rotate(angularVelocity * gDeltaTime, glm::vec3(0.0f, 1.0f, 0.0f)) * glm::vec4(gLightPosition, 1.0f);
        gLightPosition.x = newPosition.x;
        gLightPosition.y = newPosition.y;
        gLightPosition.z = newPosition.z;
    }

    // Enable z-depth
    glEnable(GL_DEPTH_TEST);

    // Clear the frame and z buffers
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

   
    
    // Set the shader to be used
    glUseProgram(gProgramId);


    // Model matrix: transformations are applied right-to-left order
    glm::mat4 model = glm::translate(gPosition) * glm::scale(gScale);

    // camera/view transformation
    glm::mat4 view = gCamera.GetViewMatrix();

    // Creates a perspective projection
    glm::mat4 projection;
    if (viewProjection) {
        projection = glm::perspective(glm::radians(gCamera.Zoom), (GLfloat)WINDOW_WIDTH / (GLfloat)WINDOW_HEIGHT, 0.1f, 100.0f);
    }
    else {
        float scale = 120;
        projection = glm::ortho((800.0f / scale), -(800.0f / scale), -(600.0f / scale), (600.0f / scale), - 2.5f, 15.5f);
    }

    // Retrieves and passes transform matrices to the Shader program
    GLint modelLoc = glGetUniformLocation(gProgramId, "model");
    GLint viewLoc = glGetUniformLocation(gProgramId, "view");
    GLint projLoc = glGetUniformLocation(gProgramId, "projection");

    glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
    glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));
    glUniformMatrix4fv(projLoc, 1, GL_FALSE, glm::value_ptr(projection));

    // Reference matrix uniforms from the Cube Shader program for the cub color, light color, light position, and camera position
    GLint objectColorLoc = glGetUniformLocation(gProgramId, "objectColor");
    GLint lightColorLoc = glGetUniformLocation(gProgramId, "lightColor");
    GLint lightColorLoc2 = glGetUniformLocation(gProgramId, "lightColor2");
    GLint lightPositionLoc = glGetUniformLocation(gProgramId, "lightPos");
    GLint lightPositionLoc2 = glGetUniformLocation(gProgramId, "lightPos2");
    GLint viewPositionLoc = glGetUniformLocation(gProgramId, "viewPosition");
    GLint lightDirection = glGetUniformLocation(gProgramId, "lightDirection");

    // Pass color, light, and camera data to the Cube Shader program's corresponding uniforms
    glUniform3f(objectColorLoc, gObjectColor.r, gObjectColor.g, gObjectColor.b);
    glUniform3f(lightColorLoc, gLightColor.r, gLightColor.g, gLightColor.b);
    glUniform3f(lightPositionLoc, gLightPosition.x, gLightPosition.y, gLightPosition.z);
    glUniform3f(lightColorLoc2, gLightColor2.r, gLightColor2.g, gLightColor2.b);
    glUniform3f(lightPositionLoc2, gLightPosition2.x, gLightPosition2.y, gLightPosition2.z);
    glUniform3f(lightDirection, gLightDirection.x, gLightDirection.y, gLightDirection.z);
    const glm::vec3 cameraPosition = gCamera.Position;
    glUniform3f(viewPositionLoc, cameraPosition.x, cameraPosition.y, cameraPosition.z);

    GLint UVScaleLoc = glGetUniformLocation(gProgramId, "uvScale");
    glUniform2fv(UVScaleLoc, 1, glm::value_ptr(gUVScale));
    
    //Draws Blade Body
    glBindVertexArray(gMesh.vao);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, gTextureIdBlack);
    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, gTextureIdBlackspec);

    model = glm::mat4(glm::translate(gPosition) * glm::scale(gScale) * glm::rotate(90.0f, glm::vec3(0.0f, 1.0f, 0.0f)));
    model = glm::translate(model, bladePosition);
    glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
    //// Draws the triangles
    glDrawArrays(GL_TRIANGLES, 0, 36);

    //Draws Blade
    glBindVertexArray(blade.vao);
    

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, gTextureIdMetal);
    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, gTextureIdMetalspec);
    model = glm::mat4(glm::translate(gPosition) * glm::scale(gScale) * glm::rotate(90.0f, glm::vec3(0.0f, 1.0f, 0.0f)));
    model = glm::translate(model, bladePosition);
    
    glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
    glDrawArrays(GL_TRIANGLES, 0, 36);

    // Draws Table
    glBindVertexArray(table.vao);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, gTextureIdWood);
    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, gTextureIdWoodspec);
    model = glm::mat4(glm::translate(gPosition) * glm::scale(gScale));
    model = glm::translate(model, gPosition);
    glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
    glDrawArrays(GL_TRIANGLES, 0, table.nVertices);
 

    //Draws Flashlight
    glBindVertexArray(cylinder.vao);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, gTextureIdMetal);
    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, gTextureIdMetalspec);
    model = glm::mat4(1.0f); // make sure to initialize matrix to identity matrix first		
    model = glm::translate(model, glm::vec3(2.5f, 0.0f, -0.2f));
    glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
    model = glm::translate(model, glm::vec3(0.0f, 0.45f, 0.0f));
    glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
    static_meshes_3D::Cylinder C(.4, 20, .85, true, true, true);
    C.render();
    model = glm::translate(model, glm::vec3(0.0f, 0.75f, 0.0f));
    glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
    static_meshes_3D::Cylinder D(.35, 20, .70, true, true, true);
    D.render();
    model = glm::translate(model, glm::vec3(0.0f, 0.0f, 0.0f));
    glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
    static_meshes_3D::Cylinder E(.4, 20, .35, true, true, true);
    E.render();
    model = glm::translate(model, glm::vec3(0.0f, 1.0f, 0.0f));
    glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
    static_meshes_3D::Cylinder F(.3, 20, 2, true, true, true);
    F.render();
    model = glm::translate(model, glm::vec3(0.0f, 0.75f, 0.0f));
    glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
    static_meshes_3D::Cylinder G(.35, 20, .70, true, true, true);
    G.render();
    
    //Draws Watch
    glBindVertexArray(watchface.vao);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, gTextureIdCarbonfiber);
    model = glm::mat4(1.0f); // make sure to initialize matrix to identity matrix first		
    model = glm::translate(model, glm::vec3(-1.0f, 0.15f, -2.5f));
    glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
    
    static_meshes_3D::Cylinder I(.6, 20, 0.3, true, true, true);
    I.render();
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, gTextureIdWatchface);
    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, gTextureIdWatchfacespec);
    model = glm::translate(model, glm::vec3(0.0f, 0.15f, 0.0f));
    glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
    static_meshes_3D::Cylinder J(.6, 20, 0.01, true, true, true);
    J.render();

    glBindVertexArray(watchband.vao);
    model = glm::mat4(1.0f); // make sure to initialize matrix to identity matrix first		
    model = glm::translate(model, glm::vec3(-0.8f, 0.0f, -2.5f));
    glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, gTextureIdWatchband);
    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, gTextureIdWatchbandspec);
    glDrawArrays(GL_TRIANGLES, 0, 72);

    //Draws Charger
    glBindVertexArray(charger.vao);
    model = glm::mat4(1.0f); // make sure to initialize matrix to identity matrix first		
    model = glm::mat4(glm::translate(glm::vec3(3.0f, 0.2f, -4.0f)) * glm::scale(glm::vec3(1.0f, 0.53f, 1.0f)) * glm::rotate(1.57f, glm::vec3(1.0f, 0.0f, 0.0f)));
    glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
    static_meshes_3D::Torus H(20, 20, 1.2, .4, true, true, true);
    H.render();

    glBindVertexArray(chargercenter.vao);
    model = glm::mat4(1.0f); // make sure to initialize matrix to identity matrix first		
    model = glm::translate(model, glm::vec3(3.0f, 0.20f, -4.0f));
    glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
    static_meshes_3D::Cylinder K(1.4, 20, 0.4, true, true, true);
    K.render();

    glBindVertexArray(chargercable.vao);
    model = glm::mat4(1.0f); // make sure to initialize matrix to identity matrix first		
    model = glm::translate(model, glm::vec3(3.0f, 0.0f, -4.0f));
    glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, gTextureIdGrayconnector);
    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, gTextureIdGrayconnectorspec);
    glDrawArrays(GL_TRIANGLES, 0, 24);

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, gTextureIdWatchband);
    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, gTextureIdWatchbandspec);
    glDrawArrays(GL_TRIANGLES, 24, 72);

    //Draws Booklet
    glBindVertexArray(booklet.vao);
    model = glm::mat4(1.0f); // make sure to initialize matrix to identity matrix first	
    model = glm::mat4(glm::translate(glm::vec3(4.5f, 0.0f, 1.5f)) * glm::rotate(0.610f, glm::vec3(0.0f, -1.0f, 0.0f)));
    glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, gTextureIdBooklet);
    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, gTextureIdBookletspec);
    glDrawArrays(GL_TRIANGLES, 0, 36);


    // LAMP: draw lamp
//----------------
    glUseProgram(gLampProgramId);

    //Transform the smaller cube used as a visual que for the light source
    model = glm::translate(gLightPosition) * glm::scale(gLightScale);
   
    // Reference matrix uniforms from the Lamp Shader program
    modelLoc = glGetUniformLocation(gLampProgramId, "model");
    viewLoc = glGetUniformLocation(gLampProgramId, "view");
    projLoc = glGetUniformLocation(gLampProgramId, "projection");

    // Pass matrix data to the Lamp Shader program's matrix uniforms
    glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
    glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));
    glUniformMatrix4fv(projLoc, 1, GL_FALSE, glm::value_ptr(projection));

    //glDrawArrays(GL_TRIANGLES, 0, table.nVertices);


    //Transform the smaller cube used as a visual que for the light source
    model = glm::translate(gLightPosition2) * glm::scale(gLightScale2);

    // Reference matrix uniforms from the Lamp Shader program
    modelLoc = glGetUniformLocation(gLampProgramId, "model");
    viewLoc = glGetUniformLocation(gLampProgramId, "view");
    projLoc = glGetUniformLocation(gLampProgramId, "projection");
    lightDirection = glGetUniformLocation(gProgramId, "lightDirection");

    // Pass matrix data to the Lamp Shader program's matrix uniforms
    glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
    glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));
    glUniformMatrix4fv(projLoc, 1, GL_FALSE, glm::value_ptr(projection));

    //glDrawArrays(GL_TRIANGLES, 0, table.nVertices);

    // Turn On and Off wireframe mode: GL_LINE for wireframe, GL_FILL for polygon
    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

    // Deactivate the Vertex Array Object
    glBindVertexArray(0);

    // glfw: swap buffers and poll IO events (keys pressed/released, mouse moved etc.)
    glfwSwapBuffers(gWindow);    // Flips the the back buffer with the front buffer every frame.
}


// Implements the UCreateMesh function
void UCreateMesh(GLMesh& mesh)
{
    // Vertex data
    GLfloat verts[] = {
        //Positions              //Normals           //Texture Coordinates

        //Base Box (Outer Knife)
         //Back
         0.5f, 0.5f, -0.25f,     1.0f, 0.0f, 0.0f,          1.0f, 1.0f,
         0.5f, 0.5f, 0.0f,       1.0f, 0.0f, 0.0f,          0.0f, 1.0f,
         0.5f, 0.0f, 0.0f,       1.0f, 0.0f, 0.0f,          0.0f, 0.0f,

         0.5f, 0.0f, 0.0f,       1.0f, 0.0f, 0.0f,          0.0f, 0.0f,
         0.5f, 0.0f, -0.25f,     1.0f, 0.0f, 0.0f,          1.0f, 0.0f,
         0.5f, 0.5f, -0.25f,     1.0f, 0.0f, 0.0f,          1.0f, 1.0f,
         //Top
          0.5f, 0.5f, -0.25f,     0.0f, 1.0f, 0.0f,         1.0f, 1.0f,
         -0.9f, 0.5f, -0.25f,     0.0f, 1.0f, 0.0f,         0.0f, 1.0f,
          0.5f, 0.5f, 0.0f,       0.0f, 1.0f, 0.0f,         1.0f, 0.0f,

         -0.9f, 0.5f, -0.25f,     0.0f, 1.0f, 0.0f,         0.0f, 1.0f,
          0.5f, 0.5f, 0.0f,       0.0f, 1.0f, 0.0f,         1.0f, 0.0f,
         -0.9f, 0.5f, 0.0f,       0.0f, 1.0f, 0.0f,         1.0f, 1.0f,
         //Front
         -0.9f, 0.5f, -0.25f,     -1.0f, 0.0f, 0.0f,        0.0f, 1.0f,
         -0.9f, 0.5f, 0.0f,       -1.0f, 0.0f, 0.0f,        1.0f, 1.0f,
         -0.9f, 0.0f, 0.0f,       -1.0f, 0.0f, 0.0f,        1.0f, 0.3f,

         -0.9f, 0.0f, 0.0f,       -1.0f, 0.0f, 0.0f,        1.0f, 0.3f,
         -0.9f, 0.0f, -0.25f,     -1.0f, 0.0f, 0.0f,        0.2f, 0.3f,
         -0.9f, 0.5f, -0.25f,     -1.0f, 0.0f, 0.0f,        0.0f, 1.0f,
         //Bottom
          -0.9f, 0.0f, 0.0f,      0.0f, -1.0f, 0.0f,        1.0f, 1.0f,
          -0.9f, 0.0f, -0.25f,    0.0f, -1.0f, 0.0f,        0.0f, 1.0f,
          0.5f, 0.0f, -0.25f,     0.0f, -1.0f, 0.0f,        0.0f, 0.0f,

          -0.9f, 0.0f, 0.0f,      0.0f, -1.0f, 0.0f,        1.0f, 1.0f,
          0.5f, 0.0f, -0.25f,     0.0f, -1.0f, 0.0f,        0.0f, 0.0f,
          0.5f, 0.0f, 0.0f,       0.0f, -1.0f, 0.0f,        1.0f, 0.0f,

          //Back side

          0.5f, 0.0f, -0.25f,     0.0f, 0.0f, -1.0f,        0.0f, 0.0f,
          -0.9f, 0.0f, -0.25f,    0.0f, 0.0f, -1.0f,        1.0f, 0.0f,
          0.5f, 0.5f, -0.25f,     0.0f, 0.0f, -1.0f,        0.0, 1.0f,

          -0.9f, 0.0f, -0.25f,    0.0f, 0.0f, -1.0f,        1.0f, 0.0f,
          -0.9f, 0.5f, -0.25f,    0.0f, 0.0f, -1.0f,        1.0f, 1.0f,
           0.5f, 0.5f, -0.25f,    0.0f, 0.0f, -1.0f,        0.0, 1.0f,

           //Front side
           -0.9f, 0.0f, 0.0f,     0.0f, 0.0f, 1.0f,         0.0f, 0.0f,
           -0.9f, 0.5f, 0.0f,     0.0f, 0.0f, 1.0f,         0.0f, 1.0f,
           0.5f, 0.5f, 0.0f,      0.0f, 0.0f, 1.0f,         1.0f, 1.0f,

           0.5f, 0.5f, 0.0f,      0.0f, 0.0f, 1.0f,         1.0f, 1.0f,
           0.5f, 0.0f, 0.0f,      0.0f, 0.0f, 1.0f,         1.0f, 0.0f,
           -0.9f, 0.0f, 0.0f,     0.0f, 0.0f, 1.0f,         0.0f, 0.0f,



    };
    GLfloat tableVerts[] = {
        // Table Plane
        //Positions              //Normals           //Texture Coordinates
        5.0f, 0.0f, 5.0f,     0.0f, 1.0f,  0.0f,         1.0f, 0.0f,
        5.0f, 0.0f, -5.0f,    0.0f, 1.0f,  0.0f,         1.0f, 1.0f,
       -5.0f, 0.0f, -5.0f,    0.0f, 1.0f,  0.0f,         0.0f, 1.0f,

       -5.0f, 0.0f, 5.0f,     0.0f, 1.0f,  0.0f,         0.0f, 0.0f,
       -5.0f, 0.0f, -5.0f,    0.0f, 1.0f,  0.0f,         0.0f, 1.0f,
        5.0f, 0.0f, 5.0f,     0.0f, 1.0f,  0.0f,         1.0f, 0.0f,
    };

    const GLuint floatsPerVertex = 3;
    const GLuint floatsPerNormal = 3;
    const GLuint floatsPerUV = 2;

    //table VAO
    unsigned int tableVAO, tableVBO;
    table.nVertices = sizeof(tableVerts) / (sizeof(tableVerts[0]) * (floatsPerVertex + floatsPerNormal + floatsPerUV));
    glGenVertexArrays(1, &table.vao);
    glGenBuffers(1, &table.vbo);
    glBindVertexArray(table.vao);
    glBindBuffer(GL_ARRAY_BUFFER, table.vbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(tableVerts), &tableVerts, GL_STATIC_DRAW);
    // Strides between vertex coordinates is 6 (x, y, z, r, g, b, a). A tightly packed stride is 0.
    GLint stride = sizeof(float) * (floatsPerVertex + floatsPerNormal + floatsPerUV);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, floatsPerVertex, GL_FLOAT, GL_FALSE, stride, 0);
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, floatsPerNormal, GL_FLOAT, GL_FALSE, stride, (void*)(sizeof(float) * floatsPerVertex));
    glEnableVertexAttribArray(2);
    glVertexAttribPointer(2, floatsPerUV, GL_FLOAT, GL_FALSE, stride, (void*)(sizeof(float) * (floatsPerVertex + floatsPerNormal)));

    GLfloat bladeVerts[] = {
        // Inner Box (Knife Blade)
        //Positions              //Normals           //Texture Coordinates
        //Back
        0.5f,  0.55f, -0.20f,     1.0f, 0.0f, 0.0f,     1.0f, 1.0f,
        0.5f,  0.55f, -0.05f,     1.0f, 0.0f, 0.0f,     0.0f, 1.0f,
        0.5f,  0.25f, -0.05f,     1.0f, 0.0f, 0.0f,     0.0f, 0.0f,

        0.5f,  0.55f, -0.20f,     1.0f, 0.0f, 0.0f,     1.0f, 1.0f,
        0.5f, 0.25f, -0.20f,      1.0f, 0.0f, 0.0f,     1.0f, 0.0f,
        0.5f,  0.25f, -0.05f,     1.0f, 0.0f, 0.0f,     0.0f, 0.0f,



        //Front
        -0.35f,  0.65f, -0.20f, -1.0f, 0.0f, 0.0f,      0.0f, 1.0f,
        -0.35f,  0.65f, -0.05f, -1.0f, 0.0f, 0.0f,      1.0f, 1.0f,
        -0.85f,  0.25f, -0.05f, -1.0f, 0.0f, 0.0f,      1.0f, 0.0f,

        -0.85f,  0.25f, -0.05f, -1.0f, 0.0f, 0.0f,      1.0f, 0.0f,
        -0.85f, 0.25f, -0.20f,  -1.0f, 0.0f, 0.0f,      0.0f, 0.0f,
        -0.35f,  0.65f, -0.20f, -1.0f, 0.0f, 0.0f,      0.0f, 1.0f,

        //Bottom
        -0.85f,  0.25f, -0.05f,  0.0f, -1.0f, 0.0f,     0.3f, 0.3f,
        -0.85f, 0.25f, -0.20f,   0.0f, -1.0f, 0.0f,     0.5f, 0.3f,
         0.5f,  0.25f, -0.05f,   0.0f, -1.0f, 0.0f,     1.0f, 0.0f,

        0.5f,  0.25f, -0.05f,    0.0f, -1.0f, 0.0f,     1.0f, 0.0f,
        0.5f, 0.25f, -0.20f,     0.0f, -1.0f, 0.0f,     0.2f, 0.3f,
        -0.85f, 0.25f, -0.20f,   0.0f, -1.0f, 0.0f,     0.5f, 0.3f,

        //Top
        -0.35f,  0.65f, -0.20f,  0.0f, 1.0f, 0.0f,      0.0f, 1.0f,
        -0.35f,  0.65f, -0.05f,  0.0f, 1.0f, 0.0f,      1.0f, 1.0f,
         0.5f,  0.55f, -0.20f,   0.0f, 1.0f, 0.0f,      0.0f, 0.0f,

         0.5f,  0.55f, -0.20f,   0.0f, 1.0f, 0.0f,      0.0f, 0.0f,
         0.5f,  0.55f, -0.05f,   0.0f, 1.0f, 0.0f,      1.0f, 0.0f,
        -0.35f,  0.65f, -0.05f,  0.0f, 1.0f, 0.0f,      1.0f, 1.0f,

        //Back Side
        0.5f, 0.55f, -0.20f,     0.0f, 0.0f, -1.0f,     1.0f, 1.0f,
         0.5f, 0.25f, -0.20f,    0.0f, 0.0f, -1.0f,     1.0f, 0.0f,
        -0.35f, 0.65f, -0.20f,   0.0f, 0.0f, -1.0f,     0.2f, 1.0f,

        -0.35f, 0.65f, -0.20f,   0.0f, 0.0f, -1.0f,     0.2f, 1.0f,
         0.5f, 0.25f, -0.20f,    0.0f, 0.0f, -1.0f,     1.0f, 0.0f,
        -0.85f, 0.25f, -0.20f,   0.0f, 0.0f, -1.0f,     0.0f, 0.0f,

        //Front Side
          0.5f, 0.55f, -0.05f,   0.0f, 0.0f, 1.0f,      1.0f, 1.0f,
          0.5f, 0.25f, -0.05f,   0.0f, 0.0f, 1.0f,      1.0f, 0.0f,
         -0.35f, 0.65f, -0.05f,  0.0f, 0.0f, 1.0f,      0.2f, 1.0f,

         -0.35f, 0.65f, -0.05f,  0.0f, 0.0f, 1.0f,      0.2f, 1.0f,
         -0.85f, 0.25f, -0.05f,  0.0f, 0.0f, 1.0f,      0.0f, 0.0f,
          0.5f, 0.25f, -0.05f,   0.0f, 0.0f, 1.0f,      1.0f, 0.0f,

    };


    //blade VAO
    unsigned int bladeVAO, bladeVBO;
    glGenVertexArrays(1, &blade.vao);
    glGenBuffers(1, &blade.vbo);
    glBindVertexArray(blade.vao);
    glBindBuffer(GL_ARRAY_BUFFER, blade.vbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(bladeVerts), &bladeVerts, GL_STATIC_DRAW);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, floatsPerVertex, GL_FLOAT, GL_FALSE, stride, 0);
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, floatsPerNormal, GL_FLOAT, GL_FALSE, stride, (void*)(sizeof(float) * floatsPerVertex));
    glEnableVertexAttribArray(2);
    glVertexAttribPointer(2, floatsPerUV, GL_FLOAT, GL_FALSE, stride, (void*)(sizeof(float) * (floatsPerVertex + floatsPerNormal)));


    mesh.nVertices = sizeof(verts) / (sizeof(verts[0]) * (floatsPerVertex + floatsPerNormal + floatsPerUV));

    glGenVertexArrays(1, &mesh.vao); // we can also generate multiple VAOs or buffers at the same time
    glBindVertexArray(mesh.vao);

    // Create VBO
    glGenBuffers(1, &mesh.vbo);
    glBindBuffer(GL_ARRAY_BUFFER, mesh.vbo); // Activates the buffer
    glBufferData(GL_ARRAY_BUFFER, sizeof(verts), verts, GL_STATIC_DRAW); // Sends vertex or coordinate data to the GPU

    


    // Create Vertex Attribute Pointers
    glVertexAttribPointer(0, floatsPerVertex, GL_FLOAT, GL_FALSE, stride, 0);
    glEnableVertexAttribArray(0);

    glVertexAttribPointer(1, floatsPerNormal, GL_FLOAT, GL_FALSE, stride, (void*)(sizeof(float) * floatsPerVertex));
    glEnableVertexAttribArray(1);

    glVertexAttribPointer(2, floatsPerUV, GL_FLOAT, GL_FALSE, stride, (void*)(sizeof(float) * (floatsPerVertex + floatsPerNormal)));
    glEnableVertexAttribArray(2);
    
    


    glGenVertexArrays(1, &cylinder.vao);
    glGenBuffers(1, &cylinder.vbo);
    glBindVertexArray(cylinder.vao);
    glBindBuffer(GL_ARRAY_BUFFER, cylinder.vbo);

    glGenVertexArrays(1, &charger.vao);
    glGenBuffers(1, &charger.vbo);
    glBindVertexArray(charger.vao);
    glBindBuffer(GL_ARRAY_BUFFER, charger.vbo);

    glGenVertexArrays(1, &chargercenter.vao);
    glGenBuffers(1, &chargercenter.vbo);
    glBindVertexArray(chargercenter.vao);
    glBindBuffer(GL_ARRAY_BUFFER, chargercenter.vbo);

    glGenVertexArrays(1, &watchface.vao);
    glGenBuffers(1, &watchface.vbo);
    glBindVertexArray(watchface.vao);
    glBindBuffer(GL_ARRAY_BUFFER, watchface.vbo);

    GLfloat watchbandVerts[] = {
        // Band 1 (Longer)
        //Positions              //Normals           //Texture Coordinates
        ////Back
        0.18f,  0.1f, 0.45f, 0.0f, 0.0f, -1.0f,      1.0f, 1.0f,
        0.18f,  0.0f, 0.45, 0.0f, 0.0f, -1.0f,      1.0f, 0.0f,
       -0.58f,  0.0f, 0.45f, 0.0f, 0.0f, -1.0f,      0.0f, 0.0f,

        -0.58f,  0.0f, 0.45f, 0.0f, 0.0f, -1.0f,      0.0f, 0.0f,
        -0.58f, 0.1f, 0.45f,  0.0f, 0.0f, -1.0f,      0.0f, 1.0f,
         0.18f,  0.1f, 0.45f, 0.0f, 0.0f, -1.0f,      1.0f, 1.0f,



        ////Front
        0.18f,  0.1f, 4.0f, 0.0f, 0.0f, 1.0f,      1.0f, 1.0f,
        0.18f,  0.0f, 4.0f, 0.0f, 0.0f, 1.0f,      1.0f, 0.0f,
       -0.58f,  0.0f, 4.0f, 0.0f, 0.0f, 1.0f,      0.0f, 0.0f,

        -0.58f,  0.0f, 4.0f, 0.0f, 0.0f, 1.0f,      0.0f, 0.0f,
        -0.58f, 0.1f, 4.0f,  0.0f, 0.0f, 1.0f,      0.0f, 1.0f,
         0.18f,  0.1f, 4.0f, 0.0f, 0.0f, 1.0f,      1.0f, 1.0f,

        //Bottom
         0.18f,  0.0f, 0.45f,  0.0f, -1.0f, 0.0f,      1.0f, 1.0f,
        -0.58f,  0.0f, 0.45f,  0.0f, -1.0f, 0.0f,      0.0f, 1.0f,
         0.18f,  0.0f, 4.0f,   0.0f, -1.0f, 0.0f,      1.0f, 0.0f,

         0.18f,  0.0f, 4.0f,   0.0f, -1.0f, 0.0f,      1.0f, 0.0f,
        -0.58f,  0.0f, 4.0f,   0.0f, -1.0f, 0.0f,      0.0f, 0.0f,
        -0.58f,  0.0f, 0.45f,  0.0f, -1.0f, 0.0f,      0.0f, 1.0f,

        //Top
         0.18f,  0.1f, 0.45f,  0.0f, 1.0f, 0.0f,      1.0f, 1.0f,
        -0.58f,  0.1f, 0.45f,  0.0f, 1.0f, 0.0f,      0.0f, 1.0f,
         0.18f,  0.1f, 4.0f,   0.0f, 1.0f, 0.0f,      1.0f, 0.0f,

         0.18f,  0.1f, 4.0f,   0.0f, 1.0f, 0.0f,      1.0f, 0.0f,
        -0.58f,  0.1f, 4.0f,   0.0f, 1.0f, 0.0f,      0.0f, 0.0f,
        -0.58f,  0.1f, 0.45f,  0.0f, 1.0f, 0.0f,      0.0f, 1.0f,

        //Right Side
        0.18f, 0.1f, 0.45f,     1.0f, 0.0f, 0.0f,     1.0f, 1.0f,
        0.18f, 0.0f, 0.45f,     1.0f, 0.0f, 0.0f,     1.0f, 0.0f,
        0.18f, 0.1f, 4.0f,      1.0f, 0.0f, 0.0f,     0.0f, 1.0f,

        0.18f, 0.1f, 4.0f,      1.0f, 0.0f, 0.0f,     0.0f, 1.0f,
        0.18f, 0.0f, 0.45f,     1.0f, 0.0f, 0.0f,     1.0f, 0.0f,
        0.18f, 0.0f, 4.0f,      1.0f, 0.0f, 0.0f,     0.0f, 0.0f,

        //Left Side
        -0.58f, 0.1f, 0.45f,     -1.0f, 0.0f, 0.0f,     1.0f, 1.0f,
        -0.58f, 0.0f, 0.45f,     -1.0f, 0.0f, 0.0f,     1.0f, 0.0f,
        -0.58f, 0.1f, 4.0f,      -1.0f, 0.0f, 0.0f,     0.0f, 1.0f,

        -0.58f, 0.1f, 4.0f,      -1.0f, 0.0f, 0.0f,     0.0f, 1.0f,
        -0.58f, 0.0f, 0.45f,     -1.0f, 0.0f, 0.0f,     1.0f, 0.0f,
        -0.58f, 0.0f, 4.0f,      -1.0f, 0.0f, 0.0f,     0.0f, 0.0f,


        // Band 2 (Shorter)
        //Positions              //Normals           //Texture Coordinates
        ////Back
        0.18f,  0.1f, -3.0f, 0.0f, 0.0f, -1.0f,      1.0f, 1.0f,
        0.18f,  0.0f, -3.0, 0.0f, 0.0f, -1.0f,      1.0f, 0.0f,
       -0.58f,  0.0f, -3.0f, 0.0f, 0.0f, -1.0f,      0.0f, 0.0f,

        -0.58f,  0.0f, -3.0f, 0.0f, 0.0f, -1.0f,      0.0f, 0.0f,
        -0.58f, 0.1f, -3.0f,  0.0f, 0.0f, -1.0f,      0.0f, 1.0f,
         0.18f,  0.1f, -3.0f, 0.0f, 0.0f, -1.0f,      1.0f, 1.0f,



         ////Front
         0.18f,  0.1f, -0.45f, 0.0f, 0.0f, 1.0f,      1.0f, 1.0f,
         0.18f,  0.0f, -0.45f, 0.0f, 0.0f, 1.0f,      1.0f, 0.0f,
        -0.58f,  0.0f, -0.45f, 0.0f, 0.0f, 1.0f,      0.0f, 0.0f,

         -0.58f,  0.0f, -0.45f, 0.0f, 0.0f, 1.0f,      0.0f, 0.0f,
         -0.58f, 0.1f, -0.45f,  0.0f, 0.0f, 1.0f,      0.0f, 1.0f,
          0.18f,  0.1f, -0.45f, 0.0f, 0.0f, 1.0f,      1.0f, 1.0f,

          //Bottom
           0.18f,  0.0f, -3.0f,  0.0f, -1.0f, 0.0f,      1.0f, 1.0f,
          -0.58f,  0.0f, -3.0f,  0.0f, -1.0f, 0.0f,      0.0f, 1.0f,
           0.18f,  0.0f, -0.45f,   0.0f, -1.0f, 0.0f,      1.0f, 0.0f,

           0.18f,  0.0f, -0.45f,   0.0f, -1.0f, 0.0f,      1.0f, 0.0f,
          -0.58f,  0.0f, -0.45f,   0.0f, -1.0f, 0.0f,      0.0f, 0.0f,
          -0.58f,  0.0f, -3.0f,  0.0f, -1.0f, 0.0f,      0.0f, 1.0f,

          //Top
           0.18f,  0.1f, -3.0f,  0.0f, 1.0f, 0.0f,      1.0f, 1.0f,
          -0.58f,  0.1f, -3.0f,  0.0f, 1.0f, 0.0f,      0.0f, 1.0f,
           0.18f,  0.1f, -0.45f,   0.0f, 1.0f, 0.0f,      1.0f, 0.0f,

           0.18f,  0.1f, -0.45f,   0.0f, 1.0f, 0.0f,      1.0f, 0.0f,
          -0.58f,  0.1f, -0.45f,   0.0f, 1.0f, 0.0f,      0.0f, 0.0f,
          -0.58f,  0.1f, -3.0f,  0.0f, 1.0f, 0.0f,      0.0f, 1.0f,

          //Right Side
          0.18f, 0.1f, -3.0f,     1.0f, 0.0f, 0.0f,     1.0f, 1.0f,
          0.18f, 0.0f, -3.0f,     1.0f, 0.0f, 0.0f,     1.0f, 0.0f,
          0.18f, 0.1f, -0.45f,      1.0f, 0.0f, 0.0f,     0.0f, 1.0f,

          0.18f, 0.1f, -0.45f,      1.0f, 0.0f, 0.0f,     0.0f, 1.0f,
          0.18f, 0.0f, -3.0f,     1.0f, 0.0f, 0.0f,     1.0f, 0.0f,
          0.18f, 0.0f, -0.45f,      1.0f, 0.0f, 0.0f,     0.0f, 0.0f,

          //Left Side
          -0.58f, 0.1f, -3.0f,     -1.0f, 0.0f, 0.0f,     1.0f, 1.0f,
          -0.58f, 0.0f, -3.0f,     -1.0f, 0.0f, 0.0f,     1.0f, 0.0f,
          -0.58f, 0.1f, -0.45f,      -1.0f, 0.0f, 0.0f,     0.0f, 1.0f,

          -0.58f, 0.1f, -0.45f,      -1.0f, 0.0f, 0.0f,     0.0f, 1.0f,
          -0.58f, 0.0f, -3.0f,     -1.0f, 0.0f, 0.0f,     1.0f, 0.0f,
          -0.58f, 0.0f, -0.45f,      -1.0f, 0.0f, 0.0f,     0.0f, 0.0f,
    };
    glGenVertexArrays(1, &watchband.vao);
    glGenBuffers(1, &watchband.vbo);
    glBindVertexArray(watchband.vao);
    glBindBuffer(GL_ARRAY_BUFFER, watchband.vbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(watchbandVerts), &watchbandVerts, GL_STATIC_DRAW);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, floatsPerVertex, GL_FLOAT, GL_FALSE, stride, 0);
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, floatsPerNormal, GL_FLOAT, GL_FALSE, stride, (void*)(sizeof(float)* floatsPerVertex));
    glEnableVertexAttribArray(2);
    glVertexAttribPointer(2, floatsPerUV, GL_FLOAT, GL_FALSE, stride, (void*)(sizeof(float)* (floatsPerVertex + floatsPerNormal)));

    GLfloat chargercableVerts[] = {
        // Charger Connector Gray Area
        //Positions              //Normals           //Texture Coordinates


           //Bottom
            0.18f,  0.01f, -2.0f,  0.0f, -1.0f, 0.0f,      1.0f, 1.0f,
           -0.18f,  0.01f, -2.0f,  0.0f, -1.0f, 0.0f,      0.0f, 1.0f,
            0.18f,  0.01f, -1.0f,   0.0f, -1.0f, 0.0f,      1.0f, 0.0f,

            0.18f,  0.01f, -1.0f,   0.0f, -1.0f, 0.0f,      1.0f, 0.0f,
           -0.18f,  0.01f, -1.0f,   0.0f, -1.0f, 0.0f,      0.0f, 0.0f,
           -0.18f,  0.01f, -2.0f,  0.0f, -1.0f, 0.0f,      0.0f, 1.0f,

           //Top
            0.18f,  0.1f, -2.0f,  0.0f, 1.0f, 0.0f,      1.0f, 1.0f,
           -0.18f,  0.1f, -2.0f,  0.0f, 1.0f, 0.0f,      0.0f, 1.0f,
            0.18f,  0.1f, -1.0f,   0.0f, 1.0f, 0.0f,      1.0f, 0.0f,

            0.18f,  0.1f, -1.0f,   0.0f, 1.0f, 0.0f,      1.0f, 0.0f,
           -0.18f,  0.1f, -1.0f,   0.0f, 1.0f, 0.0f,      0.0f, 0.0f,
           -0.18f,  0.1f, -2.0f,  0.0f, 1.0f, 0.0f,      0.0f, 1.0f,

           //Right Side
           0.18f, 0.1f, -2.0f,     1.0f, 0.0f, 0.0f,     1.0f, 1.0f,
           0.18f, 0.01f, -2.0f,     1.0f, 0.0f, 0.0f,     1.0f, 0.0f,
           0.18f, 0.1f, -1.0f,      1.0f, 0.0f, 0.0f,     0.0f, 1.0f,

           0.18f, 0.1f, -1.0f,      1.0f, 0.0f, 0.0f,     0.0f, 1.0f,
           0.18f, 0.01f, -2.0f,     1.0f, 0.0f, 0.0f,     1.0f, 0.0f,
           0.18f, 0.01f, -1.0f,      1.0f, 0.0f, 0.0f,     0.0f, 0.0f,

           //Left Side
           -0.18f, 0.1f, -2.0f,     -1.0f, 0.0f, 0.0f,     1.0f, 1.0f,
           -0.18f, 0.01f, -2.0f,     -1.0f, 0.0f, 0.0f,     1.0f, 0.0f,
           -0.18f, 0.1f, -1.0f,      -1.0f, 0.0f, 0.0f,     0.0f, 1.0f,

           -0.18f, 0.1f, -1.0f,      -1.0f, 0.0f, 0.0f,     0.0f, 1.0f,
           -0.18f, 0.01f, -2.0f,     -1.0f, 0.0f, 0.0f,     1.0f, 0.0f,
           -0.18f, 0.01f, -1.0f,      -1.0f, 0.0f, 0.0f,     0.0f, 0.0f,


           // Charger Connector Black Area
        //Positions              //Normals           //Texture Coordinates
        //Back
        0.18f,  0.1f, -2.3f, 0.0f, 0.0f, -1.0f,      1.0f, 1.0f,
        0.18f,  0.01f, -2.3, 0.0f, 0.0f, -1.0f,      1.0f, 0.0f,
        -0.18f,  0.01f, -2.3f, 0.0f, 0.0f, -1.0f,      0.0f, 0.0f,

         -0.18f,  0.01f, -2.3f, 0.0f, 0.0f, -1.0f,      0.0f, 0.0f,
         -0.18f, 0.1f, -2.3f,  0.0f, 0.0f, -1.0f,      0.0f, 1.0f,
          0.18f,  0.1f, -2.3f, 0.0f, 0.0f, -1.0f,      1.0f, 1.0f,




           //Bottom
            0.18f,  0.01f, -2.3f,  0.0f, -1.0f, 0.0f,      1.0f, 1.0f,
           -0.18f,  0.01f, -2.3f,  0.0f, -1.0f, 0.0f,      0.0f, 1.0f,
            0.18f,  0.01f, -2.0f,   0.0f, -1.0f, 0.0f,      1.0f, 0.0f,

            0.18f,  0.01f, -2.0f,   0.0f, -1.0f, 0.0f,      1.0f, 0.0f,
           -0.18f,  0.01f, -2.0f,   0.0f, -1.0f, 0.0f,      0.0f, 0.0f,
           -0.18f,  0.01f, -2.3f,  0.0f, -1.0f, 0.0f,      0.0f, 1.0f,

           //Top
            0.18f,  0.1f, -2.3f,  0.0f, 1.0f, 0.0f,      1.0f, 1.0f,
           -0.18f,  0.1f, -2.3f,  0.0f, 1.0f, 0.0f,      0.0f, 1.0f,
            0.18f,  0.1f, -2.0f,   0.0f, 1.0f, 0.0f,      1.0f, 0.0f,

            0.18f,  0.1f, -2.0f,   0.0f, 1.0f, 0.0f,      1.0f, 0.0f,
           -0.18f,  0.1f, -2.0f,   0.0f, 1.0f, 0.0f,      0.0f, 0.0f,
           -0.18f,  0.1f, -2.3f,  0.0f, 1.0f, 0.0f,      0.0f, 1.0f,

           //Right Side
           0.18f, 0.1f, -2.3f,     1.0f, 0.0f, 0.0f,     1.0f, 1.0f,
           0.18f, 0.01f, -2.3f,     1.0f, 0.0f, 0.0f,     1.0f, 0.0f,
           0.18f, 0.1f, -2.0f,      1.0f, 0.0f, 0.0f,     0.0f, 1.0f,

           0.18f, 0.1f, -2.0f,      1.0f, 0.0f, 0.0f,     0.0f, 1.0f,
           0.18f, 0.01f, -2.3f,     1.0f, 0.0f, 0.0f,     1.0f, 0.0f,
           0.18f, 0.01f, -2.0f,      1.0f, 0.0f, 0.0f,     0.0f, 0.0f,

           //Left Side
           -0.18f, 0.1f, -2.3f,     -1.0f, 0.0f, 0.0f,     1.0f, 1.0f,
           -0.18f, 0.01f, -2.3f,     -1.0f, 0.0f, 0.0f,     1.0f, 0.0f,
           -0.18f, 0.1f, -2.0f,      -1.0f, 0.0f, 0.0f,     0.0f, 1.0f,

           -0.18f, 0.1f, -2.0f,      -1.0f, 0.0f, 0.0f,     0.0f, 1.0f,
           -0.18f, 0.01f, -2.3f,     -1.0f, 0.0f, 0.0f,     1.0f, 0.0f,
           -0.18f, 0.01f, -2.0f,      -1.0f, 0.0f, 0.0f,     0.0f, 0.0f,


                // Charger Connector Cable
            //Positions              //Normals           //Texture Coordinates
            ////Back
            0.15f, 0.07f, -8.0f, 0.0f, 0.0f, -1.0f, 1.0f, 1.0f,
            0.15f, 0.015f, -8.0, 0.0f, 0.0f, -1.0f, 1.0f, 0.0f,
            -0.15f, 0.015f, -8.0f, 0.0f, 0.0f, -1.0f, 0.0f, 0.0f,

            -0.15f, 0.015f, -8.0f, 0.0f, 0.0f, -1.0f, 0.0f, 0.0f,
            -0.15f, 0.07f, -8.0f, 0.0f, 0.0f, -1.0f, 0.0f, 1.0f,
            0.15f, 0.07f, -8.0f, 0.0f, 0.0f, -1.0f, 1.0f, 1.0f,



                //Bottom
                0.15f, 0.015f, -8.0f, 0.0f, -1.0f, 0.0f, 1.0f, 1.0f,
                -0.15f, 0.015f, -8.0f, 0.0f, -1.0f, 0.0f, 0.0f, 1.0f,
                0.15f, 0.015f, -2.3f, 0.0f, -1.0f, 0.0f, 1.0f, 0.0f,

                0.15f, 0.015f, -2.3f, 0.0f, -1.0f, 0.0f, 1.0f, 0.0f,
                -0.15f, 0.015f, -2.3f, 0.0f, -1.0f, 0.0f, 0.0f, 0.0f,
                -0.15f, 0.015f, -8.0f, 0.0f, -1.0f, 0.0f, 0.0f, 1.0f,

                //Top
                0.15f, 0.07f, -8.0f, 0.0f, 1.0f, 0.0f, 1.0f, 1.0f,
                -0.15f, 0.07f, -8.0f, 0.0f, 1.0f, 0.0f, 0.0f, 1.0f,
                0.15f, 0.07f, -2.3f, 0.0f, 1.0f, 0.0f, 1.0f, 0.0f,

                0.15f, 0.07f, -2.3f, 0.0f, 1.0f, 0.0f, 1.0f, 0.0f,
                -0.15f, 0.07f, -2.3f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f,
                -0.15f, 0.07f, -8.0f, 0.0f, 1.0f, 0.0f, 0.0f, 1.0f,

                //Right Side
                0.15f, 0.07f, -8.0f, 1.0f, 0.0f, 0.0f, 1.0f, 1.0f,
                0.15f, 0.015f, -8.0f, 1.0f, 0.0f, 0.0f, 1.0f, 0.0f,
                0.15f, 0.07f, -2.3f, 1.0f, 0.0f, 0.0f, 0.0f, 1.0f,

                0.15f, 0.07f, -2.3f, 1.0f, 0.0f, 0.0f, 0.0f, 1.0f,
                0.15f, 0.015f, -8.0f, 1.0f, 0.0f, 0.0f, 1.0f, 0.0f,
                0.15f, 0.015f, -2.3f, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f,

                //Left Side
                -0.15f, 0.07f, -8.0f, -1.0f, 0.0f, 0.0f, 1.0f, 1.0f,
                -0.15f, 0.015f, -8.0f, -1.0f, 0.0f, 0.0f, 1.0f, 0.0f,
                -0.15f, 0.07f, -2.3f, -1.0f, 0.0f, 0.0f, 0.0f, 1.0f,

                -0.15f, 0.07f, -2.3f, -1.0f, 0.0f, 0.0f, 0.0f, 1.0f,
                -0.15f, 0.015f, -8.0f, -1.0f, 0.0f, 0.0f, 1.0f, 0.0f,
                -0.15f, 0.015f, -2.3f, -1.0f, 0.0f, 0.0f, 0.0f, 0.0f,
    };
    glGenVertexArrays(1, &chargercable.vao);
    glGenBuffers(1, &chargercable.vbo);
    glBindVertexArray(chargercable.vao);
    glBindBuffer(GL_ARRAY_BUFFER, chargercable.vbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(chargercableVerts), &chargercableVerts, GL_STATIC_DRAW);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, floatsPerVertex, GL_FLOAT, GL_FALSE, stride, 0);
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, floatsPerNormal, GL_FLOAT, GL_FALSE, stride, (void*)(sizeof(float)* floatsPerVertex));
    glEnableVertexAttribArray(2);
    glVertexAttribPointer(2, floatsPerUV, GL_FLOAT, GL_FALSE, stride, (void*)(sizeof(float)* (floatsPerVertex + floatsPerNormal)));

    GLfloat bookletVerts[] = {
        //Positions              //Normals           //Texture Coordinates
        //Back
        0.9f, 0.05f, -1.8f, 0.0f, 0.0f, -1.0f, 1.0f, 1.0f,
        0.9f, 0.0f, -1.8, 0.0f, 0.0f, -1.0f, 1.0f, 0.0f,
        -0.9f, 0.0f, -1.8f, 0.0f, 0.0f, -1.0f, 0.0f, 0.0f,
        
        -0.9f, 0.0f, -1.8f, 0.0f, 0.0f, -1.0f, 0.0f, 0.0f,
        -0.9f, 0.05f, -1.8f, 0.0f, 0.0f, -1.0f, 0.0f, 1.0f,
        0.9f, 0.05f, -1.8f, 0.0f, 0.0f, -1.0f, 1.0f, 1.0f,



          ////Front
          0.9f,  0.05f, 1.8f, 0.0f, 0.0f, 1.0f,      1.0f, 1.0f,
          0.9f,  0.0f, 1.8f, 0.0f, 0.0f, 1.0f,      1.0f, 0.0f,
         -0.9f,  0.0f, 1.8f, 0.0f, 0.0f, 1.0f,      0.0f, 0.0f,

          -0.9f,  0.0f, 1.8f, 0.0f, 0.0f, 1.0f,      0.0f, 0.0f,
          -0.9f, 0.05f, 1.8f,  0.0f, 0.0f, 1.0f,      0.0f, 1.0f,
           0.9f,  0.05f, 1.8f, 0.0f, 0.0f, 1.0f,      1.0f, 1.0f,

        //Bottom
        0.9f, 0.0f, -1.8f, 0.0f, -1.0f, 0.0f, 1.0f, 1.0f,
        -0.9f, 0.0f, -1.8f, 0.0f, -1.0f, 0.0f, 0.0f, 1.0f,
        0.9f, 0.0f, -1.8f, 0.0f, -1.0f, 0.0f, 1.0f, 0.0f,

        0.9f, 0.0f, -1.8f, 0.0f, -1.0f, 0.0f, 1.0f, 0.0f,
        -0.9f, 0.0f, -1.8f, 0.0f, -1.0f, 0.0f, 0.0f, 0.0f,
        -0.9f, 0.0f, -1.8f, 0.0f, -1.0f, 0.0f, 0.0f, 1.0f,

        //Top
        0.9f, 0.05f, -1.8f, 0.0f, 1.0f, 0.0f, 1.0f, 1.0f,
        -0.9f, 0.05f, -1.8f, 0.0f, 1.0f, 0.0f, 0.0f, 1.0f,
        0.9f, 0.05f, 1.8f, 0.0f, 1.0f, 0.0f, 1.0f, 0.0f,

        0.9f, 0.05f, 1.8f, 0.0f, 1.0f, 0.0f, 1.0f, 0.0f,
        -0.9f, 0.05f, 1.8f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f,
        -0.9f, 0.05f, -1.8f, 0.0f, 1.0f, 0.0f, 0.0f, 1.0f,

        //Right Side
        0.9f, 0.05f, -1.8f, 1.0f, 0.0f, 0.0f, 1.0f, 1.0f,
        0.9f, 0.0f, -1.8f, 1.0f, 0.0f, 0.0f, 1.0f, 0.0f,
        0.9f, 0.05f, 1.8f, 1.0f, 0.0f, 0.0f, 0.0f, 1.0f,

        0.9f, 0.05f, 1.8f, 1.0f, 0.0f, 0.0f, 0.0f, 1.0f,
        0.9f, 0.0f, -1.8f, 1.0f, 0.0f, 0.0f, 1.0f, 0.0f,
        0.9f, 0.0f, 1.8f, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f,

        //Left Side
        -0.9f, 0.05f, -1.8f, -1.0f, 0.0f, 0.0f, 1.0f, 1.0f,
        -0.9f, 0.0f, -1.8f, -1.0f, 0.0f, 0.0f, 1.0f, 0.0f,
        -0.9f, 0.05f, 1.8f, -1.0f, 0.0f, 0.0f, 0.0f, 1.0f,

        -0.9f, 0.05f, 1.8f, -1.0f, 0.0f, 0.0f, 0.0f, 1.0f,
        -0.9f, 0.0f, -1.8f, -1.0f, 0.0f, 0.0f, 1.0f, 0.0f,
        -0.9f, 0.0f, 1.8f, -1.0f, 0.0f, 0.0f, 0.0f, 0.0f,
    };

    glGenVertexArrays(1, &booklet.vao);
    glGenBuffers(1, &booklet.vbo);
    glBindVertexArray(booklet.vao);
    glBindBuffer(GL_ARRAY_BUFFER, booklet.vbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(bookletVerts), &bookletVerts, GL_STATIC_DRAW);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, floatsPerVertex, GL_FLOAT, GL_FALSE, stride, 0);
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, floatsPerNormal, GL_FLOAT, GL_FALSE, stride, (void*)(sizeof(float)* floatsPerVertex));
    glEnableVertexAttribArray(2);
    glVertexAttribPointer(2, floatsPerUV, GL_FLOAT, GL_FALSE, stride, (void*)(sizeof(float)* (floatsPerVertex + floatsPerNormal)));

}


void UDestroyMesh(GLMesh& mesh)
{
    glDeleteVertexArrays(1, &mesh.vao);
    glDeleteBuffers(1, &mesh.vbo);
    glDeleteVertexArrays(1, &table.vao);
    glDeleteBuffers(1, &table.vbo);
    glDeleteVertexArrays(1, &blade.vao);
    glDeleteBuffers(1, &blade.vbo);
    glDeleteVertexArrays(1, &booklet.vao);
    glDeleteBuffers(1, &booklet.vbo);
    glDeleteVertexArrays(1, &chargercable.vao);
    glDeleteBuffers(1, &chargercable.vbo);
    glDeleteVertexArrays(1, &watchband.vao);
    glDeleteBuffers(1, &watchband.vbo);
    glDeleteVertexArrays(1, &watchface.vao);
    glDeleteBuffers(1, &watchface.vbo);
    glDeleteVertexArrays(1, &cylinder.vao);
    glDeleteBuffers(1, &cylinder.vbo);
    glDeleteVertexArrays(1, &charger.vao);
    glDeleteBuffers(1, &charger.vbo);
    glDeleteVertexArrays(1, &chargercenter.vao);
    glDeleteBuffers(1, &chargercenter.vbo);


}


/*Generate and load the texture*/
bool UCreateTexture(const char* filename, GLuint& textureId)
{
    int width, height, channels;
    unsigned char* image = stbi_load(filename, &width, &height, &channels, 0);
    if (image)
    {
        flipImageVertically(image, width, height, channels);

        glGenTextures(1, &textureId);
        glBindTexture(GL_TEXTURE_2D, textureId);

        // set the texture wrapping parameters
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_MIRRORED_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_MIRRORED_REPEAT);
        // set texture filtering parameters
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

        if (channels == 3)
            glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB8, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, image);
        else if (channels == 4)
            glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, image);
        else
        {
            cout << "Not implemented to handle image with " << channels << " channels" << endl;
            return false;
        }

        glGenerateMipmap(GL_TEXTURE_2D);

        stbi_image_free(image);
        glBindTexture(GL_TEXTURE_2D, 0); // Unbind the texture

        return true;
    }

    // Error loading the image
    return false;
}


void UDestroyTexture(GLuint textureId)
{
    glGenTextures(1, &textureId);
}


// Implements the UCreateShaders function
bool UCreateShaderProgram(const char* vtxShaderSource, const char* fragShaderSource, GLuint& programId)
{
    // Compilation and linkage error reporting
    int success = 0;
    char infoLog[512];

    // Create a Shader program object.
    programId = glCreateProgram();

    // Create the vertex and fragment shader objects
    GLuint vertexShaderId = glCreateShader(GL_VERTEX_SHADER);
    GLuint fragmentShaderId = glCreateShader(GL_FRAGMENT_SHADER);

    // Retrive the shader source
    glShaderSource(vertexShaderId, 1, &vtxShaderSource, NULL);
    glShaderSource(fragmentShaderId, 1, &fragShaderSource, NULL);

    // Compile the vertex shader, and print compilation errors (if any)
    glCompileShader(vertexShaderId); // compile the vertex shader
    // check for shader compile errors
    glGetShaderiv(vertexShaderId, GL_COMPILE_STATUS, &success);
    if (!success)
    {
        glGetShaderInfoLog(vertexShaderId, 512, NULL, infoLog);
        std::cout << "ERROR::SHADER::VERTEX::COMPILATION_FAILED\n" << infoLog << std::endl;

        return false;
    }

    glCompileShader(fragmentShaderId); // compile the fragment shader
    // check for shader compile errors
    glGetShaderiv(fragmentShaderId, GL_COMPILE_STATUS, &success);
    if (!success)
    {
        glGetShaderInfoLog(fragmentShaderId, sizeof(infoLog), NULL, infoLog);
        std::cout << "ERROR::SHADER::FRAGMENT::COMPILATION_FAILED\n" << infoLog << std::endl;

        return false;
    }

    // Attached compiled shaders to the shader program
    glAttachShader(programId, vertexShaderId);
    glAttachShader(programId, fragmentShaderId);

    glLinkProgram(programId);   // links the shader program
    // check for linking errors
    glGetProgramiv(programId, GL_LINK_STATUS, &success);
    if (!success)
    {
        glGetProgramInfoLog(programId, sizeof(infoLog), NULL, infoLog);
        std::cout << "ERROR::SHADER::PROGRAM::LINKING_FAILED\n" << infoLog << std::endl;

        return false;
    }

    glUseProgram(programId);    // Uses the shader program

    return true;
}


void UDestroyShaderProgram(GLuint programId)
{
    glDeleteProgram(programId);
}
