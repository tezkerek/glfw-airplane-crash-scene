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
#include "values.h"


const float PI = 3.141592;
const float windowWidth = 800, windowHeight = 600;

// elemente pentru matricea de vizualizare
const float Refx = 0.0f, Refy = 0.0f, Refz = 0.0f, znear = 0.1, fov = 45;

// sursa de lumina
const float lightX = 500.f, lightY = 100.f, lightZ = 600.f;


class Scene {
    GLuint VaoId, VboId, EboId, skyboxVAO, skyboxVBO, skyboxEboId, myMatrixLocation, matrUmbraLocation,
        viewLocation, projLocation, lightColorLocation, lightPosLocation,
        viewPosLocation, codColLocation;

    float alpha = PI / 8, beta = 0.0f, dist = 400.0f;
    float Vx = 0.0, Vy = 0.0, Vz = 1.0;

    Shader sceneShader;
    Shader skyboxShader;
    Model airplane;
    Model tree;
    
    std::vector<std::string> faces
    {
        "assets/skybox/right.jpg",
        "assets/skybox/left.jpg",
        "assets/skybox/top.jpg",
        "assets/skybox/bottom.jpg",
        "assets/skybox/front.jpg",
        "assets/skybox/back.jpg"
    };
    
    unsigned int cubemapTexture = loadCubemap(faces);

public:
    Scene()
        : sceneShader("scene.vert", "scene.frag")
        , skyboxShader("skybox.vert", "skybox.frag")
        , airplane("assets/airplane.obj")
        , tree("assets/trees.obj") {
        sceneShader.use();
    }

    void CreateSkyboxVBO(){
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
        
            glGenVertexArrays(1, &skyboxVAO);
            glGenBuffers(1, &skyboxVBO);
            glBindVertexArray(skyboxVAO);
            glBindBuffer(GL_ARRAY_BUFFER, skyboxVBO);
            glBufferData(GL_ARRAY_BUFFER, sizeof(SkyboxVertices), &SkyboxVertices, GL_STATIC_DRAW);
            glEnableVertexAttribArray(0);
            glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
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
        CreateSkyboxVBO();
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
        sceneShader.use();
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

        codCol = 2;
        glUniform1i(codColLocation, codCol);
        rotation = glm::rotate(glm::radians(90.f), glm::vec3(1.0f, 0.f, 0.0f));
        scale = glm::scale(glm::vec3(5.0f, 5.0f, 5.0f));
        for (auto &&[treeType, coordinates] : TREE_COORDINATES) {
            const float treeXCoord = coordinates.first;
            const float treeYCoord = coordinates.second;

            transl = glm::translate(glm::vec3(treeXCoord, treeYCoord, 0.0f));

            myMatrix = transl * scale * rotation;
            glUniformMatrix4fv(myMatrixLocation, 1, GL_FALSE, &myMatrix[0][0]);

            tree.meshes[treeType].Draw(sceneShader);
        }
        
        glDepthFunc(GL_LEQUAL);  // change depth function so depth test passes when values are equal to depth buffer's content
        skyboxShader.use();
        view = glm::mat4(glm::mat3(view)); // remove translation from the view matrix
        skyboxShader.setMat4("view", view);
        skyboxShader.setMat4("projection", projection);
                // skybox cube
        glBindVertexArray(skyboxVAO);
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_CUBE_MAP, cubemapTexture);
        glDrawArrays(GL_TRIANGLES, 0, 36);
        glBindVertexArray(0);
        glDepthFunc(GL_LESS); // set depth function back to default

        
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
    
    unsigned int loadCubemap(std::vector<std::string> faces)
    {
        unsigned int textureID;
        glGenTextures(1, &textureID);
        glBindTexture(GL_TEXTURE_CUBE_MAP, textureID);

        int width, height, nrChannels;
        stbi_set_flip_vertically_on_load(false);
        for (unsigned int i = 0; i < faces.size(); i++)
        {
            unsigned char *data = stbi_load(faces[i].c_str(), &width, &height, &nrChannels, 0);
            if (data)
            {
                glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i,
                             0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data
                );
                stbi_image_free(data);
            }
            else
            {
                std::cout << "Cubemap tex failed to load at path: " << faces[i] << std::endl;
                stbi_image_free(data);
            }
        }
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

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
