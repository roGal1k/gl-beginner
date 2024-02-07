#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>
#include <glad/glad.h>
#include <KHR/khrplatform.h>
#include <iostream>

// Обработчик нажатия клавиш
void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods) {
    if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
        glfwSetWindowShouldClose(window, GLFW_TRUE);
}

int main() {
    // Инициализация GLFW
    if (!glfwInit()) {
        std::cerr << "Ошибка инициализации GLFW\n";
        return -1;
    }

    // Создание окна
    GLFWwindow* window = glfwCreateWindow(800, 600, "Example GLFW", NULL, NULL);
    if (!window) {
        std::cerr << "Ошибка создания окна GLFW\n";
        glfwTerminate();
        return -1;
    }

    // Установка контекста OpenGL для созданного окна
    glfwMakeContextCurrent(window);

    // Установка обработчика нажатия клавиш
    glfwSetKeyCallback(window, key_callback);

    // Инициализация GLAD перед вызовом OpenGL функций
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
        std::cerr << "Ошибка инициализации GLAD\n";
        return -1;
    }

    // Основной цикл программы
    while (!glfwWindowShouldClose(window)) {
        // Отрисовка

        // Обновление окна
        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    // Освобождение ресурсов GLFW
    glfwTerminate();
    return 0;
}