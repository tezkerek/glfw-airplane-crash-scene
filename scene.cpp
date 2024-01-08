#include "loadShaders.h"
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtx/transform.hpp>
#include <iostream>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>

//  Identificatorii obiectelor de tip OpenGL;
GLuint VaoId, VboId, EboId, ColorBufferId, ProgramId, myMatrixLocation,
    matrUmbraLocation, viewLocation, projLocation, matrRotlLocation,
    lightColorLocation, lightPosLocation, viewPosLocation, codColLocation;

int codCol;
float PI = 3.141592;

// matrice utilizate
glm::mat4 myMatrix, matrRot;

// elemente pentru matricea de vizualizare
float Refx = 0.0f, Refy = 0.0f, Refz = 0.0f;
float alpha = PI / 8, beta = 0.0f, dist = 400.0f;
float Obsx, Obsy, Obsz;
float Vx = 0.0, Vy = 0.0, Vz = 1.0;
glm::mat4 view;

// elemente pentru matricea de proiectie
float width = 800, height = 600, xwmin = -800.f, xwmax = 800, ywmin = -600,
      ywmax = 600, znear = 0.1, zfar = 1, fov = 45;
glm::mat4 projection;

// sursa de lumina
float xL = 500.f, yL = 100.f, zL = 600.f;

// matricea umbrei
float matrUmbra[4][4];

void ProcessKeys(GLFWwindow*, int button, int, int, int) {
    switch (button) {
    case GLFW_KEY_LEFT:
        beta -= 0.05;
        break;
    case GLFW_KEY_RIGHT:
        beta += 0.05;
        break;
    case GLFW_KEY_UP:
        alpha += 0.05;
        break;
    case GLFW_KEY_DOWN:
        alpha -= 0.05;
        break;
    case GLFW_KEY_L:
        Vx -= 0.1;
        break;
    case GLFW_KEY_R:
        Vx += 0.1;
        break;
    case GLFW_KEY_EQUAL:
        dist += 5;
        break;
    case GLFW_KEY_MINUS:
        dist -= 5;
        break;
    }
}

void CreateVBO(void) {
    GLfloat Vertices[] = {
        -500.f, -500.f, 0.f, 1.0f,
        500.f, -500.f, 0.f, 1.0f,
        500.f, 500.f, 0.f, 1.0f,
        -500.f, 500.f, 0.f, 1.0f,
    };
    GLfloat Colors[] = {
        1.f, 1.f, 0.9f,
        1.f, 1.f, 0.9f,
        1.f, 1.f, 0.9f,
        1.f, 1.f, 0.9f,
    };
    GLfloat Normals[] = {
        0.f, 0.f, 1.f,
        0.f, 0.f, 1.f,
        0.f, 0.f, 1.f,
        0.f, 0.f, 1.f,
    };
    GLushort Indices[] = {
        1, 2, 0, 2, 0, 3
    };

    glGenVertexArrays(1, &VaoId);
    glGenBuffers(1, &VboId);
    glGenBuffers(1, &EboId);

    glBindVertexArray(VaoId);
    glBindBuffer(GL_ARRAY_BUFFER, VboId);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EboId);

    glBufferData(GL_ARRAY_BUFFER,
                 sizeof(Vertices) + sizeof(Colors) + sizeof(Normals),
                 NULL,
                 GL_STATIC_DRAW);
    glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(Vertices), Vertices);
    glBufferSubData(GL_ARRAY_BUFFER, sizeof(Vertices), sizeof(Colors), Colors);
    glBufferSubData(GL_ARRAY_BUFFER,
                    sizeof(Vertices) + sizeof(Colors),
                    sizeof(Normals),
                    Normals);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER,
                 sizeof(Indices),
                 Indices,
                 GL_STATIC_DRAW);


    glEnableVertexAttribArray(0); // atributul 0 = pozitie
    glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 4 * sizeof(GLfloat), (GLvoid *)0);

    glEnableVertexAttribArray(1); // atributul 1 = culoare
    glVertexAttribPointer(1,
                          3,
                          GL_FLOAT,
                          GL_FALSE,
                          3 * sizeof(GLfloat),
                          (GLvoid *)sizeof(Vertices));

    glEnableVertexAttribArray(2); // atributul 2 = normala
    glVertexAttribPointer(2,
                          3,
                          GL_FLOAT,
                          GL_FALSE,
                          3 * sizeof(GLfloat),
                          (GLvoid *)(sizeof(Vertices) + sizeof(Colors)));
}

void DestroyVBO(void) {
    glDisableVertexAttribArray(2);
    glDisableVertexAttribArray(1);
    glDisableVertexAttribArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glDeleteBuffers(1, &VboId);
    glDeleteBuffers(1, &EboId);
    glBindVertexArray(0);
    glDeleteVertexArrays(1, &VaoId);
}

void CreateShaders(void) {
    ProgramId = LoadShaders("scene.vert", "scene.frag");
    glUseProgram(ProgramId);
}

void DestroyShaders(void) { glDeleteProgram(ProgramId); }

void Initialize(void) {
    myMatrix = glm::mat4(1.0f);
    matrRot = glm::rotate(glm::mat4(1.0f), PI / 8, glm::vec3(0.0, 0.0, 1.0));
    glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
    CreateVBO();
    CreateShaders();
    // locatii pentru shader-e
    myMatrixLocation = glGetUniformLocation(ProgramId, "myMatrix");
    matrUmbraLocation = glGetUniformLocation(ProgramId, "matrUmbra");
    viewLocation = glGetUniformLocation(ProgramId, "view");
    projLocation = glGetUniformLocation(ProgramId, "projection");
    lightColorLocation = glGetUniformLocation(ProgramId, "lightColor");
    lightPosLocation = glGetUniformLocation(ProgramId, "lightPos");
    viewPosLocation = glGetUniformLocation(ProgramId, "viewPos");
    codColLocation = glGetUniformLocation(ProgramId, "codCol");
}

void RenderFunction(void) {
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glEnable(GL_DEPTH_TEST);

    // pozitia observatorului
    Obsx = Refx + dist * cos(alpha) * cos(beta);
    Obsy = Refy + dist * cos(alpha) * sin(beta);
    Obsz = Refz + dist * sin(alpha);

    // matrice de vizualizare + proiectie
    glm::vec3 Obs =
        glm::vec3(Obsx, Obsy, Obsz); // se schimba pozitia observatorului
    glm::vec3 PctRef =
        glm::vec3(Refx, Refy, Refz); // pozitia punctului de referinta
    glm::vec3 Vert =
        glm::vec3(Vx, Vy, Vz); // verticala din planul de vizualizare
    view = glm::lookAt(Obs, PctRef, Vert);
    glUniformMatrix4fv(viewLocation, 1, GL_FALSE, &view[0][0]);
    projection =
        glm::infinitePerspective(fov, GLfloat(width) / GLfloat(height), znear);
    glUniformMatrix4fv(projLocation, 1, GL_FALSE, &projection[0][0]);

    // matricea pentru umbra
    float D = -2.5f;
    matrUmbra[0][0] = zL + D;
    matrUmbra[0][1] = 0;
    matrUmbra[0][2] = 0;
    matrUmbra[0][3] = 0;
    matrUmbra[1][0] = 0;
    matrUmbra[1][1] = zL + D;
    matrUmbra[1][2] = 0;
    matrUmbra[1][3] = 0;
    matrUmbra[2][0] = -xL;
    matrUmbra[2][1] = -yL;
    matrUmbra[2][2] = D;
    matrUmbra[2][3] = -1;
    matrUmbra[3][0] = -D * xL;
    matrUmbra[3][1] = -D * yL;
    matrUmbra[3][2] = -D * zL;
    matrUmbra[3][3] = zL;
    glUniformMatrix4fv(matrUmbraLocation, 1, GL_FALSE, &matrUmbra[0][0]);

    // Variabile uniforme pentru iluminare
    glUniform3f(lightColorLocation, 1.0f, 1.0f, 1.0f);
    glUniform3f(lightPosLocation, xL, yL, zL);
    glUniform3f(viewPosLocation, Obsx, Obsy, Obsz);

    codCol = 0;
    glUniform1i(codColLocation, codCol);
    myMatrix = glm::mat4(1.0f);
    glUniformMatrix4fv(myMatrixLocation, 1, GL_FALSE, &myMatrix[0][0]);

    // Ground
    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_SHORT, 0);

    glFlush();
}

void Cleanup(void) {
    DestroyShaders();
    DestroyVBO();
}

int main() {
    GLenum res = glfwInit();
    if (res == GLFW_FALSE) {
        std::cerr << "GLFW initialization failed\n";
        exit(1);
    }

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 1);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    auto _window =
        glfwCreateWindow(width, height, "Crash scene", nullptr, nullptr);

    if (_window == nullptr) {
        std::cerr << "Failed to create GLFW window \n";
        glfwTerminate();
        exit(-1);
    }
    glfwMakeContextCurrent(_window);

    glewInit();

    Initialize();

    while (!glfwWindowShouldClose(_window)) {
        /* updatePlane(); */
        RenderFunction();
        glfwSetKeyCallback(_window, ProcessKeys);
        glfwSwapBuffers(_window);
        glfwPollEvents();
    }
    Cleanup();
    glfwTerminate();
    return 0;
}
