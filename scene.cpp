#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <array>
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
#include "values.h"

const float PI = 3.141592;
const float windowWidth = 1600, windowHeight = 900;

// elemente pentru matricea de vizualizare
const float Refx = 0.0f, Refy = 0.0f, Refz = 0.0f, znear = 0.1, FOV = 45;

// sursa de lumina
const float lightX = -500.f, lightY = 600.f, lightZ = 0.f;

constexpr std::array faces = {"assets/skybox/right.jpg",
                              "assets/skybox/left.jpg",
                              "assets/skybox/top.jpg",
                              "assets/skybox/bottom.jpg",
                              "assets/skybox/front.jpg",
                              "assets/skybox/back.jpg"};

class Scene {
    GLuint VaoId, VboId, EboId, skyboxVAO, skyboxVBO, myMatrixLocation,
        matrUmbraLocation, viewLocation, projLocation, lightColorLocation,
        lightPosLocation, viewPosLocation, codColLocation, grassTextureId;

    float alpha = PI / 8, beta = 0.0f, dist = 600.0f;
    float Vx = 0.0, Vy = 1.0, Vz = 0.0;

    Shader sceneShader;
    Shader skyboxShader;
    Model airplane;
    Model tree;

    unsigned int cubemapTexture = loadCubemap(faces);

public:
    Scene()
        : sceneShader("scene.vert", "scene.frag")
        , skyboxShader("skybox.vert", "skybox.frag")
        , airplane("assets/airplane.obj")
        , tree("assets/trees.obj") {
        sceneShader.use();
    }

    void CreateSkyboxVBO() {
        // clang-format off
        GLfloat SkyboxVertices[] = {
            -1.0f,  1.0f, -1.0f,
            -1.0f, -1.0f, -1.0f,
             1.0f, -1.0f, -1.0f,
             1.0f, -1.0f, -1.0f,
             1.0f,  1.0f, -1.0f,
            -1.0f,  1.0f, -1.0f,

            -1.0f, -1.0f,  1.0f,
            -1.0f, -1.0f, -1.0f,
            -1.0f,  1.0f, -1.0f,
            -1.0f,  1.0f, -1.0f,
            -1.0f,  1.0f,  1.0f,
            -1.0f, -1.0f,  1.0f,

             1.0f, -1.0f, -1.0f,
             1.0f, -1.0f,  1.0f,
             1.0f,  1.0f,  1.0f,
             1.0f,  1.0f,  1.0f,
             1.0f,  1.0f, -1.0f,
             1.0f, -1.0f, -1.0f,

            -1.0f, -1.0f,  1.0f,
            -1.0f,  1.0f,  1.0f,
             1.0f,  1.0f,  1.0f,
             1.0f,  1.0f,  1.0f,
             1.0f, -1.0f,  1.0f,
            -1.0f, -1.0f,  1.0f,

            -1.0f,  1.0f, -1.0f,
             1.0f,  1.0f, -1.0f,
             1.0f,  1.0f,  1.0f,
             1.0f,  1.0f,  1.0f,
            -1.0f,  1.0f,  1.0f,
            -1.0f,  1.0f, -1.0f,

            -1.0f, -1.0f, -1.0f,
            -1.0f, -1.0f,  1.0f,
             1.0f, -1.0f, -1.0f,
             1.0f, -1.0f, -1.0f,
            -1.0f, -1.0f,  1.0f,
             1.0f, -1.0f,  1.0f,
        };
        // clang-format on

        glGenVertexArrays(1, &skyboxVAO);
        glGenBuffers(1, &skyboxVBO);
        glBindVertexArray(skyboxVAO);
        glBindBuffer(GL_ARRAY_BUFFER, skyboxVBO);
        glBufferData(GL_ARRAY_BUFFER,
                     sizeof(SkyboxVertices),
                     &SkyboxVertices,
                     GL_STATIC_DRAW);
        glEnableVertexAttribArray(0);
        glVertexAttribPointer(0,
                              3,
                              GL_FLOAT,
                              GL_FALSE,
                              3 * sizeof(float),
                              (void *)0);
    }

    void CreateVBO(void) {
        // clang-format off
        GLfloat Vertices[] = {
            -800.f, 0.f, -800.f, 1.0f,
             800.f, 0.f, -800.f, 1.0f,
             800.f, 0.f,  800.f, 1.0f,
            -800.f, 0.f,  800.f, 1.0f,
        };
        GLfloat Colors[] = {
            1.f, 1.f, 0.9f,
            1.f, 1.f, 0.9f,
            1.f, 1.f, 0.9f,
            1.f, 1.f, 0.9f,
        };
        GLfloat Normals[] = {
            0.f, 1.f, 0.f,
            0.f, 1.f, 0.f,
            0.f, 1.f, 0.f,
            0.f, 1.f, 0.f,
        };
        GLushort Indices[] = {
            1, 2, 0, 2, 0, 3
        };
        GLfloat TextureCoordinates[] = {
            0.0f,  0.0f,
            10.0f, 0.0f,
            10.0f, 10.0f,
            0.0f,  10.0f,
        };
        // clang-format on

        glGenVertexArrays(1, &VaoId);
        glGenBuffers(1, &VboId);
        glGenBuffers(1, &EboId);

        glBindVertexArray(VaoId);
        glBindBuffer(GL_ARRAY_BUFFER, VboId);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EboId);

        glBufferData(GL_ARRAY_BUFFER,
                     sizeof(Vertices) + sizeof(Colors) + sizeof(Normals) +
                         sizeof(TextureCoordinates),
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
        glBufferSubData(GL_ARRAY_BUFFER,
                        sizeof(Vertices) + sizeof(Colors) + sizeof(Normals),
                        sizeof(TextureCoordinates),
                        TextureCoordinates);
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

        glEnableVertexAttribArray(3); // atributul 3 = texturare
        glVertexAttribPointer(
            3,
            2,
            GL_FLOAT,
            GL_FALSE,
            2 * sizeof(GLfloat),
            (GLvoid *)(sizeof(Vertices) + sizeof(Colors) + sizeof(Normals)));
    }

    void DestroyVBO(void) {
        glDisableVertexAttribArray(3);
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
        CreateSkyboxVBO();
        CreateVBO();

        grassTextureId =
            TextureFromFile("forrest_ground_01.png", "assets/Texture");

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
        sceneShader.use();
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        glEnable(GL_DEPTH_TEST);

        // Observer position
        float Obsx = Refx + dist * cos(alpha) * cos(beta);
        float Obsy = Refy + dist * sin(alpha);
        float Obsz = Refz + dist * cos(alpha) * sin(beta);

        // View and projection
        glm::vec3 obsVec = glm::vec3(Obsx, Obsy, Obsz);
        glm::vec3 refVec = glm::vec3(Refx, Refy, Refz);
        glm::vec3 vertical = glm::vec3(Vx, Vy, Vz);
        glm::mat4 viewMat = glm::lookAt(obsVec, refVec, vertical);
        glUniformMatrix4fv(viewLocation, 1, GL_FALSE, &viewMat[0][0]);
        glm::mat4 projectionMat = glm::infinitePerspective(
            FOV,
            GLfloat(windowWidth) / GLfloat(windowHeight),
            znear);
        glUniformMatrix4fv(projLocation, 1, GL_FALSE, &projectionMat[0][0]);

        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, grassTextureId);

        // Shadow projection matrix
        float D = -2.5f;
        float matrUmbra[4][4];
        matrUmbra[0][0] = lightY + D;
        matrUmbra[0][1] = 0;
        matrUmbra[0][2] = 0;
        matrUmbra[0][3] = 0;

        matrUmbra[1][0] = -lightX;
        matrUmbra[1][1] = D;
        matrUmbra[1][2] = -lightZ;
        matrUmbra[1][3] = -1;

        matrUmbra[2][0] = 0;
        matrUmbra[2][1] = 0;
        matrUmbra[2][2] = lightY + D;
        matrUmbra[2][3] = 0;

        matrUmbra[3][0] = -D * lightX;
        matrUmbra[3][1] = -D * lightY;
        matrUmbra[3][2] = -D * lightZ;
        matrUmbra[3][3] = lightY;
        glUniformMatrix4fv(matrUmbraLocation, 1, GL_FALSE, &matrUmbra[0][0]);

        // Variabile uniforme pentru iluminare
        glUniform3f(lightColorLocation, 0.5f, 0.5f, 0.5f);
        glUniform3f(lightPosLocation, lightX, lightY, lightZ);
        glUniform3f(viewPosLocation, Obsx, Obsy, Obsz);

        int codCol = 2;
        glUniform1i(codColLocation, codCol);
        glm::mat4 myMatrix = glm::mat4(1.0f);
        glUniformMatrix4fv(myMatrixLocation, 1, GL_FALSE, &myMatrix[0][0]);

        // Ground
        glBindVertexArray(VaoId);
        glBindBuffer(GL_ARRAY_BUFFER, VboId);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EboId);
        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_SHORT, 0);

        glm::mat4 rotation =
            glm::rotate(glm::radians(25.0f), glm::vec3(1.0f, 0.0f, 0.5f));
        glm::mat4 scale = glm::scale(glm::vec3(20.0f, 20.0f, 20.0f));
        glm::mat4 transl = glm::translate(glm::vec3(0.0f, 15.0f, 0.0f));
        myMatrix = transl * scale * rotation;
        glUniformMatrix4fv(myMatrixLocation, 1, GL_FALSE, &myMatrix[0][0]);

        codCol = 0;
        glUniform1i(codColLocation, codCol);
        airplane.Draw(sceneShader);

        codCol = 1;
        glUniform1i(codColLocation, codCol);
        airplane.Draw(sceneShader);

        // Trees
        codCol = 2;
        glUniform1i(codColLocation, codCol);
        DrawTrees();

        // Tree shadows
        codCol = 1;
        glUniform1i(codColLocation, codCol);
        DrawTrees();

        // Change depth function so depth test passes when values are equal to
        // depth buffer's content
        glDepthFunc(GL_LEQUAL);
        skyboxShader.use();
        // Remove translation from the view matrix
        viewMat = glm::mat4(glm::mat3(viewMat));
        skyboxShader.setMat4("view", viewMat);
        skyboxShader.setMat4("projection", projectionMat);
        // Skybox cube
        glBindVertexArray(skyboxVAO);
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_CUBE_MAP, cubemapTexture);
        glDrawArrays(GL_TRIANGLES, 0, 36);
        glBindVertexArray(0);
        // Set depth function back to default
        glDepthFunc(GL_LESS);

        glFlush();
    }

    void DrawTrees() {
        glm::mat4 scale = glm::scale(glm::vec3(5.0f, 5.0f, 5.0f));
        for (auto &&[treeType, coordinates] : TREE_COORDINATES) {
            const float treeXCoord = coordinates.first;
            const float treeZCoord = coordinates.second;

            glm::mat4 transl = glm::translate(glm::vec3(treeXCoord, 0.0f, treeZCoord));

            glm::mat4 modelMat = transl * scale;
            glUniformMatrix4fv(myMatrixLocation, 1, GL_FALSE, &modelMat[0][0]);

            tree.meshes[treeType].Draw(sceneShader);
        }
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

    GLuint loadCubemap(const std::array<const char *, 6> &faces) {
        GLuint textureID;
        glGenTextures(1, &textureID);
        glBindTexture(GL_TEXTURE_CUBE_MAP, textureID);

        stbi_set_flip_vertically_on_load(false);
        for (unsigned int i = 0; i < faces.size(); i++) {
            int width, height, nrChannels;
            unsigned char *data =
                stbi_load(faces[i], &width, &height, &nrChannels, 0);
            if (data) {
                glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i,
                             0,
                             GL_RGB,
                             width,
                             height,
                             0,
                             GL_RGB,
                             GL_UNSIGNED_BYTE,
                             data);
                stbi_image_free(data);
            } else {
                std::cout << "Cubemap tex failed to load at path: " << faces[i]
                          << std::endl;
                stbi_image_free(data);
            }
        }
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_CUBE_MAP,
                        GL_TEXTURE_WRAP_S,
                        GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_CUBE_MAP,
                        GL_TEXTURE_WRAP_T,
                        GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_CUBE_MAP,
                        GL_TEXTURE_WRAP_R,
                        GL_CLAMP_TO_EDGE);

        return textureID;
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
