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

#include "model.h"
#include "shader.h"

const float PI = 3.141592;
const float windowWidth = 800, windowHeight = 600;

// elemente pentru matricea de vizualizare
const float Refx = 0.0f, Refy = 0.0f, Refz = 0.0f, znear = 0.1, fov = 45;

// sursa de lumina
const float lightX = 500.f, lightY = 100.f, lightZ = 600.f;

class Scene {
    GLuint VaoId, VboId, EboId, myMatrixLocation, matrUmbraLocation,
        viewLocation, projLocation, lightColorLocation, lightPosLocation,
        viewPosLocation, codColLocation;

    float alpha = PI / 8, beta = 0.0f, dist = 400.0f;
    float Vx = 0.0, Vy = 0.0, Vz = 1.0;

    Shader sceneShader;
    Model airplane;
    Model tree;

    const std::vector<std::pair<float, std::pair<float, float>>> treeCoordonates = {{1, {-407, -371}}, {1, {-441, -413}}, {2, {-236, 222}}, {0, {186, 149}}, {4, {302, -459}}, {1, {-332, 272}}, {0, {125, -471}}, {4, {-190, -463}}, {3, {31, 310}}, {1, {-102, -454}}, {0, {179, 232}}, {4, {-58, -354}}, {3, {58, -453}}, {1, {-396, -359}}, {4, {-130, -441}}, {2, {-27, -492}}, {1, {159, -337}}, {4, {202, 315}}, {3, {170, 291}}, {0, {-398, 145}}, {4, {-296, 142}}, {2, {430, -416}}, {3, {-101, 420}}, {1, {-191, 466}}, {4, {-317, 133}}, {2, {381, 266}}, {3, {500, -428}}, {4, {-336, -418}}, {4, {-269, 230}}, {3, {-437, 487}}, {4, {401, 144}}, {4, {404, 332}}, {1, {498, 270}}, {0, {-57, 253}}, {4, {322, 490}}, {4, {28, 470}}, {3, {172, -465}}, {1, {228, 333}}, {4, {220, -464}}, {2, {-76, 440}}, {0, {-378, -477}}, {0, {94, 485}}, {1, {-293, 281}}, {0, {321, -306}}, {2, {266, -430}}, {3, {-359, 425}}, {2, {81, -361}}, {0, {-206, 149}}, {1, {-281, -340}}, {2, {398, 112}}, {4, {-168, -360}}, {1, {397, 379}}, {0, {375, -312}}, {2, {-144, 378}}, {3, {451, 362}}, {0, {434, -317}}, {4, {-95, 224}}, {1, {-158, -412}}, {3, {-28, -458}}, {0, {-446, -360}}, {1, {-429, 216}}, {0, {-105, -365}}, {0, {275, -348}}, {3, {-328, -383}}, {4, {403, 236}}, {0, {121, 200}}, {3, {-246, 368}}, {3, {-42, 430}}, {1, {-426, -425}}, {1, {-280, 185}}, {0, {127, 354}}, {3, {-267, -362}}, {0, {-228, 423}}, {4, {368, -331}}, {3, {-254, 434}}, {3, {-385, 362}}, {3, {477, 482}}, {0, {-399, 279}}, {3, {319, 249}}, {0, {241, 328}}, {1, {264, 59}}, {2, {113, 180}}, {3, {277, -201}}, {2, {283, 364}}, {3, {419, 17}}, {3, {368, 194}}, {1, {331, -74}}, {2, {325, -75}}, {0, {117, -147}}, {1, {-352, -57}}, {2, {-324, 289}}, {0, {435, 5}}, {4, {385, -208}}, {2, {144, -482}}, {1, {370, -457}}, {2, {481, -175}}, {0, {455, 455}}, {0, {203, 444}}, {2, {-484, 116}}, {4, {-430, -455}}, {4, {232, -369}}, {4, {285, 459}}, {4, {-323, 276}}, {3, {159, 395}}, {2, {-481, 322}}, {4, {476, 175}}, {1, {364, 365}}, {0, {-343, 148}}, {1, {115, 218}}, {2, {-324, -27}}, {1, {-483, -133}}, {1, {-378, 287}}, {4, {337, -441}}, {0, {152, 182}}, {2, {-339, 486}}, {1, {387, 468}}, {2, {113, 381}}, {4, {209, 314}}, {0, {155, -332}}, {1, {-366, -216}}, {1, {-386, 137}}, {1, {286, -439}}, {1, {-334, 39}}, {0, {494, -264}}, {4, {115, -243}}, {0, {218, -308}}, {4, {168, 332}}, {1, {-463, 116}}, {3, {-335, -258}}, {3, {377, 319}}, {3, {252, 345}}, {3, {-413, 469}}, {2, {251, 379}}, {0, {-473, 51}}, {3, {138, -453}}, {0, {275, 294}}, {3, {336, 240}}, {1, {-499, -191}}, {0, {-500, -14}}, {1, {162, 18}}, {2, {354, -321}}, {3, {494, -146}}, {4, {-474, -249}}, {3, {-438, 471}}, {2,   {261, -9}}, {1, {-334, -154}}, {4, {-460, 311}}, {4, {238, 415}}, {1, {-316, -98}}, {4, {-333, 323}}};
        

public:
    Scene()
        : sceneShader("scene.vert", "scene.frag")
        , airplane("assets/airplane.obj")
        , tree("assets/trees.obj") {
        sceneShader.use();
    }

    void CreateVBO(void) {
        // clang-format off
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
        // clang-format on

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
        glBufferSubData(GL_ARRAY_BUFFER,
                        sizeof(Vertices),
                        sizeof(Colors),
                        Colors);
        glBufferSubData(GL_ARRAY_BUFFER,
                        sizeof(Vertices) + sizeof(Colors),
                        sizeof(Normals),
                        Normals);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER,
                     sizeof(Indices),
                     Indices,
                     GL_STATIC_DRAW);

        glEnableVertexAttribArray(0); // atributul 0 = pozitie
        glVertexAttribPointer(0,
                              4,
                              GL_FLOAT,
                              GL_FALSE,
                              4 * sizeof(GLfloat),
                              (GLvoid *)0);

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

    void Initialize(void) {
        glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
        CreateVBO();
        /* CreateShaders(); */
        // locatii pentru shader-e

        GLuint programId = sceneShader.ID;
        myMatrixLocation = glGetUniformLocation(programId, "myMatrix");
        matrUmbraLocation = glGetUniformLocation(programId, "matrUmbra");
        viewLocation = glGetUniformLocation(programId, "view");
        projLocation = glGetUniformLocation(programId, "projection");
        lightColorLocation = glGetUniformLocation(programId, "lightColor");
        lightPosLocation = glGetUniformLocation(programId, "lightPos");
        viewPosLocation = glGetUniformLocation(programId, "viewPos");
        codColLocation = glGetUniformLocation(programId, "codCol");
    }

    void Draw(void) {
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        glEnable(GL_DEPTH_TEST);

        // pozitia observatorului
        float Obsx = Refx + dist * cos(alpha) * cos(beta);
        float Obsy = Refy + dist * cos(alpha) * sin(beta);
        float Obsz = Refz + dist * sin(alpha);

        // matrice de vizualizare + proiectie
        glm::vec3 Obs =
            glm::vec3(Obsx, Obsy, Obsz); // se schimba pozitia observatorului
        glm::vec3 PctRef =
            glm::vec3(Refx, Refy, Refz); // pozitia punctului de referinta
        glm::vec3 Vert =
            glm::vec3(Vx, Vy, Vz); // verticala din planul de vizualizare
        glm::mat4 view = glm::lookAt(Obs, PctRef, Vert);
        glUniformMatrix4fv(viewLocation, 1, GL_FALSE, &view[0][0]);
        glm::mat4 projection = glm::infinitePerspective(
            fov,
            GLfloat(windowWidth) / GLfloat(windowHeight),
            znear);
        glUniformMatrix4fv(projLocation, 1, GL_FALSE, &projection[0][0]);

        // matricea pentru umbra
        float D = -2.5f;

        // matricea umbrei
        float matrUmbra[4][4];
        matrUmbra[0][0] = lightZ + D;
        matrUmbra[0][1] = 0;
        matrUmbra[0][2] = 0;
        matrUmbra[0][3] = 0;
        matrUmbra[1][0] = 0;
        matrUmbra[1][1] = lightZ + D;
        matrUmbra[1][2] = 0;
        matrUmbra[1][3] = 0;
        matrUmbra[2][0] = -lightX;
        matrUmbra[2][1] = -lightY;
        matrUmbra[2][2] = D;
        matrUmbra[2][3] = -1;
        matrUmbra[3][0] = -D * lightX;
        matrUmbra[3][1] = -D * lightY;
        matrUmbra[3][2] = -D * lightZ;
        matrUmbra[3][3] = lightZ;
        glUniformMatrix4fv(matrUmbraLocation, 1, GL_FALSE, &matrUmbra[0][0]);

        // Variabile uniforme pentru iluminare
        glUniform3f(lightColorLocation, 1.0f, 1.0f, 1.0f);
        glUniform3f(lightPosLocation, lightX, lightY, lightZ);
        glUniform3f(viewPosLocation, Obsx, Obsy, Obsz);

        int codCol = 0;
        glUniform1i(codColLocation, codCol);
        glm::mat4 myMatrix = glm::mat4(1.0f);
        glUniformMatrix4fv(myMatrixLocation, 1, GL_FALSE, &myMatrix[0][0]);

        // Ground
        glBindVertexArray(VaoId);
        glBindBuffer(GL_ARRAY_BUFFER, VboId);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EboId);
        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_SHORT, 0);

        glm::mat4 rotation =
            glm::rotate(glm::radians(105.0f), glm::vec3(1.0f, 0.25f, 0.0f));
        glm::mat4 scale = glm::scale(glm::vec3(20.0f, 20.0f, 20.0f));
        glm::mat4 transl = glm::translate(glm::vec3(0.0f, 0.0f, 15.0f));
        myMatrix = transl * scale * rotation;
        glUniformMatrix4fv(myMatrixLocation, 1, GL_FALSE, &myMatrix[0][0]);

        airplane.Draw(sceneShader);
        
        rotation = glm::rotate(glm::radians(90.f), glm::vec3(1.0f, 0.f, 0.0f));
        scale = glm::scale(glm::vec3(5.0f, 5.0f, 5.0f));
        for (auto &&[treeType, coordinates] : treeCoordonates)
        {
            const float treeXCoord = coordinates.first;
            const float treeYCoord = coordinates.first;
            
            transl = glm::translate(glm::vec3(treeXCoord, treeYCoord, 0.0f));

            myMatrix = transl  * scale * rotation;
            glUniformMatrix4fv(myMatrixLocation, 1, GL_FALSE, &myMatrix[0][0]);

            tree.meshes[treeType].Draw(sceneShader);
        }

        glFlush();
    }

void OnKeyPress(int button) {
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

    ~Scene() { DestroyVBO(); }
};

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

    auto _window = glfwCreateWindow(windowWidth,
                                    windowHeight,
                                    "Crash scene",
                                    nullptr,
                                    nullptr);

    if (_window == nullptr) {
        std::cerr << "Failed to create GLFW window \n";
        glfwTerminate();
        exit(-1);
    }
    glfwMakeContextCurrent(_window);

    glewInit();

    {
        static Scene scene;
        scene.Initialize();

        while (!glfwWindowShouldClose(_window)) {
            scene.Draw();
            glfwSetKeyCallback(_window,
                               [](GLFWwindow *, int button, int, int, int) {
                                   scene.OnKeyPress(button);
                               });

            glfwSwapBuffers(_window);
            glfwPollEvents();
        }
    }
    glfwTerminate();
    return 0;
}
