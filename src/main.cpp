// Use OpenGL Helpers to reduce the clutter
#include "Helpers.h"
#include "triangles.h"
// GLFW is necessary to handle the OpenGL context
#include <GLFW/glfw3.h>

#include <iostream>
#include <Eigen/Dense>

#include <OpenGL/OpenGL.h>

using namespace std;
using namespace Eigen;
typedef Eigen::Matrix<float,2,3> threePoints;
typedef Eigen::Matrix<float,3,3> threeByThree;


unsigned int tIndex = 0;
unsigned int tIndex1 = 1;

VertexBufferObject VBO(tIndex);
VertexBufferObject VBOc(tIndex1);
threeByThree V;//V is a temporary buffer for collecting 3 points, then past 3 points to instantiate new Triangle
threeByThree C;
MatrixXf totV;//The actual vertices to assign to VBO
MatrixXf totC;
Matrix4f view(4,4);
//Create dynamic array for collecting triangle instances
vector<Triangle> triangles;
int totTriangles = 0;// # of triangles on screen


// Initialize mouse interaction mode
bool insertionMode = false;
bool translationMode = false;
bool deletionMode = false;
bool colorMode =false;
bool selected = false;
bool doubleClick = false;
int vCounter=0;
int selectedIndex = 0;//the index of the selected triangle in array
double worldX, worldY;
int winW=0;
int winH=0;
int colorKey = 1;

//create a single matrix for VBO, take in triangles array by reference
void concatMatrix(const vector<Triangle> &triangles){
    int cols= (int)triangles.size()*3;
    MatrixXf newV(3,cols);
    MatrixXf newC(3,cols);

    for (int i =0; i< triangles.size(); i++){
        newV.col(i*3)<<(triangles[i].V.col(0));
        newV.col(i*3+1)<<(triangles[i].V.col(1));
        newV.col(i*3+2)<<(triangles[i].V.col(2));
        newC.col(i*3)<<triangles[i].Vc.col(0);
        newC.col(i*3+1)<<triangles[i].Vc.col(1);
        newC.col(i*3+2)<<triangles[i].Vc.col(2);
    }
    totV = newV;
    totC = newC;
}

void clearBuffer(){
    V<<0,0,0,
       0,0,0,
       1,1,1;
    vCounter=0;
    doubleClick = false;
    colorKey = 1;
}

void scaleView(int zoom){
    float a = view(0,0);
    if(zoom == 0){
        view<<a*1.25,0,0,0,
        0,a*1.25,0,0,
        0,0,1,0,
        0,0,0,1;
    }
    else{
        view<<a*0.75,0,0,0,
                0,a*0.75,0,0,
                0,0,1,0,
                0,0,0,1;
    }
}
void moveView(int move){
    float a = view(1,3);
    float b = view(0,3);
    float aspect = double(winH)/double(winW);
    switch(move){
        case 0:
            view<<aspect,0,0,0,
                    0,1,0,a+0.2,
                    0,0,1,0,
                    0,0,0,1;
            break;
        case 1:
            view<<aspect,0,0,0,
                    0,1,0,a-0.2,
                    0,0,1,0,
                    0,0,0,1;
            break;
        case 2:
            view<<aspect,0,0,b+0.2,
                    0,1,0,0,
                    0,0,1,0,
                    0,0,0,1;
            break;
        case 3:
            view<<aspect,0,0,b-0.2,
                    0,1,0,0,
                    0,0,1,0,
                    0,0,0,1;
            break;
        default:
            break;
    }
}

void scaleTriangle(int zoom){
    cout<<"Scale function is called!"<<endl;
    triangles[selectedIndex].scaleT(winW,winH,zoom);
    triangles[selectedIndex].calculateDist(worldX, worldY);
    concatMatrix(triangles);
    VBO.update(totV);
}

void rotateTriangle(int direction){
    cout<<"Rotation function is called!"<<endl;
    triangles[selectedIndex].rotateT(winW,winH,direction);
    triangles[selectedIndex].calculateDist(worldX, worldY);
    concatMatrix(triangles);
    VBO.update(totV);
}

// Update mouse position if cursor is moving
void cursorPositionCallback(GLFWwindow *window, double mouseX, double mouseY){
    glfwGetWindowSize(window, &winW, &winH);
    Matrix2f inverseView;
    inverseView << view(0,0), view(0,1), view(1,0), view(1,1);
    inverseView = inverseView.inverse().eval();
    Vector2f p_screen( (mouseX/double(winW))*2-1, ((winH-1-mouseY)/double(winH))*2-1);
    Vector2f p_canonical = inverseView*p_screen;
    worldX = p_canonical(0);
    worldY = p_canonical(1);
    if(selected){
        triangles[selectedIndex].updateVertices(worldX, worldY);
        concatMatrix(triangles);
        VBO.update(totV);
    }
};


//Update input mode
void key_setMode(GLFWwindow* window, int key, int scancode, int action, int mods){
    switch (key){
        case GLFW_KEY_I:
            insertionMode = true;
            translationMode = false;
            deletionMode = false;
            colorMode = false;
            break;
        case GLFW_KEY_O:
            translationMode = true;
            insertionMode = false;
            deletionMode = false;
            colorMode = false;
            clearBuffer();
            break;
        case GLFW_KEY_P:
            deletionMode = true;
            insertionMode = false;
            translationMode = false;
            colorMode = false;
            clearBuffer();
            break;
        case GLFW_KEY_C:
            colorMode = true;
            deletionMode = false;
            insertionMode = false;
            translationMode = false;
            clearBuffer();
            cout<<"COLOR MODE"<<endl;
            break;
        case GLFW_KEY_H:
            if(translationMode&&selected)
                rotateTriangle(0);
            break;
        case GLFW_KEY_J:
            if(translationMode&&selected)
                rotateTriangle(1);
            break;
        case GLFW_KEY_K:
            if(translationMode&&selected)
                scaleTriangle(0);                
            break;
        case GLFW_KEY_L:
            if(translationMode&&selected)
                scaleTriangle(1);
            break;
        case GLFW_KEY_EQUAL://This stands for the '+' sign
            scaleView(0);
            break;
        case GLFW_KEY_MINUS://This stands for the '-' sign
            scaleView(1);
            break;
        case GLFW_KEY_W://up
            moveView(0);
            break;
        case GLFW_KEY_A://down
            moveView(1);
            break;
        case GLFW_KEY_S://left
            moveView(2);
            break;
        case GLFW_KEY_D://right
            moveView(3);
            break;
        default:
            break;
    }
}

//Draw or translate triangle
void mouse_click_callback(GLFWwindow* window, int button, int action, int mods)
{
    if(button == GLFW_MOUSE_BUTTON_LEFT) {
        if (insertionMode) {
            if (action == GLFW_PRESS)
            {
                //update V
                vCounter++;
                V.col(0) << V.col(1)(0), V.col(1)(1),1;
                V.col(1) << V.col(2)(0), V.col(2)(1),1;
                V.col(2) << worldX, worldY,1;
            }
            if (vCounter == 3) {
                totTriangles++;
                Triangle t0(V);//instantiate triangle, pass vertices by copy
                triangles.resize((unsigned int) totTriangles);
                triangles[(unsigned int) totTriangles - 1] = t0;
                concatMatrix(triangles);
                VBO.update(totV);
                vCounter = 0;
            }
        }
        else if (translationMode) {
            if (action == GLFW_PRESS) {
                doubleClick = !doubleClick;//click once, set to true; click twice, set to false
                cout << " mouse is pressed! " << endl;
                if (!doubleClick){
                    selected = false;
                }
                else{
                    for (int i = totTriangles - 1; i >= 0; i--) {
                        if (triangles[i].checkIntersection(worldX, worldY)) {
                            triangles[i].calculateDist(worldX, worldY);
                            selected = true;
                            selectedIndex = i;
                            cout << "Triangle # " << i << " is selected!" << endl;
                            break;
                        }
                    }
                }
            }
        }
        else if (deletionMode){
            if (action == GLFW_PRESS) {
                for (int i = totTriangles - 1; i >= 0; i--) {
                    if (triangles[i].checkIntersection(worldX, worldY)) {
                        triangles.erase(triangles.begin()+i);
                        totTriangles--;
                        concatMatrix(triangles);
                        VBO.update(totV);
                        break;
                    }
                }
            }
        }
        else if(colorMode){
            if (action == GLFW_PRESS) {
                cout << "Color Mode Clicking!" << endl;
                for (int i = totTriangles - 1; i >= 0; i--) {
                    if (triangles[i].checkIntersection(worldX, worldY)) {
                        triangles[i].calculateDist(worldX, worldY);//find dist between cursor and three vertices
                        int m = triangles[i].getClosestVertex();//get index of the closest vertex
                        triangles[i].changeColor(colorKey, m);
                        selectedIndex = i;
                        concatMatrix(triangles);
                        VBOc.update(totC);
                        break;
                    }
                }
            }
        }
    }
}


int main(void)
{
    GLFWwindow* window;

    // Initialize the library
    if (!glfwInit())
        return -1;

    // Activate supersampling
    glfwWindowHint(GLFW_SAMPLES, 8);

    // Ensure that we get at least a 3.2 context
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 2);

    // On apple we have to load a core profile with forward compatibility
#ifdef __APPLE__
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif

    // Create a windowed mode window and its OpenGL context
    window = glfwCreateWindow(640, 480, "Drawing Triangle", NULL, NULL);
    if (!window)
    {
        glfwTerminate();
        return -1;
    }

    glfwGetWindowSize(window, &winW, &winH);

    // Make the window's context current
    glfwMakeContextCurrent(window);

    #ifndef __APPLE__
      glewExperimental = true;
      GLenum err = glewInit();
      if(GLEW_OK != err)
      {
        /* Problem: glewInit failed, something is seriously wrong. */
       fprintf(stderr, "Error: %s\n", glewGetErrorString(err));
      }
      glGetError(); // pull and savely ignonre unhandled errors like GL_INVALID_ENUM
      fprintf(stdout, "Status: Using GLEW %s\n", glewGetString(GLEW_VERSION));
    #endif

    int major, minor, rev;
    major = glfwGetWindowAttrib(window, GLFW_CONTEXT_VERSION_MAJOR);
    minor = glfwGetWindowAttrib(window, GLFW_CONTEXT_VERSION_MINOR);
    rev = glfwGetWindowAttrib(window, GLFW_CONTEXT_REVISION);
    printf("OpenGL version recieved: %d.%d.%d\n", major, minor, rev);
    printf("Supported OpenGL is %s\n", (const char*)glGetString(GL_VERSION));
    printf("Supported GLSL is %s\n", (const char*)glGetString(GL_SHADING_LANGUAGE_VERSION));
    // Initialize the VAO
    VertexArrayObject VAO;
    VAO.init();
    VAO.bind();

    // Initialize the VBO with the vertices data
    VBO.init();
    VBO.update(V);
    VBOc.init();
    V << 0,0,0, 0,0,0, 1,1,1;
    C << 1,0,0, 0,1,0, 0,0,1;
    VBOc.update(C);

    // Initialize the OpenGL Program
    Program program;
    const GLchar* vertex_shader =
                    "#version 150 core\n"
                    "in vec3 position;"
                    "in vec3 color;"
                    "out vec3 f_color;"
                    "uniform mat4 view;"
                    "void main()"
                    "{"
                        "gl_Position = view * vec4(position, 1.0);"
                        "f_color = color;"
                    "}";
    const GLchar* fragment_shader =
                    "#version 150 core\n"
                    "in vec3 f_color;"
                    "out vec4 outColor;"
                    "uniform vec3 triangleColor;"
                    "void main()"
                    "{"
                    "    outColor = vec4(f_color, 1.0);"
                    "}";

    // Compile the two shaders and upload the binary to the GPU
    program.init(vertex_shader,fragment_shader,"outColor");
    program.bind();

    // Bind vertex shader with "position", fragment shader with "color"
    program.bindVertexAttribArray("position",VBO);
    program.bindVertexAttribArray("color",VBOc);

    // Save the current time --- it will be used to dynamically change the triangle color
    auto t_start = std::chrono::high_resolution_clock::now();

    glfwSetCursorPosCallback(window, cursorPositionCallback);
    // Register the mouse callback
    glfwSetMouseButtonCallback(window, mouse_click_callback);
    // Register the keyboard callback
    glfwSetKeyCallback(window, key_setMode);

    float aspect = double(winH)/double(winW);
    cout<<"windH is "<<winH<<endl;
    cout<<"windW is "<<winW<<endl;
    view << aspect,0, 0, 0,
            0, 1, 0, 0,
            0, 0, 1, 0,
            0, 0, 0, 1;
    // Loop until the user closes the window
    while (!glfwWindowShouldClose(window))
    {
        // Bind VAO
        VAO.bind();
        // Bind program
        program.bind();

        VBOc.update(totC);

        glUniformMatrix4fv(program.uniform("view"), 1, GL_FALSE, view.data());

        // Clear the framebuffer
        glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

        // Draw a triangle
        for(int i = 0; i<totTriangles; i++){
            //draw shape
            if(i == selectedIndex && selected){
                glUniform3f(program.uniform("triangleColor"), 0.0f,0.0f, 1.0f);
                glDrawArrays(GL_TRIANGLES, i*3, i*3+3);
            }
            else{
                glDrawArrays(GL_TRIANGLES, i*3, i*3+3);
            }
            //draw lines
            glDrawArrays(GL_LINE_LOOP, i*3, 3);
        }
        // Swap front and back buffers
        glfwSwapBuffers(window);
        // Poll for and process events
        glfwPollEvents();
    }

    // Deallocate opengl memory
    program.free();
    VAO.free();
    VBO.free();

    // Deallocate glfw internals
    glfwTerminate();
    return 0;
}
